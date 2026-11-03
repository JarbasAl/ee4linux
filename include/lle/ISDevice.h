// C:\EEX\Low-Level Engine\Input System\ISDevice.h (RECONSTRUCTION; ISDevice.cpp 0x39715..0x39a8d). Abstract input
// device (keyboard, mouse); every device registers itself in sDevices. sizeof 0x18 (??_E stride, 0x38b89).
// Not reachable from LowLevelEngine.h, so sDevices needs no LOWLEVELENGINE_EXPORTS guard.
#ifndef LLE_ISDEVICE_H
#define LLE_ISDEVICE_H

#include <vector>

class ISEntity;

class LLE_API ISDevice {
public:
    static unsigned long GetCount() { return ISDevice::sDevices.size(); }  // 0x38b29
    static long GetDevice(unsigned long inIndex, ISDevice*& outDevice);
    virtual long GetEntity(unsigned long inIndex, ISEntity*& outEntity) const = 0;
    virtual unsigned long GetEntityCount() const = 0;
    virtual unsigned long GetFirstDigitalEntity() const = 0;
    const UString& GetName() const { return this->mName; }            // 0x38b41 (ICF)
    bool IsSuspended() const { return this->mSuspended; }             // 0x3d783 (ICF)
    virtual void Refresh() = 0;
    virtual void Resume() = 0;
    virtual long SetMode(unsigned long inMode) = 0;
    virtual void Suspend() = 0;
protected:
    ISDevice(const UString& inName);
    virtual ~ISDevice();

    bool mSuspended;                        // +0x04
    UString mName;                          // +0x08
private:
    static std::vector<ISDevice*> sDevices; // exported, 0x1012d960
};

#endif
