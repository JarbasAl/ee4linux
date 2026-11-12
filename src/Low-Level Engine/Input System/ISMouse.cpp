// C:\EEX\Low-Level Engine\Input System\ISMouse.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1003b2ce..0x1003bab2: ISMouse's out-of-line members (the rest is header-inline, emitted by Input System.cpp).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/UWideString.h"
#include "lle/ISMouse.h"
#include "lle/ISMouseAxis.h"
#include "lle/ISMouseButton.h"

// FUNCTION: LLE 0x1003b2ce
// Creates the DirectInput mouse with X/Y/Z axes and up to four buttons (only those the device reports), left suspended
ISMouse::ISMouse(IDirectInputDevice* inDevice, HWND inWindow, const UString& inName)
    : ISDevice(inName)
{
    DIDEVICEOBJECTINSTANCE theInstance;
    DIPROPDWORD theProperty;
    this->mCurrentStatus = &this->mState;
    this->mLastStatus = &this->mLastState;
    memset(&this->mState, 0, sizeof(this->mState));
    memset(&this->mLastState, 0, sizeof(this->mLastState));
    this->mDevice = inDevice;
    this->mWindow = inWindow;
    this->mUseBufferedMovement = true;
    this->Resume();
    this->mEntities = new ISEntity*[7];
    memset(this->mEntities, 0, 7 * sizeof(ISEntity*));
    memset(this->mButtons, 0, sizeof(this->mButtons));
    memset(this->mLastButtons, 0, sizeof(this->mLastButtons));
    this->mOverflowed = false;
    theProperty.diph.dwSize = sizeof(DIPROPDWORD);
    theProperty.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    theProperty.diph.dwHow = DIPH_BYOFFSET;
    theInstance.dwSize = sizeof(DIDEVICEOBJECTINSTANCE);
    if (this->mDevice->GetObjectInfo(&theInstance, DIMOFS_X, DIPH_BYOFFSET) != DIERR_NOTFOUND) {
        theProperty.diph.dwObj = DIMOFS_X;
        this->mDevice->GetProperty(DIPROP_GRANULARITY, &theProperty.diph);
        this->mEntities[kMouseXAxis] = new ISMouseAxis("X Axis", &this->mState.lX, (LONG&)theProperty.dwData);
    }
    if (this->mDevice->GetObjectInfo(&theInstance, DIMOFS_Y, DIPH_BYOFFSET) != DIERR_NOTFOUND) {
        theProperty.diph.dwObj = DIMOFS_Y;
        this->mDevice->GetProperty(DIPROP_GRANULARITY, &theProperty.diph);
        this->mEntities[kMouseYAxis] = new ISMouseAxis("Y Axis", &this->mState.lY, (LONG&)theProperty.dwData);
    }
    if (this->mDevice->GetObjectInfo(&theInstance, DIMOFS_Z, DIPH_BYOFFSET) != DIERR_NOTFOUND) {
        theProperty.diph.dwObj = DIMOFS_Z;
        this->mDevice->GetProperty(DIPROP_GRANULARITY, &theProperty.diph);
        this->mEntities[kMouseZAxis] = new ISMouseAxis("Z Axis", &this->mState.lZ, (LONG&)theProperty.dwData);
    }
    if (this->mDevice->GetObjectInfo(&theInstance, DIMOFS_BUTTON0, DIPH_BYOFFSET) != DIERR_NOTFOUND)
        this->mEntities[kMouseLeftButton] = new ISMouseButton(this, 0, "Left button");
    if (this->mDevice->GetObjectInfo(&theInstance, DIMOFS_BUTTON1, DIPH_BYOFFSET) != DIERR_NOTFOUND)
        this->mEntities[kMouseRightButton] = new ISMouseButton(this, 1, "Right button");
    if (this->mDevice->GetObjectInfo(&theInstance, DIMOFS_BUTTON2, DIPH_BYOFFSET) != DIERR_NOTFOUND)
        this->mEntities[kMouseMiddleButton] = new ISMouseButton(this, 2, "Middle button");
    if (this->mDevice->GetObjectInfo(&theInstance, DIMOFS_BUTTON3, DIPH_BYOFFSET) != DIERR_NOTFOUND)
        this->mEntities[kMouseOtherButton] = new ISMouseButton(this, 3, "Other button");
    this->mEntityCount = 7;
    this->mFirstDigitalEntity = 3;
    this->Suspend();
}

