// The registry as a text file in the game folder, in the format of Wine's .reg files.
//
// The file is ee-port-registry.reg in the folder the game starts in (EE_PORT_REGISTRY names another), so the settings
// travel with the folder. When it does not exist, the first registry call imports the game's keys from the Wine
// prefix (WINEPREFIX, else ~/.wine): user.reg for HKEY_CURRENT_USER, system.reg for HKEY_LOCAL_MACHINE, where a
// 32-bit program's keys are under Software\Wow6432Node. A change writes the whole file again.
#include "Win32Internal.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace eeport {

struct RegistryValue {
    std::string mName;
    DWORD mType = REG_NONE;
    std::vector<unsigned char> mData;
};

struct RegistryKey {
    std::string mPath;                                  // the full path as written, from the root's name
    std::map<std::string, RegistryValue> mValues;       // by lower-case name; "" is the default value
};

// An open key: the full path, lower case
struct OpenKey {
    std::string mPath;
};

static std::mutex sLock;
static std::map<std::string, RegistryKey> sKeys;        // by lower-case full path
static std::string sFile;
static bool sLoaded;

static std::string Lower(std::string inText)
{
    for (char& c : inText)
        c = (char)tolower((unsigned char)c);
    return inText;
}

static const char* RootName(HKEY inRoot)
{
    if (inRoot == HKEY_CLASSES_ROOT) return "HKEY_CLASSES_ROOT";
    if (inRoot == HKEY_CURRENT_USER) return "HKEY_CURRENT_USER";
    if (inRoot == HKEY_LOCAL_MACHINE) return "HKEY_LOCAL_MACHINE";
    if (inRoot == HKEY_USERS) return "HKEY_USERS";
    if (inRoot == HKEY_CURRENT_CONFIG) return "HKEY_CURRENT_CONFIG";
    if (inRoot == HKEY_DYN_DATA) return "HKEY_DYN_DATA";
    return NULL;
}

// The full path of a subkey of an open or root key, or "" for a handle that is neither
static std::string PathOf(HKEY inKey, const char* inSubKey)
{
    std::string theBase;
    if (const char* theRoot = RootName(inKey))
        theBase = theRoot;
    else if (inKey)
        theBase = reinterpret_cast<OpenKey*>(inKey)->mPath;
    else
        return "";
    std::string theSub = inSubKey ? inSubKey : "";
    while (!theSub.empty() && theSub.back() == '\\')
        theSub.pop_back();
    while (!theSub.empty() && theSub.front() == '\\')
        theSub.erase(0, 1);
    return theSub.empty() ? theBase : theBase + "\\" + theSub;
}

// ---- the .reg text ----
// A quoted string from inText at ioPosition (which is at the opening quote), with Wine's escapes
static std::string ReadQuoted(const std::string& inText, size_t& ioPosition)
{
    std::string theResult;
    ++ioPosition;
    while (ioPosition < inText.size() && inText[ioPosition] != '"') {
        char c = inText[ioPosition++];
        if (c != '\\' || ioPosition >= inText.size()) {
            theResult += c;
            continue;
        }
        char e = inText[ioPosition++];
        if (e == 'n')
            theResult += '\n';
        else if (e == 'r')
            theResult += '\r';
        else if (e == 't')
            theResult += '\t';
        else if (e == '0')
            theResult += '\0';
        else if (e == 'x') {
            unsigned theCode = 0;
            int theDigits = 0;
            while (theDigits < 4 && ioPosition < inText.size() && isxdigit((unsigned char)inText[ioPosition])) {
                char d = inText[ioPosition++];
                theCode = theCode * 16 + (unsigned)(isdigit((unsigned char)d) ? d - '0' : tolower(d) - 'a' + 10);
                ++theDigits;
            }
            theResult += (char)(theCode < 256 ? theCode : '?');
        } else
            theResult += e;
    }
    ++ioPosition;
    return theResult;
}

