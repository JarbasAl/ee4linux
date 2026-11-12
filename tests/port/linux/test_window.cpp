// Tests of the native build's windows and messages (src/Port/Linux/Win32Window.cpp and Win32Keys.cpp) on SDL's
// offscreen video driver: classes, the creation messages, rectangles, posted, sent and thread messages, the order in
// which PeekMessage returns them, timers, the SDL events that become messages, focus, the clipboard and the cursor
// count.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <SDL.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
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

struct Seen {
    HWND mWindow;
    UINT mMessage;
    WPARAM mWParam;
    LPARAM mLParam;
};
static std::vector<Seen> sSeen;
static LRESULT sCreateResult;

static LRESULT CALLBACK Recorder(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    sSeen.push_back({inWindow, inMessage, inWParam, inLParam});
    if (inMessage == WM_CREATE)
        return sCreateResult;
    return DefWindowProcA(inWindow, inMessage, inWParam, inLParam);
}

static LRESULT CALLBACK Subclass(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    if (inMessage == WM_USER + 9)
        return 99;
    return Recorder(inWindow, inMessage, inWParam, inLParam);
}

// The messages a window has seen, in order
static std::vector<UINT> MessagesOf(HWND inWindow)
{
    std::vector<UINT> theMessages;
    for (const Seen& theSeen : sSeen)
        if (theSeen.mWindow == inWindow)
            theMessages.push_back(theSeen.mMessage);
    return theMessages;
}

// Takes and dispatches every message that is waiting
static void Drain()
{
    MSG theMessage;
    for (int i = 0; i < 1000 && PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE); ++i) {
        TranslateMessage(&theMessage);
        DispatchMessageA(&theMessage);
    }
}

// The first waiting message for a window other than WM_PAINT, removed; message 0 when there is none
static MSG Next(HWND inWindow)
{
    MSG theMessage = {};
    while (PeekMessageA(&theMessage, inWindow, 0, 0, PM_REMOVE)) {
        if (theMessage.message != WM_PAINT)
            return theMessage;
        ValidateRect(theMessage.hwnd, NULL);
    }
    return MSG{};
}

static HWND Make(const char* inClass, DWORD inStyle, int inX, int inY, int inWidth, int inHeight, HWND inParent = NULL)
{
    return CreateWindowExA(0, inClass, "title", inStyle, inX, inY, inWidth, inHeight, inParent, NULL, NULL, NULL);
}

static void TestClasses()
{
    WNDCLASSEXA theClass = {sizeof theClass};
    theClass.style = CS_DBLCLKS;
    theClass.lpfnWndProc = Recorder;
    theClass.lpszClassName = "Recorder Class";
    CHECK(RegisterClassExA(&theClass) != 0);
    SetLastError(0);
    CHECK(RegisterClassExA(&theClass) == 0 && GetLastError() == ERROR_CLASS_ALREADY_EXISTS);
    WNDCLASSEXA theFound = {sizeof theFound};
    CHECK(GetClassInfoExA(NULL, "RECORDER class", &theFound) && theFound.lpfnWndProc == Recorder &&
          theFound.style == CS_DBLCLKS);
    WNDCLASSA thePlain = {};
    thePlain.lpfnWndProc = Recorder;
    thePlain.lpszClassName = "Plain";
    CHECK(RegisterClassA(&thePlain) != 0);
    SetLastError(0);
    CHECK(Make("No Such Class", 0, 0, 0, 10, 10) == NULL && GetLastError() == ERROR_CANNOT_FIND_WND_CLASS);
    CHECK(UnregisterClassA("plain", NULL) && !GetClassInfoExA(NULL, "Plain", &theFound));
}

