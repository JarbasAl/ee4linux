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

struct SDL_Surface;
struct SDL_Window;

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

// ---- keys (Win32Keys.cpp) ----
// The virtual-key code, the PC scan code (set 1) and the extended flag of an SDL scancode; false for a key the game
// has no code for
bool KeyOf(int inScancode, BYTE* outVirtualKey, BYTE* outScanCode, bool* outExtended);
// Whether a virtual key is down now
bool IsKeyDown(int inVirtualKey);

// ---- windows and messages (Win32Window.cpp) ----
// Starts SDL video. The calling thread becomes the one that pumps SDL events into window messages: the native main
// calls it first; otherwise the first thread to need video does.
bool StartVideo();
// The native SDL window of a window, made on first use: with OpenGL for the DirectDraw layer, or with a software
// surface for GDI. An OpenGL request replaces a software window; a software request on an OpenGL window gets NULL,
// as the DirectDraw layer owns what it shows. NULL also for a window owned by a thread other than the video
// thread, or without video.
SDL_Window* NativeWindow(HWND inWindow, bool inOpenGL);
// The window of a native SDL window, or NULL
HWND WindowOfNative(SDL_Window* inNative);
// For BeginPaint: validates the window and says whether its background needs erasing; false for no window
bool TakePaint(HWND inWindow, bool* outErase);
// A cursor or icon from an ARGB8888 image, which the cursor then owns
HCURSOR MakeCursor(SDL_Surface* inImage, int inHotX, int inHotY);

// ---- GDI (Win32Gdi.cpp) ----
// A DC that draws into pixels the caller keeps: the DirectDraw layer's surfaces. inPitch is the bytes from one row
// to the next; 16-bit pixels take their layout from the masks. DeleteDC releases the DC, not the pixels.
HDC CreateBufferDC(void* inBits, int inWidth, int inHeight, int inPitch, int inBitCount, DWORD inRedMask,
                   DWORD inGreenMask, DWORD inBlueMask);
// True when GDI wrote pixels of a CreateBufferDC DC since the last call; outRect bounds them
bool TakeBufferDrawn(HDC inDC, RECT* outRect);
// On the video thread: shows what GDI drew into windows since the last call
void PresentWindows();
// Releases the pixels GDI keeps for a destroyed window
void ForgetWindowSurface(HWND inWindow);

// ---- DirectInput (DirectInput.cpp) ----
// The video thread passes SDL's input. A key is its DIK code: the PC scan code, plus 0x80 for an extended key.
void InputKey(BYTE inCode, bool inDown);
void InputMouseMove(int inX, int inY);
void InputMouseWheel(int inNotches);
// Buttons 0 to 3: left, right, middle, first extra
void InputMouseButton(int inButton, bool inDown);
// The keyboard focus moved to a window, or to none of the game's
void InputFocus(HWND inFocus);
// Whether DirectInput holds the mouse exclusively: the window then gets no mouse messages
bool MouseHeld();
// On the video thread: puts SDL into relative mouse mode while DirectInput holds the mouse
void ApplyMouseMode();

// ---- Miles (Miles.cpp) ----
// For tests: Miles drivers opened after this call open no audio device and mix only through MixMiles
void MilesWithoutDevice();
// Mixes inFrames stereo frames of a Miles driver's playing samples into outSamples, at the driver's rate
void MixMiles(void* inDriver, int16_t* outSamples, int inFrames);
// The music the Miles mixer adds to the first driver opened: DirectMusic's synthesizer. It adds inFrames stereo
// frames at inRate into ioStereo, interleaved, at full scale 1, on the audio thread under the mixer's lock.
using MusicSource = void (*)(float* ioStereo, int inFrames, int inRate);
void SetMusicSource(MusicSource inSource);
// The device rate of the driver the music plays through, or 0 while no driver is open
int MusicRate();

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
