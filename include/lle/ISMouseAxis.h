// C:\EEX\Low-Level Engine\Input System\ISMouseAxis.h (RECONSTRUCTION; ISMouseAxis.cpp 0x3bab2..0x3bafc). One mouse
// axis reading a field of ISMouse's current DIMOUSESTATE. sizeof 0x20 (??_E stride, 0x3931f).
#ifndef LLE_ISMOUSEAXIS_H
#define LLE_ISMOUSEAXIS_H

#include "lle/ISEntity.h"

class LLE_API ISMouseAxis : public ISEntity {
    friend class ISMouse;
public:
    virtual long GetDirectionNames(UString& outNegative, UString& outPositive) const { return kErrorFailed; }
    virtual const UWideString& GetName() const { return this->mName; }
    virtual long GetNormalizedValue() const { return *this->mValue / this->mGranularity; }
    virtual long GetValue() const { return *this->mValue; }
    virtual bool IsAnalog() const { return true; }
    virtual bool IsCalibrated() const { return true; }
    virtual bool JustDoublePressed() { return false; }
    virtual bool JustPressed() { return false; }
    virtual bool JustUnpressed() { return false; }
    virtual unsigned long JustPressedAmount() { return 0; }
    virtual unsigned long JustUnpressedAmount() { return 0; }
    virtual bool NeedsCalibration() const { return false; }
    virtual long SetCalibration(ISCalibration& inCalibration) { return kErrorFailed; }
    virtual long SetMode(unsigned long inMode) { return kErrorFailed; }
protected:
    virtual void CheckForDoublePress() {}
private:
    ISMouseAxis(const UWideString& inName, LONG* inValue, LONG& inGranularity);
    virtual ~ISMouseAxis();

    UWideString mName;                      // +0x04
    LONG* mValue;                           // +0x18
    LONG mGranularity;                      // +0x1c DIPROP_GRANULARITY
};

#endif
