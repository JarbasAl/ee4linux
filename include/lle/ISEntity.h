// C:\EEX\Low-Level Engine\Input System\ISEntity.h (RECONSTRUCTION). Abstract input element (key, axis, button).
// No ISEntity.cpp: every member is inline (emitted first by Input System.cpp, 0x38abf..0x38b29). Virtual slot order
// proven by the vtables (ISEntity 0x98108: 14 pure virtuals, the dtor at +0x38, CheckForDoublePress at +0x3c).
#ifndef LLE_ISENTITY_H
#define LLE_ISENTITY_H

class UString;
class UWideString;
struct ISCalibration;

class LLE_API ISEntity {
    friend class ISKeyboard;                // the devices delete their entities (the dtor is protected)
    friend class ISMouse;
public:
    virtual long GetDirectionNames(UString& outNegative, UString& outPositive) const = 0;
    virtual const UWideString& GetName() const = 0;
    virtual long GetNormalizedValue() const = 0;
    virtual long GetValue() const = 0;
    virtual bool IsAnalog() const = 0;
    virtual bool IsCalibrated() const = 0;
    virtual bool JustDoublePressed() = 0;
    virtual bool JustPressed() = 0;
    virtual bool JustUnpressed() = 0;
    virtual unsigned long JustPressedAmount() = 0;
    virtual unsigned long JustUnpressedAmount() = 0;
    virtual bool NeedsCalibration() const = 0;
    virtual long SetCalibration(ISCalibration& inCalibration) = 0;
    virtual long SetMode(unsigned long inMode) = 0;
protected:
    ISEntity() {}                           // 0x38abf
    virtual ~ISEntity() {}                  // 0x38ac8
    virtual void CheckForDoublePress() = 0;
};

#endif