static void TestCreation()
{
    sSeen.clear();
    HWND theWindow = Make("Recorder Class", WS_POPUP | WS_VISIBLE, 100, 50, 320, 200);
    CHECK(theWindow != NULL);
    std::vector<UINT> theMessages = MessagesOf(theWindow);
    // Windows' order for a visible top-level window, up to the activation
    std::vector<UINT> kStart = {WM_NCCREATE, WM_CREATE, WM_SIZE, WM_MOVE, WM_SHOWWINDOW};
    CHECK(theMessages.size() >= kStart.size() &&
          std::equal(kStart.begin(), kStart.end(), theMessages.begin()));
    CHECK(sSeen[2].mLParam == MAKELPARAM(320, 200) && sSeen[3].mLParam == MAKELPARAM(100, 50));
    CHECK(GetFocus() == theWindow);
    char theName[64];
    CHECK(GetClassNameA(theWindow, theName, sizeof theName) == 14 && strcmp(theName, "Recorder Class") == 0);
    CHECK(GetWindowTextA(theWindow, theName, sizeof theName) == 5 && strcmp(theName, "title") == 0);
    CHECK(IsWindowVisible(theWindow));

    RECT theRect;
    CHECK(GetWindowRect(theWindow, &theRect) && theRect.left == 100 && theRect.top == 50 && theRect.right == 420 &&
          theRect.bottom == 250);
    CHECK(GetClientRect(theWindow, &theRect) && theRect.left == 0 && theRect.right == 320 && theRect.bottom == 200);
    POINT thePoint = {5, 6};
    CHECK(ClientToScreen(theWindow, &thePoint) && thePoint.x == 105 && thePoint.y == 56);
    CHECK(ScreenToClient(theWindow, &thePoint) && thePoint.x == 5 && thePoint.y == 6);
    sSeen.clear();
    CHECK(SetWindowPos(theWindow, NULL, 10, 20, 640, 480, SWP_NOZORDER));
    CHECK(MessagesOf(theWindow) == std::vector<UINT>({WM_MOVE, WM_SIZE}));
    CHECK(sSeen.size() == 2 && sSeen[1].mLParam == MAKELPARAM(640, 480));
    sSeen.clear();
    CHECK(SetWindowPos(theWindow, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE));
    CHECK(sSeen.empty());

    CHECK(SetWindowLongA(theWindow, GWL_USERDATA, 1234) == 0 && GetWindowLongA(theWindow, GWL_USERDATA) == 1234);
    CHECK(GetWindowLongA(theWindow, GWL_STYLE) == (LONG)(WS_POPUP | WS_VISIBLE));
    CHECK(SetWindowLongA(theWindow, GWL_WNDPROC, (LONG)(LONG_PTR)Subclass) == (LONG)(LONG_PTR)Recorder);
    CHECK(SendMessageA(theWindow, WM_USER + 9, 0, 0) == 99);
    SetWindowLongA(theWindow, GWL_WNDPROC, (LONG)(LONG_PTR)Recorder);
    CHECK(SendMessageA(theWindow, WM_USER + 9, 0, 0) == 0);

    // WM_CREATE returning -1 fails the creation, and the window is destroyed
    sSeen.clear();
    sCreateResult = -1;
    CHECK(Make("Recorder Class", WS_POPUP, 0, 0, 10, 10) == NULL);
    sCreateResult = 0;
    CHECK(!sSeen.empty() && sSeen.back().mMessage == WM_NCDESTROY);

    sSeen.clear();
    CHECK(DestroyWindow(theWindow));
    CHECK(MessagesOf(theWindow) == std::vector<UINT>({WM_DESTROY, WM_NCDESTROY}));
    CHECK(!GetWindowRect(theWindow, &theRect) && !DestroyWindow(theWindow) && GetFocus() == NULL);
}

