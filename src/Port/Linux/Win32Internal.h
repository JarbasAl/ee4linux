// The native Linux build's Win32 layer: what its units share. Wine's headers declare the Win32 functions; the units
// in src/Port/Linux define the ones the game calls, on POSIX and SDL.
#ifndef EE_PORT_LINUX_WIN32INTERNAL_H
#define EE_PORT_LINUX_WIN32INTERNAL_H

#include <windows.h>

#include <atomic>
#include <cstdint>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string>

namespace eeport {

// Writes a line to stderr with the "ee-port: " prefix
void Log(const char* inFormat, ...) __attribute__((format(printf, 1, 2)));

// Logs once per call site that the game called something the layer does not do
#define EE_PORT_UNSUPPORTED(name)                                                       \
    do {                                                                                \
        static std::atomic<bool> sLogged{false};                                        \
        if (!sLogged.exchange(true))                                                    \
            eeport::Log("%s is not supported; the call does nothing", name);            \
    } while (0)

// Calls a game callback with 32-bit arguments, whether it is stdcall or cdecl: Windows calls the game's cdecl
// callbacks through stdcall types (UResourceFile's enumeration callbacks), and its callers restore the stack pointer
// from their frame. This call saves esp, aligns the stack to 16 bytes as the Linux i386 ABI requires, pushes the
// arguments, calls, and restores esp.
inline uintptr_t CallCallback(const void* inFunction, const uintptr_t* inArguments, int inCount)
{
    uintptr_t theResult;
    __asm__ __volatile__(
        "movl %%esp, %%edi\n\t"
        "andl $-16, %%esp\n\t"
        "leal (,%3,4), %%edx\n\t"
        "negl %%edx\n\t"
        "andl $15, %%edx\n\t"
        "subl %%edx, %%esp\n\t"
        "movl %3, %%ecx\n\t"
        "1:\n\t"
        "testl %%ecx, %%ecx\n\t"
        "jz 2f\n\t"
        "pushl -4(%2,%%ecx,4)\n\t"
        "decl %%ecx\n\t"
        "jmp 1b\n\t"
        "2:\n\t"
        "call *%1\n\t"
        "movl %%edi, %%esp\n\t"
        : "=a"(theResult)
        : "S"(inFunction), "b"(inArguments), "a"(inCount)
        : "ecx", "edx", "edi", "memory", "cc");
    return theResult;
}

// ---- kernel objects ----
// A HANDLE is a pointer to one. Its waitable state changes only under gWaitLock, and every change notifies
// gWaitChanged: a waiter holds the lock, tests its handles and sleeps on the condition.
struct KernelObject {
    enum Kind { kEvent, kMutex, kThread, kFile, kMapping, kFind, kTimer };

    explicit KernelObject(Kind inKind) : mKind(inKind) {}
    virtual ~KernelObject() {}

    // Under gWaitLock: whether a wait by inThread would succeed now
    virtual bool IsSignaled(DWORD inThread) const { (void)inThread; return false; }
    // Under gWaitLock: the side effect of a successful wait (an auto-reset event resets, a mutex is owned)
    virtual void Acquired(DWORD inThread) { (void)inThread; }

    const Kind mKind;
    std::atomic<int> mReferences{1};
};

extern std::mutex gWaitLock;
extern std::condition_variable gWaitChanged;

HANDLE NewHandle(KernelObject* inObject);
// The object of a handle, or NULL for NULL, INVALID_HANDLE_VALUE and the pseudo handles
KernelObject* ObjectOf(HANDLE inHandle);

// The calling thread's id: sequential from 1, the first thread to ask (the main thread) gets 1
DWORD CurrentThreadID();

// ---- thread message queues (Win32Message.cpp) ----
struct ThreadQueue {
    std::deque<MSG> mMessages;      // under gWaitLock
};
// The queue of a thread, made on first use; NULL for an id that never had one when inCreate is false
ThreadQueue* QueueOf(DWORD inThread, bool inCreate);

// ---- modules (Win32Module.cpp) ----
// The bytes of a resource in a PE module and their size, or NULL; inLanguage 0 takes the first language
const void* FindResourceData(HMODULE inModule, LPCSTR inType, LPCSTR inName, WORD inLanguage, DWORD* outSize);

// ---- paths (Win32Path.cpp) ----
// The Linux path of a Windows path: Z: is the root, backslashes become slashes, and a component that does not exist
// as written is matched without regard to case. A missing file keeps its last component as written.
std::string LinuxPath(const char* inWindowsPath);
// The Windows path of a Linux path: an absolute path gets Z: and backslashes
std::string WindowsPath(const std::string& inLinuxPath);

} // namespace eeport

#endif
