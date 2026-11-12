// Kernel objects, waits, critical sections, threads, clocks and the last error, on POSIX threads
#include "Win32Internal.h"

#include <process.h>

#include <cerrno>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <pthread.h>
#include <sched.h>
#include <thread>
#include <unistd.h>

namespace eeport {

std::mutex gWaitLock;
std::condition_variable gWaitChanged;

void Log(const char* inFormat, ...)
{
    char theText[1024];
    va_list theArguments;
    va_start(theArguments, inFormat);
    vsnprintf(theText, sizeof theText, inFormat, theArguments);
    va_end(theArguments);
    fprintf(stderr, "ee-port: %s\n", theText);
}

HANDLE NewHandle(KernelObject* inObject)
{
    return reinterpret_cast<HANDLE>(inObject);
}

KernelObject* ObjectOf(HANDLE inHandle)
{
    if (inHandle == NULL || inHandle == INVALID_HANDLE_VALUE || inHandle == GetCurrentThread())
        return NULL;
    return reinterpret_cast<KernelObject*>(inHandle);
}

static void Release(KernelObject* inObject)
{
    if (inObject->mReferences.fetch_sub(1) == 1)
        delete inObject;
}

static std::atomic<DWORD> sNextThreadID{1};
static thread_local DWORD tThreadID;

DWORD CurrentThreadID()
{
    if (!tThreadID)
        tThreadID = sNextThreadID.fetch_add(1);
    return tThreadID;
}

static thread_local DWORD tLastError;

// ---- events and mutexes ----
struct Event : KernelObject {
    Event(bool inManual, bool inSignaled) : KernelObject(kEvent), mManual(inManual), mSignaled(inSignaled) {}
    bool IsSignaled(DWORD) const override { return this->mSignaled; }
    void Acquired(DWORD) override
    {
        if (!this->mManual)
            this->mSignaled = false;
    }
    const bool mManual;
    bool mSignaled;
};

struct Mutex : KernelObject {
    Mutex() : KernelObject(kMutex) {}
    bool IsSignaled(DWORD inThread) const override { return this->mOwner == 0 || this->mOwner == inThread; }
    void Acquired(DWORD inThread) override
    {
        this->mOwner = inThread;
        ++this->mCount;
    }
    DWORD mOwner = 0;
    DWORD mCount = 0;
};

// ---- threads ----
struct Thread : KernelObject {
    Thread() : KernelObject(kThread) {}
    bool IsSignaled(DWORD) const override { return this->mFinished; }
    LPTHREAD_START_ROUTINE mStart = NULL;
    void* mArgument = NULL;
    DWORD mID = 0;
    DWORD mExitCode = STILL_ACTIVE;
    bool mFinished = false;
    bool mSuspended = false;        // started suspended and not resumed yet
    int mPriority = THREAD_PRIORITY_NORMAL;
    pthread_t mPThread{};
};

static thread_local Thread* tThread;

// The calling thread's thread object. A thread this layer starts gets its object in ThreadMain; a thread it did not
// start (the main thread of the entry point) gets one on first use, so that GetCurrentThread's pseudo handle refers to
// a real object there too.
static Thread* CurrentThreadObject()
{
    if (!tThread) {
        tThread = new Thread;
        tThread->mID = CurrentThreadID();
    }
    return tThread;
}

static void FinishThread(Thread* inThread, DWORD inExitCode)
{
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        inThread->mExitCode = inExitCode;
        inThread->mFinished = true;
    }
    gWaitChanged.notify_all();
    Release(inThread);
}

static void* ThreadMain(void* inThread)
{
    Thread* theThread = static_cast<Thread*>(inThread);
    tThread = theThread;
    tThreadID = theThread->mID;
    {
        std::unique_lock<std::mutex> theLock(gWaitLock);
        gWaitChanged.wait(theLock, [theThread] { return !theThread->mSuspended; });
    }
    DWORD theResult = theThread->mStart(theThread->mArgument);
    FinishThread(theThread, theResult);
    return NULL;
}