static void TestQueue()
{
    HWND theWindow = Make("Recorder Class", WS_POPUP, 0, 0, 10, 10);
    Drain();
    MSG theMessage;
    CHECK(!PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE));

    // The filter: a range takes the later message first, and PM_NOREMOVE leaves it
    PostMessageA(theWindow, WM_USER + 1, 1, 0);
    PostMessageA(theWindow, WM_USER + 2, 2, 0);
    CHECK(PeekMessageA(&theMessage, theWindow, WM_USER + 2, WM_USER + 2, PM_NOREMOVE) && theMessage.wParam == 2);
    CHECK(PeekMessageA(&theMessage, theWindow, WM_USER + 2, WM_USER + 5, PM_REMOVE) && theMessage.wParam == 2);
    CHECK(PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE) && theMessage.wParam == 1 && theMessage.hwnd == theWindow);
    // hwnd -1 takes thread messages only
    PostMessageA(theWindow, WM_USER + 3, 0, 0);
    PostThreadMessageA(GetCurrentThreadId(), WM_USER + 4, 4, 0);
    CHECK(PeekMessageA(&theMessage, (HWND)-1, 0, 0, PM_REMOVE) && theMessage.message == WM_USER + 4 &&
          theMessage.hwnd == NULL);
    CHECK(PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE) && theMessage.message == WM_USER + 3);

    // Posted messages come before WM_QUIT, WM_QUIT before WM_PAINT; DefWindowProc validates the window
    InvalidateRect(theWindow, NULL, TRUE);
    PostQuitMessage(7);
    PostMessageA(theWindow, WM_USER + 5, 0, 0);
    CHECK(PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE) && theMessage.message == WM_USER + 5);
    CHECK(PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE) && theMessage.message == WM_QUIT &&
          theMessage.wParam == 7);
    CHECK(PeekMessageA(&theMessage, NULL, 0, 0, PM_NOREMOVE) && theMessage.message == WM_PAINT &&
          theMessage.hwnd == theWindow);
    CHECK(PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE) && theMessage.message == WM_PAINT);
    DispatchMessageA(&theMessage);
    CHECK(!PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE));
    DestroyWindow(theWindow);
}

static std::atomic<DWORD> sWorkerThread;
static std::atomic<HWND> sWorkerWindow;
static HWND sMainWindow;
static std::atomic<DWORD> sProcedureThread;

static LRESULT CALLBACK WorkerProcedure(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    if (inMessage == WM_USER + 20) {
        sProcedureThread = GetCurrentThreadId();
        // A send back to the thread that is waiting for this one runs there without a deadlock
        return SendMessageA(sMainWindow, WM_USER + 21, inWParam, 0) + 1;
    }
    return DefWindowProcA(inWindow, inMessage, inWParam, inLParam);
}

static LRESULT CALLBACK MainProcedure(HWND inWindow, UINT inMessage, WPARAM inWParam, LPARAM inLParam)
{
    if (inMessage == WM_USER + 21)
        return inWParam * 10 + (GetCurrentThreadId() == sWorkerThread ? 1000 : 0);
    return DefWindowProcA(inWindow, inMessage, inWParam, inLParam);
}

