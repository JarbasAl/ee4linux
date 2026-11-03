// C:\EEX\Low-Level Engine\Input System\ISMouseButton.h (RECONSTRUCTION; ISMouseButton.cpp 0x3bafc..0x3bb89). One
// mouse button. sizeof 0x28 (??_E stride, 0x3944d); same layout as ISKey (their operator= ICF-fold at 0x39083).
#ifndef LLE_ISMOUSEBUTTON_H
#define LLE_ISMOUSEBUTTON_H

#include "lle/ISEntity.h"
#include "lle/ISMouse.h"

class LLE_API ISMouseButton : public ISEntity {
    friend class ISMouse;
public:
    virtual long GetDirectionNames(UString& outNegative, UString& outPositive) const { return kErrorFailed; }
    virtual const UWideString& GetName() const { return this->mName; }
    virtual long GetNormalizedValue() const { return this->GetValue() ? 0x7fff : 0; }
    virtual long GetValue() const { return this->mMouse->mButtons[this->mIndex] & 1; }
    virtual bool IsAnalog() const { return false; }
    virtual bool IsCalibrated() const { return false; }
    virtual bool JustDoublePressed() { return this->mDoublePressed; }
    virtual bool JustPressed() { return this->JustPressedAmount() > 0; }
    virtual bool JustUnpressed() { return this->JustUnpressedAmount() > 0; }
    virtual unsigned long JustPressedAmount();
    virtual unsigned long JustUnpressedAmount();
    virtual bool NeedsCalibration() const { return false; }
    virtual long SetCalibration(ISCalibration& inCalibration) { return kErrorFailed; }
    virtual long SetMode(unsigned long inMode) { return kErrorFailed; }
protected:
    virtual void CheckForDoublePress();
private:
    ISMouseButton(ISMouse* inMouse, unsigned long inIndex, const UWideString& inName);
    virtual ~ISMouseButton();

    UWideString mName;                      // +0x04
    ISMouse* mMouse;                        // +0x18
    unsigned long mIndex;                   // +0x1c button number
    unsigned long mLastPressTime;           // +0x20
    bool mDoublePressed;                    // +0x24
};

inline unsigned long ISMouseButton::JustPressedAmount()
{
    unsigned char theCurrent = this->mMouse->mButtons[this->mIndex];
    unsigned char theChanges = theCurrent - this->mMouse->mLastButtons[this->mIndex];
    if (!(theChanges & 1))
        return theChanges / 2;
    if (theCurrent & 1)
        return (theChanges - 1) / 2 + 1;
    return (theChanges - 1) / 2;
}

inline unsigned long ISMouseButton::JustUnpressedAmount()
{
    unsigned char theCurrent = this->mMouse->mButtons[this->mIndex];
    unsigned char theChanges = theCurrent - this->mMouse->mLastButtons[this->mIndex];
    if (!(theChanges & 1))
        return theChanges / 2;
    if (!(theCurrent & 1))
        return (theChanges - 1) / 2 + 1;
    return (theChanges - 1) / 2;
}

#endif