static HANDLE StartThread(SIZE_T inStackSize, LPTHREAD_START_ROUTINE inStart, void* inArgument, DWORD inFlags,
                          DWORD* outID)
{
    Thread* theThread = new Thread;
    theThread->mStart = inStart;
    theThread->mArgument = inArgument;
    theThread->mID = sNextThreadID.fetch_add(1);
    theThread->mSuspended = (inFlags & CREATE_SUSPENDED) != 0;
    theThread->mReferences = 2;     // the handle and the running thread
    pthread_attr_t theAttributes;
    pthread_attr_init(&theAttributes);
    pthread_attr_setdetachstate(&theAttributes, PTHREAD_CREATE_DETACHED);
    if (inStackSize)
        pthread_attr_setstacksize(&theAttributes, inStackSize < 65536 ? 65536 : inStackSize);
    int theError = pthread_create(&theThread->mPThread, &theAttributes, ThreadMain, theThread);
    pthread_attr_destroy(&theAttributes);
    if (theError) {
        delete theThread;
        tLastError = ERROR_NOT_ENOUGH_MEMORY;
        return NULL;
    }
    if (outID)
        *outID = theThread->mID;
    return NewHandle(theThread);
}

static unsigned long long MonotonicNanoseconds()
{
    timespec theTime;
    clock_gettime(CLOCK_MONOTONIC, &theTime);
    return (unsigned long long)theTime.tv_sec * 1000000000ull + (unsigned long long)theTime.tv_nsec;
}

} // namespace eeport

using namespace eeport;

extern "C" {

DWORD WINAPI GetLastError(void) { return tLastError; }
void WINAPI SetLastError(DWORD inError) { tLastError = inError; }

void WINAPI OutputDebugStringA(LPCSTR inText)
{
    fputs(inText, stderr);
}

void WINAPI ExitProcess(DWORD inCode)
{
    fflush(NULL);
    _exit((int)inCode);
}

BOOL WINAPI CloseHandle(HANDLE inHandle)
{
    KernelObject* theObject = ObjectOf(inHandle);
    if (!theObject) {
        tLastError = ERROR_INVALID_HANDLE;
        return FALSE;
    }
    Release(theObject);
    return TRUE;
}

BOOL WINAPI DuplicateHandle(HANDLE inSourceProcess, HANDLE inSource, HANDLE inTargetProcess, LPHANDLE outTarget,
                            DWORD inAccess, BOOL inInherit, DWORD inOptions)
{
    (void)inSourceProcess, (void)inTargetProcess, (void)inAccess, (void)inInherit;
    KernelObject* theObject = inSource == GetCurrentThread() ? CurrentThreadObject() : ObjectOf(inSource);
    if (!theObject) {
        tLastError = ERROR_INVALID_HANDLE;
        return FALSE;
    }
    theObject->mReferences.fetch_add(1);
    *outTarget = NewHandle(theObject);
    if (inOptions & DUPLICATE_CLOSE_SOURCE)
        Release(theObject);
    return TRUE;
}

HANDLE WINAPI CreateEventA(LPSECURITY_ATTRIBUTES inAttributes, BOOL inManualReset, BOOL inInitialState, LPCSTR inName)
{
    (void)inAttributes;
    if (inName)
        Log("CreateEventA(\"%s\"): named events are not shared between processes here", inName);
    return NewHandle(new Event(inManualReset, inInitialState));
}

static BOOL SetEventState(HANDLE inHandle, bool inSignaled)
{
    KernelObject* theObject = ObjectOf(inHandle);
    if (!theObject || theObject->mKind != KernelObject::kEvent) {
        tLastError = ERROR_INVALID_HANDLE;
        return FALSE;
    }
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        static_cast<Event*>(theObject)->mSignaled = inSignaled;
    }
    if (inSignaled)
        gWaitChanged.notify_all();
    return TRUE;
}