static void TestThreads()
{
    WNDCLASSA theClass = {};
    theClass.lpfnWndProc = WorkerProcedure;
    theClass.lpszClassName = "Worker";
    RegisterClassA(&theClass);
    theClass.lpfnWndProc = MainProcedure;
    theClass.lpszClassName = "Main";
    RegisterClassA(&theClass);
    sMainWindow = Make("Main", WS_POPUP, 0, 0, 10, 10);

    std::atomic<bool> theReady{false};
    std::atomic<int> theThreadMessage{0};
    std::atomic<long long> theWakeMicroseconds{-1};
    std::chrono::steady_clock::time_point thePosted;
    std::thread theWorker([&] {
        sWorkerThread = GetCurrentThreadId();
        // A thread without a queue refuses thread messages; this check runs before the queue exists
        SetLastError(0);
        BOOL theRefused = !PostThreadMessageA(sWorkerThread, WM_USER + 30, 0, 0) &&
                          GetLastError() == ERROR_INVALID_THREAD_ID;
        sWorkerWindow = Make("Worker", WS_POPUP, 0, 0, 10, 10);
        theThreadMessage = theRefused ? 1 : -1;
        theReady = true;
        MSG theMessage;
        while (GetMessageA(&theMessage, NULL, 0, 0)) {
            if (theMessage.message == WM_USER + 31) {
                theWakeMicroseconds = std::chrono::duration_cast<std::chrono::microseconds>(
                                          std::chrono::steady_clock::now() - thePosted)
                                          .count();
                theThreadMessage = (int)theMessage.wParam;
            }
            DispatchMessageA(&theMessage);
        }
        DestroyWindow(sWorkerWindow);
    });
    while (!theReady)
        std::this_thread::yield();
    CHECK(theThreadMessage == 1);

    // A send to another thread's window runs on that thread and returns its result
    CHECK(SendMessageA(sWorkerWindow, WM_USER + 20, 4, 0) == 41);
    CHECK(sProcedureThread == sWorkerThread);
    // A thread message wakes a thread that waits in GetMessage
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    thePosted = std::chrono::steady_clock::now();
    CHECK(PostThreadMessageA(sWorkerThread, WM_USER + 31, 77, 0));
    for (int i = 0; i < 200 && theThreadMessage != 77; ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    CHECK(theThreadMessage == 77 && theWakeMicroseconds >= 0 && theWakeMicroseconds < 30000);
    PostThreadMessageA(sWorkerThread, WM_QUIT, 0, 0);
    PostMessageA(sWorkerWindow, WM_NULL, 0, 0);
    theWorker.join();
    DestroyWindow(sMainWindow);
}

static std::atomic<int> sTimerCalls;
static void CALLBACK OnTimer(HWND, UINT inMessage, UINT_PTR, DWORD)
{
    if (inMessage == WM_TIMER)
        ++sTimerCalls;
}

static void TestTimers()
{
    HWND theWindow = Make("Recorder Class", WS_POPUP, 0, 0, 10, 10);
    Drain();
    sSeen.clear();
    CHECK(SetTimer(theWindow, 5, 30, NULL) == 1);
    auto theStart = std::chrono::steady_clock::now();
    MSG theMessage;
    CHECK(GetMessageA(&theMessage, theWindow, 0, 0) && theMessage.message == WM_TIMER && theMessage.wParam == 5);
    auto theElapsed = std::chrono::steady_clock::now() - theStart;
    CHECK(theElapsed >= std::chrono::milliseconds(25) && theElapsed < std::chrono::milliseconds(300));
    CHECK(KillTimer(theWindow, 5) && !KillTimer(theWindow, 5));
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    CHECK(!PeekMessageA(&theMessage, theWindow, WM_TIMER, WM_TIMER, PM_REMOVE));

    // A timer without a window gets its own id and calls its procedure through DispatchMessage
    UINT_PTR theID = SetTimer(NULL, 0, 20, OnTimer);
    CHECK(theID != 0);
    while (sTimerCalls < 2 && std::chrono::steady_clock::now() - theStart < std::chrono::seconds(2))
        if (GetMessageA(&theMessage, NULL, 0, 0))
            DispatchMessageA(&theMessage);
    CHECK(sTimerCalls == 2 && KillTimer(NULL, theID));
    DestroyWindow(theWindow);
}

static void TestTranslate()
{
    HWND theWindow = Make("Recorder Class", WS_POPUP, 0, 0, 10, 10);
    Drain();
    MSG theKey = {theWindow, WM_KEYDOWN, VK_RETURN, 0x001c0001};
    CHECK(TranslateMessage(&theKey));
    MSG theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_CHAR && theMessage.wParam == '\r' && theMessage.lParam == 0x001c0001);
    theKey.wParam = 'A';
    CHECK(!TranslateMessage(&theKey));       // SDL text input gives the letter
    SDL_SetModState(KMOD_LCTRL);
    CHECK(TranslateMessage(&theKey) && Next(theWindow).wParam == 1);
    SDL_SetModState(KMOD_NONE);
    CHECK(Next(theWindow).message == 0);
    DestroyWindow(theWindow);
}