// FUNCTION: LLE 0x1003b689
// Destroys the mouse and every axis and button entity it owns
ISMouse::~ISMouse()
{
    for (unsigned long i = 0; i < this->mEntityCount; i++)
        delete this->mEntities[i];
    delete[] this->mEntities;
}

// FUNCTION: LLE 0x1003b6c4
// Returns the axis or button entity at an index; kErrorNotFound for an absent one, kErrorBeyondEnd past the end
long ISMouse::GetEntity(unsigned long inIndex, ISEntity*& outEntity) const
{
    if (inIndex <= this->mEntityCount - 1) {
        outEntity = this->mEntities[inIndex];
        return outEntity == NULL ? kErrorNotFound : kErrorOK;
    }
    outEntity = NULL;
    return kErrorBeyondEnd;
}

// FUNCTION: LLE 0x1003b6f5
// Clears the mouse state, then reads the device's current buttons and position directly and flushes its buffer
void ISMouse::RecomputeCurrentButtonState()
{
    DWORD theCount = INFINITE;
    this->mCurrentStatus = &this->mState;
    this->mLastStatus = &this->mLastState;
    memset(&this->mState, 0, sizeof(this->mState));
    memset(&this->mLastState, 0, sizeof(this->mLastState));
    memset(this->mButtons, 0, sizeof(this->mButtons));
    memset(this->mLastButtons, 0, sizeof(this->mLastButtons));
    if (!(this->mDevice->GetDeviceState(sizeof(DIMOUSESTATE), this->mCurrentStatus) >= DI_OK))
        return;
    this->mDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), NULL, &theCount, 0);
    this->mButtons[0] = (this->mCurrentStatus->rgbButtons[0] & 0x80) == 0x80;
    this->mButtons[1] = (this->mCurrentStatus->rgbButtons[1] & 0x80) == 0x80;
    this->mButtons[2] = (this->mCurrentStatus->rgbButtons[2] & 0x80) == 0x80;
    this->mButtons[3] = (this->mCurrentStatus->rgbButtons[3] & 0x80) == 0x80;
    if (!this->mUseBufferedMovement) {
        this->mCurrentStatus->lX = 0;
        this->mCurrentStatus->lY = 0;
        this->mCurrentStatus->lZ = 0;
    }
    memcpy(this->mLastButtons, this->mButtons, sizeof(this->mButtons));
    memcpy(this->mLastStatus, this->mCurrentStatus, sizeof(DIMOUSESTATE));
}