BOOL WINAPI SetEvent(HANDLE inEvent) { return SetEventState(inEvent, true); }
BOOL WINAPI ResetEvent(HANDLE inEvent) { return SetEventState(inEvent, false); }

HANDLE WINAPI CreateMutexA(LPSECURITY_ATTRIBUTES inAttributes, BOOL inInitialOwner, LPCSTR inName)
{
    (void)inAttributes;
    if (inName)
        Log("CreateMutexA(\"%s\"): named mutexes are not shared between processes here", inName);
    Mutex* theMutex = new Mutex;
    if (inInitialOwner) {
        theMutex->mOwner = CurrentThreadID();
        theMutex->mCount = 1;
    }
    tLastError = 0;     // the game asks for ERROR_ALREADY_EXISTS to find a second copy of itself
    return NewHandle(theMutex);
}

BOOL WINAPI ReleaseMutex(HANDLE inMutex)
{
    KernelObject* theObject = ObjectOf(inMutex);
    if (!theObject || theObject->mKind != KernelObject::kMutex) {
        tLastError = ERROR_INVALID_HANDLE;
        return FALSE;
    }
    Mutex* theMutex = static_cast<Mutex*>(theObject);
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        if (theMutex->mOwner != CurrentThreadID()) {
            tLastError = ERROR_NOT_OWNER;
            return FALSE;
        }
        if (--theMutex->mCount == 0)
            theMutex->mOwner = 0;
    }
    gWaitChanged.notify_all();
    return TRUE;
}

DWORD WINAPI WaitForMultipleObjects(DWORD inCount, const HANDLE* inHandles, BOOL inAll, DWORD inMilliseconds)
{
    KernelObject* theObjects[MAXIMUM_WAIT_OBJECTS];
    if (inCount == 0 || inCount > MAXIMUM_WAIT_OBJECTS) {
        tLastError = ERROR_INVALID_PARAMETER;
        return WAIT_FAILED;
    }
    for (DWORD i = 0; i < inCount; ++i) {
        theObjects[i] = inHandles[i] == GetCurrentThread() ? CurrentThreadObject() : ObjectOf(inHandles[i]);
        if (!theObjects[i]) {
            tLastError = ERROR_INVALID_HANDLE;
            return WAIT_FAILED;
        }
    }
    DWORD theThread = CurrentThreadID();
    auto theDeadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(inMilliseconds);
    std::unique_lock<std::mutex> theLock(gWaitLock);
    for (;;) {
        if (inAll) {
            bool theReady = true;
            for (DWORD i = 0; i < inCount && theReady; ++i)
                theReady = theObjects[i]->IsSignaled(theThread);
            if (theReady) {
                for (DWORD i = 0; i < inCount; ++i)
                    theObjects[i]->Acquired(theThread);
                return WAIT_OBJECT_0;
            }
        } else {
            for (DWORD i = 0; i < inCount; ++i)
                if (theObjects[i]->IsSignaled(theThread)) {
                    theObjects[i]->Acquired(theThread);
                    return WAIT_OBJECT_0 + i;
                }
        }
        if (inMilliseconds == 0)
            return WAIT_TIMEOUT;
        if (inMilliseconds == INFINITE)
            gWaitChanged.wait(theLock);
        else if (gWaitChanged.wait_until(theLock, theDeadline) == std::cv_status::timeout) {
            inMilliseconds = 0;     // one more test of the handles, then the timeout
        }
    }
}

DWORD WINAPI WaitForSingleObject(HANDLE inHandle, DWORD inMilliseconds)
{
    return WaitForMultipleObjects(1, &inHandle, FALSE, inMilliseconds);
}

void WINAPI Sleep(DWORD inMilliseconds)
{
    if (inMilliseconds == 0)
        sched_yield();
    else
        std::this_thread::sleep_for(std::chrono::milliseconds(inMilliseconds));
}

