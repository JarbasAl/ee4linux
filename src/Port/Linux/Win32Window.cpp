// The native Linux build's windows and messages on SDL2.
//
// A window (HWND) is an object of this unit; making one makes no native window. A native SDL window appears when the
// window is first drawn to (NativeWindow): the DirectDraw layer asks for an OpenGL one, GDI for a software one. A
// window has no frame here: its window rectangle is its client rectangle, and SDL draws any decoration outside it.
//
// Each thread has one message queue for posted window messages and thread messages. A thread that owns a window makes
// that window's native window, and pumps SDL events into window messages whenever it peeks or waits for a message:
// SDL is entered under gSdlLock, so the game's window threads may take turns. PeekMessage returns, in Windows' order,
// sent messages (handled at once), posted messages, WM_QUIT, WM_PAINT, then WM_TIMER.
#include "Win32Internal.h"

#include <SDL.h>
#include <imm.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <cwchar>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;
using eeport::gSdlLock;
using eeport::gWaitChanged;
using eeport::gWaitLock;

struct WindowClass {
    std::string mName;
    std::string mMenuName;
    WNDCLASSEXA mClass;
};

struct Window {
    WindowClass* mClass = nullptr;
    std::string mTitle;
    DWORD mStyle = 0;
    DWORD mExStyle = 0;
    RECT mRect = {};                    // in screen coordinates; also the client area
    HWND mParent = NULL;                // the parent of a child window, the owner of a top-level one
    LONG_PTR mID = 0;
    HINSTANCE mInstance = NULL;
    WNDPROC mProcedure = nullptr;
    LONG_PTR mUserData = 0;
    DWORD mThread = 0;
    bool mVisible = false;
    bool mMinimized = false;
    bool mInvalid = false;
    bool mErase = false;
    bool mDestroyed = false;
    SDL_Window* mNative = nullptr;
    bool mNativeGL = false;
};

struct SentMessage {
    MSG mMessage;
    LRESULT mResult = 0;
    bool mDone = false;
};

struct Timer {
    HWND mWindow;
    UINT_PTR mID;
    UINT mInterval;
    TIMERPROC mProcedure;
    Clock::time_point mDue;
};

// Under gWaitLock
struct ThreadQueue {
    std::deque<MSG> mPosted;
    std::deque<SentMessage*> mSent;
    std::vector<Timer> mTimers;
    bool mQuit = false;
    int mQuitCode = 0;
};

std::map<DWORD, ThreadQueue> gQueues;           // under gWaitLock
std::recursive_mutex gWindowsLock;
std::set<Window*> gWindows;                     // under gWindowsLock
std::map<std::string, WindowClass> gClasses;    // lower-case names, under gWindowsLock
Window gDesktop;
bool gVideoStarted = false;
bool gAppActive = false;
HWND gFocus = NULL;
HWND gCapture = NULL;

// Moves the keyboard focus, which DirectInput's foreground devices follow
void SetFocusWindow(HWND inWindow)
{
    gFocus = inWindow;
    eeport::InputFocus(inWindow);
}

// The mouse position in screen coordinates, kept from SDL events by the thread that pumps, so that any thread reads it
// without calling into the windowing system
std::atomic<LONG> gCursorX{0}, gCursorY{0};

// A cursor handle points to one of these. The SDL cursor is made under gSdlLock when the cursor is first shown.
struct Cursor {
    SDL_SystemCursor mSystem = SDL_SYSTEM_CURSOR_ARROW;
    SDL_Surface* mImage = nullptr;       // a made cursor; NULL for a system cursor
    int mHotX = 0, mHotY = 0;
    SDL_Cursor* mNative = nullptr;
};
std::mutex gCursorLock;
Cursor* gCursor = nullptr;              // under gCursorLock
int gCursorCount = 0;                   // ShowCursor's display count, under gCursorLock
bool gCursorChanged = true;             // under gCursorLock

std::string Lower(const char* inText)
{
    std::string theText = inText ? inText : "";
    for (char& c : theText)
        c = (char)tolower((unsigned char)c);
    return theText;
}

Window* WindowOf(HWND inWindow)
{
    std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
    if (inWindow == (HWND)&gDesktop)
        return &gDesktop;
    Window* theWindow = (Window*)inWindow;
    return gWindows.count(theWindow) && !theWindow->mDestroyed ? theWindow : nullptr;
}

// Under gWaitLock
ThreadQueue& QueueOf(DWORD inThread)
{
    return gQueues[inThread];
}

// True when the thread owns a window: such a thread pumps SDL events when it peeks or waits for a message, so it
// wakes often enough to do that
bool PumpsEvents(DWORD inThread)
{
    std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
    for (const Window* theWindow : gWindows)
        if (!theWindow->mDestroyed && theWindow->mThread == inThread)
            return true;
    return false;
}

void Post(DWORD inThread, HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    MSG theMessage = {inWindow, inMessage, inWParam, inLParam, GetTickCount(), {gCursorX, gCursorY}};
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        QueueOf(inThread).mPosted.push_back(theMessage);
    }
    gWaitChanged.notify_all();
}

void PostToWindow(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    if (Window* theWindow = WindowOf(inWindow))
        Post(theWindow->mThread, inWindow, inMessage, inWParam, inLParam);
}

DWORD WindowStyleForSDL(const Window* inWindow)
{
    DWORD theFlags = SDL_WINDOW_HIDDEN;
    if (inWindow->mStyle & WS_POPUP || !(inWindow->mStyle & WS_CAPTION))
        theFlags |= SDL_WINDOW_BORDERLESS;
    if (inWindow->mStyle & WS_THICKFRAME)
        theFlags |= SDL_WINDOW_RESIZABLE;
    if (inWindow->mExStyle & WS_EX_TOPMOST)
        theFlags |= SDL_WINDOW_ALWAYS_ON_TOP;
    if (inWindow->mExStyle & WS_EX_TOOLWINDOW)
        theFlags |= SDL_WINDOW_SKIP_TASKBAR;
    return theFlags;
}

// Brings a window's native SDL window in line with its rectangle, title and visibility
void SyncNative(Window* inWindow)
{
    std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
    if (!inWindow->mNative)
        return;
    SDL_SetWindowTitle(inWindow->mNative, inWindow->mTitle.c_str());
    SDL_SetWindowPosition(inWindow->mNative, inWindow->mRect.left, inWindow->mRect.top);
    SDL_SetWindowSize(inWindow->mNative, std::max(1L, inWindow->mRect.right - inWindow->mRect.left),
                      std::max(1L, inWindow->mRect.bottom - inWindow->mRect.top));
    if (inWindow->mVisible && !inWindow->mMinimized) {
        SDL_ShowWindow(inWindow->mNative);
        SDL_RaiseWindow(inWindow->mNative);
    } else if (!inWindow->mVisible)
        SDL_HideWindow(inWindow->mNative);
}

LPARAM PointParam(int inX, int inY)
{
    return MAKELPARAM((WORD)(SHORT)inX, (WORD)(SHORT)inY);
}

WPARAM MouseKeys()
{
    Uint32 theButtons = SDL_GetMouseState(NULL, NULL);
    SDL_Keymod theModifiers = SDL_GetModState();
    return (theButtons & SDL_BUTTON_LMASK ? MK_LBUTTON : 0) | (theButtons & SDL_BUTTON_RMASK ? MK_RBUTTON : 0) |
           (theButtons & SDL_BUTTON_MMASK ? MK_MBUTTON : 0) | (theModifiers & KMOD_SHIFT ? MK_SHIFT : 0) |
           (theModifiers & KMOD_CTRL ? MK_CONTROL : 0);
}

void SetAppActive(bool inActive)
{
    if (gAppActive == inActive)
        return;
    gAppActive = inActive;
    std::vector<Window*> theTopLevel;
    {
        std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
        for (Window* theWindow : gWindows)
            if (!theWindow->mDestroyed && !(theWindow->mStyle & WS_CHILD))
                theTopLevel.push_back(theWindow);
    }
    for (Window* theWindow : theTopLevel)
        PostToWindow((HWND)theWindow, WM_ACTIVATEAPP, inActive, 0);
}