static std::string Quote(const std::string& inText)
{
    std::string theResult = "\"";
    for (unsigned char c : inText) {
        if (c == '\\' || c == '"') {
            theResult += '\\';
            theResult += (char)c;
        } else if (c < 0x20 || c >= 0x7f) {
            char theCode[8];
            snprintf(theCode, sizeof theCode, "\\x%02x", c);
            theResult += theCode;
        } else
            theResult += (char)c;
    }
    return theResult + "\"";
}

static std::vector<unsigned char> HexBytes(const std::string& inText)
{
    std::vector<unsigned char> theBytes;
    std::istringstream theStream(inText);
    std::string thePart;
    while (std::getline(theStream, thePart, ','))
        if (!thePart.empty())
            theBytes.push_back((unsigned char)strtoul(thePart.c_str(), NULL, 16));
    return theBytes;
}

// Reads a .reg file. Each key's section name is prefixed with inRoot ("" when the names are full paths) after
// inRename maps a subtree to another (Software\Wow6432Node\ to Software\). inKeep says which keys to take.
static void ReadRegistryFile(const std::string& inFile, const std::string& inRoot,
                     const std::vector<std::pair<std::string, std::string>>& inRename,
                     bool (*inKeep)(const std::string& inPath))
{
    std::ifstream theStream(inFile);
    if (!theStream)
        return;
    RegistryKey* theKey = NULL;
    std::string theLine, thePending;
    while (std::getline(theStream, theLine)) {
        if (!theLine.empty() && theLine.back() == '\r')
            theLine.pop_back();
        if (!theLine.empty() && theLine.back() == '\\') {     // a hex value continues on the next line
            theLine.pop_back();
            thePending += theLine;
            continue;
        }
        theLine = thePending + theLine;
        thePending.clear();
        size_t theStart = theLine.find_first_not_of(" \t");
        if (theStart == std::string::npos || theLine[theStart] == ';' || theLine[theStart] == '#')
            continue;
        if (theLine[theStart] == '[') {
            size_t theEnd = theLine.find(']', theStart);
            std::string theName;
            for (size_t i = theStart + 1; i < theEnd; ++i) {
                if (theLine[i] == '\\' && i + 1 < theEnd && theLine[i + 1] == '\\')
                    ++i;
                theName += theLine[i];
            }
            for (const auto& theRename : inRename)
                if (Lower(theName).compare(0, theRename.first.size(), Lower(theRename.first)) == 0)
                    theName = theRename.second + theName.substr(theRename.first.size());
            std::string thePath = inRoot.empty() ? theName : inRoot + "\\" + theName;
            if (!inKeep(thePath)) {
                theKey = NULL;
                continue;
            }
            theKey = &sKeys[Lower(thePath)];
            if (theKey->mPath.empty())
                theKey->mPath = thePath;
            continue;
        }
        if (!theKey)
            continue;
        size_t thePosition = theStart;
        std::string theName;
        if (theLine[thePosition] == '@')
            ++thePosition;
        else if (theLine[thePosition] == '"')
            theName = ReadQuoted(theLine, thePosition);
        else
            continue;
        if (thePosition >= theLine.size() || theLine[thePosition] != '=')
            continue;
        std::string theData = theLine.substr(thePosition + 1);
        RegistryValue theValue;
        theValue.mName = theName;
        if (!theData.empty() && theData[0] == '"') {
            size_t theQuote = 0;
            std::string theText = ReadQuoted(theData, theQuote);
            theValue.mType = REG_SZ;
            theValue.mData.assign(theText.begin(), theText.end());
            theValue.mData.push_back(0);
        } else if (theData.compare(0, 7, "str(2):") == 0 && theData.size() > 7) {
            size_t theQuote = 7;
            std::string theText = ReadQuoted(theData, theQuote);
            theValue.mType = REG_EXPAND_SZ;
            theValue.mData.assign(theText.begin(), theText.end());
            theValue.mData.push_back(0);
        } else if (theData.compare(0, 6, "dword:") == 0) {
            DWORD theNumber = (DWORD)strtoul(theData.c_str() + 6, NULL, 16);
            theValue.mType = REG_DWORD;
            theValue.mData.assign(reinterpret_cast<unsigned char*>(&theNumber),
                                  reinterpret_cast<unsigned char*>(&theNumber) + 4);
        } else if (theData.compare(0, 4, "hex:") == 0) {
            theValue.mType = REG_BINARY;
            theValue.mData = HexBytes(theData.substr(4));
        } else if (theData.compare(0, 4, "hex(") == 0) {
            size_t theClose = theData.find("):");
            if (theClose == std::string::npos)
                continue;
            theValue.mType = (DWORD)strtoul(theData.c_str() + 4, NULL, 16);
            theValue.mData = HexBytes(theData.substr(theClose + 2));
            // Wine writes REG_SZ and REG_EXPAND_SZ it cannot print as UTF-16 hex: keep the low bytes
            if (theValue.mType == REG_SZ || theValue.mType == REG_EXPAND_SZ || theValue.mType == REG_MULTI_SZ) {
                std::vector<unsigned char> theNarrow;
                for (size_t i = 0; i + 1 < theValue.mData.size(); i += 2)
                    theNarrow.push_back(theValue.mData[i + 1] ? '?' : theValue.mData[i]);
                theValue.mData = theNarrow;
            }
        } else
            continue;
        theKey->mValues[Lower(theName)] = theValue;
    }
}