// ---- critical sections: a recursive POSIX mutex, its pointer in LockSemaphore ----
static pthread_mutex_t* MutexOf(LPCRITICAL_SECTION inSection)
{
    return reinterpret_cast<pthread_mutex_t*>(inSection->LockSemaphore);
}

void WINAPI InitializeCriticalSection(LPCRITICAL_SECTION outSection)
{
    memset(outSection, 0, sizeof *outSection);
    pthread_mutex_t* theMutex = new pthread_mutex_t;
    pthread_mutexattr_t theAttributes;
    pthread_mutexattr_init(&theAttributes);
    pthread_mutexattr_settype(&theAttributes, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(theMutex, &theAttributes);
    pthread_mutexattr_destroy(&theAttributes);
    outSection->LockSemaphore = reinterpret_cast<HANDLE>(theMutex);
    outSection->LockCount = -1;
}

void WINAPI DeleteCriticalSection(LPCRITICAL_SECTION ioSection)
{
    pthread_mutex_t* theMutex = MutexOf(ioSection);
    if (theMutex) {
        pthread_mutex_destroy(theMutex);
        delete theMutex;
    }
    ioSection->LockSemaphore = NULL;
}

void WINAPI EnterCriticalSection(LPCRITICAL_SECTION ioSection)
{
    pthread_mutex_lock(MutexOf(ioSection));
    ioSection->OwningThread = reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(CurrentThreadID()));
    ++ioSection->RecursionCount;
}

BOOL WINAPI TryEnterCriticalSection(LPCRITICAL_SECTION ioSection)
{
    if (pthread_mutex_trylock(MutexOf(ioSection)) != 0)
        return FALSE;
    ioSection->OwningThread = reinterpret_cast<HANDLE>(static_cast<ULONG_PTR>(CurrentThreadID()));
    ++ioSection->RecursionCount;
    return TRUE;
}

void WINAPI LeaveCriticalSection(LPCRITICAL_SECTION ioSection)
{
    if (--ioSection->RecursionCount == 0)
        ioSection->OwningThread = NULL;
    pthread_mutex_unlock(MutexOf(ioSection));
}

// ---- threads ----
HANDLE WINAPI CreateThread(LPSECURITY_ATTRIBUTES inAttributes, SIZE_T inStackSize, LPTHREAD_START_ROUTINE inStart,
                           LPVOID inArgument, DWORD inFlags, LPDWORD outID)
{
    (void)inAttributes;
    return StartThread(inStackSize, inStart, inArgument, inFlags, outID);
}

uintptr_t _beginthreadex(void* inSecurity, unsigned int inStackSize,
                         unsigned int(__attribute__((stdcall)) * inStart)(void*), void* inArgument,
                         unsigned int inFlags, unsigned int* outThreadID)
{
    (void)inSecurity;
    DWORD theID = 0;
    HANDLE theThread = StartThread(inStackSize, reinterpret_cast<LPTHREAD_START_ROUTINE>(inStart), inArgument,
                                   inFlags, &theID);
    if (outThreadID)
        *outThreadID = theID;
    return reinterpret_cast<uintptr_t>(theThread);
}

void WINAPI ExitThread(DWORD inExitCode)
{
    if (tThread)
        FinishThread(tThread, inExitCode);
    pthread_exit(NULL);
}

void _endthreadex(unsigned int inExitCode)
{
    ExitThread(inExitCode);
}

BOOL WINAPI TerminateThread(HANDLE inThread, DWORD inExitCode)
{
    (void)inThread, (void)inExitCode;
    EE_PORT_UNSUPPORTED("TerminateThread");
    return FALSE;
}

BOOL WINAPI GetExitCodeThread(HANDLE inThread, LPDWORD outExitCode)
{
    KernelObject* theObject = inThread == GetCurrentThread() ? CurrentThreadObject() : ObjectOf(inThread);
    if (!theObject || theObject->mKind != KernelObject::kThread) {
        *outExitCode = STILL_ACTIVE;
        return inThread == GetCurrentThread();
    }
    std::lock_guard<std::mutex> theLock(gWaitLock);
    *outExitCode = static_cast<Thread*>(theObject)->mExitCode;
    return TRUE;
}