// FUNCTION: LLE 0x1003b7cc
// Reads the buffered mouse events: position, wheel and button press/release counts, then updates double presses
void ISMouse::Refresh()
{
    DIDEVICEOBJECTDATA theData[256];
    DWORD theCount = 256;
    memcpy(this->mLastStatus, this->mCurrentStatus, sizeof(DIMOUSESTATE));
    memcpy(this->mLastButtons, this->mButtons, sizeof(this->mButtons));
    HRESULT theResult = this->mDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), theData, &theCount, 0);
    this->mCurrentStatus->lX = 0;
    this->mCurrentStatus->lY = 0;
    this->mCurrentStatus->lZ = 0;
    this->mOverflowed = false;
    if (theResult >= DI_OK && theCount > 0) {
        this->mOverflowed = true;
        if (theResult == DI_BUFFEROVERFLOW)
            this->RecomputeCurrentButtonState();
        else {
            DIMOUSESTATE theState;
            if (this->mDevice->GetDeviceState(sizeof(DIMOUSESTATE), &theState) >= DI_OK) {
                this->mCurrentStatus->lX = theState.lX;
                this->mCurrentStatus->lY = theState.lY;
            }
            for (unsigned long i = 0; i < theCount; i++) {
                DWORD theValue = theData[i].dwData;
                switch (theData[i].dwOfs) {
                case DIMOFS_BUTTON3:
                    this->mCurrentStatus->rgbButtons[3] = (BYTE)theValue;
                    if (((theValue & 0x80) == 0x80 && !(this->mButtons[3] & 1)) || (theValue == 0 && (this->mButtons[3] & 1)))
                        this->mButtons[3]++;
                    break;
                case DIMOFS_BUTTON2:
                    this->mCurrentStatus->rgbButtons[2] = (BYTE)theValue;
                    if (((theValue & 0x80) == 0x80 && !(this->mButtons[2] & 1)) || (theValue == 0 && (this->mButtons[2] & 1)))
                        this->mButtons[2]++;
                    break;
                case DIMOFS_BUTTON1:
                    this->mCurrentStatus->rgbButtons[1] = (BYTE)theValue;
                    if (((theValue & 0x80) == 0x80 && !(this->mButtons[1] & 1)) || (theValue == 0 && (this->mButtons[1] & 1)))
                        this->mButtons[1]++;
                    break;
                case DIMOFS_BUTTON0:
                    this->mCurrentStatus->rgbButtons[0] = (BYTE)theValue;
                    if (((theValue & 0x80) == 0x80 && !(this->mButtons[0] & 1)) || (theValue == 0 && (this->mButtons[0] & 1)))
                        this->mButtons[0]++;
                    break;
                case DIMOFS_Z:
                    this->mCurrentStatus->lZ += theValue;
                    break;
                }
            }
            if (!this->mUseBufferedMovement) {
                this->mCurrentStatus->lX = 0;
                this->mCurrentStatus->lY = 0;
                this->mCurrentStatus->lZ = 0;
            }
        }
    }
    for (int j = 0; j < 7; j++)
        if (this->mEntities[j])
            this->mEntities[j]->CheckForDoublePress();
}

// FUNCTION: LLE 0x1003b9ba
// Reads the current cursor movement from the device (zeroed when buffered movement is off)
void ISMouse::RefreshXY()
{
    DIMOUSESTATE theState;
    if (this->mDevice->GetDeviceState(sizeof(DIMOUSESTATE), &theState) >= DI_OK) {
        this->mCurrentStatus->lX = theState.lX;
        this->mCurrentStatus->lY = theState.lY;
    }
    if (!this->mUseBufferedMovement) {
        this->mCurrentStatus->lX = 0;
        this->mCurrentStatus->lY = 0;
    }
}

// FUNCTION: LLE 0x1003b9fc
// Takes exclusive foreground control of the mouse and re-reads its state; records the native error on failure
void ISMouse::Resume()
{
    HRESULT theResult = this->mDevice->SetCooperativeLevel(this->mWindow, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    if (theResult == DI_OK) {
        theResult = this->mDevice->Acquire();
        if (theResult == DI_OK || theResult == S_FALSE) {
            this->mSuspended = false;
            this->RecomputeCurrentButtonState();
            return;
        }
    }
    ESRecordNativeError(theResult);
}

// FUNCTION: LLE 0x1003ba39
// Sets the mouse axis mode (0 relative, 1 absolute); kErrorFailed for any other mode
long ISMouse::SetMode(unsigned long inMode)
{
    long theResult = kErrorOK;
    if (inMode != 0 && inMode != 1)
        theResult = kErrorFailed;
    else {
        DIPROPDWORD theProperty;
        theProperty.diph.dwSize = sizeof(DIPROPDWORD);
        theProperty.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        theProperty.diph.dwObj = 0;
        theProperty.diph.dwHow = DIPH_DEVICE;
        theProperty.dwData = inMode != 0;
        this->mDevice->SetProperty(DIPROP_AXISMODE, &theProperty.diph);
    }
    return theResult;
}

// FUNCTION: LLE 0x1003ba87
// Releases the mouse to non-exclusive use and marks it suspended; records the native error on failure
void ISMouse::Suspend()
{
    this->mDevice->Unacquire();
    this->mSuspended = true;
    HRESULT theResult = this->mDevice->SetCooperativeLevel(this->mWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
    if (theResult != DI_OK)
        ESRecordNativeError(theResult);
}
