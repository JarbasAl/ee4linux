// DirectInput 7 (dinput.dll) for the game's system keyboard and mouse (Input System.cpp), fed by SDL's events.
// Win32Window.cpp passes each key, mouse movement, button and wheel event from the thread that pumps; an acquired device
// keeps its state and its buffer of events, which the game reads from any thread.
//
// The rules are DirectInput's: a foreground device is acquired only while its window's top-level window has the
// focus, and loses the acquisition when the focus goes; a buffered device keeps DIPROP_BUFFERSIZE events and reports
// DI_BUFFEROVERFLOW once after it drops one; relative axes report the movement since the last read. An exclusive
// foreground mouse puts SDL into relative mouse mode, and the window then gets no mouse messages, as on Windows.
#include "Win32Internal.h"

#include <dinput.h>
#include <SDL.h>

#include <algorithm>
#include <cstring>
#include <deque>
#include <set>

namespace {

enum Kind { kKeyboard, kMouse };

// DIMOFS_* and the mouse's DIMOUSESTATE
const DWORD kMouseX = 0, kMouseY = 4, kMouseZ = 8, kMouseButton0 = 12;
const int kMouseButtons = 4;

class Device;

std::mutex gLock;
std::set<Device*> gDevices;
DWORD gSequence;
HWND gFocusRoot;                // the top-level window of the keyboard focus, or NULL when the game has none
bool gKeys[256];
bool gButtons[kMouseButtons];
std::atomic<bool> gRelative{false};

// The top-level window of a window. The window functions take their own lock, so this runs outside gLock.
HWND Root(HWND inWindow)
{
    for (HWND theParent = inWindow ? GetParent(inWindow) : NULL; theParent; theParent = GetParent(inWindow))
        inWindow = theParent;
    return inWindow;
}

class Device final : public IDirectInputDeviceA {
public:
    explicit Device(Kind inKind) : mKind(inKind) {}

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirectInputDeviceA)) {
            *outObject = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        *outObject = this;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override
    {
        long theCount = --mReferences;
        if (theCount == 0) {
            {
                std::lock_guard<std::mutex> theGuard(gLock);
                gDevices.erase(this);
            }
            UpdateRelative();
            delete this;
        }
        return ULONG(theCount);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCapabilities(LPDIDEVCAPS outCaps) override
    {
        if (!outCaps || (outCaps->dwSize != sizeof(DIDEVCAPS_DX3) && outCaps->dwSize != sizeof(DIDEVCAPS)))
            return DIERR_INVALIDPARAM;
        DWORD theSize = outCaps->dwSize;
        memset(outCaps, 0, theSize);
        outCaps->dwSize = theSize;
        outCaps->dwFlags = DIDC_ATTACHED;
        if (mKind == kKeyboard) {
            outCaps->dwDevType = DIDEVTYPE_KEYBOARD | (DIDEVTYPEKEYBOARD_PCENH << 8);
            outCaps->dwButtons = 128;
        } else {
            outCaps->dwDevType = DIDEVTYPE_MOUSE | (DIDEVTYPEMOUSE_UNKNOWN << 8);
            outCaps->dwAxes = 3;
            outCaps->dwButtons = kMouseButtons;
        }
        return DI_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD) override
    {
        EE_PORT_UNSUPPORTED("IDirectInputDevice::EnumObjects");
        return DIERR_UNSUPPORTED;
    }

    // DIPROP_BUFFERSIZE and DIPROP_AXISMODE for the device, and DIPROP_GRANULARITY for a mouse axis: 1 for X and Y,
    // WHEEL_DELTA for the wheel
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetProperty(REFGUID inProperty, LPDIPROPHEADER ioHeader) override
    {
        if (!ValidHeader(ioHeader))
            return DIERR_INVALIDPARAM;
        DWORD& theData = reinterpret_cast<LPDIPROPDWORD>(ioHeader)->dwData;
        std::lock_guard<std::mutex> theGuard(gLock);
        switch (PropertyNumber(inProperty)) {
        case 1:
            theData = mBufferSize;
            return DI_OK;
        case 2:
            theData = mAxisMode;
            return DI_OK;
        case 3:
            if (mKind != kMouse || ioHeader->dwHow != DIPH_BYOFFSET ||
                (ioHeader->dwObj != kMouseX && ioHeader->dwObj != kMouseY && ioHeader->dwObj != kMouseZ))
                return DIERR_NOTFOUND;
            theData = ioHeader->dwObj == kMouseZ ? WHEEL_DELTA : 1;
            return DI_OK;
        }
        return DIERR_UNSUPPORTED;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetProperty(REFGUID inProperty, LPCDIPROPHEADER inHeader) override
    {
        if (!ValidHeader(inHeader))
            return DIERR_INVALIDPARAM;
        DWORD theData = reinterpret_cast<const DIPROPDWORD*>(inHeader)->dwData;
        std::lock_guard<std::mutex> theGuard(gLock);
        switch (PropertyNumber(inProperty)) {
        case 1:
            if (inHeader->dwHow != DIPH_DEVICE)
                return DIERR_INVALIDPARAM;
            if (mAcquired)
                return DIERR_ACQUIRED;
            mBufferSize = theData;
            return DI_OK;
        case 2:
            if (inHeader->dwHow != DIPH_DEVICE || (theData != DIPROPAXISMODE_ABS && theData != DIPROPAXISMODE_REL))
                return DIERR_INVALIDPARAM;
            if (mAcquired)
                return DIERR_ACQUIRED;
            mAxisMode = theData;
            return DI_OK;
        }
        return DIERR_UNSUPPORTED;
    }

    // A foreground device is acquired only while its window's top-level window has the focus
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Acquire() override
    {
        HRESULT theResult;
        {
            std::lock_guard<std::mutex> theGuard(gLock);
            if (!mDataSize)
                return DIERR_INVALIDPARAM;
            if (mAcquired)
                return DI_NOEFFECT;
            if ((mCooperation & DISCL_FOREGROUND) && (!gFocusRoot || gFocusRoot != mRoot))
                return DIERR_OTHERAPPHASPRIO;
            mAcquired = true;
            mBuffer.clear();
            mOverflow = false;
            mX = mY = mZ = 0;
            theResult = DI_OK;
        }
        UpdateRelative();
        return theResult;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Unacquire() override
    {
        {
            std::lock_guard<std::mutex> theGuard(gLock);
            if (!mAcquired)
                return DI_NOEFFECT;
            mAcquired = false;
        }
        UpdateRelative();
        return DI_OK;
    }

    // The keyboard's 256 key bytes, or the mouse's DIMOUSESTATE: relative axes move by what happened since the
    // last call
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceState(DWORD inSize, LPVOID outData) override
    {
        std::lock_guard<std::mutex> theGuard(gLock);
        if (!mAcquired)
            return DIERR_NOTACQUIRED;
        if (!outData || inSize != mDataSize)
            return DIERR_INVALIDPARAM;
        if (mKind == kKeyboard) {
            BYTE* theKeys = static_cast<BYTE*>(outData);
            for (int i = 0; i < 256; ++i)
                theKeys[i] = gKeys[i] ? 0x80 : 0;
            return DI_OK;
        }
        DIMOUSESTATE* theState = static_cast<DIMOUSESTATE*>(outData);
        theState->lX = mX;
        theState->lY = mY;
        theState->lZ = mZ;
        for (int i = 0; i < kMouseButtons; ++i)
            theState->rgbButtons[i] = gButtons[i] ? 0x80 : 0;
        if (mAxisMode == DIPROPAXISMODE_REL)
            mX = mY = mZ = 0;
        return DI_OK;
    }

    // Takes up to *ioCount buffered events, oldest first, into an array of inSize-byte entries; with no array it
    // only removes them. DI_BUFFEROVERFLOW says an event was dropped since the last call.
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceData(DWORD inSize, LPDIDEVICEOBJECTDATA outData,
                                                                 LPDWORD ioCount, DWORD inFlags) override
    {
        std::lock_guard<std::mutex> theGuard(gLock);
        if (!mAcquired)
            return DIERR_NOTACQUIRED;
        if (!mBufferSize)
            return DIERR_NOTBUFFERED;
        if (!ioCount || (outData && inSize < sizeof(DIDEVICEOBJECTDATA_DX3)))
            return DIERR_INVALIDPARAM;
        DWORD theCount = std::min<DWORD>(*ioCount, DWORD(mBuffer.size()));
        BYTE* theOut = reinterpret_cast<BYTE*>(outData);
        for (DWORD i = 0; outData && i < theCount; ++i) {
            memset(theOut + i * inSize, 0, inSize);
            memcpy(theOut + i * inSize, &mBuffer[i], sizeof(DIDEVICEOBJECTDATA_DX3));
        }
        *ioCount = theCount;
        HRESULT theResult = mOverflow ? DI_BUFFEROVERFLOW : DI_OK;
        if (!(inFlags & DIGDD_PEEK)) {
            mBuffer.erase(mBuffer.begin(), mBuffer.begin() + theCount);
            mOverflow = false;
        }
        return theResult;
    }

    // c_dfDIKeyboard for the keyboard, c_dfDIMouse for the mouse: the layer knows them by their data size
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetDataFormat(LPCDIDATAFORMAT inFormat) override
    {
        if (!inFormat || inFormat->dwSize != sizeof(DIDATAFORMAT) ||
            inFormat->dwDataSize != (mKind == kKeyboard ? 256 : sizeof(DIMOUSESTATE)))
            return DIERR_INVALIDPARAM;
        std::lock_guard<std::mutex> theGuard(gLock);
        if (mAcquired)
            return DIERR_ACQUIRED;
        mDataSize = inFormat->dwDataSize;
        return DI_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetEventNotification(HANDLE inEvent) override
    {
        std::lock_guard<std::mutex> theGuard(gLock);
        if (mAcquired)
            return DIERR_ACQUIRED;
        mEvent = inEvent;
        return DI_OK;
    }
    // One of DISCL_EXCLUSIVE and DISCL_NONEXCLUSIVE and one of DISCL_FOREGROUND and DISCL_BACKGROUND; DirectInput
    // refuses an exclusive background keyboard
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND inWindow, DWORD inFlags) override
    {
        bool theExclusive = inFlags & DISCL_EXCLUSIVE, theForeground = inFlags & DISCL_FOREGROUND;
        if (theExclusive == bool(inFlags & DISCL_NONEXCLUSIVE) || theForeground == bool(inFlags & DISCL_BACKGROUND))
            return DIERR_INVALIDPARAM;
        if ((theForeground || theExclusive) && !IsWindow(inWindow))
            return E_HANDLE;
        if (mKind == kKeyboard && theExclusive && !theForeground)
            return DIERR_UNSUPPORTED;
        HWND theRoot = Root(inWindow);
        std::lock_guard<std::mutex> theGuard(gLock);
        if (mAcquired)
            return DIERR_ACQUIRED;
        mRoot = theRoot;
        mCooperation = inFlags;
        return DI_OK;
    }
    // The mouse's X, Y and Z axes and four buttons, or a keyboard key, by offset
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA outInstance, DWORD inObject,
                                                                 DWORD inHow) override
    {
        if (!outInstance || (outInstance->dwSize != sizeof(DIDEVICEOBJECTINSTANCE_DX3A) &&
                             outInstance->dwSize != sizeof(DIDEVICEOBJECTINSTANCEA)))
            return DIERR_INVALIDPARAM;
        if (inHow != DIPH_BYOFFSET)
            return DIERR_UNSUPPORTED;
        DIDEVICEOBJECTINSTANCEA theInstance{};
        theInstance.dwSize = outInstance->dwSize;
        theInstance.dwOfs = inObject;
        if (mKind == kKeyboard) {
            if (inObject > 255)
                return DIERR_NOTFOUND;
            theInstance.guidType = GUID_Key;
            theInstance.dwType = DIDFT_PSHBUTTON | DIDFT_MAKEINSTANCE(inObject);
            snprintf(theInstance.tszName, sizeof theInstance.tszName, "Key %lu", inObject);
        } else if (inObject == kMouseX || inObject == kMouseY || inObject == kMouseZ) {
            static const GUID* const kAxes[] = {&GUID_XAxis, &GUID_YAxis, &GUID_ZAxis};
            static const char* const kNames[] = {"X-axis", "Y-axis", "Wheel"};
            theInstance.guidType = *kAxes[inObject / 4];
            theInstance.dwType = DIDFT_RELAXIS | DIDFT_MAKEINSTANCE(inObject / 4);
            snprintf(theInstance.tszName, sizeof theInstance.tszName, "%s", kNames[inObject / 4]);
        } else if (inObject >= kMouseButton0 && inObject < kMouseButton0 + kMouseButtons) {
            theInstance.guidType = GUID_Button;
            theInstance.dwType = DIDFT_PSHBUTTON | DIDFT_MAKEINSTANCE(inObject - kMouseButton0);
            snprintf(theInstance.tszName, sizeof theInstance.tszName, "Button %lu", inObject - kMouseButton0);
        } else {
            return DIERR_NOTFOUND;
        }
        memcpy(outInstance, &theInstance, theInstance.dwSize);
        return DI_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceInfo(LPDIDEVICEINSTANCEA outInstance) override
    {
        if (!outInstance || (outInstance->dwSize != sizeof(DIDEVICEINSTANCE_DX3A) &&
                             outInstance->dwSize != sizeof(DIDEVICEINSTANCEA)))
            return DIERR_INVALIDPARAM;
        DWORD theSize = outInstance->dwSize;
        memset(outInstance, 0, theSize);
        outInstance->dwSize = theSize;
        outInstance->guidInstance = mKind == kKeyboard ? GUID_SysKeyboard : GUID_SysMouse;
        outInstance->guidProduct = outInstance->guidInstance;
        outInstance->dwDevType = mKind == kKeyboard ? DIDEVTYPE_KEYBOARD | (DIDEVTYPEKEYBOARD_PCENH << 8)
                                                    : DIDEVTYPE_MOUSE | (DIDEVTYPEMOUSE_UNKNOWN << 8);
        const char* theName = mKind == kKeyboard ? "Keyboard" : "Mouse";
        snprintf(outInstance->tszInstanceName, sizeof outInstance->tszInstanceName, "%s", theName);
        snprintf(outInstance->tszProductName, sizeof outInstance->tszProductName, "%s", theName);
        return DI_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RunControlPanel(HWND, DWORD) override { return DI_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Initialize(HINSTANCE, DWORD, REFGUID) override { return DI_OK; }

    // Under gLock: records an event of this device's kind
    void Record(DWORD inOffset, DWORD inData)
    {
        if (!mAcquired)
            return;
        if (mBufferSize) {
            if (mBuffer.size() < mBufferSize)
                mBuffer.push_back({inOffset, inData, GetTickCount(), gSequence});
            else
                mOverflow = true;
        }
        if (mEvent)
            SetEvent(mEvent);
    }

    // Under gLock: moves a mouse axis
    void Move(DWORD inOffset, LONG inDelta)
    {
        if (!mAcquired)
            return;
        LONG& theAxis = inOffset == kMouseX ? mX : inOffset == kMouseY ? mY : mZ;
        theAxis += inDelta;
        Record(inOffset, DWORD(mAxisMode == DIPROPAXISMODE_ABS ? theAxis : inDelta));
    }

    // Under gLock: the focus moved into the top-level window inRoot (NULL when the game lost it)
    void FocusChanged(HWND inRoot)
    {
        if (mAcquired && (mCooperation & DISCL_FOREGROUND) && (!inRoot || inRoot != mRoot))
            mAcquired = false;
    }

    // Under gLock: whether this is an acquired exclusive mouse
    bool HoldsMouse() const { return mKind == kMouse && mAcquired && (mCooperation & DISCL_EXCLUSIVE); }

    const Kind mKind;

private:
    // A DIPROP_* property is a number in the place of a GUID's address (MAKEDIPROP). The compiler may take a
    // reference's address to be aligned, which would fold the small numbers away, so the asm hides where it came from.
    static uintptr_t PropertyNumber(REFGUID inProperty)
    {
        uintptr_t theNumber = reinterpret_cast<uintptr_t>(&inProperty);
        __asm__("" : "+r"(theNumber));
        return theNumber;
    }

    static bool ValidHeader(const DIPROPHEADER* inHeader)
    {
        return inHeader && inHeader->dwHeaderSize == sizeof(DIPROPHEADER) && inHeader->dwSize == sizeof(DIPROPDWORD);
    }

    static void UpdateRelative()
    {
        std::lock_guard<std::mutex> theGuard(gLock);
        bool theRelative = false;
        for (Device* theDevice : gDevices)
            theRelative = theRelative || theDevice->HoldsMouse();
        gRelative = theRelative;
    }

    std::atomic<long> mReferences{1};
    DWORD mDataSize = 0;
    HWND mRoot = NULL;               // the top-level window of the cooperative window
    DWORD mCooperation = DISCL_NONEXCLUSIVE | DISCL_BACKGROUND;
    bool mAcquired = false;
    DWORD mBufferSize = 0;
    std::deque<DIDEVICEOBJECTDATA_DX3> mBuffer;
    bool mOverflow = false;
    DWORD mAxisMode = DIPROPAXISMODE_REL;
    LONG mX = 0, mY = 0, mZ = 0;
    HANDLE mEvent = NULL;
};

class DirectInput final : public IDirectInputA {
public:
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirectInputA)) {
            *outObject = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        *outObject = this;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override
    {
        long theCount = --mReferences;
        if (theCount == 0)
            delete this;
        return ULONG(theCount);
    }
    // The system keyboard and the system mouse
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateDevice(REFGUID inDevice, LPDIRECTINPUTDEVICEA* outDevice,
                                                                LPUNKNOWN inOuter) override
    {
        if (!outDevice)
            return DIERR_INVALIDPARAM;
        *outDevice = nullptr;
        if (inOuter)
            return DIERR_NOAGGREGATION;
        Device* theDevice;
        if (IsEqualGUID(inDevice, GUID_SysKeyboard))
            theDevice = new Device(kKeyboard);
        else if (IsEqualGUID(inDevice, GUID_SysMouse))
            theDevice = new Device(kMouse);
        else
            return DIERR_DEVICENOTREG;
        std::lock_guard<std::mutex> theGuard(gLock);
        gDevices.insert(theDevice);
        *outDevice = theDevice;
        return DI_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumDevices(DWORD, LPDIENUMDEVICESCALLBACKA, LPVOID, DWORD) override
    {
        EE_PORT_UNSUPPORTED("IDirectInput::EnumDevices");
        return DIERR_UNSUPPORTED;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDeviceStatus(REFGUID inDevice) override
    {
        return IsEqualGUID(inDevice, GUID_SysKeyboard) || IsEqualGUID(inDevice, GUID_SysMouse) ? DI_OK
                                                                                               : DIERR_DEVICENOTREG;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RunControlPanel(HWND, DWORD) override { return DI_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Initialize(HINSTANCE, DWORD) override { return DI_OK; }

private:
    std::atomic<long> mReferences{1};
};

void RecordAll(Kind inKind, DWORD inOffset, DWORD inData)
{
    ++gSequence;
    for (Device* theDevice : gDevices)
        if (theDevice->mKind == inKind)
            theDevice->Record(inOffset, inData);
}

} // namespace

// The data formats are known by their size; nothing reads their objects
extern "C" const DIDATAFORMAT c_dfDIKeyboard = {sizeof(DIDATAFORMAT), sizeof(DIOBJECTDATAFORMAT), 0, 256, 0, nullptr};
extern "C" const DIDATAFORMAT c_dfDIMouse = {sizeof(DIDATAFORMAT), sizeof(DIOBJECTDATAFORMAT), DIDF_RELAXIS,
                                             sizeof(DIMOUSESTATE), 0, nullptr};

extern "C" HRESULT WINAPI DirectInputCreateA(HINSTANCE, DWORD, LPDIRECTINPUTA* outDirectInput, LPUNKNOWN inOuter)
{
    if (!outDirectInput)
        return DIERR_INVALIDPARAM;
    *outDirectInput = nullptr;
    if (inOuter)
        return DIERR_NOAGGREGATION;
    *outDirectInput = new DirectInput;
    return DI_OK;
}

// A key that does not change, such as SDL's repeats of a held key, records nothing, as in DirectInput
void eeport::InputKey(BYTE inCode, bool inDown)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (gKeys[inCode] == inDown)
        return;
    gKeys[inCode] = inDown;
    RecordAll(kKeyboard, inCode, inDown ? 0x80 : 0);
}

void eeport::InputMouseMove(int inX, int inY)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    for (int i = 0; i < 2; ++i) {
        int theDelta = i ? inY : inX;
        if (!theDelta)
            continue;
        ++gSequence;
        for (Device* theDevice : gDevices)
            if (theDevice->mKind == kMouse)
                theDevice->Move(i ? kMouseY : kMouseX, theDelta);
    }
}

void eeport::InputMouseWheel(int inNotches)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (!inNotches)
        return;
    ++gSequence;
    for (Device* theDevice : gDevices)
        if (theDevice->mKind == kMouse)
            theDevice->Move(kMouseZ, inNotches * WHEEL_DELTA);
}

void eeport::InputMouseButton(int inButton, bool inDown)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (inButton < 0 || inButton >= kMouseButtons || gButtons[inButton] == inDown)
        return;
    gButtons[inButton] = inDown;
    RecordAll(kMouse, kMouseButton0 + DWORD(inButton), inDown ? 0x80 : 0);
}

void eeport::InputFocus(HWND inFocus)
{
    HWND theRoot = Root(inFocus);
    std::lock_guard<std::mutex> theGuard(gLock);
    gFocusRoot = theRoot;
    if (!theRoot) {
        std::fill(std::begin(gKeys), std::end(gKeys), false);
        std::fill(std::begin(gButtons), std::end(gButtons), false);
    }
    bool theRelative = false;
    for (Device* theDevice : gDevices) {
        theDevice->FocusChanged(theRoot);
        theRelative = theRelative || theDevice->HoldsMouse();
    }
    gRelative = theRelative;
}

bool eeport::MouseHeld()
{
    return gRelative;
}

void eeport::ApplyMouseMode()
{
    bool theRelative = gRelative;
    if (bool(SDL_GetRelativeMouseMode()) != theRelative)
        SDL_SetRelativeMouseMode(theRelative ? SDL_TRUE : SDL_FALSE);
}