static void Push(SDL_Event inEvent)
{
    CHECK(SDL_PushEvent(&inEvent) == 1);
}

static SDL_Event KeyEvent(Uint32 inType, SDL_Scancode inCode, Uint16 inModifiers, Uint32 inWindowID, Uint8 inRepeat = 0)
{
    SDL_Event theEvent = {};
    theEvent.type = inType;
    theEvent.key.windowID = inWindowID;
    theEvent.key.state = inType == SDL_KEYDOWN ? SDL_PRESSED : SDL_RELEASED;
    theEvent.key.repeat = inRepeat;
    theEvent.key.keysym.scancode = inCode;
    theEvent.key.keysym.mod = inModifiers;
    return theEvent;
}

static void TestEvents()
{
    HWND theWindow = Make("Recorder Class", WS_POPUP | WS_VISIBLE, 100, 50, 320, 200);
    SDL_Window* theNative = eeport::NativeWindow(theWindow, false);
    CHECK(theNative != NULL);
    CHECK(eeport::WindowOfNative(theNative) == theWindow);
    CHECK(eeport::NativeWindow(theWindow, false) == theNative);
    Uint32 theID = SDL_GetWindowID(theNative);
    int theX, theY;
    SDL_GetWindowPosition(theNative, &theX, &theY);
    CHECK(theX == 100 && theY == 50);
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    Drain();
    SetFocus(theWindow);

    // Keys: the scan code in bits 16-23, the extended bit 24, Alt bit 29, the previous state bit 30, release bit 31
    Push(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_A, KMOD_NONE, theID));
    MSG theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_KEYDOWN && theMessage.wParam == 'A' && theMessage.lParam == 0x001e0001);
    Push(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_A, KMOD_NONE, theID, 1));
    CHECK(Next(theWindow).lParam == 0x401e0001);
    Push(KeyEvent(SDL_KEYUP, SDL_SCANCODE_A, KMOD_NONE, theID));
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_KEYUP && (DWORD)theMessage.lParam == 0xc01e0001u);
    Push(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_RIGHT, KMOD_NONE, theID));
    theMessage = Next(theWindow);
    CHECK(theMessage.wParam == VK_RIGHT && theMessage.lParam == 0x014d0001);
    Push(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_F4, KMOD_LALT, theID));
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_SYSKEYDOWN && theMessage.wParam == VK_F4 && theMessage.lParam == 0x203e0001);
    Push(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_F10, KMOD_NONE, theID));
    CHECK(Next(theWindow).message == WM_SYSKEYDOWN);
    Push(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_KP_ENTER, KMOD_NONE, theID));
    theMessage = Next(theWindow);
    CHECK(theMessage.wParam == VK_RETURN && theMessage.lParam == 0x011c0001);

    // Text: UTF-8 from SDL becomes WM_CHAR in code page 1252, one message per character
    SDL_Event theText = {};
    theText.type = SDL_TEXTINPUT;
    theText.text.windowID = theID;
    strcpy(theText.text.text, "\xc3\xa9\xe2\x82\xac" "z");     // e acute, euro sign, z
    Push(theText);
    CHECK(Next(theWindow).wParam == 0xe9);
    CHECK(Next(theWindow).wParam == 0x80);
    CHECK(Next(theWindow).wParam == 'z');
    CHECK(Next(theWindow).message == 0);

    // Mouse: client coordinates in lParam, the screen position in GetCursorPos; a second click is a double click for
    // a CS_DBLCLKS class
    SDL_Event theButton = {};
    theButton.type = SDL_MOUSEBUTTONDOWN;
    theButton.button.windowID = theID;
    theButton.button.button = SDL_BUTTON_LEFT;
    theButton.button.state = SDL_PRESSED;
    theButton.button.clicks = 1;
    theButton.button.x = 10;
    theButton.button.y = 20;
    Push(theButton);
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_LBUTTONDOWN && theMessage.lParam == MAKELPARAM(10, 20));
    POINT thePoint;
    CHECK(GetCursorPos(&thePoint) && thePoint.x == 110 && thePoint.y == 70);
    theButton.button.clicks = 2;
    Push(theButton);
    CHECK(Next(theWindow).message == WM_LBUTTONDBLCLK);
    theButton.type = SDL_MOUSEBUTTONUP;
    theButton.button.button = SDL_BUTTON_RIGHT;
    Push(theButton);
    CHECK(Next(theWindow).message == WM_RBUTTONUP);
    SDL_Event theMotion = {};
    theMotion.type = SDL_MOUSEMOTION;
    theMotion.motion.windowID = theID;
    theMotion.motion.x = 300;
    theMotion.motion.y = 7;
    Push(theMotion);
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_MOUSEMOVE && theMessage.lParam == MAKELPARAM(300, 7) && theMessage.pt.x == 400);

    // Window events
    SDL_Event theWindowEvent = {};
    theWindowEvent.type = SDL_WINDOWEVENT;
    theWindowEvent.window.windowID = theID;
    theWindowEvent.window.event = SDL_WINDOWEVENT_MINIMIZED;
    Push(theWindowEvent);
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_SIZE && theMessage.wParam == SIZE_MINIMIZED);
    theWindowEvent.window.event = SDL_WINDOWEVENT_FOCUS_LOST;
    Push(theWindowEvent);
    CHECK(Next(theWindow).message == WM_ACTIVATE && Next(theWindow).message == WM_KILLFOCUS && GetFocus() == NULL);
    // No window of the game has the focus after the events, so the application is inactive
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_ACTIVATEAPP && theMessage.wParam == FALSE);
    Push(KeyEvent(SDL_KEYDOWN, SDL_SCANCODE_A, KMOD_NONE, theID));
    CHECK(Next(theWindow).message == 0);        // no focus, no key messages
    theWindowEvent.window.event = SDL_WINDOWEVENT_FOCUS_GAINED;
    Push(theWindowEvent);
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_ACTIVATEAPP && theMessage.wParam == TRUE);
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_ACTIVATE && theMessage.wParam == WA_ACTIVE && GetFocus() == theWindow);
    CHECK(Next(theWindow).message == WM_SETFOCUS);
    theWindowEvent.window.event = SDL_WINDOWEVENT_CLOSE;
    Push(theWindowEvent);
    theMessage = Next(theWindow);
    CHECK(theMessage.message == WM_CLOSE);
    DispatchMessageA(&theMessage);              // DefWindowProc destroys the window
    RECT theRect;
    CHECK(!GetWindowRect(theWindow, &theRect) && eeport::WindowOfNative(theNative) == NULL);
}

