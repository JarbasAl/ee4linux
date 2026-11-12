// Modules: an ELF shared object is loaded with dlopen; a Windows PE file (Language.dll, a retail plug-in, the retail
// exe) becomes a module of resources alone, with no code. The engine's plug-in scan loads every *.dll in the game
// folder and keeps the ones that export its entry points, so a retail plug-in next to the port's is skipped.
//
// The main module's resources (the start screen's bitmap, the cursors) are the retail exe's: EE_PORT_EXE_RESOURCES
// names it, else EE-AOC.exe in the folder the game starts in.
#include "Win32Internal.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <string>
#include <unistd.h>
#include <vector>

namespace eeport {

struct Module {
    std::string mPath;
    void* mLibrary = NULL;                  // an ELF module's dlopen handle
    const char* mBuiltIn = NULL;            // a system DLL the layer implements: its name
    std::vector<unsigned char> mImage;      // a PE module's file
    DWORD mResourceRVA = 0;                 // the resource directory, by RVA
    struct Section {
        DWORD mRVA, mSize, mFileOffset;
    };
    std::vector<Section> mSections;
    int mReferences = 1;
};

static std::mutex sModulesLock;

// The system DLLs the game loads by name and the layer implements: LoadLibrary gives a module whose functions are
// the layer's own. WON finds AlphaBlend this way.
struct BuiltInFunction {
    const char* mModule;
    const char* mName;
    void* mAddress;
};
static const BuiltInFunction kBuiltInFunctions[] = {
    {"msimg32.dll", "AlphaBlend", reinterpret_cast<void*>(&AlphaBlend)},
};

// The built-in module a LoadLibrary name means, or NULL: the name without its folder, compared without case
static const char* BuiltIn(LPCSTR inName)
{
    if (!inName)
        return NULL;
    const char* theBase = inName;
    for (const char* c = inName; *c; ++c)
        if (*c == '\\' || *c == '/')
            theBase = c + 1;
    for (const BuiltInFunction& theFunction : kBuiltInFunctions)
        if (strcasecmp(theBase, theFunction.mModule) == 0)
            return theFunction.mModule;
    return NULL;
}

static DWORD Read32(const std::vector<unsigned char>& inImage, size_t inOffset)
{
    if (inOffset + 4 > inImage.size())
        return 0;
    DWORD theValue;
    memcpy(&theValue, &inImage[inOffset], 4);
    return theValue;
}

static WORD Read16(const std::vector<unsigned char>& inImage, size_t inOffset)
{
    if (inOffset + 2 > inImage.size())
        return 0;
    WORD theValue;
    memcpy(&theValue, &inImage[inOffset], 2);
    return theValue;
}

// The file offset of an RVA, or 0 when no section holds it
static size_t FileOffset(const Module* inModule, DWORD inRVA)
{
    for (const Module::Section& theSection : inModule->mSections)
        if (inRVA >= theSection.mRVA && inRVA < theSection.mRVA + theSection.mSize)
            return theSection.mFileOffset + (inRVA - theSection.mRVA);
    return 0;
}

// Reads a PE file's sections and resource directory; false when the bytes are not a 32-bit PE image
static bool ParsePE(Module* ioModule)
{
    const std::vector<unsigned char>& theImage = ioModule->mImage;
    if (theImage.size() < 0x40 || theImage[0] != 'M' || theImage[1] != 'Z')
        return false;
    DWORD thePE = Read32(theImage, 0x3c);
    if (Read32(theImage, thePE) != 0x00004550)      // "PE\0\0"
        return false;
    WORD theSections = Read16(theImage, thePE + 6);
    WORD theOptionalSize = Read16(theImage, thePE + 20);
    size_t theOptional = thePE + 24;
    if (Read16(theImage, theOptional) != 0x10b)     // PE32
        return false;
    ioModule->mResourceRVA = theOptionalSize >= 120 ? Read32(theImage, theOptional + 112) : 0;
    size_t theTable = theOptional + theOptionalSize;
    for (WORD i = 0; i < theSections; ++i) {
        size_t theEntry = theTable + i * 40u;
        Module::Section theSection;
        DWORD theVirtualSize = Read32(theImage, theEntry + 8);
        theSection.mRVA = Read32(theImage, theEntry + 12);
        DWORD theRawSize = Read32(theImage, theEntry + 16);
        theSection.mFileOffset = Read32(theImage, theEntry + 20);
        theSection.mSize = theVirtualSize > theRawSize ? theVirtualSize : theRawSize;
        ioModule->mSections.push_back(theSection);
    }
    return true;
}

// A resource directory's entries: each name (an id, or a string offset with the high bit set) and target
struct ResourceEntry {
    DWORD mName;
    DWORD mTarget;
};

static std::vector<ResourceEntry> DirectoryEntries(const Module* inModule, DWORD inDirectory)
{
    std::vector<ResourceEntry> theEntries;
    size_t theBase = FileOffset(inModule, inModule->mResourceRVA);
    if (!theBase)
        return theEntries;
    size_t theDirectory = theBase + inDirectory;
    WORD theCount = (WORD)(Read16(inModule->mImage, theDirectory + 12) + Read16(inModule->mImage, theDirectory + 14));
    for (WORD i = 0; i < theCount; ++i) {
        size_t theEntry = theDirectory + 16 + i * 8u;
        theEntries.push_back({Read32(inModule->mImage, theEntry), Read32(inModule->mImage, theEntry + 4)});
    }
    return theEntries;
}

// A resource name entry as a string, upper case as Windows compares them
static std::string EntryString(const Module* inModule, DWORD inName)
{
    size_t theOffset = FileOffset(inModule, inModule->mResourceRVA) + (inName & 0x7fffffff);
    WORD theLength = Read16(inModule->mImage, theOffset);
    std::string theText;
    for (WORD i = 0; i < theLength; ++i) {
        WORD c = Read16(inModule->mImage, theOffset + 2 + i * 2u);
        theText += (char)toupper(c < 256 ? c : '?');
    }
    return theText;
}

static bool NameMatches(const Module* inModule, DWORD inEntryName, LPCSTR inName)
{
    if (IS_INTRESOURCE(inName))
        return !(inEntryName & 0x80000000) && inEntryName == (DWORD)(ULONG_PTR)inName;
    if (!(inEntryName & 0x80000000))
        return inName[0] == '#' && inEntryName == (DWORD)atoi(inName + 1);
    std::string theWanted = inName;
    for (char& c : theWanted)
        c = (char)toupper((unsigned char)c);
    return EntryString(inModule, inEntryName) == theWanted;
}

// The subdirectory of an entry that matches inName, as an offset in the resource section, or 0
static DWORD FindEntry(const Module* inModule, DWORD inDirectory, LPCSTR inName)
{
    for (const ResourceEntry& theEntry : DirectoryEntries(inModule, inDirectory))
        if (NameMatches(inModule, theEntry.mName, inName))
            return theEntry.mTarget;
    return 0;
}

static Module* ModuleOf(HMODULE inModule) { return reinterpret_cast<Module*>(inModule); }

static Module* sMain;

// The main module, made on first use from the retail exe's resources
static Module* MainModule()
{
    std::lock_guard<std::mutex> theLock(sModulesLock);
    if (!sMain) {
        sMain = new Module;
        const char* thePath = getenv("EE_PORT_EXE_RESOURCES");
        sMain->mPath = thePath && *thePath ? thePath : "EE-AOC.exe";
        if (FILE* theFile = fopen(LinuxPath(sMain->mPath.c_str()).c_str(), "rb")) {
            fseek(theFile, 0, SEEK_END);
            sMain->mImage.resize((size_t)ftell(theFile));
            fseek(theFile, 0, SEEK_SET);
            if (fread(sMain->mImage.data(), 1, sMain->mImage.size(), theFile) != sMain->mImage.size())
                sMain->mImage.clear();
            fclose(theFile);
        }
        if (!ParsePE(sMain)) {
            Log("the main module has no resources: %s is not a readable PE file", sMain->mPath.c_str());
            sMain->mImage.clear();
        }
    }
    return sMain;
}

const void* FindResourceData(HMODULE inModule, LPCSTR inType, LPCSTR inName, WORD inLanguage, DWORD* outSize)
{
    Module* theModule = inModule ? ModuleOf(inModule) : MainModule();
    if (theModule->mImage.empty() || !theModule->mResourceRVA)
        return NULL;
    DWORD theType = FindEntry(theModule, 0, inType);
    if (!(theType & 0x80000000))
        return NULL;
    DWORD theName = FindEntry(theModule, theType & 0x7fffffff, inName);
    if (!(theName & 0x80000000))
        return NULL;
    std::vector<ResourceEntry> theLanguages = DirectoryEntries(theModule, theName & 0x7fffffff);
    if (theLanguages.empty())
        return NULL;
    const ResourceEntry* theChoice = &theLanguages[0];
    for (const ResourceEntry& theEntry : theLanguages)
        if (inLanguage && theEntry.mName == inLanguage)
            theChoice = &theEntry;
    size_t theData = FileOffset(theModule, theModule->mResourceRVA) + (theChoice->mTarget & 0x7fffffff);
    size_t theBytes = FileOffset(theModule, Read32(theModule->mImage, theData));
    DWORD theSize = Read32(theModule->mImage, theData + 4);
    if (!theBytes || theBytes + theSize > theModule->mImage.size())
        return NULL;
    if (outSize)
        *outSize = theSize;
    return &theModule->mImage[theBytes];
}

} // namespace eeport