static void WriteRegistryFile()
{
    std::string theText = "WINE REGISTRY Version 2\n;; The Empire Earth native port's registry (ee-port-registry.reg)\n";
    for (const auto& theEntry : sKeys) {
        const RegistryKey& theKey = theEntry.second;
        std::string theName;
        for (char c : theKey.mPath)
            theName += c == '\\' ? std::string("\\\\") : std::string(1, c);
        theText += "\n[" + theName + "]\n";
        for (const auto& theValueEntry : theKey.mValues) {
            const RegistryValue& theValue = theValueEntry.second;
            std::string theLine = theValue.mName.empty() ? "@=" : Quote(theValue.mName) + "=";
            const std::vector<unsigned char>& theData = theValue.mData;
            bool theTerminated = !theData.empty() && theData.back() == 0 &&
                                 memchr(theData.data(), 0, theData.size() - 1) == NULL;
            if (theValue.mType == REG_SZ && theTerminated)
                theLine += Quote(std::string(theData.begin(), theData.end() - 1));
            else if (theValue.mType == REG_EXPAND_SZ && theTerminated)
                theLine += "str(2):" + Quote(std::string(theData.begin(), theData.end() - 1));
            else if (theValue.mType == REG_DWORD && theData.size() == 4) {
                char theNumber[16];
                DWORD theDword;
                memcpy(&theDword, theData.data(), 4);
                snprintf(theNumber, sizeof theNumber, "dword:%08x", (unsigned)theDword);
                theLine += theNumber;
            } else {
                char thePrefix[16];
                if (theValue.mType == REG_BINARY)
                    snprintf(thePrefix, sizeof thePrefix, "hex:");
                else
                    snprintf(thePrefix, sizeof thePrefix, "hex(%x):", (unsigned)theValue.mType);
                theLine += thePrefix;
                for (size_t i = 0; i < theData.size(); ++i) {
                    char theByte[4];
                    snprintf(theByte, sizeof theByte, i ? ",%02x" : "%02x", theData[i]);
                    theLine += theByte;
                }
            }
            theText += theLine + "\n";
        }
    }
    std::string theTemporary = sFile + ".new";
    FILE* theOut = fopen(theTemporary.c_str(), "wb");
    if (!theOut) {
        Log("cannot write the registry file %s", theTemporary.c_str());
        return;
    }
    bool theOK = fwrite(theText.data(), 1, theText.size(), theOut) == theText.size();
    theOK = fclose(theOut) == 0 && theOK;
    if (!theOK || rename(theTemporary.c_str(), sFile.c_str()) != 0)
        Log("cannot write the registry file %s", sFile.c_str());
}