// Shows the current cursor, or hides it when the display count is negative or the cursor is NULL
void ApplyCursor()
{
    std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
    std::lock_guard<std::mutex> theLock(gCursorLock);
    if (!gCursorChanged)
        return;
    gCursorChanged = false;
    if (gCursor && !gCursor->mNative)
        gCursor->mNative = gCursor->mImage
                               ? SDL_CreateColorCursor(gCursor->mImage, gCursor->mHotX, gCursor->mHotY)
                               : SDL_CreateSystemCursor(gCursor->mSystem);
    if (gCursor && gCursor->mNative)
        SDL_SetCursor(gCursor->mNative);
    SDL_ShowCursor(gCursor && gCursor->mNative && gCursorCount >= 0 ? SDL_ENABLE : SDL_DISABLE);
}

void CursorChanged()
{
    {
        std::lock_guard<std::mutex> theLock(gCursorLock);
        gCursorChanged = true;
    }
    ApplyCursor();
}

// Turns one SDL event into window messages
void Translate(const SDL_Event& inEvent)
{
    switch (inEvent.type) {
    case SDL_WINDOWEVENT: {
        HWND theWindow = eeport::WindowOfNative(SDL_GetWindowFromID(inEvent.window.windowID));
        Window* theObject = WindowOf(theWindow);
        if (!theObject)
            return;
        switch (inEvent.window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            PostToWindow(theWindow, WM_CLOSE, 0, 0);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            SetAppActive(true);
            SetFocusWindow(theWindow);
            PostToWindow(theWindow, WM_ACTIVATE, WA_ACTIVE, 0);
            PostToWindow(theWindow, WM_SETFOCUS, 0, 0);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            PostToWindow(theWindow, WM_ACTIVATE, WA_INACTIVE, 0);
            PostToWindow(theWindow, WM_KILLFOCUS, 0, 0);
            if (gFocus == theWindow)
                SetFocusWindow(NULL);
            break;
        case SDL_WINDOWEVENT_MINIMIZED:
            theObject->mMinimized = true;
            PostToWindow(theWindow, WM_SIZE, SIZE_MINIMIZED, 0);
            break;
        case SDL_WINDOWEVENT_RESTORED:
        case SDL_WINDOWEVENT_MAXIMIZED: {
            theObject->mMinimized = false;
            RECT& theRect = theObject->mRect;
            PostToWindow(theWindow, WM_SIZE,
                         inEvent.window.event == SDL_WINDOWEVENT_MAXIMIZED ? SIZE_MAXIMIZED : SIZE_RESTORED,
                         PointParam(theRect.right - theRect.left, theRect.bottom - theRect.top));
            break;
        }
        case SDL_WINDOWEVENT_MOVED: {
            RECT& theRect = theObject->mRect;
            OffsetRect(&theRect, inEvent.window.data1 - theRect.left, inEvent.window.data2 - theRect.top);
            PostToWindow(theWindow, WM_MOVE, 0, PointParam(theRect.left, theRect.top));
            break;
        }
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
            RECT& theRect = theObject->mRect;
            theRect.right = theRect.left + inEvent.window.data1;
            theRect.bottom = theRect.top + inEvent.window.data2;
            PostToWindow(theWindow, WM_SIZE, SIZE_RESTORED, PointParam(inEvent.window.data1, inEvent.window.data2));
            InvalidateRect(theWindow, NULL, TRUE);
            break;
        }
        case SDL_WINDOWEVENT_EXPOSED:
            InvalidateRect(theWindow, NULL, FALSE);
            break;
        }
        break;
    }
    case SDL_KEYDOWN:
    case SDL_KEYUP: {
        BYTE theKey, theScan;
        bool theExtended;
        if (!gFocus || !eeport::KeyOf(inEvent.key.keysym.scancode, &theKey, &theScan, &theExtended))
            return;
        bool theDown = inEvent.type == SDL_KEYDOWN;
        bool theAlt = (inEvent.key.keysym.mod & KMOD_ALT) != 0;
        UINT theMessage = theAlt || theKey == VK_F10 ? (theDown ? WM_SYSKEYDOWN : WM_SYSKEYUP)
                                                       : (theDown ? WM_KEYDOWN : WM_KEYUP);
        LPARAM theFlags = 1 | (theScan << 16) | (theExtended ? 1 << 24 : 0) | (theAlt ? 1 << 29 : 0) |
                          (theDown ? (inEvent.key.repeat ? 1 << 30 : 0) : (3u << 30));
        eeport::InputKey(BYTE(theScan | (theExtended ? 0x80 : 0)), theDown);
        PostToWindow(gFocus, theMessage, theKey, theFlags);
        break;
    }
    case SDL_TEXTINPUT: {
        if (!gFocus)
            return;
        char theText[sizeof inEvent.text.text];
        WCHAR theWide[sizeof inEvent.text.text];
        int theCount = MultiByteToWideChar(CP_UTF8, 0, inEvent.text.text, -1, theWide, sizeof inEvent.text.text);
        theCount = WideCharToMultiByte(CP_ACP, 0, theWide, theCount, theText, sizeof theText, "?", nullptr);
        for (int i = 0; i < theCount && theText[i]; ++i)
            PostToWindow(gFocus, WM_CHAR, (unsigned char)theText[i], 1);
        break;
    }
    case SDL_MOUSEMOTION:
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEWHEEL: {
        if (inEvent.type == SDL_MOUSEMOTION) {
            eeport::InputMouseMove(inEvent.motion.xrel, inEvent.motion.yrel);
        } else if (inEvent.type == SDL_MOUSEWHEEL) {
            eeport::InputMouseWheel(inEvent.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -inEvent.wheel.y
                                                                                      : inEvent.wheel.y);
        } else {
            // DirectInput's buttons 0 to 3 are the left, right, middle and first extra buttons
            static const int kButtons[] = {-1, 0, 2, 1, 3};
            if (inEvent.button.button < sizeof kButtons / sizeof kButtons[0])
                eeport::InputMouseButton(kButtons[inEvent.button.button], inEvent.type == SDL_MOUSEBUTTONDOWN);
        }
        // the window gets no mouse messages while DirectInput holds the mouse
        if (eeport::MouseHeld())
            return;
        HWND theWindow = gCapture ? gCapture
                                  : eeport::WindowOfNative(SDL_GetWindowFromID(inEvent.type == SDL_MOUSEWHEEL
                                                                                    ? inEvent.wheel.windowID
                                                                                    : inEvent.motion.windowID));
        Window* theObject = WindowOf(theWindow);
        if (!theObject)
            return;
        if (inEvent.type == SDL_MOUSEWHEEL) {
            PostToWindow(theWindow, WM_MOUSEWHEEL, MAKEWPARAM(MouseKeys(), (WORD)(SHORT)(inEvent.wheel.y * WHEEL_DELTA)),
                         PointParam(gCursorX, gCursorY));
            return;
        }
        // The event's position is relative to the native window it happened in, which is not the capture window
        HWND theNativeOwner = eeport::WindowOfNative(SDL_GetWindowFromID(inEvent.motion.windowID));
        Window* theOrigin = WindowOf(theNativeOwner) ? WindowOf(theNativeOwner) : theObject;
        int theEventX = inEvent.type == SDL_MOUSEMOTION ? inEvent.motion.x : inEvent.button.x;
        int theEventY = inEvent.type == SDL_MOUSEMOTION ? inEvent.motion.y : inEvent.button.y;
        gCursorX = theOrigin->mRect.left + theEventX;
        gCursorY = theOrigin->mRect.top + theEventY;
        LPARAM thePoint = PointParam(gCursorX - theObject->mRect.left, gCursorY - theObject->mRect.top);
        if (inEvent.type == SDL_MOUSEMOTION) {
            PostToWindow(theWindow, WM_MOUSEMOVE, MouseKeys(), thePoint);
            return;
        }
        static const UINT kMessages[3][3] = {{WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK},
                                             {WM_MBUTTONDOWN, WM_MBUTTONUP, WM_MBUTTONDBLCLK},
                                             {WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK}};
        int theButton = inEvent.button.button - SDL_BUTTON_LEFT;
        if (theButton < 0 || theButton > 2)
            return;
        bool theDouble = inEvent.type == SDL_MOUSEBUTTONDOWN && inEvent.button.clicks == 2 &&
                         (theObject->mClass->mClass.style & CS_DBLCLKS);
        int theKind = inEvent.type == SDL_MOUSEBUTTONUP ? 1 : theDouble ? 2 : 0;
        PostToWindow(theWindow, kMessages[theButton][theKind], MouseKeys(), thePoint);
        break;
    }
    }
}

