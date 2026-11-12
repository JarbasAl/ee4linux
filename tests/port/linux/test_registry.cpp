// Tests of the native build's registry (src/Port/Linux/Win32Registry.cpp): the import from a Wine prefix, the
// values' types and sizes as Windows returns them, the file it writes, and key deletion.
#include <windows.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/stat.h>

static int sFailures;
static int sChecks;

#define CHECK(condition)                                                                 \
    do {                                                                                 \
        ++sChecks;                                                                       \
        if (!(condition)) {                                                              \
            ++sFailures;                                                                 \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);         \
        }                                                                                \
    } while (0)

static void Write(const std::string& inPath, const char* inText)
{
    std::ofstream theStream(inPath, std::ios::binary);
    theStream << inText;
}

static std::string Read(const std::string& inPath)
{
    std::ifstream theStream(inPath, std::ios::binary);
    std::stringstream theText;
    theText << theStream.rdbuf();
    return theText.str();
}

static const char kUserReg[] =
    "WINE REGISTRY Version 2\n"
    ";; All keys relative to \\\\User\\\\S-1-5-21-0-0-0-1000\n"
    "\n"
    "#arch=win64\n"
    "\n"
    "[Software\\\\Mad Doc Software\\\\EE-AOC] 1794416562\n"
    "#time=1dd71fe7740a9e0\n"
    "\"Display Shadows\"=dword:00000001\n"
    "\"Installed From Volume\"=\"C:\"\n"
    "\"Installed From Directory\"=\"\\\\PROGRAM FILES (X86)\\\\EMPIRE EARTH\\\\Empire Earth - The Art of Conquest\\\\\"\n"
    "\"Blob\"=hex:01,02,\\\n"
    "  03,ff\n"
    "\"Quote \\\"me\\\"\"=\"caf\\xe9\"\n"
    "\n"
    "[Software\\\\Other Game] 1794416562\n"
    "\"x\"=\"y\"\n";

static const char kSystemReg[] =
    "WINE REGISTRY Version 2\n"
    "[Software\\\\Wow6432Node\\\\Sierra\\\\CDKeys] 1794416562\n"
    "\"EmpireEarth\"=hex:aa,bb\n"
    "[Software\\\\Microsoft\\\\Windows] 1794416562\n"
    "\"z\"=\"w\"\n";

