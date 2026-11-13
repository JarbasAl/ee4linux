// Tests of the overflow log (src/Port/Linux/FloatWatch.cpp). On x86-64, with EE_PORT_FPE_LOG set, each instruction
// that overflows single precision is logged once with its image and offset, and the program runs on with the infinity
// it gives without the variable. Other arithmetic faults keep their default action. The i386 build installs nothing.
//
// The layer reads the variable when it loads, so each case runs this program again as a child with the variable set
// or unset, and reads the child's standard error.
#include <dlfcn.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cfloat>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

static int sFailures;
static int sChecks;

#define CHECK(condition)                                                                 \
    do {                                                                                 \
        ++sChecks;                                                                       \
        if (!(condition)) {                                                              \
            ++sFailures;                                                                 \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);         \
        }                                                                                \
    } while (0)

static volatile float sLarge = FLT_MAX;
// both volatile: the compiler turns 1 / x into a comparison with no division in it
static volatile int sOne = 1;
static volatile int sZero = 0;

__attribute__((noinline)) static float Double(float inValue)
{
    return inValue * 2.0f;
}

__attribute__((noinline)) static float Quadruple(float inValue)
{
    return inValue * 4.0f;
}

static unsigned long OffsetOf(void* inAddress)
{
    Dl_info theInfo;
    return dladdr(inAddress, &theInfo) ? (unsigned long)((char*)inAddress - (char*)theInfo.dli_fbase) : 0;
}

// The child's cases. "overflow": each of two instructions overflows twice, one of them once in a second thread, and
// one multiplication stays in range; the child prints the two functions' offsets and exits 0 when every result is
// the one IEEE single precision gives. "divide": an integer division by zero. "handler": prints whether SIGFPE has
// a handler.
static int Child(const char* inCase)
{
    if (!strcmp(inCase, "divide"))
        return sOne / sZero;
    if (!strcmp(inCase, "handler")) {
        struct sigaction theAction;
        sigaction(SIGFPE, NULL, &theAction);
        fprintf(stderr, "handler %s\n", theAction.sa_handler == SIG_DFL ? "default" : "installed");
        return 0;
    }
    float theFirst = Double(sLarge);
    float theSecond = Double(sLarge);
    float theThreaded = 0.0f;
    std::thread theThread([&theThreaded] { theThreaded = Quadruple(sLarge); });
    theThread.join();
    float theFourth = Quadruple(sLarge);
    float theHalf = sLarge * 0.5f;
    fprintf(stderr, "functions 0x%lx 0x%lx\n", OffsetOf((void*)Double), OffsetOf((void*)Quadruple));
    bool theInfinite = theFirst == INFINITY && theSecond == INFINITY && theThreaded == INFINITY && theFourth == INFINITY;
    return theInfinite && theHalf == 0x1.fffffep126f ? 0 : 3;
}

struct Result {
    int mStatus;
    std::string mError;
};

static Result Run(const char* inCase, bool inLog)
{
    int thePipe[2];
    if (pipe(thePipe))
        return {-1, ""};
    pid_t theChild = fork();
    if (theChild == 0) {
        dup2(thePipe[1], STDERR_FILENO);
        close(thePipe[0]);
        if (inLog)
            setenv("EE_PORT_FPE_LOG", "1", 1);
        else
            unsetenv("EE_PORT_FPE_LOG");
        execl("/proc/self/exe", "test_fpe", inCase, (char*)NULL);
        _exit(127);
    }
    close(thePipe[1]);
    Result theResult{-1, ""};
    char theBuffer[4096];
    ssize_t theCount;
    while ((theCount = read(thePipe[0], theBuffer, sizeof theBuffer)) > 0)
        theResult.mError.append(theBuffer, theCount);
    close(thePipe[0]);
    waitpid(theChild, &theResult.mStatus, 0);
    return theResult;
}

static std::vector<std::string> LinesWith(const std::string& inText, const char* inPrefix)
{
    std::vector<std::string> theLines;
    size_t theStart = 0;
    while (theStart < inText.size()) {
        size_t theEnd = inText.find('\n', theStart);
        if (theEnd == std::string::npos)
            theEnd = inText.size();
        std::string theLine = inText.substr(theStart, theEnd - theStart);
        if (theLine.compare(0, strlen(inPrefix), inPrefix) == 0)
            theLines.push_back(theLine);
        theStart = theEnd + 1;
    }
    return theLines;
}

// The offset after "test_fpe+" in a log line; 0 when the line names another image
static unsigned long LoggedOffset(const std::string& inLine)
{
    size_t thePosition = inLine.find("test_fpe+0x");
    return thePosition == std::string::npos ? 0 : strtoul(inLine.c_str() + thePosition + 9, NULL, 16);
}

int main(int argc, char** argv)
{
    if (argc == 2)
        return Child(argv[1]);
    const char* kLogged = "ee-port: float overflow at ";

#ifdef EE_PORT_LINUX64
    Result theLogged = Run("overflow", true);
    CHECK(WIFEXITED(theLogged.mStatus) && WEXITSTATUS(theLogged.mStatus) == 0);
    std::vector<std::string> theSites = LinesWith(theLogged.mError, kLogged);
    CHECK(theSites.size() == 2);
    unsigned long theDouble = 0, theQuadruple = 0;
    std::vector<std::string> theFunctions = LinesWith(theLogged.mError, "functions ");
    CHECK(theFunctions.size() == 1 && sscanf(theFunctions[0].c_str(), "functions %lx %lx", &theDouble, &theQuadruple) == 2);
    // each site is the multiplication inside its function, which is a few instructions long: a site belongs to the
    // function that starts nearest below it, as the compiler may place the two functions 8 bytes apart
    int theInDouble = 0, theInQuadruple = 0;
    for (const std::string& theSite : theSites) {
        unsigned long theOffset = LoggedOffset(theSite);
        bool theAboveDouble = theOffset >= theDouble && theOffset < theDouble + 16;
        bool theAboveQuadruple = theOffset >= theQuadruple && theOffset < theQuadruple + 16;
        if (theAboveDouble && theAboveQuadruple)
            (theDouble > theQuadruple ? theAboveQuadruple : theAboveDouble) = false;
        theInDouble += theAboveDouble;
        theInQuadruple += theAboveQuadruple;
    }
    CHECK(theInDouble == 1 && theInQuadruple == 1);

    Result theQuiet = Run("overflow", false);
    CHECK(WIFEXITED(theQuiet.mStatus) && WEXITSTATUS(theQuiet.mStatus) == 0);
    CHECK(LinesWith(theQuiet.mError, kLogged).empty());

    Result theDivide = Run("divide", true);
    CHECK(WIFSIGNALED(theDivide.mStatus) && WTERMSIG(theDivide.mStatus) == SIGFPE);
    CHECK(LinesWith(theDivide.mError, kLogged).empty());

    Result theHandler = Run("handler", true);
    CHECK(LinesWith(theHandler.mError, "handler installed").size() == 1);
#else
    Result theHandler = Run("handler", true);
    CHECK(LinesWith(theHandler.mError, "handler default").size() == 1);
#endif
    if (sFailures) {
        printf("%d of %d checks failed\n", sFailures, sChecks);
        return 1;
    }
    printf("%d checks passed\n", sChecks);
    return 0;
}