// Turns the pending SDL events into window messages, for the thread that pumps them
void Pump()
{
    std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
    if (!gVideoStarted)
        return;
    SDL_Event theEvent;
    while (SDL_PollEvent(&theEvent))
        Translate(theEvent);
    eeport::ApplyMouseMode();
    ApplyCursor();
    eeport::PresentWindows();
    if (!gFocus)
        SetAppActive(false);    // focus moved from one window of the game to another within the events above
}

bool Matches(const MSG& inMessage, HWND inWindow, UINT inFirst, UINT inLast)
{
    if (inWindow == (HWND)-1 ? inMessage.hwnd != NULL : inWindow && inMessage.hwnd != inWindow)
        return false;
    return (inFirst == 0 && inLast == 0) || (inMessage.message >= inFirst && inMessage.message <= inLast);
}

LRESULT CallProcedure(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || !theWindow->mProcedure)
        return 0;
    return theWindow->mProcedure(inWindow, inMessage, inWParam, inLParam);
}

// Under gWaitLock, released while each sent message runs: handles the messages other threads sent to this one
void HandleSent(std::unique_lock<std::mutex>& ioLock, ThreadQueue& ioQueue)
{
    while (!ioQueue.mSent.empty()) {
        SentMessage* theSent = ioQueue.mSent.front();
        ioQueue.mSent.pop_front();
        ioLock.unlock();
        LRESULT theResult = CallProcedure(theSent->mMessage.hwnd, theSent->mMessage.message, theSent->mMessage.wParam,
                                          theSent->mMessage.lParam);
        ioLock.lock();
        theSent->mResult = theResult;
        theSent->mDone = true;
        gWaitChanged.notify_all();
    }
}

// Takes the next message for the filter; false when there is none
bool NextMessage(LPMSG outMessage, HWND inWindow, UINT inFirst, UINT inLast, bool inRemove)
{
    Pump();
    DWORD theThread = eeport::CurrentThreadID();
    std::unique_lock<std::mutex> theLock(gWaitLock);
    ThreadQueue& theQueue = QueueOf(theThread);
    HandleSent(theLock, theQueue);
    for (auto it = theQueue.mPosted.begin(); it != theQueue.mPosted.end(); ++it)
        if (Matches(*it, inWindow, inFirst, inLast)) {
            *outMessage = *it;
            if (inRemove)
                theQueue.mPosted.erase(it);
            return true;
        }
    if (theQueue.mQuit && ((inFirst == 0 && inLast == 0) || (WM_QUIT >= inFirst && WM_QUIT <= inLast))) {
        *outMessage = {NULL, WM_QUIT, (WPARAM)theQueue.mQuitCode, 0, GetTickCount(), {0, 0}};
        if (inRemove)
            theQueue.mQuit = false;
        return true;
    }
    theLock.unlock();
    MSG thePaint = {NULL, WM_PAINT, 0, 0, GetTickCount(), {0, 0}};
    {
        std::lock_guard<std::recursive_mutex> theWindowsLock(gWindowsLock);
        for (Window* theWindow : gWindows)
            if (theWindow->mInvalid && !theWindow->mDestroyed && theWindow->mThread == theThread) {
                thePaint.hwnd = (HWND)theWindow;
                if (Matches(thePaint, inWindow, inFirst, inLast)) {
                    *outMessage = thePaint;
                    return true;
                }
            }
    }
    theLock.lock();
    auto theNow = Clock::now();
    for (Timer& theTimer : theQueue.mTimers)
        if (theTimer.mDue <= theNow) {
            MSG theMessage = {theTimer.mWindow, WM_TIMER, theTimer.mID, (LPARAM)theTimer.mProcedure, GetTickCount(),
                              {0, 0}};
            if (!Matches(theMessage, inWindow, inFirst, inLast))
                continue;
            if (inRemove)
                theTimer.mDue = theNow + std::chrono::milliseconds(theTimer.mInterval);
            *outMessage = theMessage;
            return true;
        }
    return false;
}

// Under gWaitLock: the time to wait for this thread's next timer, at most inLongest
Clock::duration WaitTime(DWORD inThread, Clock::duration inLongest)
{
    auto theNow = Clock::now();
    Clock::duration theWait = inLongest;
    for (const Timer& theTimer : QueueOf(inThread).mTimers)
        theWait = std::min(theWait, std::max(Clock::duration::zero(), theTimer.mDue - theNow));
    return theWait;
}

} // namespace

namespace eeport {

std::recursive_mutex gSdlLock;

bool StartVideo()
{
    std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
    if (gVideoStarted)
        return true;
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
        Log("SDL video does not start: %s", SDL_GetError());
        return false;
    }
    gVideoStarted = true;
    SDL_Rect theBounds = {0, 0, 1024, 768};
    SDL_GetDisplayBounds(0, &theBounds);
    SetRect(&gDesktop.mRect, theBounds.x, theBounds.y, theBounds.x + theBounds.w, theBounds.y + theBounds.h);
    SDL_StartTextInput();
    return true;
}

SDL_Window* NativeWindow(HWND inWindow, bool inOpenGL)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop || !StartVideo() || theWindow->mThread != CurrentThreadID())
        return nullptr;
    std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
    if (theWindow->mNative && theWindow->mNativeGL && !inOpenGL)
        return nullptr;
    if (theWindow->mNative && theWindow->mNativeGL != inOpenGL) {
        SDL_DestroyWindow(theWindow->mNative);
        theWindow->mNative = nullptr;
    }
    if (!theWindow->mNative) {
        RECT& theRect = theWindow->mRect;
        theWindow->mNative = SDL_CreateWindow(theWindow->mTitle.c_str(), theRect.left, theRect.top,
                                              std::max(1L, theRect.right - theRect.left),
                                              std::max(1L, theRect.bottom - theRect.top),
                                              WindowStyleForSDL(theWindow) | (inOpenGL ? SDL_WINDOW_OPENGL : 0));
        if (!theWindow->mNative) {
            Log("SDL window \"%s\" is not made: %s", theWindow->mTitle.c_str(), SDL_GetError());
            return nullptr;
        }
        theWindow->mNativeGL = inOpenGL;
        SDL_SetWindowData(theWindow->mNative, "hwnd", theWindow);
        SyncNative(theWindow);
    }
    return theWindow->mNative;
}

HWND WindowOfNative(SDL_Window* inNative)
{
    return inNative ? (HWND)SDL_GetWindowData(inNative, "hwnd") : NULL;
}

bool TakePaint(HWND inWindow, bool* outErase)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop)
        return false;
    *outErase = theWindow->mErase;
    theWindow->mErase = false;
    theWindow->mInvalid = false;
    return true;
}

HCURSOR MakeCursor(SDL_Surface* inImage, int inHotX, int inHotY)
{
    Cursor* theCursor = new Cursor;
    theCursor->mImage = inImage;
    theCursor->mHotX = inHotX;
    theCursor->mHotY = inHotY;
    return (HCURSOR)theCursor;
}

} // namespace eeport

// ---- classes ----
ATOM WINAPI RegisterClassExA(const WNDCLASSEXA* inClass)
{
    std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
    std::string theKey = Lower(inClass->lpszClassName);
    if (gClasses.count(theKey)) {
        SetLastError(ERROR_CLASS_ALREADY_EXISTS);
        return 0;
    }
    WindowClass& theClass = gClasses[theKey];
    theClass.mName = inClass->lpszClassName;
    theClass.mClass = *inClass;
    theClass.mClass.lpszClassName = theClass.mName.c_str();
    if (IS_INTRESOURCE(inClass->lpszMenuName))
        theClass.mClass.lpszMenuName = inClass->lpszMenuName;
    else {
        theClass.mMenuName = inClass->lpszMenuName ? inClass->lpszMenuName : "";
        theClass.mClass.lpszMenuName = inClass->lpszMenuName ? theClass.mMenuName.c_str() : NULL;
    }
    return (ATOM)(0xc000 + gClasses.size());
}

ATOM WINAPI RegisterClassA(const WNDCLASSA* inClass)
{
    WNDCLASSEXA theClass = {sizeof theClass, inClass->style, inClass->lpfnWndProc, inClass->cbClsExtra,
                            inClass->cbWndExtra, inClass->hInstance, inClass->hIcon, inClass->hCursor,
                            inClass->hbrBackground, inClass->lpszMenuName, inClass->lpszClassName, NULL};
    return RegisterClassExA(&theClass);
}

BOOL WINAPI UnregisterClassA(LPCSTR inName, HINSTANCE)
{
    std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
    return gClasses.erase(Lower(inName)) ? TRUE : FALSE;
}

