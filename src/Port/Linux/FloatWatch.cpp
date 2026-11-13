// Native x86-64 build only: with EE_PORT_FPE_LOG set, the layer logs each instruction that overflows single precision.
//
// The i386 build keeps float intermediates in x87 extended precision, where a product such as FLT_MAX * x stays
// finite. The x86-64 build computes them in SSE single precision, where the same product is infinite, and 0 times
// that infinity is NaN. Code that relies on x87's range therefore gives an infinity or a NaN on x86-64 only, and no
// compiler warning finds it (docs/limits.md).
//
// The constructor unmasks SSE's overflow exception before the game starts a thread, and every thread inherits it. At
// an overflow, the handler logs the instruction the first time it sees it, masks the exception and sets the trap flag.
// The instruction then runs again and gives the infinity it gives on a normal run, and the trap after that one
// instruction unmasks the exception again. The game runs on as without the variable.
#ifdef EE_PORT_LINUX64

#include <dlfcn.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>
#include <xmmintrin.h>

#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace {

constexpr unsigned kOverflowMask = 0x400; // MXCSR OM
constexpr unsigned kOverflowFlag = 0x8;   // MXCSR OE
constexpr greg_t kTrapFlag = 0x100;       // EFLAGS TF

std::atomic<uintptr_t> sSites[1024];
// initial-exec: the handlers read it, and the general TLS model can allocate on a thread's first access
__thread bool sStepping __attribute__((tls_model("initial-exec")));

// true the first time an address is seen; when the table is full, no more sites are logged
bool FirstTime(uintptr_t inAddress)
{
    for (std::atomic<uintptr_t>& theSite : sSites) {
        uintptr_t theValue = theSite.load(std::memory_order_relaxed);
        if (theValue == 0 && theSite.compare_exchange_strong(theValue, inAddress))
            return true;
        if (theValue == inAddress)
            return false;
    }
    return false;
}

void Append(char*& ioOut, const char* inEnd, const char* inText)
{
    while (*inText && ioOut < inEnd)
        *ioOut++ = *inText++;
}

void AppendHex(char*& ioOut, const char* inEnd, uintptr_t inValue)
{
    char theDigits[16];
    int theCount = 0;
    do {
        theDigits[theCount++] = "0123456789abcdef"[inValue & 15];
        inValue >>= 4;
    } while (inValue);
    Append(ioOut, inEnd, "0x");
    while (theCount && ioOut < inEnd)
        *ioOut++ = theDigits[--theCount];
}

// One line per site: the image and the offset into it, which addr2line -e <image> turns into a source line
void LogSite(uintptr_t inAddress)
{
    char theLine[512];
    char* theOut = theLine;
    const char* theEnd = theLine + sizeof theLine - 1;
    Append(theOut, theEnd, "ee-port: float overflow at ");
    Dl_info theInfo;
    // dladdr is not on POSIX's async-signal-safe list. It only reads the loader's tables, and the loader does no
    // float arithmetic, so the overflow cannot interrupt it.
    if (dladdr(reinterpret_cast<void*>(inAddress), &theInfo) && theInfo.dli_fname && *theInfo.dli_fname) {
        const char* theName = strrchr(theInfo.dli_fname, '/');
        Append(theOut, theEnd, theName ? theName + 1 : theInfo.dli_fname);
        Append(theOut, theEnd, "+");
        AppendHex(theOut, theEnd, inAddress - reinterpret_cast<uintptr_t>(theInfo.dli_fbase));
        if (theInfo.dli_sname) {
            Append(theOut, theEnd, " (");
            Append(theOut, theEnd, theInfo.dli_sname);
            Append(theOut, theEnd, ")");
        }
    } else {
        AppendHex(theOut, theEnd, inAddress);
    }
    *theOut++ = '\n';
    [[maybe_unused]] ssize_t theWritten = write(STDERR_FILENO, theLine, theOut - theLine);
}

void OnFloatException(int inSignal, siginfo_t* inInfo, void* inContext)
{
    if (inInfo->si_code != FPE_FLTOVF) {
        // Any other arithmetic fault: the instruction runs again and ends the game as it does without the handler
        signal(inSignal, SIG_DFL);
        return;
    }
    mcontext_t& theMachine = static_cast<ucontext_t*>(inContext)->uc_mcontext;
    uintptr_t theAddress = theMachine.gregs[REG_RIP];
    if (FirstTime(theAddress))
        LogSite(theAddress);
    theMachine.fpregs->mxcsr |= kOverflowMask;
    theMachine.gregs[REG_EFL] |= kTrapFlag;
    sStepping = true;
}

void OnTrap(int inSignal, siginfo_t*, void* inContext)
{
    if (!sStepping) {
        // Not the step after an overflow: the default action, as without the handler
        signal(inSignal, SIG_DFL);
        raise(inSignal);
        return;
    }
    sStepping = false;
    mcontext_t& theMachine = static_cast<ucontext_t*>(inContext)->uc_mcontext;
    theMachine.gregs[REG_EFL] &= ~kTrapFlag;
    theMachine.fpregs->mxcsr &= ~(kOverflowMask | kOverflowFlag);
}

__attribute__((constructor)) void WatchFloatOverflow()
{
    if (!getenv("EE_PORT_FPE_LOG"))
        return;
    struct sigaction theAction = {};
    theAction.sa_flags = SA_SIGINFO;
    sigemptyset(&theAction.sa_mask);
    theAction.sa_sigaction = OnFloatException;
    sigaction(SIGFPE, &theAction, nullptr);
    theAction.sa_sigaction = OnTrap;
    sigaction(SIGTRAP, &theAction, nullptr);
    _mm_setcsr(_mm_getcsr() & ~kOverflowMask);
}

} // namespace

#endif // EE_PORT_LINUX64
