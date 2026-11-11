// C:\EEX\Low-Level Engine\PIMModule.h (RECONSTRUCTION; folder inferred; PIMModule.cpp 0x4b275..0x4b4b7). A loaded
// plug-in module: a DLL found in a directory whose PIMGetType matches the requested type. Engine-only: its static
// data members would cost the plug-ins `$E` numbers, so lle/LowLevelEngine.h does not include it.
// Inline members: exported COMDATs emitted by the first includer in link order (0x38647..0x386aa).
#ifndef LLE_PIMMODULE_H
#define LLE_PIMMODULE_H

class LLE_API PIMModule {                  // proven sizeof 0x10 (GetModule: operator new(0x10)); operator= implicit
public:
    void* GetEntry(const UString& inName) const { return (void*)GetProcAddress(this->mInstance, inName); }
#if defined(LOWLEVELENGINE_EXPORTS) || defined(LLE_EXE_HEADERS)   // the exe stores __imp_sDirectory/sType and calls GetModule inline (0x28655a)
    static long GetFirstModule(const FSFileSpec& inDirectory, unsigned long inType, PIMModule*& outModule)
    {
        PIMModule::sDirectory = inDirectory;
        PIMModule::sType = inType;
        return PIMModule::GetModule(true, outModule);
    }
#else
    static long GetFirstModule(const FSFileSpec& inDirectory, unsigned long inType, PIMModule*& outModule);
#endif
    const UString& GetName() const { return (*this->mGetName)(); }
    static long GetNextModule(PIMModule*& outModule) { return PIMModule::GetModule(false, outModule); }
    unsigned long GetType() const { return (*this->mGetType)(); }
    bool IsValid() const { return (*this->mIsValid)(); }
    void Unload() { delete this; }

private:
    PIMModule(HINSTANCE inInstance, const UString& (*inGetName)(), unsigned long (*inGetType)(),
              bool (*inIsValid)());
    ~PIMModule();
    static long GetModule(bool inFirst, PIMModule*& outModule);

    // Inferred engine-only visibility ($E numbering of the plug-ins; see TSScheduler.h)
#if defined(LOWLEVELENGINE_EXPORTS) || defined(LLE_EXE_HEADERS)   // exe only: the renderers see PIMModule (SEMixer.h) and number their $E chains
    // as if they did not see these
    static FSFileSpec sDirectory;           // exported ?sDirectory@PIMModule@@0VFSFileSpec@@A (0x1012ddf8)
    static unsigned long sType;             // exported ?sType@PIMModule@@0KA (0x1012de40)
#endif

    const UString& (*mGetName)();           // +0x00 PIMGetName
    unsigned long (*mGetType)();            // +0x04 PIMGetType
    bool (*mIsValid)();                     // +0x08 PIMIsValid
    HINSTANCE mInstance;                    // +0x0c
};

LLE_API long PIMInitialize();

#endif