BOOL WINAPI GetClassInfoExA(HINSTANCE, LPCSTR inName, WNDCLASSEXA* outClass)
{
    std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
    auto theEntry = gClasses.find(Lower(inName));
    if (theEntry == gClasses.end()) {
        SetLastError(ERROR_CLASS_DOES_NOT_EXIST);
        return FALSE;
    }
    *outClass = theEntry->second.mClass;
    return TRUE;
}

BOOL WINAPI GetClassInfoA(HINSTANCE inInstance, LPCSTR inName, WNDCLASSA* outClass)
{
    WNDCLASSEXA theClass;
    if (!GetClassInfoExA(inInstance, inName, &theClass))
        return FALSE;
    *outClass = {theClass.style, theClass.lpfnWndProc, theClass.cbClsExtra, theClass.cbWndExtra, theClass.hInstance,
                 theClass.hIcon, theClass.hCursor, theClass.hbrBackground, theClass.lpszMenuName,
                 theClass.lpszClassName};
    return TRUE;
}

INT WINAPI GetClassNameA(HWND inWindow, LPSTR outName, INT inSize)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || inSize <= 0)
        return 0;
    const char* theName = theWindow == &gDesktop ? "#32769" : theWindow->mClass->mName.c_str();
    lstrcpynA(outName, theName, inSize);
    return (INT)strlen(outName);
}

// ---- windows ----
HWND WINAPI CreateWindowExA(DWORD inExStyle, LPCSTR inClassName, LPCSTR inTitle, DWORD inStyle, INT inX, INT inY,
                            INT inWidth, INT inHeight, HWND inParent, HMENU inMenu, HINSTANCE inInstance,
                            LPVOID inParameter)
{
    Window* theWindow = new Window;
    {
        std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
        auto theClass = gClasses.find(IS_INTRESOURCE(inClassName) ? std::string() : Lower(inClassName));
        if (theClass == gClasses.end()) {
            delete theWindow;
            SetLastError(ERROR_CANNOT_FIND_WND_CLASS);
            return NULL;
        }
        theWindow->mClass = &theClass->second;
        gWindows.insert(theWindow);
    }
    eeport::StartVideo();
    if (inX == CW_USEDEFAULT)
        inX = inY = 0;
    if (inWidth == CW_USEDEFAULT) {
        inWidth = 640;
        inHeight = 480;
    }
    theWindow->mTitle = inTitle ? inTitle : "";
    theWindow->mStyle = inStyle;
    theWindow->mExStyle = inExStyle;
    theWindow->mParent = inParent;
    theWindow->mID = (LONG_PTR)inMenu;
    theWindow->mInstance = inInstance;
    theWindow->mProcedure = theWindow->mClass->mClass.lpfnWndProc;
    theWindow->mThread = eeport::CurrentThreadID();
    SetRect(&theWindow->mRect, inX, inY, inX + inWidth, inY + inHeight);
    if ((inStyle & WS_CHILD) && WindowOf(inParent))
        OffsetRect(&theWindow->mRect, WindowOf(inParent)->mRect.left, WindowOf(inParent)->mRect.top);
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        QueueOf(theWindow->mThread);
    }
    HWND theHandle = (HWND)theWindow;
    CREATESTRUCTA theCreate = {inParameter, inInstance, inMenu, inParent, inHeight, inWidth, inY, inX,
                               (LONG)inStyle, inTitle, inClassName, inExStyle};
    if (!CallProcedure(theHandle, WM_NCCREATE, 0, (LPARAM)&theCreate) ||
        CallProcedure(theHandle, WM_CREATE, 0, (LPARAM)&theCreate) == -1) {
        DestroyWindow(theHandle);
        return NULL;
    }
    CallProcedure(theHandle, WM_SIZE, SIZE_RESTORED, PointParam(inWidth, inHeight));
    CallProcedure(theHandle, WM_MOVE, 0, PointParam(theWindow->mRect.left, theWindow->mRect.top));
    if (inStyle & WS_VISIBLE) {
        theWindow->mStyle &= ~WS_VISIBLE;
        ShowWindow(theHandle, SW_SHOW);
    }
    return theHandle;
}

BOOL WINAPI DestroyWindow(HWND inWindow)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop)
        return FALSE;
    CallProcedure(inWindow, WM_DESTROY, 0, 0);
    CallProcedure(inWindow, WM_NCDESTROY, 0, 0);
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        ThreadQueue& theQueue = QueueOf(theWindow->mThread);
        theQueue.mPosted.erase(std::remove_if(theQueue.mPosted.begin(), theQueue.mPosted.end(),
                                              [inWindow](const MSG& m) { return m.hwnd == inWindow; }),
                               theQueue.mPosted.end());
        theQueue.mTimers.erase(std::remove_if(theQueue.mTimers.begin(), theQueue.mTimers.end(),
                                              [inWindow](const Timer& t) { return t.mWindow == inWindow; }),
                               theQueue.mTimers.end());
    }
    eeport::ForgetWindowSurface(inWindow);
    HWND theNext = NULL;
    {
        std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
        std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
        if (theWindow->mNative)
            SDL_DestroyWindow(theWindow->mNative);
        theWindow->mNative = nullptr;
        theWindow->mDestroyed = true;    // the object stays, so that a stale handle is never a new window
        if (gCapture == inWindow)
            gCapture = NULL;
        if (gFocus != inWindow)
            return TRUE;
        SetFocusWindow(NULL);
        // As on Windows, the latest visible top-level window of the thread becomes active
        for (Window* theOther : gWindows)
            if (!theOther->mDestroyed && theOther->mVisible && !(theOther->mStyle & WS_CHILD) &&
                theOther->mThread == theWindow->mThread && (!theNext || theOther > (Window*)theNext))
                theNext = (HWND)theOther;
    }
    if (theNext) {
        SetFocusWindow(theNext);
        CallProcedure(theNext, WM_ACTIVATE, WA_ACTIVE, 0);
        CallProcedure(theNext, WM_SETFOCUS, 0, 0);
    }
    return TRUE;
}

BOOL WINAPI ShowWindow(HWND inWindow, INT inCommand)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    bool theWasVisible = theWindow->mVisible;
    switch (inCommand) {
    case SW_HIDE:
        theWindow->mVisible = false;
        theWindow->mStyle &= ~WS_VISIBLE;
        break;
    case SW_MINIMIZE:
    case SW_SHOWMINIMIZED:
    case SW_SHOWMINNOACTIVE:
        theWindow->mVisible = true;
        theWindow->mMinimized = true;
        theWindow->mStyle |= WS_VISIBLE;
        if (theWindow->mNative) {
            std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
            SDL_MinimizeWindow(theWindow->mNative);
        }
        break;
    default:
        theWindow->mVisible = true;
        theWindow->mStyle |= WS_VISIBLE;
        if (theWindow->mMinimized && theWindow->mNative) {
            std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
            SDL_RestoreWindow(theWindow->mNative);
        }
        theWindow->mMinimized = false;
        break;
    }
    if (theWindow->mVisible != theWasVisible) {
        CallProcedure(inWindow, WM_SHOWWINDOW, theWindow->mVisible, 0);
        if (theWindow->mVisible)
            InvalidateRect(inWindow, NULL, TRUE);
    }
    SyncNative(theWindow);
    if (theWindow->mVisible && inCommand != SW_SHOWNA && inCommand != SW_SHOWNOACTIVATE &&
        inCommand != SW_SHOWMINNOACTIVE && !(theWindow->mStyle & WS_CHILD) && gFocus != inWindow) {
        if (!gAppActive && !theWindow->mNative)
            SetAppActive(true);
        if (!theWindow->mNative) {
            SetFocusWindow(inWindow);
            CallProcedure(inWindow, WM_ACTIVATE, WA_ACTIVE, 0);
            CallProcedure(inWindow, WM_SETFOCUS, 0, 0);
        }
    }
    return theWasVisible;
}

BOOL WINAPI CloseWindow(HWND inWindow)
{
    return WindowOf(inWindow) ? (ShowWindow(inWindow, SW_MINIMIZE), TRUE) : FALSE;
}

BOOL WINAPI IsWindow(HWND inWindow)
{
    return WindowOf(inWindow) != nullptr;
}

BOOL WINAPI IsWindowVisible(HWND inWindow)
{
    Window* theWindow = WindowOf(inWindow);
    return theWindow && (theWindow == &gDesktop || theWindow->mVisible);
}