DWORD WINAPI ResumeThread(HANDLE inThread)
{
    KernelObject* theObject = ObjectOf(inThread);
    if (!theObject || theObject->mKind != KernelObject::kThread)
        return (DWORD)-1;
    DWORD theWas;
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        theWas = static_cast<Thread*>(theObject)->mSuspended ? 1 : 0;
        static_cast<Thread*>(theObject)->mSuspended = false;
    }
    gWaitChanged.notify_all();
    return theWas;
}

DWORD WINAPI SuspendThread(HANDLE inThread)
{
    (void)inThread;
    EE_PORT_UNSUPPORTED("SuspendThread");
    return (DWORD)-1;
}

BOOL WINAPI SetThreadPriority(HANDLE inThread, int inPriority)
{
    KernelObject* theObject = inThread == GetCurrentThread() ? CurrentThreadObject() : ObjectOf(inThread);
    if (theObject && theObject->mKind == KernelObject::kThread)
        static_cast<Thread*>(theObject)->mPriority = inPriority;
    return TRUE;
}

int WINAPI GetThreadPriority(HANDLE inThread)
{
    KernelObject* theObject = inThread == GetCurrentThread() ? CurrentThreadObject() : ObjectOf(inThread);
    if (theObject && theObject->mKind == KernelObject::kThread)
        return static_cast<Thread*>(theObject)->mPriority;
    return THREAD_PRIORITY_NORMAL;
}

DWORD WINAPI GetCurrentThreadId(void) { return CurrentThreadID(); }

// The pseudo handles of Windows: -1 is the process, -2 the calling thread
HANDLE WINAPI GetCurrentProcess(void) { return reinterpret_cast<HANDLE>(~static_cast<ULONG_PTR>(0)); }
HANDLE WINAPI GetCurrentThread(void) { return reinterpret_cast<HANDLE>(~static_cast<ULONG_PTR>(1)); }

// ---- clocks ----
DWORD WINAPI GetTickCount(void) { return (DWORD)(MonotonicNanoseconds() / 1000000ull); }
DWORD WINAPI timeGetTime(void) { return (DWORD)(MonotonicNanoseconds() / 1000000ull); }
MMRESULT WINAPI timeBeginPeriod(UINT inPeriod) { (void)inPeriod; return TIMERR_NOERROR; }
MMRESULT WINAPI timeEndPeriod(UINT inPeriod) { (void)inPeriod; return TIMERR_NOERROR; }

BOOL WINAPI QueryPerformanceCounter(LARGE_INTEGER* outCount)
{
    outCount->QuadPart = (LONGLONG)MonotonicNanoseconds();
    return TRUE;
}

BOOL WINAPI QueryPerformanceFrequency(LARGE_INTEGER* outFrequency)
{
    outFrequency->QuadPart = 1000000000;
    return TRUE;
}

void WINAPI GetLocalTime(LPSYSTEMTIME outTime)
{
    timespec theNow;
    clock_gettime(CLOCK_REALTIME, &theNow);
    struct tm theLocal;
    localtime_r(&theNow.tv_sec, &theLocal);
    outTime->wYear = (WORD)(theLocal.tm_year + 1900);
    outTime->wMonth = (WORD)(theLocal.tm_mon + 1);
    outTime->wDayOfWeek = (WORD)theLocal.tm_wday;
    outTime->wDay = (WORD)theLocal.tm_mday;
    outTime->wHour = (WORD)theLocal.tm_hour;
    outTime->wMinute = (WORD)theLocal.tm_min;
    outTime->wSecond = (WORD)theLocal.tm_sec;
    outTime->wMilliseconds = (WORD)(theNow.tv_nsec / 1000000);
}

} // extern "C"
