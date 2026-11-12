// Tests of the native build's DirectInput (src/Port/Linux/DirectInput.cpp) on SDL's offscreen video driver: the
// game's own set-up calls (Input System.cpp), the SDL events that become key, button, axis and wheel data, the
// buffer, overflow and flush, relative and absolute axes, foreground acquisition as the focus moves, and the mouse
// messages an exclusive mouse holds back. The GUIDs and error numbers are written as Microsoft documents them, not
// taken from the headers.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <dinput.h>
#include <ddraw.h>
#include <d3d.h>
#include <dsound.h>
#include <SDL.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

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

// The documented values
const HRESULT kNotAcquired = HRESULT(0x8007000C);
const HRESULT kOtherAppHasPriority = HRESULT(0x80070005);
const HRESULT kAcquired = HRESULT(0x800700AA);
const HRESULT kNotBuffered = HRESULT(0x80040207);
const HRESULT kInvalidParam = HRESULT(0x80070057);
const HRESULT kNotFound = HRESULT(0x80070002);
const HRESULT kDeviceNotRegistered = HRESULT(0x80040154);
const HRESULT kUnsupported = HRESULT(0x80004001);
const HRESULT kBufferOverflow = 1;
const HRESULT kNoEffect = 1;
const DWORD kDikA = 0x1e, kDikB = 0x30, kDikNumpadEnter = 0x9c, kDikRightControl = 0x9d, kDikUp = 0xc8,
            kDikPause = 0xc5;

static bool SameGUID(const GUID& inGUID, unsigned long inData1, unsigned short inData2, unsigned short inData3,
                     const unsigned char (&inData4)[8])
{
    return inGUID.Data1 == inData1 && inGUID.Data2 == inData2 && inGUID.Data3 == inData3 &&
           memcmp(inGUID.Data4, inData4, 8) == 0;
}

static void Drain()
{
    MSG theMessage;
    for (int i = 0; i < 1000 && PeekMessageA(&theMessage, NULL, 0, 0, PM_REMOVE); ++i)
        DispatchMessageA(&theMessage);
}

// Pumps the SDL events and says whether a mouse message reached the window
static bool MouseMessage(HWND inWindow)
{
    bool theSeen = false;
    MSG theMessage;
    for (int i = 0; i < 1000 && PeekMessageA(&theMessage, inWindow, 0, 0, PM_REMOVE); ++i)
        theSeen = theSeen || (theMessage.message >= WM_MOUSEFIRST && theMessage.message <= WM_MOUSELAST);
    return theSeen;
}

static void Push(SDL_Event inEvent)
{
    CHECK(SDL_PushEvent(&inEvent) == 1);
}

static void Key(Uint32 inType, SDL_Scancode inCode, Uint32 inWindowID, Uint8 inRepeat = 0)
{
    SDL_Event theEvent = {};
    theEvent.type = inType;
    theEvent.key.windowID = inWindowID;
    theEvent.key.state = inType == SDL_KEYDOWN ? SDL_PRESSED : SDL_RELEASED;
    theEvent.key.repeat = inRepeat;
    theEvent.key.keysym.scancode = inCode;
    Push(theEvent);
}

static void Motion(int inX, int inY, Uint32 inWindowID)
{
    SDL_Event theEvent = {};
    theEvent.type = SDL_MOUSEMOTION;
    theEvent.motion.windowID = inWindowID;
    theEvent.motion.x = 10;
    theEvent.motion.y = 10;
    theEvent.motion.xrel = inX;
    theEvent.motion.yrel = inY;
    Push(theEvent);
}

static void Button(Uint32 inType, Uint8 inButton, Uint32 inWindowID)
{
    SDL_Event theEvent = {};
    theEvent.type = inType;
    theEvent.button.windowID = inWindowID;
    theEvent.button.button = inButton;
    theEvent.button.state = inType == SDL_MOUSEBUTTONDOWN ? SDL_PRESSED : SDL_RELEASED;
    theEvent.button.clicks = 1;
    Push(theEvent);
}