BOOL WINAPI SetWindowPos(HWND inWindow, HWND, INT inX, INT inY, INT inWidth, INT inHeight, UINT inFlags)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop)
        return FALSE;
    RECT& theRect = theWindow->mRect;
    RECT theOld = theRect;
    if (!(inFlags & SWP_NOMOVE))
        OffsetRect(&theRect, inX - theRect.left, inY - theRect.top);
    if (!(inFlags & SWP_NOSIZE)) {
        theRect.right = theRect.left + inWidth;
        theRect.bottom = theRect.top + inHeight;
    }
    if (inFlags & SWP_SHOWWINDOW)
        ShowWindow(inWindow, SW_SHOWNA);
    if (inFlags & SWP_HIDEWINDOW)
        ShowWindow(inWindow, SW_HIDE);
    SyncNative(theWindow);
    if (theRect.left != theOld.left || theRect.top != theOld.top)
        CallProcedure(inWindow, WM_MOVE, 0, PointParam(theRect.left, theRect.top));
    if (theRect.right - theRect.left != theOld.right - theOld.left ||
        theRect.bottom - theRect.top != theOld.bottom - theOld.top) {
        CallProcedure(inWindow, WM_SIZE, SIZE_RESTORED, PointParam(theRect.right - theRect.left, theRect.bottom - theRect.top));
        InvalidateRect(inWindow, NULL, TRUE);
    }
    return TRUE;
}

BOOL WINAPI MoveWindow(HWND inWindow, INT inX, INT inY, INT inWidth, INT inHeight, BOOL inRepaint)
{
    BOOL theResult = SetWindowPos(inWindow, NULL, inX, inY, inWidth, inHeight, SWP_NOZORDER | SWP_NOACTIVATE);
    if (theResult && inRepaint)
        InvalidateRect(inWindow, NULL, TRUE);
    return theResult;
}

// A window has no frame here, so its window rectangle is its client rectangle
BOOL WINAPI AdjustWindowRect(LPRECT, DWORD, BOOL)
{
    return TRUE;
}

BOOL WINAPI GetWindowRect(HWND inWindow, LPRECT outRect)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    *outRect = theWindow->mRect;
    return TRUE;
}

BOOL WINAPI GetClientRect(HWND inWindow, LPRECT outRect)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    SetRect(outRect, 0, 0, theWindow->mRect.right - theWindow->mRect.left, theWindow->mRect.bottom - theWindow->mRect.top);
    return TRUE;
}

BOOL WINAPI ClientToScreen(HWND inWindow, LPPOINT ioPoint)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    ioPoint->x += theWindow->mRect.left;
    ioPoint->y += theWindow->mRect.top;
    return TRUE;
}

BOOL WINAPI ScreenToClient(HWND inWindow, LPPOINT ioPoint)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    ioPoint->x -= theWindow->mRect.left;
    ioPoint->y -= theWindow->mRect.top;
    return TRUE;
}

BOOL WINAPI GetWindowPlacement(HWND inWindow, WINDOWPLACEMENT* outPlacement)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    outPlacement->length = sizeof *outPlacement;
    outPlacement->flags = 0;
    outPlacement->showCmd = !theWindow->mVisible ? SW_HIDE : theWindow->mMinimized ? SW_SHOWMINIMIZED : SW_SHOWNORMAL;
    outPlacement->ptMinPosition = {-1, -1};
    outPlacement->ptMaxPosition = {-1, -1};
    outPlacement->rcNormalPosition = theWindow->mRect;
    return TRUE;
}

HWND WINAPI GetDesktopWindow(void)
{
    eeport::StartVideo();
    return (HWND)&gDesktop;
}

HWND WINAPI GetParent(HWND inWindow)
{
    Window* theWindow = WindowOf(inWindow);
    return theWindow ? theWindow->mParent : NULL;
}

// The top-level visible window at a screen point, the latest made first
HWND WINAPI WindowFromPoint(POINT inPoint)
{
    std::lock_guard<std::recursive_mutex> theLock(gWindowsLock);
    Window* theFound = nullptr;
    for (Window* theWindow : gWindows)
        if (!theWindow->mDestroyed && theWindow->mVisible && !theWindow->mMinimized && PtInRect(&theWindow->mRect, inPoint) &&
            (!theFound || theWindow > theFound))
            theFound = theWindow;
    return theFound ? (HWND)theFound : (HWND)&gDesktop;
}

HWND WINAPI SetFocus(HWND inWindow)
{
    HWND thePrevious = gFocus;
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || inWindow == thePrevious)
        return thePrevious;
    if (WindowOf(thePrevious))
        CallProcedure(thePrevious, WM_KILLFOCUS, (WPARAM)inWindow, 0);
    SetFocusWindow(inWindow);
    if (theWindow->mNative) {
        std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
        SDL_RaiseWindow(theWindow->mNative);
    }
    CallProcedure(inWindow, WM_SETFOCUS, (WPARAM)thePrevious, 0);
    return thePrevious;
}

HWND WINAPI GetFocus(void)
{
    return gFocus;
}

HWND WINAPI SetCapture(HWND inWindow)
{
    HWND thePrevious = gCapture;
    gCapture = WindowOf(inWindow) ? inWindow : NULL;
    {
        std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
        SDL_CaptureMouse(gCapture ? SDL_TRUE : SDL_FALSE);
    }
    return thePrevious;
}

BOOL WINAPI ReleaseCapture(void)
{
    gCapture = NULL;
    {
        std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
        SDL_CaptureMouse(SDL_FALSE);
    }
    return TRUE;
}

INT WINAPI GetWindowTextA(HWND inWindow, LPSTR outText, INT inSize)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || inSize <= 0)
        return 0;
    lstrcpynA(outText, theWindow->mTitle.c_str(), inSize);
    return (INT)strlen(outText);
}

BOOL WINAPI SetWindowTextA(HWND inWindow, LPCSTR inText)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    theWindow->mTitle = inText ? inText : "";
    if (theWindow->mNative) {
        std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
        SDL_SetWindowTitle(theWindow->mNative, theWindow->mTitle.c_str());
    }
    return TRUE;
}

LONG WINAPI GetWindowLongA(HWND inWindow, INT inIndex)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return 0;
    switch (inIndex) {
    case GWL_WNDPROC: return (LONG)(LONG_PTR)theWindow->mProcedure;
    case GWL_HINSTANCE: return (LONG)(LONG_PTR)theWindow->mInstance;
    case GWL_ID: return (LONG)theWindow->mID;
    case GWL_STYLE: return (LONG)theWindow->mStyle;
    case GWL_EXSTYLE: return (LONG)theWindow->mExStyle;
    case GWL_USERDATA: return (LONG)theWindow->mUserData;
    }
    SetLastError(ERROR_INVALID_INDEX);
    return 0;
}

LONG WINAPI SetWindowLongA(HWND inWindow, INT inIndex, LONG inValue)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return 0;
    LONG thePrevious = GetWindowLongA(inWindow, inIndex);
    switch (inIndex) {
    case GWL_WNDPROC: theWindow->mProcedure = (WNDPROC)(LONG_PTR)inValue; break;
    case GWL_HINSTANCE: theWindow->mInstance = (HINSTANCE)(LONG_PTR)inValue; break;
    case GWL_ID: theWindow->mID = inValue; break;
    case GWL_STYLE: theWindow->mStyle = (DWORD)inValue; break;
    case GWL_EXSTYLE: theWindow->mExStyle = (DWORD)inValue; break;
    case GWL_USERDATA: theWindow->mUserData = inValue; break;
    default:
        SetLastError(ERROR_INVALID_INDEX);
        return 0;
    }
    return thePrevious;
}

// ---- painting ----
BOOL WINAPI InvalidateRect(HWND inWindow, const RECT*, BOOL inErase)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop)
        return inWindow == NULL || theWindow == &gDesktop;
    theWindow->mInvalid = true;
    theWindow->mErase = theWindow->mErase || inErase;
    gWaitChanged.notify_all();
    return TRUE;
}

BOOL WINAPI ValidateRect(HWND inWindow, const RECT*)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    theWindow->mInvalid = false;
    return TRUE;
}

