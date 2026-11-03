// EEPersistentTemplates.h (RECONSTRUCTION; header and template names from RTTI, the rest inferred). The saved-game
// persistents Empire Earth.cpp registers with FSPersistentList (0x13fa74: tags 2, 4, 3, 6, 0xa, ...): a plain value
// (EEPTGeneric<unsigned long>, vtable 0x442158), a value with its own Load/Save
// (EEPTRestorable<GE3DPoint>, 0x442194) and an owned pointer to a restorable game object (EEPTPRestorable<EEMap>,
// <EEScenarioManager>, <EEEffectManager>, <EEEnvironmentManager>; 0x442138 / 0x442118 / 0x4420f8 / 0x4420d8). Every
// member is inline: the bodies are Empire Earth.cpp's deferred COMDATs (0x144aa0..), compiled without /Og there.
// Requires lle/FSPersistent.h, lle/FSFile.h.
#ifndef EE_EEPERSISTENTTEMPLATES_H
#define EE_EEPERSISTENTTEMPLATES_H

template <class T> class EEPTGeneric : public FSPersistent {
public:
    EEPTGeneric(long inTagID) : FSPersistent(inTagID) { this->mValue = 0; }
    // with a value (EESaveLoad 0x14c81d: the value is read before FSPersistent's ctor, stored after the vptr)
    EEPTGeneric(const T& inValue, long inTagID) : FSPersistent(inTagID) { this->mValue = inValue; }
    virtual void GetChecksum(ESChecksum& ioChecksum) const {}
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
    {
        long theResult = kErrorOK;
        unsigned long theRead = 0;
        theResult = inFile.Read((unsigned char*)&this->mValue, sizeof(T), theRead);
        if (theResult == kErrorOK && theRead != sizeof(T))
            theResult = 0xe;
        return theResult;
    }
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile)
    {
        long theResult = kErrorOK;
        unsigned long theWritten = 0;
        theResult = outFile.Write((const unsigned char*)&this->mValue, sizeof(T), theWritten);
        if (theResult == kErrorOK && theWritten != sizeof(T))
            theResult = 0xe;
        return theResult;
    }
    virtual FSPersistent* CreateDefault() const { return new EEPTGeneric<T>(this->GetTagID()); }

    T mValue;                               // +0x08
};

template <class T> class EEPTRestorable : public FSPersistent {
public:
    EEPTRestorable(const T& inValue, long inTagID) : FSPersistent(inTagID) { this->mValue = inValue; }
    virtual void GetChecksum(ESChecksum& ioChecksum) const {}
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
    {
        long theResult = kErrorOK;
        theResult = this->mValue.Load(inFile);
        return theResult;
    }
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile)
    {
        long theResult = kErrorOK;
        theResult = this->mValue.Save(outFile);
        return theResult;
    }
    virtual FSPersistent* CreateDefault() const { return new EEPTRestorable<T>(T(), this->GetTagID()); }

    T mValue;                               // +0x08
};

template <class T> class EEPTPRestorable : public FSPersistent {
public:
    EEPTPRestorable(long inTagID) : FSPersistent(inTagID)
    {
        this->mObject = NULL;
        this->mOwned = true;
    }
    EEPTPRestorable(T* inObject, long inTagID) : FSPersistent(inTagID)
    {
        this->mObject = inObject;
        this->mOwned = true;
    }
    virtual ~EEPTPRestorable()
    {
        if (this->mOwned) {
            delete this->mObject;
            this->mObject = NULL;
        }
    }
    virtual void GetChecksum(ESChecksum& ioChecksum) const { this->mObject->GetChecksum(ioChecksum); }
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
    {
        long theResult = kErrorOK;
        theResult = this->mObject->Load(inMajorVersion, inMinorVersion, inFile);
        return theResult;
    }
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile)
    {
        long theResult = kErrorOK;
        theResult = this->mObject->Save(inMajorVersion, inMinorVersion, outFile);
        return theResult;
    }
    virtual FSPersistent* CreateDefault() const { return new EEPTPRestorable<T>(new T, this->GetTagID()); }

    T* mObject;                             // +0x08
    bool mOwned;                            // +0x0c
};

// 0x14a61f (cdecl; a Persistent unit, 0x14a009..0x14ac35): reads the saved game or scenario inFile and returns its
// persistent of tag inTagID (EECampaign::ListEffectSounds 0x28d219 passes 6, the scenario manager); keseOK when
// found. Name is provisional.
long EELoadPersistent(const UString& inFile, long inTagID, FSPersistent*& outPersistent);

#endif