static void TestFocus()
{
    HWND theFirst = Make("Recorder Class", WS_POPUP | WS_VISIBLE, 0, 0, 10, 10);
    HWND theSecond = Make("Recorder Class", WS_POPUP | WS_VISIBLE, 0, 0, 10, 10);
    HWND theHidden = Make("Recorder Class", WS_POPUP, 0, 0, 10, 10);
    CHECK(GetFocus() == theSecond);
    sSeen.clear();
    DestroyWindow(theSecond);
    // As on Windows, destroying the active window activates the latest other visible top-level window
    CHECK(GetFocus() == theFirst);
    CHECK(MessagesOf(theFirst) == std::vector<UINT>({WM_ACTIVATE, WM_SETFOCUS}));
    POINT thePoint = {5, 5};
    CHECK(WindowFromPoint(thePoint) == theFirst);
    thePoint = {10, 5};
    CHECK(WindowFromPoint(thePoint) == GetDesktopWindow());
    DestroyWindow(theFirst);
    DestroyWindow(theHidden);
    CHECK(GetFocus() == NULL);
}

static void TestClipboard()
{
    CHECK(OpenClipboard(NULL) && EmptyClipboard());
    HGLOBAL theBlock = GlobalAlloc(GMEM_MOVEABLE, 5);
    memcpy(GlobalLock(theBlock), "caf\xe9", 5);
    CHECK(SetClipboardData(CF_TEXT, theBlock) == theBlock);
    CloseClipboard();
    char* theUTF8 = SDL_GetClipboardText();
    CHECK(strcmp(theUTF8, "caf\xc3\xa9") == 0);        // SDL holds UTF-8
    SDL_free(theUTF8);
    CHECK(IsClipboardFormatAvailable(CF_TEXT) && IsClipboardFormatAvailable(CF_UNICODETEXT));
    CHECK(!IsClipboardFormatAvailable(CF_BITMAP));
    OpenClipboard(NULL);
    const wchar_t* theWide = (const wchar_t*)GetClipboardData(CF_UNICODETEXT);
    CHECK(theWide && wcscmp(theWide, L"café") == 0);

    EmptyClipboard();
    const wchar_t kEuro[] = L"€5";
    theBlock = GlobalAlloc(GMEM_MOVEABLE, sizeof kEuro);
    memcpy(GlobalLock(theBlock), kEuro, sizeof kEuro);
    CHECK(SetClipboardData(CF_UNICODETEXT, theBlock) == theBlock);
    const char* theAnsi = (const char*)GetClipboardData(CF_TEXT);
    CHECK(theAnsi && strcmp(theAnsi, "\x80" "5") == 0 && GlobalSize((HGLOBAL)theAnsi) == 3);
    CloseClipboard();
}