// Invalidates as asked; RDW_UPDATENOW paints at once. The desktop has nothing to repaint.
BOOL WINAPI RedrawWindow(HWND inWindow, const RECT* inRect, HRGN, UINT inFlags)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop)
        return theWindow != nullptr;
    if (inFlags & RDW_INVALIDATE)
        InvalidateRect(inWindow, inRect, (inFlags & RDW_ERASE) != 0);
    if (inFlags & RDW_VALIDATE)
        ValidateRect(inWindow, inRect);
    if ((inFlags & RDW_UPDATENOW) && theWindow->mInvalid)
        CallProcedure(inWindow, WM_PAINT, 0, 0);
    return TRUE;
}

BOOL WINAPI UpdateWindow(HWND inWindow)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow)
        return FALSE;
    if (theWindow->mInvalid)
        CallProcedure(inWindow, WM_PAINT, 0, 0);
    return TRUE;
}

// ---- messages ----
BOOL WINAPI PostMessageA(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    if (inWindow == NULL) {
        Post(eeport::CurrentThreadID(), NULL, inMessage, inWParam, inLParam);
        return TRUE;
    }
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop) {
        SetLastError(ERROR_INVALID_WINDOW_HANDLE);
        return FALSE;
    }
    Post(theWindow->mThread, inWindow, inMessage, inWParam, inLParam);
    return TRUE;
}

// A thread has a queue once it has called a message function or made a window, as on Windows; TSThread::Post
// retries until the receiving thread has one
BOOL WINAPI PostThreadMessageA(DWORD inThread, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        if (!gQueues.count(inThread)) {
            SetLastError(ERROR_INVALID_THREAD_ID);
            return FALSE;
        }
    }
    Post(inThread, NULL, inMessage, inWParam, inLParam);
    return TRUE;
}

// A message to a window of this thread runs at once; one to another thread's window waits until that thread takes
// it in a message call
LRESULT WINAPI SendMessageA(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    Window* theWindow = WindowOf(inWindow);
    if (!theWindow || theWindow == &gDesktop)
        return 0;
    DWORD theThread = eeport::CurrentThreadID();
    if (theWindow->mThread == theThread)
        return CallProcedure(inWindow, inMessage, inWParam, inLParam);
    SentMessage theSent;
    theSent.mMessage = {inWindow, inMessage, inWParam, inLParam, GetTickCount(), {0, 0}};
    std::unique_lock<std::mutex> theLock(gWaitLock);
    QueueOf(theWindow->mThread).mSent.push_back(&theSent);
    gWaitChanged.notify_all();
    while (!theSent.mDone) {
        HandleSent(theLock, QueueOf(theThread));    // two threads sending to each other do not deadlock
        if (!theSent.mDone)
            gWaitChanged.wait_for(theLock, std::chrono::milliseconds(10));
    }
    return theSent.mResult;
}

void WINAPI PostQuitMessage(INT inCode)
{
    {
        std::lock_guard<std::mutex> theLock(gWaitLock);
        ThreadQueue& theQueue = QueueOf(eeport::CurrentThreadID());
        theQueue.mQuit = true;
        theQueue.mQuitCode = inCode;
    }
    gWaitChanged.notify_all();
}

BOOL WINAPI PeekMessageA(LPMSG outMessage, HWND inWindow, UINT inFirst, UINT inLast, UINT inFlags)
{
    return NextMessage(outMessage, inWindow, inFirst, inLast, (inFlags & PM_REMOVE) != 0);
}

// Waits for a message; FALSE for WM_QUIT. A thread that owns a window wakes every 5 ms to pump SDL events.
BOOL WINAPI GetMessageA(LPMSG outMessage, HWND inWindow, UINT inFirst, UINT inLast)
{
    DWORD theThread = eeport::CurrentThreadID();
    while (!NextMessage(outMessage, inWindow, inFirst, inLast, true)) {
        std::unique_lock<std::mutex> theLock(gWaitLock);
        gWaitChanged.wait_for(theLock, WaitTime(theThread, PumpsEvents(theThread) ? std::chrono::milliseconds(5)
                                                                            : std::chrono::milliseconds(50)));
    }
    return outMessage->message != WM_QUIT;
}

// The text of a key comes from SDL's text input, as WM_CHAR, so TranslateMessage adds only the characters SDL does
// not give as text: Backspace, Tab, Enter and Escape, and Ctrl with a letter
BOOL WINAPI TranslateMessage(const MSG* inMessage)
{
    if (inMessage->message != WM_KEYDOWN)
        return FALSE;
    WPARAM theKey = inMessage->wParam, theChar = 0;
    if (theKey == VK_BACK || theKey == VK_TAB || theKey == VK_RETURN || theKey == VK_ESCAPE)
        theChar = theKey == VK_RETURN ? '\r' : theKey;
    else if (theKey >= 'A' && theKey <= 'Z' && eeport::IsKeyDown(VK_CONTROL) && !eeport::IsKeyDown(VK_MENU))
        theChar = theKey - 'A' + 1;
    if (!theChar)
        return FALSE;
    PostMessageA(inMessage->hwnd, WM_CHAR, theChar, inMessage->lParam);
    return TRUE;
}

LRESULT WINAPI DispatchMessageA(const MSG* inMessage)
{
    if (inMessage->message == WM_TIMER && inMessage->lParam) {
        ((TIMERPROC)inMessage->lParam)(inMessage->hwnd, WM_TIMER, inMessage->wParam, GetTickCount());
        return 0;
    }
    if (!inMessage->hwnd)
        return 0;
    return CallProcedure(inMessage->hwnd, inMessage->message, inMessage->wParam, inMessage->lParam);
}

LRESULT WINAPI DefWindowProcA(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    switch (inMessage) {
    case WM_NCCREATE:
        return TRUE;
    case WM_CLOSE:
        DestroyWindow(inWindow);
        return 0;
    case WM_PAINT:
        ValidateRect(inWindow, NULL);
        return 0;
    case WM_ERASEBKGND: {
        Window* theWindow = WindowOf(inWindow);
        HBRUSH theBrush = theWindow && theWindow->mClass ? theWindow->mClass->mClass.hbrBackground : NULL;
        if (!theBrush)
            return 0;
        RECT theRect;
        GetClientRect(inWindow, &theRect);
        FillRect((HDC)inWParam, &theRect, theBrush);
        return 1;
    }
    case WM_MOUSEACTIVATE:
        return MA_ACTIVATE;
    case WM_SETCURSOR:
        if (Window* theWindow = WindowOf(inWindow))
            SetCursor(theWindow->mClass ? theWindow->mClass->mClass.hCursor : NULL);
        return TRUE;
    case WM_SYSCOMMAND:
        switch (inWParam & 0xfff0) {
        case SC_CLOSE:
            PostMessageA(inWindow, WM_CLOSE, 0, 0);
            break;
        case SC_MINIMIZE:
            ShowWindow(inWindow, SW_MINIMIZE);
            break;
        case SC_RESTORE:
            ShowWindow(inWindow, SW_RESTORE);
            break;
        }
        return 0;
    case WM_SYSKEYDOWN:
        if (inWParam == VK_F4 && (inLParam & (1 << 29)))
            PostMessageA(inWindow, WM_CLOSE, 0, 0);
        return 0;
    }
    return 0;
}

// ---- timers ----
UINT_PTR WINAPI SetTimer(HWND inWindow, UINT_PTR inID, UINT inInterval, TIMERPROC inProcedure)
{
    Window* theWindow = inWindow ? WindowOf(inWindow) : nullptr;
    if (inWindow && !theWindow)
        return 0;
    DWORD theThread = theWindow ? theWindow->mThread : eeport::CurrentThreadID();
    std::lock_guard<std::mutex> theLock(gWaitLock);
    ThreadQueue& theQueue = QueueOf(theThread);
    if (!inWindow) {
        static UINT_PTR sNextID = 0x7f00;
        inID = ++sNextID;
    }
    Timer theTimer = {inWindow, inID, std::max(inInterval, 10u), inProcedure,
                      Clock::now() + std::chrono::milliseconds(std::max(inInterval, 10u))};
    for (Timer& theOld : theQueue.mTimers)
        if (theOld.mWindow == inWindow && theOld.mID == inID) {
            theOld = theTimer;
            return inWindow ? 1 : inID;
        }
    theQueue.mTimers.push_back(theTimer);
    gWaitChanged.notify_all();
    return inWindow ? 1 : inID;
}