static void Wheel(int inY, bool inFlipped, Uint32 inWindowID)
{
    SDL_Event theEvent = {};
    theEvent.type = SDL_MOUSEWHEEL;
    theEvent.wheel.windowID = inWindowID;
    theEvent.wheel.y = inY;
    theEvent.wheel.direction = inFlipped ? SDL_MOUSEWHEEL_FLIPPED : SDL_MOUSEWHEEL_NORMAL;
    Push(theEvent);
}

static void Focus(Uint8 inEvent, Uint32 inWindowID)
{
    SDL_Event theEvent = {};
    theEvent.type = SDL_WINDOWEVENT;
    theEvent.window.event = inEvent;
    theEvent.window.windowID = inWindowID;
    Push(theEvent);
}

static DIPROPDWORD Property(DWORD inObject, DWORD inHow, DWORD inData)
{
    DIPROPDWORD theProperty;
    theProperty.diph.dwSize = sizeof(DIPROPDWORD);
    theProperty.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    theProperty.diph.dwObj = inObject;
    theProperty.diph.dwHow = inHow;
    theProperty.dwData = inData;
    return theProperty;
}

// Takes up to 16 buffered events in the game's entry size
static DWORD Take(IDirectInputDevice* inDevice, DIDEVICEOBJECTDATA* outData, HRESULT* outResult = NULL)
{
    memset(outData, 0xff, 16 * sizeof(DIDEVICEOBJECTDATA));
    DWORD theCount = 16;
    HRESULT theResult = inDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), outData, &theCount, 0);
    if (outResult)
        *outResult = theResult;
    return theResult >= 0 ? theCount : 0;
}

