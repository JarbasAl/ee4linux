# Threads, kernel objects and waits

`src/Port/Linux/Win32Sync.cpp`

## Kernel objects

A `HANDLE` is a pointer to a `KernelObject` with a reference count. The kinds are events, mutexes, threads, files,
mappings, finds and timers. A null handle, `INVALID_HANDLE_VALUE` and the two pseudo handles are not objects.

Waitable state changes only under `gWaitLock`, and every change notifies `gWaitChanged`. A waiter holds the lock,
tests its handles, and sleeps on the condition variable until a state changes or its deadline passes.

| Windows call | The layer |
| --- | --- |
| `CreateEventA`, `SetEvent`, `ResetEvent` | a manual-reset or auto-reset flag; an auto-reset event resets when a wait takes it |
| `CreateMutexA`, `ReleaseMutex` | an owner and a count, recursive for its owner |
| `WaitForSingleObject`, `WaitForMultipleObjects` | one loop over the handles, wait-for-one or wait-for-all, with a deadline |
| `CreateThread`, `_beginthreadex`, `ResumeThread`, `ExitThread`, `_endthreadex` | a POSIX thread, started suspended when `CREATE_SUSPENDED` is set |
| `GetExitCodeThread`, `SetThreadPriority`, `GetThreadPriority` | the thread's object |
| `InitializeCriticalSection`, `Enter`, `TryEnter`, `Leave`, `Delete` | a recursive POSIX mutex, its pointer kept in `LockSemaphore` |
| `Sleep`, `GetTickCount`, `timeGetTime`, `QueryPerformanceCounter`, `QueryPerformanceFrequency` | POSIX clocks; the counter is nanoseconds |

`CreateMutexA` with a name logs a line and makes an unshared mutex: the layer has no cross-process objects, so the
game's single-instance check cannot see a second copy. `TerminateThread` and `SuspendThread` are not implemented; the
game does not call them.

## Threads and the pseudo handle

`GetCurrentThread()` returns the pseudo handle `-2`, as Windows does, and `GetCurrentThreadId()` returns a number
counted from 1. The first thread to ask is the one the entry point runs on, so the main thread's id is 1.

A thread that the layer starts gets a `Thread` object in its entry function. A thread that it did not start (the main
thread) gets one on first use: `CurrentThreadObject()`. That matters because the game duplicates the pseudo handle
with `DuplicateHandle` during start-up, and the duplicate must refer to a real object.

## Thread ids and message queues

Every thread has one message queue, made when first used. A queue holds posted messages, messages another thread sent
to it, `WM_QUIT`, and its timers.

| Windows call | The layer |
| --- | --- |
| `PostMessageA`, `PostThreadMessageA` | appends to the target thread's queue |
| `SendMessageA` | pushes the message into the target queue and waits for the target thread to run it; a send back to the sending thread runs at once |
| `PeekMessageA`, `GetMessageA` | sent messages first, then posted, then `WM_QUIT`, then `WM_PAINT`, then `WM_TIMER` |
| `DispatchMessageA` | calls the window procedure, with the timer branch for `WM_TIMER` |
| `PostQuitMessage` | sets `WM_QUIT` in the calling thread's queue |

`GetMessageA` sleeps on `gWaitChanged`. A thread that owns a window wakes every 5 ms, because it also pumps SDL
events; any other thread wakes every 50 ms.

## The two locks

| Lock | Guards |
| --- | --- |
| `gWaitLock` (with `gWaitChanged`) | all kernel-object state, every message queue, and the timers |
| `gWindowsLock` | the window list and the window classes |
| `gSdlLock` | every call into SDL video, from any thread |
| `gLock` (in `DirectDrawInternal.h`) | every DirectDraw and Direct3D call, as `DDSCL_MULTITHREADED` asks |
| `gGdiLock` | every GDI object and every device context |

A thread that owns a window makes its native SDL window, pumps SDL events, and shows what GDI drew. Because several
game threads own windows (the start screen's thread, then the game's own), SDL is entered under `gSdlLock` and the
threads take turns: the lock is recursive, so a pumping thread can present inside its own pump.

## Timers

`SetTimer` adds a timer to the calling thread's queue; the queue returns `WM_TIMER` when the deadline passes, and the
thread's wait time is capped by its nearest timer. `timeBeginPeriod` and `timeEndPeriod` succeed and do nothing: the
layer does not raise the system tick rate.

## Floating point and the x87 stack

The game sets the x87 precision per thread (`EESetThreadFloatingPoint`). The native build compiles with
`-mfpmath=387`, as VC6 did, so the same control word has the same effect. PIC builds keep the x87 stack aligned to
8 bytes, which the code does not see.