BOOL WINAPI KillTimer(HWND inWindow, UINT_PTR inID)
{
    DWORD theThread = inWindow && WindowOf(inWindow) ? WindowOf(inWindow)->mThread : eeport::CurrentThreadID();
    std::lock_guard<std::mutex> theLock(gWaitLock);
    ThreadQueue& theQueue = QueueOf(theThread);
    auto theEnd = std::remove_if(theQueue.mTimers.begin(), theQueue.mTimers.end(),
                                 [&](const Timer& t) { return t.mWindow == inWindow && t.mID == inID; });
    bool theFound = theEnd != theQueue.mTimers.end();
    theQueue.mTimers.erase(theEnd, theQueue.mTimers.end());
    return theFound;
}

// ---- hooks: the game's WH_CALLWNDPROC hook has no effect on Windows either (a global hook without a module) ----
HHOOK WINAPI SetWindowsHookExA(INT, HOOKPROC, HINSTANCE, DWORD)
{
    SetLastError(ERROR_HOOK_NEEDS_HMOD);
    return NULL;
}

BOOL WINAPI UnhookWindowsHookEx(HHOOK)
{
    return TRUE;
}

LRESULT WINAPI CallNextHookEx(HHOOK, INT, WPARAM, LPARAM)
{
    return 0;
}

// ---- screen ----
INT WINAPI GetSystemMetrics(INT inIndex)
{
    eeport::StartVideo();
    switch (inIndex) {
    case SM_CXSCREEN:
    case SM_CXMAXIMIZED:
    case SM_CXFULLSCREEN:
        return gDesktop.mRect.right - gDesktop.mRect.left;
    case SM_CYSCREEN:
    case SM_CYMAXIMIZED:
    case SM_CYFULLSCREEN:
        return gDesktop.mRect.bottom - gDesktop.mRect.top;
    case SM_CXCURSOR:
    case SM_CYCURSOR:
    case SM_CXICON:
    case SM_CYICON:
        return 32;
    case SM_CXSMICON:
    case SM_CYSMICON:
        return 16;
    case SM_CXDOUBLECLK:
    case SM_CYDOUBLECLK:
        return 4;
    case SM_CMOUSEBUTTONS:
        return 3;
    case SM_MOUSEPRESENT:
        return 1;
    }
    return 0;
}

// Windows' classic colour scheme
DWORD WINAPI GetSysColor(INT inIndex)
{
    switch (inIndex) {
    case COLOR_WINDOW: case COLOR_HIGHLIGHTTEXT: case COLOR_BTNHIGHLIGHT: case COLOR_CAPTIONTEXT:
        return RGB(255, 255, 255);
    case COLOR_WINDOWTEXT: case COLOR_BTNTEXT: case COLOR_MENUTEXT: case COLOR_INFOTEXT: case COLOR_WINDOWFRAME:
    case COLOR_3DDKSHADOW:
        return RGB(0, 0, 0);
    case COLOR_HIGHLIGHT: case COLOR_ACTIVECAPTION:
        return RGB(0, 0, 128);
    case COLOR_BTNSHADOW: case COLOR_GRAYTEXT: case COLOR_INACTIVECAPTION:
        return RGB(128, 128, 128);
    case COLOR_INFOBK:
        return RGB(255, 255, 225);
    case COLOR_BACKGROUND:
        return RGB(0, 128, 128);
    case COLOR_INACTIVECAPTIONTEXT:
        return RGB(192, 192, 192);
    }
    return RGB(192, 192, 192);      // COLOR_BTNFACE, COLOR_MENU, COLOR_SCROLLBAR and the rest
}

// The game changes display modes through DirectDraw; WON's calls to these never run
LONG WINAPI ChangeDisplaySettingsA(LPDEVMODEA, DWORD)
{
    return DISP_CHANGE_FAILED;
}

BOOL WINAPI EnumDisplaySettingsA(LPCSTR, DWORD, LPDEVMODEA)
{
    return FALSE;
}

// ---- message boxes: SDL's own, which work before any window exists ----
INT WINAPI MessageBoxA(HWND, LPCSTR inText, LPCSTR inCaption, UINT inType)
{
    eeport::Log("message box \"%s\": %s", inCaption ? inCaption : "", inText ? inText : "");
    Uint32 theFlags = (inType & MB_ICONMASK) == MB_ICONHAND ? SDL_MESSAGEBOX_ERROR
                    : (inType & MB_ICONMASK) == MB_ICONEXCLAMATION ? SDL_MESSAGEBOX_WARNING : SDL_MESSAGEBOX_INFORMATION;
    if (getenv("EE_PORT_NO_MESSAGE_BOXES"))
        return IDOK;
    SDL_ShowSimpleMessageBox(theFlags, inCaption ? inCaption : "", inText ? inText : "", NULL);
    return IDOK;
}

// ---- rectangles ----
BOOL WINAPI SetRect(LPRECT outRect, INT inLeft, INT inTop, INT inRight, INT inBottom)
{
    if (!outRect)
        return FALSE;
    *outRect = {inLeft, inTop, inRight, inBottom};
    return TRUE;
}

BOOL WINAPI OffsetRect(LPRECT ioRect, INT inX, INT inY)
{
    if (!ioRect)
        return FALSE;
    ioRect->left += inX;
    ioRect->right += inX;
    ioRect->top += inY;
    ioRect->bottom += inY;
    return TRUE;
}

BOOL WINAPI IsRectEmpty(const RECT* inRect)
{
    return !inRect || inRect->right <= inRect->left || inRect->bottom <= inRect->top;
}

// FALSE and an empty rectangle when the two do not overlap
BOOL WINAPI IntersectRect(LPRECT outRect, const RECT* inFirst, const RECT* inSecond)
{
    RECT theRect = {std::max(inFirst->left, inSecond->left), std::max(inFirst->top, inSecond->top),
                    std::min(inFirst->right, inSecond->right), std::min(inFirst->bottom, inSecond->bottom)};
    if (IsRectEmpty(&theRect))
        SetRect(&theRect, 0, 0, 0, 0);
    *outRect = theRect;
    return !IsRectEmpty(&theRect);
}

BOOL WINAPI InflateRect(LPRECT ioRect, INT inX, INT inY)
{
    if (!ioRect)
        return FALSE;
    ioRect->left -= inX;
    ioRect->right += inX;
    ioRect->top -= inY;
    ioRect->bottom += inY;
    return TRUE;
}

// The right and bottom edges are outside the rectangle
BOOL WINAPI PtInRect(const RECT* inRect, POINT inPoint)
{
    return inRect && inPoint.x >= inRect->left && inPoint.x < inRect->right && inPoint.y >= inRect->top &&
           inPoint.y < inRect->bottom;
}

// ---- cursors ----
// A standard cursor maps to SDL's system cursor. A cursor resource of the game is drawn as the arrow: the game hides
// the system cursor over its window and draws its own.
HCURSOR WINAPI LoadCursorA(HINSTANCE inInstance, LPCSTR inName)
{
    static const struct {
        ULONG_PTR mID;
        SDL_SystemCursor mSystem;
    } kStandard[] = {
        {32512, SDL_SYSTEM_CURSOR_ARROW},    {32513, SDL_SYSTEM_CURSOR_IBEAM},   {32514, SDL_SYSTEM_CURSOR_WAIT},
        {32515, SDL_SYSTEM_CURSOR_CROSSHAIR}, {32642, SDL_SYSTEM_CURSOR_SIZENWSE}, {32643, SDL_SYSTEM_CURSOR_SIZENESW},
        {32644, SDL_SYSTEM_CURSOR_SIZEWE},   {32645, SDL_SYSTEM_CURSOR_SIZENS},  {32646, SDL_SYSTEM_CURSOR_SIZEALL},
        {32648, SDL_SYSTEM_CURSOR_NO},       {32649, SDL_SYSTEM_CURSOR_HAND},   {32650, SDL_SYSTEM_CURSOR_WAITARROW},
    };
    static std::map<std::pair<HINSTANCE, ULONG_PTR>, Cursor*> sLoaded;   // LoadCursor returns a shared cursor
    std::lock_guard<std::mutex> theLock(gCursorLock);
    ULONG_PTR theID = IS_INTRESOURCE(inName) ? (ULONG_PTR)inName : (ULONG_PTR)std::hash<std::string>()(inName);
    Cursor*& theCursor = sLoaded[{inInstance, theID}];
    if (!theCursor) {
        theCursor = new Cursor;
        if (!inInstance)
            for (const auto& theStandard : kStandard)
                if (theStandard.mID == theID)
                    theCursor->mSystem = theStandard.mSystem;
    }
    return (HCURSOR)theCursor;
}

