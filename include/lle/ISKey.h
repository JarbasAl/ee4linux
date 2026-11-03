// C:\EEX\Low-Level Engine\Input System\ISKey.h (RECONSTRUCTION; ISKey.cpp 0x39b2a..0x39ca1). One keyboard key.
// sizeof 0x28 (??_E stride, 0x390b6). Inline members emitted by Input System.cpp (0x38faf..0x390b6).
#ifndef LLE_ISKEY_H
#define LLE_ISKEY_H

#include "lle/ISEntity.h"
#include "lle/ISKeyboard.h"

class LLE_API ISKey : public ISEntity {
    friend class ISExtendedKey;
    friend class ISKeyboard;
public:
    virtual long GetDirectionNames(UString& outNegative, UString& outPositive) const { return kErrorFailed; }
    virtual unsigned long GetIndex() const { return this->mIndex; }
    virtual const UWideString& GetName() const { return this->mName; }
    virtual long GetNormalizedValue() const { return this->GetValue() ? 0x7fff : 0; }
    virtual long GetValue() const { return this->mKeyboard->GetCurrentStatus()[this->mIndex] & 1; }
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
    ISKey(ISKeyboard* inKeyboard, unsigned long inIndex, const UWideString& inName);
    ISKey(const ISKey* inKey);
    virtual ~ISKey();

    UWideString mName;                      // +0x04
    ISKeyboard* mKeyboard;                  // +0x18
    unsigned long mIndex;                   // +0x1c DIK_* scan code
    unsigned long mLastPressTime;           // +0x20 TMGetTime of the last press (0: none pending)
    bool mDoublePressed;                    // +0x24
};

// Pressed/released transitions since the last Refresh: each key's status byte counts transitions (bit 0 = down)
inline unsigned long ISKey::JustPressedAmount()
{
    unsigned char theCurrent = this->mKeyboard->GetCurrentStatus()[this->mIndex];
    unsigned char theChanges = theCurrent - this->mKeyboard->GetLastStatus()[this->mIndex];
    if (!(theChanges & 1))
        return theChanges / 2;
    if (theCurrent & 1)
        return (theChanges - 1) / 2 + 1;
    return (theChanges - 1) / 2;
}

inline unsigned long ISKey::JustUnpressedAmount()
{
    unsigned char theCurrent = this->mKeyboard->GetCurrentStatus()[this->mIndex];
    unsigned char theChanges = theCurrent - this->mKeyboard->GetLastStatus()[this->mIndex];
    if (!(theChanges & 1))
        return theChanges / 2;
    if (!(theCurrent & 1))
        return (theChanges - 1) / 2 + 1;
    return (theChanges - 1) / 2;
}

#endif