// The game's own keys and Sierra's (WON's CD keys and lobby settings), the subtrees an import takes
static bool IsGameKey(const std::string& inPath)
{
    static const char* const kRoots[] = {
        "hkey_current_user\\software\\sssi",          "hkey_current_user\\software\\mad doc software",
        "hkey_current_user\\software\\sierra",        "hkey_local_machine\\software\\sssi",
        "hkey_local_machine\\software\\mad doc software", "hkey_local_machine\\software\\sierra",
    };
    std::string thePath = Lower(inPath);
    for (const char* theRoot : kRoots) {
        size_t theLength = strlen(theRoot);
        if (thePath.compare(0, theLength, theRoot) == 0 && (thePath.size() == theLength || thePath[theLength] == '\\'))
            return true;
    }
    return false;
}

static bool KeepAll(const std::string&) { return true; }

static void Load()
{
    if (sLoaded)
        return;
    sLoaded = true;
    const char* theFile = getenv("EE_PORT_REGISTRY");
    sFile = theFile && *theFile ? theFile : "ee-port-registry.reg";
    char theFolder[4096];
    if (sFile[0] != '/' && getcwd(theFolder, sizeof theFolder))
        sFile = std::string(theFolder) + "/" + sFile;
    if (FILE* theExisting = fopen(sFile.c_str(), "rb")) {
        fclose(theExisting);
        ReadRegistryFile(sFile, "", {}, KeepAll);
        Log("registry: %zu keys from %s", sKeys.size(), sFile.c_str());
        return;
    }
    const char* thePrefix = getenv("WINEPREFIX");
    std::string theWine = thePrefix && *thePrefix ? thePrefix : std::string(getenv("HOME") ? getenv("HOME") : "") + "/.wine";
    ReadRegistryFile(theWine + "/user.reg", "HKEY_CURRENT_USER", {}, IsGameKey);
    ReadRegistryFile(theWine + "/system.reg", "HKEY_LOCAL_MACHINE", {{"Software\\Wow6432Node\\", "Software\\"}}, IsGameKey);
    Log("registry: %zu keys imported from the Wine prefix %s into %s", sKeys.size(), theWine.c_str(), sFile.c_str());
    WriteRegistryFile();
}

static HKEY NewKey(const std::string& inPath)
{
    OpenKey* theKey = new OpenKey;
    theKey->mPath = sKeys[Lower(inPath)].mPath;
    return reinterpret_cast<HKEY>(theKey);
}

static RegistryKey* FindKey(HKEY inKey, const char* inSubKey)
{
    std::string thePath = PathOf(inKey, inSubKey);
    if (thePath.empty())
        return NULL;
    if (RootName(inKey) && (!inSubKey || !*inSubKey)) {
        RegistryKey& theRoot = sKeys[Lower(thePath)];     // a root always exists
        if (theRoot.mPath.empty())
            theRoot.mPath = thePath;
        return &theRoot;
    }
    auto theEntry = sKeys.find(Lower(thePath));
    return theEntry == sKeys.end() ? NULL : &theEntry->second;
}

} // namespace eeport

using namespace eeport;