HICON WINAPI LoadIconA(HINSTANCE, LPCSTR)
{
    static Cursor sIcon;
    return (HICON)&sIcon;
}

// A monochrome cursor: AND 1 and XOR 0 is transparent, AND 0 gives black or white, and AND 1 with XOR 1 (inverted
// screen) is drawn black
HCURSOR WINAPI CreateCursor(HINSTANCE, INT inHotX, INT inHotY, INT inWidth, INT inHeight, LPCVOID inAndMask,
                            LPCVOID inXorMask)
{
    std::unique_lock<std::recursive_mutex> theLock(gSdlLock);
    SDL_Surface* theImage = SDL_CreateRGBSurfaceWithFormat(0, inWidth, inHeight, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!theImage)
        return NULL;
    int theStride = ((inWidth + 15) / 16) * 2;     // mask rows are WORD aligned
    for (int y = 0; y < inHeight; ++y)
        for (int x = 0; x < inWidth; ++x) {
            int theBit = 0x80 >> (x & 7);
            bool theAnd = ((const BYTE*)inAndMask)[y * theStride + x / 8] & theBit;
            bool theXor = ((const BYTE*)inXorMask)[y * theStride + x / 8] & theBit;
            Uint32 thePixel = theAnd && !theXor ? 0x00000000 : !theAnd && theXor ? 0xffffffff : 0xff000000;
            ((Uint32*)((Uint8*)theImage->pixels + y * theImage->pitch))[x] = thePixel;
        }
    return eeport::MakeCursor(theImage, inHotX, inHotY);
}

BOOL WINAPI DestroyCursor(HCURSOR inCursor)
{
    Cursor* theCursor = (Cursor*)inCursor;
    if (!theCursor || !theCursor->mImage)
        return theCursor != nullptr;    // a loaded cursor is shared and stays
    {
        std::lock_guard<std::mutex> theLock(gCursorLock);
        if (gCursor == theCursor) {
            gCursor = nullptr;
            gCursorChanged = true;
        }
    }
    {
        std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
        SDL_FreeCursor(theCursor->mNative);
        SDL_FreeSurface(theCursor->mImage);
        delete theCursor;
    }
    return TRUE;
}

HCURSOR WINAPI SetCursor(HCURSOR inCursor)
{
    Cursor* thePrevious;
    {
        std::lock_guard<std::mutex> theLock(gCursorLock);
        thePrevious = gCursor;
        if (thePrevious == (Cursor*)inCursor)
            return inCursor;
        gCursor = (Cursor*)inCursor;
    }
    CursorChanged();
    return (HCURSOR)thePrevious;
}

// The display count starts at 0; the cursor shows while the count is 0 or more
INT WINAPI ShowCursor(BOOL inShow)
{
    int theCount;
    {
        std::lock_guard<std::mutex> theLock(gCursorLock);
        theCount = gCursorCount += inShow ? 1 : -1;
    }
    CursorChanged();
    return theCount;
}

BOOL WINAPI GetCursorPos(LPPOINT outPoint)
{
    if (!outPoint)
        return FALSE;
    outPoint->x = gCursorX;
    outPoint->y = gCursorY;
    return TRUE;
}

BOOL WINAPI SetCursorPos(INT inX, INT inY)
{
    gCursorX = inX;
    gCursorY = inY;
    {
        std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
        SDL_WarpMouseGlobal(inX, inY);
    }
    return TRUE;
}

// ---- clipboard: text only, through SDL's clipboard in UTF-8 ----
// CF_TEXT is code page 1252. CF_UNICODETEXT is wchar_t text, 32 bits here: only the game reads what it writes.
namespace {
std::mutex gClipboardLock;
std::vector<HGLOBAL> gClipboardBlocks;      // blocks the clipboard owns, freed when it is emptied
}

BOOL WINAPI OpenClipboard(HWND)
{
    return eeport::StartVideo();
}

BOOL WINAPI CloseClipboard(void)
{
    return TRUE;
}

BOOL WINAPI EmptyClipboard(void)
{
    std::lock_guard<std::mutex> theLock(gClipboardLock);
    for (HGLOBAL theBlock : gClipboardBlocks)
        GlobalFree(theBlock);
    gClipboardBlocks.clear();
    return TRUE;
}

BOOL WINAPI IsClipboardFormatAvailable(UINT inFormat)
{
    if (inFormat != CF_TEXT && inFormat != CF_UNICODETEXT)
        return FALSE;
    std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
    return eeport::StartVideo() && SDL_HasClipboardText();
}

HANDLE WINAPI SetClipboardData(UINT inFormat, HANDLE inData)
{
    if ((inFormat != CF_TEXT && inFormat != CF_UNICODETEXT) || !inData)
        return NULL;
    size_t theSize = GlobalSize(inData);
    std::string theText;
    if (inFormat == CF_TEXT) {
        std::string theAnsi((const char*)inData, strnlen((const char*)inData, theSize));
        std::vector<wchar_t> theWide(theAnsi.size() + 1);
        int theCount = MultiByteToWideChar(CP_ACP, 0, theAnsi.c_str(), -1, theWide.data(), (int)theWide.size());
        theText.resize(theCount * 4);
        theText.resize(std::max(0, WideCharToMultiByte(CP_UTF8, 0, theWide.data(), -1, &theText[0],
                                                       (int)theText.size(), nullptr, nullptr) - 1));
    } else {
        std::wstring theWide((const wchar_t*)inData, wcsnlen((const wchar_t*)inData, theSize / sizeof(wchar_t)));
        theText.resize(theWide.size() * 4 + 1);
        theText.resize(std::max(0, WideCharToMultiByte(CP_UTF8, 0, theWide.c_str(), -1, &theText[0],
                                                       (int)theText.size(), nullptr, nullptr) - 1));
    }
    {
        std::lock_guard<std::recursive_mutex> theLock(gSdlLock);
        if (!eeport::StartVideo() || SDL_SetClipboardText(theText.c_str()) != 0)
            return NULL;
    }
    std::lock_guard<std::mutex> theLock(gClipboardLock);
    gClipboardBlocks.push_back(inData);
    return inData;
}

// The text in a block the clipboard owns until it is next emptied
HANDLE WINAPI GetClipboardData(UINT inFormat)
{
    if (inFormat != CF_TEXT && inFormat != CF_UNICODETEXT)
        return NULL;
    std::lock_guard<std::recursive_mutex> theSdlLock(gSdlLock);
    if (!eeport::StartVideo() || !SDL_HasClipboardText())
        return NULL;
    char* theText = SDL_GetClipboardText();
    std::vector<wchar_t> theWide(strlen(theText) + 1);
    int theCount = MultiByteToWideChar(CP_UTF8, 0, theText, -1, theWide.data(), (int)theWide.size());
    SDL_free(theText);
    HGLOBAL theBlock;
    if (inFormat == CF_UNICODETEXT) {
        theBlock = GlobalAlloc(GMEM_FIXED, theCount * sizeof(wchar_t));
        memcpy(theBlock, theWide.data(), theCount * sizeof(wchar_t));
    } else {
        theBlock = GlobalAlloc(GMEM_FIXED, theCount);
        WideCharToMultiByte(CP_ACP, 0, theWide.data(), -1, (char*)theBlock, theCount, "?", nullptr);
    }
    std::lock_guard<std::mutex> theLock(gClipboardLock);
    gClipboardBlocks.push_back(theBlock);
    return theBlock;
}

// ---- input method: none; the game's IME code runs only on IME messages, which this layer never sends ----
HIMC WINAPI ImmGetContext(HWND)
{
    return NULL;
}

BOOL WINAPI ImmReleaseContext(HWND, HIMC)
{
    return TRUE;
}

LONG WINAPI ImmGetCompositionStringA(HIMC, DWORD, LPVOID, DWORD)
{
    return IMM_ERROR_GENERAL;
}

LONG WINAPI ImmGetCompositionStringW(HIMC, DWORD, LPVOID, DWORD)
{
    return IMM_ERROR_GENERAL;
}

DWORD WINAPI ImmGetCandidateListA(HIMC, DWORD, LPCANDIDATELIST, DWORD)
{
    return 0;
}

BOOL WINAPI ImmNotifyIME(HIMC, DWORD, DWORD, DWORD)
{
    return FALSE;
}

BOOL WINAPI ImmSetOpenStatus(HIMC, BOOL)
{
    return FALSE;
}
