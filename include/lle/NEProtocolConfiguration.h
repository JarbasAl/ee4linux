// C:\EEX\Low-Level Engine\Network Engine\NEProtocolConfiguration.h (RECONSTRUCTION; NEProtocolConfiguration.cpp
// 0x44c71..0x45535). A protocol's parameter table: each parameter has a name and either an arbitrary string value
// or a list of discrete named values. Requires UString and FSFileSpec (lle/LowLevelEngine.h).
#ifndef LLE_NEPROTOCOLCONFIGURATION_H
#define LLE_NEPROTOCOLCONFIGURATION_H

// Not exported: its ctor (0x44cda) and vector deleting dtor (0x44d02) are local COMDATs. Name is provisional.
struct NEProtocolParameter {                // proven sizeof 0x30 (new[] count * 0x30 + 4)
    NEProtocolParameter() : mName(64), mValue(64)
    {
        this->mDiscreteValueNames = NULL;
        this->mDiscreteValueCount = 0;
        this->mDiscreteValue = 0;
        this->mIsDiscrete = false;
    }

    UString* mDiscreteValueNames;           // +0x00 new UString[mDiscreteValueCount] (capacity 64 each)
    UString mName;                          // +0x04
    UString mValue;                         // +0x14 the arbitrary value (unused when discrete)
    unsigned long mDiscreteValueCount;      // +0x24
    unsigned long mDiscreteValue;           // +0x28 index of the selected discrete value
    bool mIsDiscrete;                       // +0x2c
};

class LLE_API NEProtocolConfiguration {    // proven sizeof 0xc (TCP NPIMGetConfiguration: operator new(0xc))
public:
    // no user-declared operator=: the exported one is the implicit member-wise copy (ICF-folded at 0x12ad2)
    NEProtocolConfiguration();
    NEProtocolConfiguration(unsigned long inParameterCount, unsigned long inProtocolType);
    NEProtocolConfiguration(const NEProtocolConfiguration& inConfiguration);
    ~NEProtocolConfiguration();

    // proven inline: exported COMDATs (GetProtocolType 0x44c71 at the head of this unit, GetParameterCount folded
    // at 0x4380); TCP NPIMSetConfiguration reads the fields directly
    unsigned long GetParameterCount() const { return this->mParameterCount; }
    unsigned long GetProtocolType() const { return this->mProtocolType; }
    long GetParameterDiscreteValue(unsigned long inParameter, unsigned long& outValue) const;
    long GetParameterDiscreteValueCount(unsigned long inParameter, unsigned long& outCount) const;
    long GetParameterDiscreteValueName(unsigned long inParameter, unsigned long inValue, UString& outName) const;
    long GetParameterName(unsigned long inParameter, UString& outName) const;
    long GetParameterValue(unsigned long inParameter, UString& outValue) const;
    long IsParameterDiscrete(unsigned long inParameter, bool& outIsDiscrete) const;
    long Load(FSFileSpec& inFile);
    long Save(FSFileSpec& inFile) const;
    long SetParameterDiscreteValue(unsigned long inParameter, unsigned long inValue);
    long SetParameterName(unsigned long inParameter, const UString& inName);
    long SetParameterToArbitrary(unsigned long inParameter);
    long SetParameterToDiscrete(unsigned long inParameter, unsigned long inCount, UString* inNames);
    long SetParameterValue(unsigned long inParameter, const UString& inValue);

private:
    NEProtocolParameter* mParameters;       // +0x00 new NEProtocolParameter[mParameterCount]
    unsigned long mParameterCount;          // +0x04
    unsigned long mProtocolType;            // +0x08
};

#endif