int main()
{
    setenv("SDL_VIDEODRIVER", "offscreen", 1);
    setenv("EE_PORT_NO_MESSAGE_BOXES", "1", 1);
    CHECK(eeport::StartVideo());

    // ---- the GUIDs as Microsoft's headers define them ----
    CHECK(SameGUID(GUID_SysMouse, 0x6f1d2b60, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}));
    CHECK(SameGUID(GUID_SysKeyboard, 0x6f1d2b61, 0xd5a0, 0x11cf, {0xbf, 0xc7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00}));
    CHECK(SameGUID(IID_IDirectDraw7, 0x15e65ec0, 0x3b9c, 0x11d2, {0xb9, 0x2f, 0x00, 0x60, 0x97, 0x97, 0xea, 0x5b}));
    CHECK(SameGUID(IID_IDirect3D7, 0xf5049e77, 0x4861, 0x11d2, {0xa4, 0x07, 0x00, 0xa0, 0xc9, 0x06, 0x29, 0xa8}));
    CHECK(SameGUID(IID_IDirect3DTnLHalDevice, 0xf5049e78, 0x4861, 0x11d2,
                   {0xa4, 0x07, 0x00, 0xa0, 0xc9, 0x06, 0x29, 0xa8}));
    CHECK(SameGUID(IID_IDirect3DHALDevice, 0x84e63de0, 0x46aa, 0x11cf, {0x81, 0x6f, 0x00, 0x00, 0xc0, 0x20, 0x15, 0x6e}));
    CHECK(SameGUID(IID_IDirectSound, 0x279afa83, 0x4981, 0x11ce, {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60}));
    CHECK(SameGUID(IID_IUnknown, 0x00000000, 0x0000, 0x0000, {0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46}));

    WNDCLASSA theClass = {};
    theClass.lpfnWndProc = DefWindowProcA;
    theClass.lpszClassName = "Input Test";
    CHECK(RegisterClassA(&theClass) != 0);
    HWND theWindow = CreateWindowExA(0, "Input Test", "game", WS_POPUP | WS_VISIBLE, 0, 0, 320, 200, NULL, NULL, NULL,
                                     NULL);
    HWND theChild = CreateWindowExA(0, "Input Test", "child", WS_CHILD | WS_VISIBLE, 0, 0, 10, 10, theWindow, NULL,
                                    NULL, NULL);
    HWND theOther = CreateWindowExA(0, "Input Test", "other", WS_POPUP, 0, 0, 100, 100, NULL, NULL, NULL, NULL);
    Uint32 theID = SDL_GetWindowID(eeport::NativeWindow(theWindow, false));
    SDL_PumpEvents();
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
    Drain();
    SetFocus(theWindow);

    // ---- the game's set-up (ISInitialize) ----
    IDirectInput* theInput = NULL;
    CHECK(DirectInputCreateA(NULL, 0x0700, &theInput, NULL) == DI_OK && theInput != NULL);
    IDirectInputDevice* theKeyboard = NULL;
    IDirectInputDevice* theMouse = NULL;
    IDirectInputDevice* theNone = NULL;
    GUID theJoystick = GUID_SysMouse;
    theJoystick.Data1 = 0x6f1d2b70;
    CHECK(theInput->CreateDevice(theJoystick, &theNone, NULL) == kDeviceNotRegistered && theNone == NULL);
    CHECK(theInput->CreateDevice(GUID_SysKeyboard, &theKeyboard, NULL) == DI_OK && theKeyboard);
    CHECK(theInput->CreateDevice(GUID_SysMouse, &theMouse, NULL) == DI_OK && theMouse);
    CHECK(c_dfDIKeyboard.dwDataSize == 256 && c_dfDIMouse.dwDataSize == 16 && c_dfDIMouse.dwFlags == DIDF_RELAXIS);
    CHECK(theKeyboard->Acquire() == kInvalidParam);       // no data format yet
    CHECK(theKeyboard->SetDataFormat(&c_dfDIMouse) == kInvalidParam);
    CHECK(theKeyboard->SetDataFormat(&c_dfDIKeyboard) == DI_OK);
    CHECK(theKeyboard->SetCooperativeLevel(theWindow, DISCL_EXCLUSIVE | DISCL_BACKGROUND) == kUnsupported);
    CHECK(theKeyboard->SetCooperativeLevel(theWindow, DISCL_EXCLUSIVE | DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) ==
          kInvalidParam);
    CHECK(theKeyboard->SetCooperativeLevel(theWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) == DI_OK);
    DIPROPDWORD theSize = Property(0, DIPH_DEVICE, 256);
    CHECK(theKeyboard->SetProperty(DIPROP_BUFFERSIZE, &theSize.diph) == DI_OK);
    CHECK(theMouse->SetDataFormat(&c_dfDIMouse) == DI_OK);
    CHECK(theMouse->SetCooperativeLevel(theWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND) == DI_OK);
    CHECK(theMouse->SetProperty(DIPROP_BUFFERSIZE, &theSize.diph) == DI_OK);

    // ---- the mouse's objects (ISMouse's constructor) ----
    DIDEVICEOBJECTINSTANCE theObject;
    theObject.dwSize = sizeof theObject;
    for (DWORD theOffset : {DIMOFS_X, DIMOFS_Y, DIMOFS_Z, DIMOFS_BUTTON0, DIMOFS_BUTTON1, DIMOFS_BUTTON2,
                            DIMOFS_BUTTON3})
        CHECK(theMouse->GetObjectInfo(&theObject, theOffset, DIPH_BYOFFSET) == DI_OK && theObject.dwOfs == theOffset);
    CHECK(theMouse->GetObjectInfo(&theObject, 16, DIPH_BYOFFSET) == kNotFound);
    DIPROPDWORD theGranularity = Property(DIMOFS_X, DIPH_BYOFFSET, 0);
    CHECK(theMouse->GetProperty(DIPROP_GRANULARITY, &theGranularity.diph) == DI_OK && theGranularity.dwData == 1);
    theGranularity = Property(DIMOFS_Z, DIPH_BYOFFSET, 0);
    CHECK(theMouse->GetProperty(DIPROP_GRANULARITY, &theGranularity.diph) == DI_OK && theGranularity.dwData == 120);
    theGranularity = Property(DIMOFS_BUTTON0, DIPH_BYOFFSET, 0);
    CHECK(theMouse->GetProperty(DIPROP_GRANULARITY, &theGranularity.diph) == kNotFound);

    // ---- acquisition ----
    BYTE theKeys[256];
    CHECK(theKeyboard->GetDeviceState(256, theKeys) == kNotAcquired);
    CHECK(theKeyboard->Acquire() == DI_OK);
    CHECK(theKeyboard->Acquire() == kNoEffect);
    CHECK(theKeyboard->SetProperty(DIPROP_BUFFERSIZE, &theSize.diph) == kAcquired);
    CHECK(theKeyboard->SetCooperativeLevel(theWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) == kAcquired);
    CHECK(theKeyboard->GetDeviceState(255, theKeys) == kInvalidParam);
    CHECK(theMouse->Acquire() == DI_OK);
    CHECK(eeport::MouseHeld());
    // the focus on a child keeps the top-level window in the foreground
    SetFocus(theChild);
    CHECK(theKeyboard->GetDeviceState(256, theKeys) == DI_OK);
    SetFocus(theWindow);

    // ---- keys from SDL: the DIK code, 0x80 down and 0 up, no repeats ----
    DIDEVICEOBJECTDATA theData[16];
    Key(SDL_KEYDOWN, SDL_SCANCODE_A, theID);
    Key(SDL_KEYDOWN, SDL_SCANCODE_A, theID, 1);
    Key(SDL_KEYUP, SDL_SCANCODE_A, theID);
    Key(SDL_KEYDOWN, SDL_SCANCODE_KP_ENTER, theID);
    Key(SDL_KEYDOWN, SDL_SCANCODE_RCTRL, theID);
    Key(SDL_KEYDOWN, SDL_SCANCODE_UP, theID);
    Key(SDL_KEYDOWN, SDL_SCANCODE_PAUSE, theID);
    Drain();
    DWORD theCount = Take(theKeyboard, theData);
    CHECK(theCount == 6);
    if (theCount == 6) {
        CHECK(theData[0].dwOfs == kDikA && theData[0].dwData == 0x80);
        CHECK(theData[1].dwOfs == kDikA && theData[1].dwData == 0);
        CHECK(theData[2].dwOfs == kDikNumpadEnter && theData[3].dwOfs == kDikRightControl);
        CHECK(theData[4].dwOfs == kDikUp && theData[5].dwOfs == kDikPause);
        bool theOrdered = true;
        for (DWORD i = 1; i < theCount; ++i)
            theOrdered = theOrdered && DISEQUENCE_COMPARE(theData[i - 1].dwSequence, <, theData[i].dwSequence) &&
                         theData[i - 1].dwTimeStamp <= theData[i].dwTimeStamp;
        CHECK(theOrdered);
        // the game's 20-byte entries: the bytes after the DirectX 7 fields are cleared
        CHECK(theData[0].uAppData == 0);
    }
    CHECK(theKeyboard->GetDeviceState(256, theKeys) == DI_OK);
    CHECK(theKeys[kDikA] == 0 && theKeys[kDikUp] == 0x80 && theKeys[kDikNumpadEnter] == 0x80 && theKeys[kDikB] == 0);
    CHECK(Take(theKeyboard, theData) == 0);

    // ---- the buffer: peek, flush, and an overflow reported once ----
    eeport::InputKey(BYTE(kDikB), true);
    eeport::InputKey(BYTE(kDikB), false);
    DWORD thePeek = 16;
    CHECK(theKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), theData, &thePeek, DIGDD_PEEK) == DI_OK &&
          thePeek == 2);
    DWORD theFlush = INFINITE;
    CHECK(theKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &theFlush, 0) == DI_OK && theFlush == 2);
    CHECK(Take(theKeyboard, theData) == 0);
    theKeyboard->Unacquire();
    CHECK(theKeyboard->Unacquire() == kNoEffect);
    DIPROPDWORD theSmall = Property(0, DIPH_DEVICE, 2);
    CHECK(theKeyboard->SetProperty(DIPROP_BUFFERSIZE, &theSmall.diph) == DI_OK);
    CHECK(theKeyboard->Acquire() == DI_OK);
    eeport::InputKey(BYTE(kDikB), true);
    eeport::InputKey(BYTE(kDikB), false);
    eeport::InputKey(BYTE(kDikB), true);
    HRESULT theResult;
    theCount = Take(theKeyboard, theData, &theResult);
    CHECK(theResult == kBufferOverflow && theCount == 2 && theData[0].dwData == 0x80 && theData[1].dwData == 0);
    CHECK(Take(theKeyboard, theData, &theResult) == 0 && theResult == DI_OK);
    theKeyboard->Unacquire();
    DIPROPDWORD theNoBuffer = Property(0, DIPH_DEVICE, 0);
    theKeyboard->SetProperty(DIPROP_BUFFERSIZE, &theNoBuffer.diph);
    theKeyboard->Acquire();
    Take(theKeyboard, theData, &theResult);
    CHECK(theResult == kNotBuffered);
    theKeyboard->Unacquire();
    theKeyboard->SetProperty(DIPROP_BUFFERSIZE, &theSize.diph);
    theKeyboard->Acquire();
    eeport::InputKey(BYTE(kDikB), false);

    // ---- the mouse from SDL: relative movement since the last read, the wheel in WHEEL_DELTA steps ----
    DIMOUSESTATE theState;
    CHECK(theMouse->GetDeviceState(sizeof theState, &theState) == DI_OK);
    Motion(5, -3, theID);
    Motion(2, 0, theID);
    Wheel(1, false, theID);
    Wheel(1, true, theID);
    Button(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_RIGHT, theID);
    Button(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_MIDDLE, theID);
    Button(SDL_MOUSEBUTTONDOWN, SDL_BUTTON_X1, theID);
    Button(SDL_MOUSEBUTTONUP, SDL_BUTTON_MIDDLE, theID);
    // the window gets no mouse messages while DirectInput holds the mouse
    CHECK(!MouseMessage(theWindow));
    CHECK(theMouse->GetDeviceState(sizeof theState, &theState) == DI_OK);
    CHECK(theState.lX == 7 && theState.lY == -3 && theState.lZ == 0);
    CHECK(theState.rgbButtons[0] == 0 && theState.rgbButtons[1] == 0x80 && theState.rgbButtons[2] == 0 &&
          theState.rgbButtons[3] == 0x80);
    CHECK(theMouse->GetDeviceState(sizeof theState, &theState) == DI_OK && theState.lX == 0 && theState.lY == 0);
    theCount = Take(theMouse, theData);
    // three movements, two wheel steps, three presses and the middle button's release
    CHECK(theCount == 9);
    if (theCount == 9) {
        CHECK(theData[0].dwOfs == DIMOFS_X && LONG(theData[0].dwData) == 5);
        CHECK(theData[1].dwOfs == DIMOFS_Y && LONG(theData[1].dwData) == -3);
        CHECK(theData[2].dwOfs == DIMOFS_X && LONG(theData[2].dwData) == 2);
        CHECK(theData[3].dwOfs == DIMOFS_Z && LONG(theData[3].dwData) == 120);
        CHECK(theData[4].dwOfs == DIMOFS_Z && LONG(theData[4].dwData) == -120);
        CHECK(theData[5].dwOfs == DIMOFS_BUTTON1 && theData[5].dwData == 0x80);
        CHECK(theData[6].dwOfs == DIMOFS_BUTTON2 && theData[6].dwData == 0x80);
        CHECK(theData[7].dwOfs == DIMOFS_BUTTON3 && theData[7].dwData == 0x80);
        CHECK(theData[8].dwOfs == DIMOFS_BUTTON2 && theData[8].dwData == 0);
    }

    // ---- absolute axes count from the acquisition ----
    DIPROPDWORD theAbsolute = Property(0, DIPH_DEVICE, DIPROPAXISMODE_ABS);
    CHECK(theMouse->SetProperty(DIPROP_AXISMODE, &theAbsolute.diph) == kAcquired);
    theMouse->Unacquire();
    CHECK(!eeport::MouseHeld());
    CHECK(theMouse->SetProperty(DIPROP_AXISMODE, &theAbsolute.diph) == DI_OK);
    // released, the mouse sends messages again
    Motion(1, 1, theID);
    CHECK(MouseMessage(theWindow));
    CHECK(theMouse->Acquire() == DI_OK);
    Motion(5, 0, theID);
    Motion(7, 0, theID);
    Drain();
    CHECK(theMouse->GetDeviceState(sizeof theState, &theState) == DI_OK && theState.lX == 12);
    CHECK(theMouse->GetDeviceState(sizeof theState, &theState) == DI_OK && theState.lX == 12);
    theCount = Take(theMouse, theData);
    CHECK(theCount == 2 && LONG(theData[0].dwData) == 5 && LONG(theData[1].dwData) == 12);
    theMouse->Unacquire();
    DIPROPDWORD theRelative = Property(0, DIPH_DEVICE, DIPROPAXISMODE_REL);
    theMouse->SetProperty(DIPROP_AXISMODE, &theRelative.diph);
    theMouse->Acquire();

    // ---- a non-exclusive mouse (ISMouse::Suspend) leaves the window its mouse messages ----
    theMouse->Unacquire();
    CHECK(theMouse->SetCooperativeLevel(theWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND) == DI_OK);
    CHECK(theMouse->Acquire() == DI_OK);
    CHECK(!eeport::MouseHeld());
    Motion(3, 0, theID);
    CHECK(MouseMessage(theWindow));
    CHECK(theMouse->GetDeviceState(sizeof theState, &theState) == DI_OK && theState.lX == 3);
    theMouse->Unacquire();
    theMouse->SetCooperativeLevel(theWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    theMouse->Acquire();

    // ---- the focus: another top-level window takes the foreground devices away ----
    Key(SDL_KEYDOWN, SDL_SCANCODE_A, theID);
    Drain();
    SetFocus(theOther);
    CHECK(theKeyboard->GetDeviceState(256, theKeys) == kNotAcquired);
    CHECK(!eeport::MouseHeld());
    CHECK(theKeyboard->Acquire() == kOtherAppHasPriority);
    SetFocus(theWindow);
    CHECK(theKeyboard->Acquire() == DI_OK);
    // the game losing the focus releases every key it held
    CHECK(theKeyboard->GetDeviceState(256, theKeys) == DI_OK && theKeys[kDikA] == 0x80);
    Focus(SDL_WINDOWEVENT_FOCUS_LOST, theID);
    Drain();
    CHECK(theKeyboard->GetDeviceState(256, theKeys) == kNotAcquired);
    Focus(SDL_WINDOWEVENT_FOCUS_GAINED, theID);
    Drain();
    CHECK(theKeyboard->Acquire() == DI_OK && theMouse->Acquire() == DI_OK);
    CHECK(theKeyboard->GetDeviceState(256, theKeys) == DI_OK && theKeys[kDikA] == 0);
    CHECK(eeport::MouseHeld());

    // ---- release (ISShutdown) ----
    CHECK(theMouse->Release() == 0);
    CHECK(!eeport::MouseHeld());
    CHECK(theKeyboard->Release() == 0);
    CHECK(theInput->Release() == 0);

    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