using namespace eeport;

extern "C" {

HMODULE WINAPI LoadLibraryExA(LPCSTR inName, HANDLE inFile, DWORD inFlags)
{
    (void)inFile, (void)inFlags;
    Module* theModule = new Module;
    theModule->mBuiltIn = BuiltIn(inName);
    if (theModule->mBuiltIn)
        return reinterpret_cast<HMODULE>(theModule);
    theModule->mPath = LinuxPath(inName);
    unsigned char theMagic[4] = {};
    FILE* theFile = fopen(theModule->mPath.c_str(), "rb");
    if (!theFile) {
        delete theModule;
        SetLastError(ERROR_MOD_NOT_FOUND);
        return NULL;
    }
    size_t theRead = fread(theMagic, 1, 4, theFile);
    if (theRead == 4 && memcmp(theMagic, "\x7f" "ELF", 4) == 0) {
        fclose(theFile);
        theModule->mLibrary = dlopen(theModule->mPath.c_str(), RTLD_NOW | RTLD_LOCAL);
        if (!theModule->mLibrary) {
            Log("LoadLibrary(\"%s\"): %s", inName, dlerror());
            delete theModule;
            SetLastError(ERROR_DLL_INIT_FAILED);
            return NULL;
        }
        return reinterpret_cast<HMODULE>(theModule);
    }
    fseek(theFile, 0, SEEK_END);
    theModule->mImage.resize((size_t)ftell(theFile));
    fseek(theFile, 0, SEEK_SET);
    theRead = fread(theModule->mImage.data(), 1, theModule->mImage.size(), theFile);
    fclose(theFile);
    if (theRead != theModule->mImage.size() || !ParsePE(theModule)) {
        delete theModule;
        SetLastError(ERROR_BAD_EXE_FORMAT);
        return NULL;
    }
    return reinterpret_cast<HMODULE>(theModule);
}

HMODULE WINAPI LoadLibraryA(LPCSTR inName)
{
    return LoadLibraryExA(inName, NULL, 0);
}

BOOL WINAPI FreeLibrary(HMODULE inModule)
{
    Module* theModule = ModuleOf(inModule);
    if (!theModule || theModule == sMain)
        return theModule != NULL;
    if (theModule->mLibrary)
        dlclose(theModule->mLibrary);
    delete theModule;
    return TRUE;
}

// The main module for NULL; a system DLL is not a module here, so callers take their fallbacks
HMODULE WINAPI GetModuleHandleA(LPCSTR inName)
{
    if (!inName)
        return reinterpret_cast<HMODULE>(MainModule());
    SetLastError(ERROR_MOD_NOT_FOUND);
    return NULL;
}

FARPROC WINAPI GetProcAddress(HMODULE inModule, LPCSTR inName)
{
    Module* theModule = ModuleOf(inModule);
    void* theAddress = NULL;
    if (theModule && theModule->mBuiltIn && !IS_INTRESOURCE(inName)) {
        for (const BuiltInFunction& theFunction : kBuiltInFunctions)
            if (theFunction.mModule == theModule->mBuiltIn && strcmp(theFunction.mName, inName) == 0)
                theAddress = theFunction.mAddress;
    } else if (theModule && theModule->mLibrary && !IS_INTRESOURCE(inName))
        theAddress = dlsym(theModule->mLibrary, inName);
    if (!theAddress)
        SetLastError(ERROR_PROC_NOT_FOUND);
    return reinterpret_cast<FARPROC>(theAddress);
}

// A string of a string table, in the module's first language, as ANSI: Windows converts the UTF-16 with the
// system code page, which is single-byte for the game's languages; a character above 255 becomes '?'
INT WINAPI LoadStringA(HINSTANCE inModule, UINT inID, LPSTR outText, INT inSize)
{
    DWORD theSize = 0;
    const unsigned char* theBlock = static_cast<const unsigned char*>(
        FindResourceData(inModule, RT_STRING, MAKEINTRESOURCEA((inID >> 4) + 1), 0, &theSize));
    if (!theBlock || inSize <= 0) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return 0;
    }
    size_t thePosition = 0;
    for (UINT i = 0; i < (inID & 15); ++i) {
        if (thePosition + 2 > theSize)
            return 0;
        WORD theLength;
        memcpy(&theLength, theBlock + thePosition, 2);
        thePosition += 2 + theLength * 2u;
    }
    if (thePosition + 2 > theSize)
        return 0;
    WORD theLength;
    memcpy(&theLength, theBlock + thePosition, 2);
    if (theLength == 0) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return 0;
    }
    INT theCount = 0;
    for (; theCount < theLength && theCount < inSize - 1; ++theCount) {
        WORD c;
        memcpy(&c, theBlock + thePosition + 2 + theCount * 2, 2);
        outText[theCount] = (char)(c < 256 ? c : '?');
    }
    outText[theCount] = 0;
    return theCount;
}

