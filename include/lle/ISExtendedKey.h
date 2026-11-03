// C:\EEX\Low-Level Engine\Input System\ISExtendedKey.h (RECONSTRUCTION; ISExtendedKey.cpp 0x39a8d..0x39b2a). A
// logical key that is down when either of two keys is (left/right Shift, ...). sizeof 0x6c (??_E stride, 0x3925a).
// Inline members emitted by Input System.cpp (0x39110..0x392b5).
#ifndef LLE_ISEXTENDEDKEY_H
#define LLE_ISEXTENDEDKEY_H

#include "lle/ISKey.h"

class LLE_API ISExtendedKey : public ISEntity {
    friend class ISKeyboard;
public:
    virtual long GetDirectionNames(UString& outNegative, UString& outPositive) const { return kErrorFailed; }
    virtual unsigned long GetIndex() const { return this->mIndex; }
    virtual const UWideString& GetName() const { return this->mName; }
    virtual long GetNormalizedValue() const { return this->mKey1.GetNormalizedValue() | this->mKey2.GetNormalizedValue(); }
    virtual long GetValue() const { return this->mKey1.GetValue() | this->mKey2.GetValue(); }
    virtual bool IsAnalog() const { return false; }
    virtual bool IsCalibrated() const { return false; }
    virtual bool JustDoublePressed() { return this->mKey1.JustDoublePressed() || this->mKey2.JustDoublePressed(); }
    virtual bool JustPressed() { return this->mKey1.JustPressed() || this->mKey2.JustPressed(); }
    virtual bool JustUnpressed() { return this->mKey1.JustUnpressed() || this->mKey2.JustUnpressed(); }
    virtual unsigned long JustPressedAmount() { return this->JustPressed() ? 1 : 0; }
    virtual unsigned long JustUnpressedAmount() { return this->JustUnpressed() ? 1 : 0; }
    virtual bool NeedsCalibration() const { return false; }
    virtual long SetCalibration(ISCalibration& inCalibration) { return kErrorFailed; }
    virtual long SetMode(unsigned long inMode) { return kErrorFailed; }
protected:
    virtual void CheckForDoublePress() {}
private:
    ISExtendedKey(const ISKey* inKey1, const ISKey* inKey2, unsigned long inIndex);
    virtual ~ISExtendedKey();

    UWideString mName;                      // +0x04 "<key1> and <key2>"
    ISKey mKey1;                            // +0x18
    ISKey mKey2;                            // +0x40
    unsigned long mIndex;                   // +0x68
};

#endif