extern "C" {

LSTATUS WINAPI RegOpenKeyExA(HKEY inKey, LPCSTR inSubKey, DWORD inOptions, REGSAM inAccess, PHKEY outKey)
{
    (void)inOptions, (void)inAccess;
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    RegistryKey* theKey = FindKey(inKey, inSubKey);
    if (!theKey)
        return ERROR_FILE_NOT_FOUND;
    *outKey = NewKey(theKey->mPath);
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegCreateKeyExA(HKEY inKey, LPCSTR inSubKey, DWORD inReserved, LPSTR inClass, DWORD inOptions,
                               REGSAM inAccess, LPSECURITY_ATTRIBUTES inSecurity, PHKEY outKey, LPDWORD outDisposition)
{
    (void)inReserved, (void)inClass, (void)inOptions, (void)inAccess, (void)inSecurity;
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    std::string thePath = PathOf(inKey, inSubKey);
    if (thePath.empty())
        return ERROR_INVALID_HANDLE;
    bool theCreated = false;
    // Every key on the way exists afterwards, as in Windows
    for (size_t thePosition = thePath.find('\\'); ; thePosition = thePath.find('\\', thePosition + 1)) {
        std::string thePart = thePath.substr(0, thePosition);
        RegistryKey& theKey = sKeys[Lower(thePart)];
        if (theKey.mPath.empty()) {
            theKey.mPath = thePart;
            theCreated = true;
        }
        if (thePosition == std::string::npos)
            break;
    }
    if (outDisposition)
        *outDisposition = theCreated ? REG_CREATED_NEW_KEY : REG_OPENED_EXISTING_KEY;
    if (theCreated)
        WriteRegistryFile();
    *outKey = NewKey(thePath);
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegCloseKey(HKEY inKey)
{
    if (!RootName(inKey))
        delete reinterpret_cast<OpenKey*>(inKey);
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegQueryValueExA(HKEY inKey, LPCSTR inName, LPDWORD inReserved, LPDWORD outType, LPBYTE outData,
                                LPDWORD ioSize)
{
    (void)inReserved;
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    RegistryKey* theKey = FindKey(inKey, NULL);
    if (!theKey)
        return ERROR_INVALID_HANDLE;
    auto theEntry = theKey->mValues.find(Lower(inName ? inName : ""));
    if (theEntry == theKey->mValues.end())
        return ERROR_FILE_NOT_FOUND;
    const RegistryValue& theValue = theEntry->second;
    if (outType)
        *outType = theValue.mType;
    DWORD theSize = (DWORD)theValue.mData.size();
    if (!ioSize)
        return outData ? ERROR_INVALID_PARAMETER : ERROR_SUCCESS;
    if (outData && *ioSize < theSize) {
        *ioSize = theSize;
        return ERROR_MORE_DATA;
    }
    if (outData)
        memcpy(outData, theValue.mData.data(), theSize);
    *ioSize = theSize;
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegSetValueExA(HKEY inKey, LPCSTR inName, DWORD inReserved, DWORD inType, const BYTE* inData,
                              DWORD inSize)
{
    (void)inReserved;
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    RegistryKey* theKey = FindKey(inKey, NULL);
    if (!theKey)
        return ERROR_INVALID_HANDLE;
    RegistryValue theValue;
    theValue.mName = inName ? inName : "";
    theValue.mType = inType;
    theValue.mData.assign(inData, inData + inSize);
    // Windows stores a string with its terminator even when the size leaves it out
    if ((inType == REG_SZ || inType == REG_EXPAND_SZ) && (theValue.mData.empty() || theValue.mData.back() != 0))
        theValue.mData.push_back(0);
    theKey->mValues[Lower(theValue.mName)] = theValue;
    WriteRegistryFile();
    return ERROR_SUCCESS;
}

LSTATUS WINAPI RegDeleteValueA(HKEY inKey, LPCSTR inName)
{
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    RegistryKey* theKey = FindKey(inKey, NULL);
    if (!theKey)
        return ERROR_INVALID_HANDLE;
    if (theKey->mValues.erase(Lower(inName ? inName : "")) == 0)
        return ERROR_FILE_NOT_FOUND;
    WriteRegistryFile();
    return ERROR_SUCCESS;
}

// Copies a name into a caller's buffer of ioLength characters with its terminator; ioLength becomes the length
static LSTATUS CopyName(const std::string& inName, LPSTR outName, LPDWORD ioLength)
{
    if (!outName || !ioLength)
        return ERROR_INVALID_PARAMETER;
    if (*ioLength <= inName.size())
        return ERROR_MORE_DATA;
    memcpy(outName, inName.c_str(), inName.size() + 1);
    *ioLength = (DWORD)inName.size();
    return ERROR_SUCCESS;
}

// The subkeys in Windows' order, by name in upper case. A key in the file may exist only as part of a deeper path,
// so the names come from every key below this one.
LSTATUS WINAPI RegEnumKeyExA(HKEY inKey, DWORD inIndex, LPSTR outName, LPDWORD ioNameLength, LPDWORD inReserved,
                             LPSTR outClass, LPDWORD ioClassLength, PFILETIME outLastWrite)
{
    (void)inReserved;
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    RegistryKey* theKey = FindKey(inKey, NULL);
    if (!theKey)
        return ERROR_INVALID_HANDLE;
    std::string thePrefix = Lower(theKey->mPath) + "\\";
    std::map<std::string, std::string> theNames;
    for (auto theEntry = sKeys.lower_bound(thePrefix);
         theEntry != sKeys.end() && theEntry->first.compare(0, thePrefix.size(), thePrefix) == 0; ++theEntry) {
        std::string theName = theEntry->second.mPath.substr(thePrefix.size());
        theName = theName.substr(0, theName.find('\\'));
        std::string theOrder = theName;
        for (char& c : theOrder)
            c = (char)toupper((unsigned char)c);
        theNames.emplace(theOrder, theName);
    }
    if (inIndex >= theNames.size())
        return ERROR_NO_MORE_ITEMS;
    if (outClass && ioClassLength && *ioClassLength)
        *outClass = 0;
    if (ioClassLength)
        *ioClassLength = 0;
    if (outLastWrite)
        memset(outLastWrite, 0, sizeof *outLastWrite);
    return CopyName(std::next(theNames.begin(), inIndex)->second, outName, ioNameLength);
}

// The values by name; Windows gives the order they were made in, which the registry file does not keep
LSTATUS WINAPI RegEnumValueA(HKEY inKey, DWORD inIndex, LPSTR outName, LPDWORD ioNameLength, LPDWORD inReserved,
                             LPDWORD outType, LPBYTE outData, LPDWORD ioDataSize)
{
    (void)inReserved;
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    RegistryKey* theKey = FindKey(inKey, NULL);
    if (!theKey)
        return ERROR_INVALID_HANDLE;
    if (inIndex >= theKey->mValues.size())
        return ERROR_NO_MORE_ITEMS;
    const RegistryValue& theValue = std::next(theKey->mValues.begin(), inIndex)->second;
    LSTATUS theResult = CopyName(theValue.mName, outName, ioNameLength);
    if (theResult != ERROR_SUCCESS)
        return theResult;
    if (outType)
        *outType = theValue.mType;
    DWORD theSize = (DWORD)theValue.mData.size();
    if (outData && ioDataSize && *ioDataSize < theSize) {
        *ioDataSize = theSize;
        return ERROR_MORE_DATA;
    }
    if (outData && ioDataSize)
        memcpy(outData, theValue.mData.data(), theSize);
    if (ioDataSize)
        *ioDataSize = theSize;
    return ERROR_SUCCESS;
}

// Deletes a key that has no subkeys; one with subkeys stays, as in Windows
LSTATUS WINAPI RegDeleteKeyA(HKEY inKey, LPCSTR inSubKey)
{
    std::lock_guard<std::mutex> theLock(sLock);
    Load();
    std::string thePath = Lower(PathOf(inKey, inSubKey));
    if (thePath.empty() || !inSubKey || !*inSubKey)
        return ERROR_INVALID_PARAMETER;
    auto theEntry = sKeys.find(thePath);
    if (theEntry == sKeys.end())
        return ERROR_FILE_NOT_FOUND;
    auto theChild = sKeys.lower_bound(thePath + "\\");
    if (theChild != sKeys.end() && theChild->first.compare(0, thePath.size() + 1, thePath + "\\") == 0)
        return ERROR_ACCESS_DENIED;
    sKeys.erase(theEntry);
    WriteRegistryFile();
    return ERROR_SUCCESS;
}

} // extern "C"