BOOL WINAPI EnumResourceNamesA(HMODULE inModule, LPCSTR inType, ENUMRESNAMEPROCA inProc, LONG_PTR inParam)
{
    Module* theModule = inModule ? ModuleOf(inModule) : MainModule();
    if (!theModule || theModule->mImage.empty()) {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }
    DWORD theType = FindEntry(theModule, 0, inType);
    if (!(theType & 0x80000000)) {
        SetLastError(ERROR_RESOURCE_TYPE_NOT_FOUND);
        return FALSE;
    }
    for (const ResourceEntry& theEntry : DirectoryEntries(theModule, theType & 0x7fffffff)) {
        std::string theString;
        uintptr_t theName;
        if (theEntry.mName & 0x80000000) {
            theString = EntryString(theModule, theEntry.mName);
            theName = reinterpret_cast<uintptr_t>(theString.c_str());
        } else
            theName = theEntry.mName;
        uintptr_t theArguments[] = {reinterpret_cast<uintptr_t>(inModule), reinterpret_cast<uintptr_t>(inType),
                                    theName, static_cast<uintptr_t>(inParam)};
        if (!(CallCallback(reinterpret_cast<const void*>(inProc), theArguments, 4) & 0xff))
            break;
    }
    return TRUE;
}

BOOL WINAPI EnumResourceLanguagesA(HMODULE inModule, LPCSTR inType, LPCSTR inName, ENUMRESLANGPROCA inProc,
                                   LONG_PTR inParam)
{
    Module* theModule = inModule ? ModuleOf(inModule) : MainModule();
    if (!theModule || theModule->mImage.empty()) {
        SetLastError(ERROR_RESOURCE_DATA_NOT_FOUND);
        return FALSE;
    }
    DWORD theType = FindEntry(theModule, 0, inType);
    DWORD theName = (theType & 0x80000000) ? FindEntry(theModule, theType & 0x7fffffff, inName) : 0;
    if (!(theName & 0x80000000)) {
        SetLastError(ERROR_RESOURCE_NAME_NOT_FOUND);
        return FALSE;
    }
    for (const ResourceEntry& theEntry : DirectoryEntries(theModule, theName & 0x7fffffff)) {
        uintptr_t theArguments[] = {reinterpret_cast<uintptr_t>(inModule), reinterpret_cast<uintptr_t>(inType),
                                    reinterpret_cast<uintptr_t>(inName), theEntry.mName & 0xffff,
                                    static_cast<uintptr_t>(inParam)};
        if (!(CallCallback(reinterpret_cast<const void*>(inProc), theArguments, 5) & 0xff))
            break;
    }
    return TRUE;
}

} // extern "C"