int main()
{
    std::string theRoot = getenv("EE_PORT_TEST_DIR");
    std::string thePrefix = theRoot + "/prefix";
    mkdir(thePrefix.c_str(), 0755);
    Write(thePrefix + "/user.reg", kUserReg);
    Write(thePrefix + "/system.reg", kSystemReg);
    std::string theFile = theRoot + "/ee-port-registry.reg";
    setenv("WINEPREFIX", thePrefix.c_str(), 1);
    setenv("EE_PORT_REGISTRY", theFile.c_str(), 1);

    // The game's key, in another case
    HKEY theKey = NULL;
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, "software\\mad doc software\\ee-aoc", 0, KEY_READ, &theKey) == ERROR_SUCCESS);
    DWORD theType = 0, theSize = sizeof(DWORD), theDword = 0;
    CHECK(RegQueryValueExA(theKey, "display shadows", NULL, &theType, (BYTE*)&theDword, &theSize) == ERROR_SUCCESS);
    CHECK(theType == REG_DWORD && theSize == 4 && theDword == 1);

    const char kDirectory[] = "\\PROGRAM FILES (X86)\\EMPIRE EARTH\\Empire Earth - The Art of Conquest\\";
    char theText[256] = {};
    theSize = sizeof theText;
    CHECK(RegQueryValueExA(theKey, "Installed From Directory", NULL, &theType, (BYTE*)theText, &theSize) == ERROR_SUCCESS);
    CHECK(theType == REG_SZ && strcmp(theText, kDirectory) == 0 && theSize == sizeof kDirectory);

    // The size alone, then a buffer one byte short
    theSize = 0;
    CHECK(RegQueryValueExA(theKey, "Installed From Volume", NULL, NULL, NULL, &theSize) == ERROR_SUCCESS && theSize == 3);
    theSize = 2;
    CHECK(RegQueryValueExA(theKey, "Installed From Volume", NULL, NULL, (BYTE*)theText, &theSize) == ERROR_MORE_DATA);
    CHECK(theSize == 3);

    unsigned char theBlob[8] = {};
    theSize = sizeof theBlob;
    CHECK(RegQueryValueExA(theKey, "Blob", NULL, &theType, theBlob, &theSize) == ERROR_SUCCESS);
    CHECK(theType == REG_BINARY && theSize == 4 && theBlob[0] == 1 && theBlob[2] == 3 && theBlob[3] == 0xff);

    theSize = sizeof theText;
    CHECK(RegQueryValueExA(theKey, "Quote \"me\"", NULL, &theType, (BYTE*)theText, &theSize) == ERROR_SUCCESS);
    CHECK(strcmp(theText, "caf\xe9") == 0);
    CHECK(RegQueryValueExA(theKey, "No Such Value", NULL, NULL, NULL, &theSize) == ERROR_FILE_NOT_FOUND);

    // Only the game's subtrees come over; Wow6432Node is the 32-bit view of HKEY_LOCAL_MACHINE\Software
    HKEY theOther = NULL;
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Other Game", 0, KEY_READ, &theOther) == ERROR_FILE_NOT_FOUND);
    CHECK(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows", 0, KEY_READ, &theOther) == ERROR_FILE_NOT_FOUND);
    CHECK(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Sierra\\CDKeys", 0, KEY_READ, &theOther) == ERROR_SUCCESS);
    theSize = sizeof theBlob;
    CHECK(RegQueryValueExA(theOther, "EmpireEarth", NULL, NULL, theBlob, &theSize) == ERROR_SUCCESS && theSize == 2);
    RegCloseKey(theOther);

    // A string set without its terminator is stored with it; the file holds the new value
    CHECK(RegSetValueExA(theKey, "New Value", 0, REG_SZ, (const BYTE*)"hello", 5) == ERROR_SUCCESS);
    theSize = sizeof theText;
    CHECK(RegQueryValueExA(theKey, "new value", NULL, NULL, (BYTE*)theText, &theSize) == ERROR_SUCCESS);
    CHECK(theSize == 6 && strcmp(theText, "hello") == 0);
    std::string theSaved = Read(theFile);
    CHECK(theSaved.find("[HKEY_CURRENT_USER\\\\Software\\\\Mad Doc Software\\\\EE-AOC]") != std::string::npos);
    CHECK(theSaved.find("\"New Value\"=\"hello\"") != std::string::npos);
    CHECK(theSaved.find("\"Installed From Directory\"=\"\\\\PROGRAM FILES (X86)\\\\EMPIRE EARTH\\\\") != std::string::npos);
    CHECK(theSaved.find("\"Display Shadows\"=dword:00000001") != std::string::npos);
    CHECK(theSaved.find("\"Blob\"=hex:01,02,03,ff") != std::string::npos);
    CHECK(theSaved.find("Other Game") == std::string::npos);
    CHECK(RegDeleteValueA(theKey, "New Value") == ERROR_SUCCESS);
    CHECK(RegDeleteValueA(theKey, "New Value") == ERROR_FILE_NOT_FOUND);
    CHECK(Read(theFile).find("New Value") == std::string::npos);
    RegCloseKey(theKey);

    // Created keys, and a key with a subkey that a sibling sorts between
    DWORD theDisposition = 0;
    CHECK(RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key\\Sub", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &theKey,
                          &theDisposition) == ERROR_SUCCESS && theDisposition == REG_CREATED_NEW_KEY);
    RegCloseKey(theKey);
    CHECK(RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key A", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &theKey,
                          &theDisposition) == ERROR_SUCCESS && theDisposition == REG_CREATED_NEW_KEY);
    RegCloseKey(theKey);
    CHECK(RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &theKey,
                          &theDisposition) == ERROR_SUCCESS && theDisposition == REG_OPENED_EXISTING_KEY);
    RegCloseKey(theKey);
    CHECK(RegDeleteKeyA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key") == ERROR_ACCESS_DENIED);
    CHECK(RegDeleteKeyA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key\\Sub") == ERROR_SUCCESS);
    CHECK(RegDeleteKeyA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key") == ERROR_SUCCESS);
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key", 0, KEY_READ, &theKey) == ERROR_FILE_NOT_FOUND);
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\SSSI\\T\\Key A", 0, KEY_READ, &theKey) == ERROR_SUCCESS);
    RegCloseKey(theKey);

    // Subkeys in Windows' order, by upper case: 'A' (0x41) before '_' (0x5f), though 'a' (0x61) sorts after it
    CHECK(RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Order\\_x", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &theKey, NULL) ==
          ERROR_SUCCESS);
    RegCloseKey(theKey);
    CHECK(RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\Order\\a\\deeper", 0, NULL, 0, KEY_ALL_ACCESS, NULL, &theKey,
                          NULL) == ERROR_SUCCESS);
    RegCloseKey(theKey);
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Order", 0, KEY_READ, &theKey) == ERROR_SUCCESS);
    char theName[64];
    DWORD theLength = sizeof theName;
    CHECK(RegEnumKeyExA(theKey, 0, theName, &theLength, NULL, NULL, NULL, NULL) == ERROR_SUCCESS &&
          strcmp(theName, "a") == 0 && theLength == 1);
    theLength = sizeof theName;
    CHECK(RegEnumKeyExA(theKey, 1, theName, &theLength, NULL, NULL, NULL, NULL) == ERROR_SUCCESS &&
          strcmp(theName, "_x") == 0 && theLength == 2);
    theLength = sizeof theName;
    CHECK(RegEnumKeyExA(theKey, 2, theName, &theLength, NULL, NULL, NULL, NULL) == ERROR_NO_MORE_ITEMS);
    RegCloseKey(theKey);
    // A key that the file holds only inside a deeper path still lists; a name and its terminator must fit
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, "Software", 0, KEY_READ, &theKey) == ERROR_SUCCESS);
    theLength = sizeof theName;
    FILETIME theTime = {1, 1};
    CHECK(RegEnumKeyExA(theKey, 0, theName, &theLength, NULL, NULL, NULL, &theTime) == ERROR_SUCCESS &&
          strcmp(theName, "Mad Doc Software") == 0 && theLength == 16 && theTime.dwLowDateTime == 0);
    theLength = 16;
    CHECK(RegEnumKeyExA(theKey, 0, theName, &theLength, NULL, NULL, NULL, NULL) == ERROR_MORE_DATA);
    theLength = 17;
    CHECK(RegEnumKeyExA(theKey, 0, theName, &theLength, NULL, NULL, NULL, NULL) == ERROR_SUCCESS && theLength == 16);
    int theSubkeys = 0;
    for (DWORD i = 0;; ++i) {
        theLength = sizeof theName;
        if (RegEnumKeyExA(theKey, i, theName, &theLength, NULL, NULL, NULL, NULL) != ERROR_SUCCESS)
            break;
        ++theSubkeys;
    }
    CHECK(theSubkeys == 3);                                 // Mad Doc Software, Order, SSSI
    RegCloseKey(theKey);

    // Every value once, with its type and data
    CHECK(RegOpenKeyExA(HKEY_CURRENT_USER, "Software\\Mad Doc Software\\EE-AOC", 0, KEY_READ, &theKey) == ERROR_SUCCESS);
    std::string theSeen;
    int theValues = 0;
    for (DWORD i = 0;; ++i) {
        theLength = sizeof theName;
        DWORD theValueType = 0;
        if (RegEnumValueA(theKey, i, theName, &theLength, NULL, &theValueType, NULL, NULL) != ERROR_SUCCESS)
            break;
        ++theValues;
        theSeen += std::string(theName) + ";";
        if (strcmp(theName, "Blob") == 0) {
            theLength = sizeof theName;
            theSize = 2;
            CHECK(RegEnumValueA(theKey, i, theName, &theLength, NULL, NULL, theBlob, &theSize) == ERROR_MORE_DATA &&
                  theSize == 4);
            theLength = sizeof theName;
            theSize = sizeof theBlob;
            CHECK(RegEnumValueA(theKey, i, theName, &theLength, NULL, NULL, theBlob, &theSize) == ERROR_SUCCESS &&
                  theValueType == REG_BINARY && theSize == 4 && theBlob[3] == 0xff);
        }
    }
    CHECK(theValues == 5);
    for (const char* theExpected : {"Blob;", "Display Shadows;", "Installed From Directory;", "Installed From Volume;",
                                    "Quote \"me\";"})
        CHECK(theSeen.find(theExpected) != std::string::npos);
    RegCloseKey(theKey);

    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