static void TestMisc()
{
    RECT theRect;
    CHECK(SetRect(&theRect, 1, 2, 11, 12));
    POINT theInside = {1, 2}, theRight = {11, 5}, theBelow = {5, 12};
    CHECK(PtInRect(&theRect, theInside) && !PtInRect(&theRect, theRight) && !PtInRect(&theRect, theBelow));
    CHECK(OffsetRect(&theRect, -1, 3) && theRect.left == 0 && theRect.top == 5 && theRect.right == 10 &&
          theRect.bottom == 15);

    SDL_Rect theBounds;
    CHECK(SDL_GetDisplayBounds(0, &theBounds) == 0);
    CHECK(GetSystemMetrics(SM_CXSCREEN) == theBounds.w && GetSystemMetrics(SM_CYSCREEN) == theBounds.h);
    CHECK(GetWindowRect(GetDesktopWindow(), &theRect) && theRect.right - theRect.left == theBounds.w);

    CHECK(ShowCursor(FALSE) == -1 && ShowCursor(FALSE) == -2 && ShowCursor(TRUE) == -1 && ShowCursor(TRUE) == 0);
    HCURSOR theArrow = LoadCursorA(NULL, IDC_ARROW);
    CHECK(theArrow && LoadCursorA(NULL, IDC_ARROW) == theArrow && LoadCursorA(NULL, IDC_IBEAM) != theArrow);
    CHECK(SetCursor(theArrow) == NULL && SetCursor(NULL) == theArrow);
    BYTE theAnd[4 * 32], theXor[4 * 32];
    memset(theAnd, 0xff, sizeof theAnd);
    memset(theXor, 0, sizeof theXor);
    HCURSOR theMade = CreateCursor(NULL, 1, 2, 32, 32, theAnd, theXor);
    CHECK(theMade && DestroyCursor(theMade));
    CHECK(SetCursorPos(30, 40));
    POINT thePoint;
    CHECK(GetCursorPos(&thePoint) && thePoint.x == 30 && thePoint.y == 40);
    CHECK(ImmGetContext(NULL) == NULL);
}

int main()
{
    setenv("SDL_VIDEODRIVER", "offscreen", 1);
    setenv("EE_PORT_NO_MESSAGE_BOXES", "1", 1);
    CHECK(eeport::StartVideo());
    TestClasses();
    TestCreation();
    TestQueue();
    TestThreads();
    TestTimers();
    TestTranslate();
    TestEvents();
    TestFocus();
    TestClipboard();
    TestMisc();
    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
