// C:\EEX\Low-Level Engine\PIMModule.cpp (folder and file name inferred: sorts after Network Engine.cpp, before
// the Sound Engine's SECD.cpp)
// Unit flags (docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/PIMModule.h"

FSFileSpec PIMModule::sDirectory;
unsigned long PIMModule::sType;
static bool sInitialized;                   // 0x1012de44, set by PIMInitialize (provisional name)

// FUNCTION: LLE 0x1004b2b0
// Creates a plug-in module from a loaded DLL and its PIMGetName/PIMGetType/PIMIsValid entry points
PIMModule::PIMModule(HINSTANCE inInstance, const UString& (*inGetName)(), unsigned long (*inGetType)(),
                     bool (*inIsValid)())
{
    this->mInstance = inInstance;
    this->mGetName = inGetName;
    this->mGetType = inGetType;
    this->mIsValid = inIsValid;
}

// FUNCTION: LLE 0x1004b2d0
// Destroys the module, unloading its DLL
PIMModule::~PIMModule()
{
    FreeLibrary(this->mInstance);
}

// FUNCTION: LLE 0x1004b2da
// Loads the first (or next) *.dll in the plug-in directory that exports the PIM entry points and has the
// requested type; kErrorNoMoreFiles when none is left
long PIMModule::GetModule(bool inFirst, PIMModule*& outModule)
{
    FSFileSpec theFile;
    UString thePath(MAX_PATH);
    long theResult;
    bool theSearching;
    HINSTANCE theInstance;
    const UString& (*theGetName)();
    unsigned long (*theGetType)();
    bool (*theIsValid)();
    if (PIMModule::sDirectory.IsDirectory()) {
        outModule = NULL;
        theSearching = true;
        do {
            if (inFirst) {
                {
                    UString theFilter("*.dll");
                    theResult = PIMModule::sDirectory.GetFirstFile(theFilter, theFile);
                }
                inFirst = false;
            } else
                theResult = PIMModule::sDirectory.GetNextFile(theFile);
            if (theResult == kErrorOK) {
                thePath = theFile.GetVolume() + theFile.GetDirectory() + theFile.GetFile();
                theInstance = LoadLibraryEx(thePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
                UMSetFloatingPointPrecision(kFPPrecision53);
                if (theInstance != NULL) {
                    theGetName = (const UString& (*)())GetProcAddress(theInstance, "PIMGetName");
                    theGetType = (unsigned long (*)())GetProcAddress(theInstance, "PIMGetType");
                    theIsValid = (bool (*)())GetProcAddress(theInstance, "PIMIsValid");
                    if (theGetName != NULL && theGetType != NULL && theIsValid != NULL && PIMModule::sType == (*theGetType)()) {
                        outModule = new PIMModule(theInstance, theGetName, theGetType, theIsValid);
                        theResult = kErrorOK;
                        break;
                    }
                    FreeLibrary(theInstance);
                } else {
                    ESRecordNativeError(GetLastError());
                    ESGetNativeError();
                }
            } else {
                theSearching = false;
                theResult = kErrorNoMoreFiles;
            }
        } while (theSearching);
    } else
        theResult = kErrorFailed;
    return theResult;
}

static void PIMShutdown(bool inForced)
{
}

// FUNCTION: LLE 0x1004b4a4
// Marks the plug-in system initialised and registers its (empty) shutdown handler
long PIMInitialize()
{
    sInitialized = true;
    return URegisterShutdown(PIMShutdown);
}

// Compiler-generated for this unit (names in analysis/lle/names.txt):
// SYNTHETIC: LLE 0x1004b275
// SYNTHETIC: LLE 0x1004b27f
// SYNTHETIC: LLE 0x1004b289
// SYNTHETIC: LLE 0x1004b295
