// The native Linux build's Win32 layer: text conversion, version and locale queries, global memory, multimedia
// timers, INI files, processes, COM start-up, and the multimedia devices the game asks about but this build does not
// have (mixer, aux, MCI CD audio, waveOut).
#include "Win32Internal.h"

#include <mmsystem.h>
#include <objbase.h>
#include <shellapi.h>

#include <cerrno>
#include <cstddef>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <map>
#include <memory>
#include <spawn.h>
#include <string>
#include <sys/statvfs.h>
#include <thread>
#include <unistd.h>

extern char** environ;

// ---- text ----
namespace {

// Windows-1252 bytes 0x80-0x9f; the other bytes are their own code points. 0 marks the five unassigned bytes, which
// Windows maps to the same code point.
const char16_t kHigh1252[32] = {
    0x20ac, 0, 0x201a, 0x0192, 0x201e, 0x2026, 0x2020, 0x2021, 0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0, 0x017d, 0,
    0, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, 0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0, 0x017e,
    0x0178};

char32_t FromByte(unsigned char inByte)
{
    if (inByte >= 0x80 && inByte < 0xa0 && kHigh1252[inByte - 0x80])
        return kHigh1252[inByte - 0x80];
    return inByte;
}

// The Windows-1252 byte of a code point, or -1
int ToByte(char32_t inCode)
{
    if (inCode < 0x80 || (inCode >= 0xa0 && inCode < 0x100))
        return (int)inCode;
    for (int i = 0; i < 32; ++i)
        if (kHigh1252[i] ? kHigh1252[i] == inCode : inCode == (char32_t)(0x80 + i))
            return 0x80 + i;
    return -1;
}

// Decodes UTF-8 at ioText (before inEnd) to one code point; U+FFFD for a bad sequence
char32_t DecodeUTF8(const unsigned char*& ioText, const unsigned char* inEnd)
{
    unsigned char theLead = *ioText++;
    int theMore = theLead < 0x80 ? 0 : theLead >= 0xf0 ? 3 : theLead >= 0xe0 ? 2 : theLead >= 0xc0 ? 1 : -1;
    if (theMore < 0)
        return 0xfffd;
    char32_t theCode = theMore ? theLead & (0x3f >> theMore) : theLead;
    for (; theMore > 0; --theMore) {
        if (ioText == inEnd || (*ioText & 0xc0) != 0x80)
            return 0xfffd;
        theCode = (theCode << 6) | (*ioText++ & 0x3f);
    }
    return theCode;
}

// The code points of a byte string in the code page: CP_UTF8 is UTF-8, every other page is Windows-1252
std::u32string Decode(UINT inCodePage, LPCSTR inText, int inLength)
{
    const unsigned char* theText = (const unsigned char*)inText;
    const unsigned char* theEnd = theText + (inLength < 0 ? strlen(inText) + 1 : (size_t)inLength);
    std::u32string theCodes;
    while (theText < theEnd)
        theCodes += inCodePage == CP_UTF8 ? DecodeUTF8(theText, theEnd) : FromByte(*theText++);
    return theCodes;
}

template <class Char> int ToWide(UINT inCodePage, LPCSTR inText, int inLength, Char* outText, int inSize)
{
    if (!inText || inLength == 0 || inSize < 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    std::u32string theCodes = Decode(inCodePage, inText, inLength);
    std::basic_string<Char> theWide;
    for (char32_t theCode : theCodes) {
        if (sizeof(Char) == 2 && theCode > 0xffff) {     // a UTF-16 surrogate pair
            theWide += (Char)(0xd800 + ((theCode - 0x10000) >> 10));
            theWide += (Char)(0xdc00 + ((theCode - 0x10000) & 0x3ff));
        } else
            theWide += (Char)theCode;
    }
    if (inSize == 0)
        return (int)theWide.size();
    if ((int)theWide.size() > inSize) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    std::copy(theWide.begin(), theWide.end(), outText);
    return (int)theWide.size();
}

template <class Char>
int FromWide(UINT inCodePage, const Char* inText, int inLength, LPSTR outText, int inSize, LPCSTR inDefault,
             LPBOOL outUsedDefault)
{
    if (!inText || inLength == 0 || inSize < 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    if (inLength < 0)
        inLength = (int)std::char_traits<Char>::length(inText) + 1;
    std::string theBytes;
    bool theUsedDefault = false;
    for (int i = 0; i < inLength; ++i) {
        char32_t theCode = (char32_t)inText[i];
        if (sizeof(Char) == 2 && theCode >= 0xd800 && theCode < 0xdc00 && i + 1 < inLength)
            theCode = 0x10000 + ((theCode - 0xd800) << 10) + ((char32_t)inText[++i] - 0xdc00);
        if (inCodePage == CP_UTF8) {
            if (theCode < 0x80)
                theBytes += (char)theCode;
            else if (theCode < 0x800)
                theBytes += {(char)(0xc0 | theCode >> 6), (char)(0x80 | (theCode & 0x3f))};
            else if (theCode < 0x10000)
                theBytes += {(char)(0xe0 | theCode >> 12), (char)(0x80 | ((theCode >> 6) & 0x3f)),
                             (char)(0x80 | (theCode & 0x3f))};
            else
                theBytes += {(char)(0xf0 | theCode >> 18), (char)(0x80 | ((theCode >> 12) & 0x3f)),
                             (char)(0x80 | ((theCode >> 6) & 0x3f)), (char)(0x80 | (theCode & 0x3f))};
            continue;
        }
        int theByte = ToByte(theCode);
        if (theByte < 0) {
            theByte = inDefault ? (unsigned char)*inDefault : '?';
            theUsedDefault = true;
        }
        theBytes += (char)theByte;
    }
    if (outUsedDefault)
        *outUsedDefault = theUsedDefault;
    if (inSize == 0)
        return (int)theBytes.size();
    if ((int)theBytes.size() > inSize) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    memcpy(outText, theBytes.data(), theBytes.size());
    return (int)theBytes.size();
}

} // namespace

INT WINAPI MultiByteToWideChar(UINT inCodePage, DWORD, LPCSTR inText, INT inLength, LPWSTR outText, INT inSize)
{
    return ToWide(inCodePage, inText, inLength, outText, inSize);
}

INT WINAPI WideCharToMultiByte(UINT inCodePage, DWORD, LPCWSTR inText, INT inLength, LPSTR outText, INT inSize,
                               LPCSTR inDefault, LPBOOL outUsedDefault)
{
    return FromWide(inCodePage, inText, inLength, outText, inSize, inDefault, outUsedDefault);
}

int ee_port_MultiByteToWideChar(UINT inCodePage, DWORD, LPCSTR inText, int inLength, wchar_t* outText, int inSize)
{
    return ToWide(inCodePage, inText, inLength, outText, inSize);
}

int ee_port_WideCharToMultiByte(UINT inCodePage, DWORD, const wchar_t* inText, int inLength, LPSTR outText,
                                int inSize, LPCSTR inDefault, LPBOOL outUsedDefault)
{
    return FromWide(inCodePage, inText, inLength, outText, inSize, inDefault, outUsedDefault);
}

LPSTR WINAPI lstrcpyA(LPSTR outText, LPCSTR inText)
{
    return strcpy(outText, inText);
}

LPSTR WINAPI lstrcpynA(LPSTR outText, LPCSTR inText, INT inSize)
{
    if (inSize <= 0)
        return outText;
    strncpy(outText, inText, inSize - 1);
    outText[inSize - 1] = '\0';
    return outText;
}

LPSTR WINAPI lstrcatA(LPSTR ioText, LPCSTR inText)
{
    return strcat(ioText, inText);
}

// wsprintf writes at most 1024 bytes, its terminator included
#ifdef __x86_64__
// A WINAPIV function is a Windows-ABI function here, and its argument list cannot be handed to the C library's
// vsnprintf, which takes a System V one. The game's formats are integers, characters, strings and one wide integer, so
// wsprintf formats them itself, reading the Windows argument list.
static int AppendText(char* outText, int inTail, int inLimit, const char* inText, int inCount)
{
    for (int i = 0; i < inCount; ++i)
        if (inTail + i < inLimit)
            outText[inTail + i] = inText[i];
    return inTail + inCount;
}

static int FormatMs(const char* inFormat, __ms_va_list& ioArguments, char* outText, int inLimit)
{
    int theTail = 0;
    for (const char* theFormat = inFormat; *theFormat; ++theFormat) {
        if (*theFormat != '%') {
            theTail = AppendText(outText, theTail, inLimit, theFormat, 1);
            continue;
        }
        const char* theStart = theFormat;
        ++theFormat;
        if (*theFormat == '%') {
            theTail = AppendText(outText, theTail, inLimit, "%", 1);
            continue;
        }
        char theSpec[32];
        char theText[2048];      // one conversion, before it is cut to wsprintf's 1024
        int theLength = 1;
        theSpec[0] = '%';
        while (*theFormat && strchr("-+ #0", *theFormat) && theLength < 28)
            theSpec[theLength++] = *theFormat++;
        while (*theFormat >= '0' && *theFormat <= '9' && theLength < 28)
            theSpec[theLength++] = *theFormat++;
        if (*theFormat == '.' && theLength < 28) {
            theSpec[theLength++] = *theFormat++;
            while (*theFormat >= '0' && *theFormat <= '9' && theLength < 28)
                theSpec[theLength++] = *theFormat++;
        }
        bool theWide = false;
        if ((*theFormat == 'l' || *theFormat == 'L' || *theFormat == 'h') && theLength < 28) {
            theWide = *theFormat == 'l' || *theFormat == 'L';
            theSpec[theLength++] = *theFormat++;
            if ((*theFormat == 'l' || *theFormat == 'h') && theLength < 28)
                theSpec[theLength++] = *theFormat++;
        }
        char theConversion = *theFormat;
        theSpec[theLength++] = theConversion;
        theSpec[theLength] = '\0';
        int theCount = 0;
        switch (theConversion) {
        case 'd':
        case 'i':
            theCount = theWide ? snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, long))
                               : snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, int));
            break;
        case 'u':
        case 'o':
        case 'x':
        case 'X':
            theCount = theWide
                           ? snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, unsigned long))
                           : snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, unsigned));
            break;
        case 'c':
            theCount = snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, int));
            break;
        case 's':
            theCount = snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, const char*));
            break;
        case 'p':
            theCount = snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, void*));
            break;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
            theCount = snprintf(theText, sizeof theText, theSpec, __builtin_va_arg(ioArguments, double));
            break;
        default:
            // an unknown conversion stays as written
            theTail = AppendText(outText, theTail, inLimit, theStart, (int)(theFormat - theStart + 1));
            continue;
        }
        if (theCount > 0)
            theTail = AppendText(outText, theTail, inLimit, theText, theCount);
    }
    return theTail;
}

INT WINAPIV wsprintfA(LPSTR outText, LPCSTR inFormat, ...)
{
    if (!outText || !inFormat)
        return 0;
    __ms_va_list theArguments;
    __builtin_ms_va_start(theArguments, inFormat);
    int theLength = FormatMs(inFormat, theArguments, outText, 1023);
    __builtin_ms_va_end(theArguments);
    if (theLength > 1023)
        theLength = 1023;
    outText[theLength] = '\0';
    return theLength;
}
#else
INT WINAPIV wsprintfA(LPSTR outText, LPCSTR inFormat, ...)
{
    va_list theArguments;
    va_start(theArguments, inFormat);
    int theLength = vsnprintf(outText, 1024, inFormat, theArguments);
    va_end(theArguments);
    return theLength < 0 ? -1 : theLength > 1023 ? 1023 : theLength;
}
#endif

// inNumber * inNumerator / inDenominator, rounded half away from zero; -1 for a zero divisor or an overflow
INT WINAPI MulDiv(INT inNumber, INT inNumerator, INT inDenominator)
{
    if (inDenominator == 0)
        return -1;
    long long theProduct = (long long)inNumber * inNumerator;
    long long theResult = theProduct / inDenominator, theRemainder = theProduct % inDenominator;
    if (2 * (theRemainder < 0 ? -theRemainder : theRemainder) >= (inDenominator < 0 ? -(long long)inDenominator
                                                                                     : inDenominator))
        theResult += (theProduct < 0) != (inDenominator < 0) ? -1 : 1;
    return theResult > 0x7fffffffLL || theResult < -0x7fffffffLL - 1 ? -1 : (INT)theResult;
}

// ---- version, locale ----
// Windows XP (5.1, build 2600)
DWORD WINAPI GetVersion(void)
{
    return 0x0a280105;
}

LANGID WINAPI GetUserDefaultLangID(void)
{
    return MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
}

HKL WINAPI GetKeyboardLayout(DWORD)
{
    return (HKL)(ULONG_PTR)0x04090409;
}

// The locale values the game reads, for US English
INT WINAPI GetLocaleInfoA(LCID, LCTYPE inType, LPSTR outText, INT inSize)
{
    const char* theValue;
    switch (inType & ~LOCALE_NOUSEROVERRIDE) {
    case LOCALE_IDEFAULTANSICODEPAGE: theValue = "1252"; break;
    case LOCALE_IDEFAULTCODEPAGE: theValue = "437"; break;
    case LOCALE_SDECIMAL: theValue = "."; break;
    case LOCALE_STHOUSAND: theValue = ","; break;
    case LOCALE_SENGLANGUAGE: theValue = "English"; break;
    case LOCALE_SABBREVLANGNAME: theValue = "ENU"; break;
    default:
        EE_PORT_UNSUPPORTED("GetLocaleInfoA for this value");
        SetLastError(ERROR_INVALID_FLAGS);
        return 0;
    }
    int theSize = (int)strlen(theValue) + 1;
    if (inSize == 0)
        return theSize;
    if (inSize < theSize) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return 0;
    }
    memcpy(outText, theValue, theSize);
    return theSize;
}

void WINAPI GetSystemTimeAsFileTime(LPFILETIME outTime)
{
    // 100-nanosecond intervals since 1601-01-01
    auto theNow = std::chrono::system_clock::now().time_since_epoch();
    unsigned long long theTicks = std::chrono::duration_cast<std::chrono::nanoseconds>(theNow).count() / 100
                                  + 116444736000000000ULL;
    outTime->dwLowDateTime = (DWORD)theTicks;
    outTime->dwHighDateTime = (DWORD)(theTicks >> 32);
}

// ---- files and folders ----
// The main module's path is the running binary's
DWORD WINAPI GetModuleFileNameA(HMODULE inModule, LPSTR outPath, DWORD inSize)
{
    if (inModule != NULL && inModule != GetModuleHandleA(NULL)) {
        EE_PORT_UNSUPPORTED("GetModuleFileNameA for a module other than the main one");
        SetLastError(ERROR_MOD_NOT_FOUND);
        return 0;
    }
    char theLinux[4096];
    ssize_t theLength = readlink("/proc/self/exe", theLinux, sizeof theLinux - 1);
    if (theLength < 0 || inSize == 0)
        return 0;
    theLinux[theLength] = '\0';
    std::string thePath = eeport::WindowsPath(theLinux);
    lstrcpynA(outPath, thePath.c_str(), (INT)inSize);
    if (thePath.size() >= inSize) {
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return inSize;
    }
    return (DWORD)thePath.size();
}

UINT WINAPI GetWindowsDirectoryA(LPSTR outPath, UINT inSize)
{
    const char kWindows[] = "C:\\windows";
    if (inSize < sizeof kWindows)
        return sizeof kWindows;
    memcpy(outPath, kWindows, sizeof kWindows);
    return sizeof kWindows - 1;
}

// The free space of the volume of a path (the current folder for NULL), at most 2 GB as Windows 9x reports it: a
// caller may multiply the counts in 32 bits
BOOL WINAPI GetDiskFreeSpaceA(LPCSTR inRoot, LPDWORD outSectorsPerCluster, LPDWORD outBytesPerSector,
                              LPDWORD outFreeClusters, LPDWORD outClusters)
{
    struct statvfs theVolume;
    std::string thePath = inRoot ? eeport::LinuxPath(inRoot) : std::string(".");
    if (statvfs(thePath.c_str(), &theVolume) != 0) {
        SetLastError(ERROR_PATH_NOT_FOUND);
        return FALSE;
    }
    const unsigned long long kCluster = 4096, kLimit = 0x7fff8000ULL / kCluster;
    unsigned long long theFree = (unsigned long long)theVolume.f_bavail * theVolume.f_frsize / kCluster;
    unsigned long long theTotal = (unsigned long long)theVolume.f_blocks * theVolume.f_frsize / kCluster;
    if (outSectorsPerCluster)
        *outSectorsPerCluster = 8;
    if (outBytesPerSector)
        *outBytesPerSector = 512;
    if (outFreeClusters)
        *outFreeClusters = (DWORD)(theFree < kLimit ? theFree : kLimit);
    if (outClusters)
        *outClusters = (DWORD)(theTotal < kLimit ? theTotal : kLimit);
    return TRUE;
}

// A volume's serial number, stable for this machine (WON's CD-key code reads it)
BOOL WINAPI GetVolumeInformationA(LPCSTR, LPSTR outName, DWORD inNameSize, LPDWORD outSerial,
                                  LPDWORD outMaximumComponent, LPDWORD outFlags, LPSTR outFileSystem,
                                  DWORD inFileSystemSize)
{
    DWORD theSerial = 0x45454c58;   // "EELX", when the machine has no id
    std::ifstream theID("/etc/machine-id");
    std::string theText;
    if (theID >> theText)
        for (char c : theText)
            theSerial = theSerial * 31 + (unsigned char)c;
    if (outName && inNameSize)
        lstrcpynA(outName, "LINUX", (INT)inNameSize);
    if (outSerial)
        *outSerial = theSerial;
    if (outMaximumComponent)
        *outMaximumComponent = 255;
    if (outFlags)
        *outFlags = FILE_CASE_PRESERVED_NAMES;
    if (outFileSystem && inFileSystemSize)
        lstrcpynA(outFileSystem, "NTFS", (INT)inFileSystemSize);
    return TRUE;
}

// Linux has no short names: the path is its own short form
DWORD WINAPI GetShortPathNameA(LPCSTR inPath, LPSTR outPath, DWORD inSize)
{
    DWORD theLength = (DWORD)strlen(inPath);
    if (theLength >= inSize)
        return theLength + 1;
    memmove(outPath, inPath, theLength + 1);
    return theLength;
}

// Reads a value from an INI file: section and key names compare without case. A NULL key lists the section's keys,
// each followed by a NUL, with an extra NUL at the end.
INT WINAPI GetPrivateProfileStringA(LPCSTR inSection, LPCSTR inKey, LPCSTR inDefault, LPSTR outText, UINT inSize,
                                    LPCSTR inFile)
{
    if (!outText || inSize == 0)
        return 0;
    std::ifstream theStream(eeport::LinuxPath(inFile));
    std::string theLine, theList;
    bool theInSection = false, theFound = false;
    std::string theValue;
    auto theTrim = [](std::string inText) {
        size_t theStart = inText.find_first_not_of(" \t\r");
        size_t theEnd = inText.find_last_not_of(" \t\r");
        return theStart == std::string::npos ? std::string() : inText.substr(theStart, theEnd - theStart + 1);
    };
    while (!theFound && std::getline(theStream, theLine)) {
        theLine = theTrim(theLine);
        if (theLine.empty() || theLine[0] == ';')
            continue;
        if (theLine[0] == '[') {
            size_t theClose = theLine.find(']');
            theInSection = inSection && strcasecmp(theTrim(theLine.substr(1, theClose - 1)).c_str(), inSection) == 0;
            continue;
        }
        size_t theEquals = theLine.find('=');
        if (!theInSection || theEquals == std::string::npos)
            continue;
        std::string theName = theTrim(theLine.substr(0, theEquals));
        if (!inKey)
            theList += theName + '\0';
        else if (strcasecmp(theName.c_str(), inKey) == 0) {
            theValue = theTrim(theLine.substr(theEquals + 1));
            if (theValue.size() >= 2 && (theValue[0] == '"' || theValue[0] == '\'') && theValue.back() == theValue[0])
                theValue = theValue.substr(1, theValue.size() - 2);
            theFound = true;
        }
    }
    if (!inKey) {
        size_t theLength = theList.size() < inSize - 1 ? theList.size() : inSize - 2;
        memcpy(outText, theList.data(), theLength);
        outText[theLength] = '\0';
        if (inSize > 1)
            outText[theLength + 1] = '\0';
        return (INT)theLength;
    }
    if (!theFound)
        theValue = inDefault ? theTrim(inDefault) : std::string();
    lstrcpynA(outText, theValue.c_str(), (INT)inSize);
    return (INT)strlen(outText);
}

// ---- global memory: a handle is the block's address, its size kept before it ----
namespace {
struct GlobalBlock {
    SIZE_T mSize;
    alignas(8) unsigned char mData[1];
};
GlobalBlock* BlockOf(HGLOBAL inMemory)
{
    return inMemory ? (GlobalBlock*)((unsigned char*)inMemory - offsetof(GlobalBlock, mData)) : nullptr;
}
}

HGLOBAL WINAPI GlobalAlloc(UINT inFlags, SIZE_T inSize)
{
    GlobalBlock* theBlock = (GlobalBlock*)((inFlags & GMEM_ZEROINIT) ? calloc(1, offsetof(GlobalBlock, mData) + inSize)
                                                                     : malloc(offsetof(GlobalBlock, mData) + inSize));
    if (!theBlock) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return NULL;
    }
    theBlock->mSize = inSize;
    return theBlock->mData;
}

HGLOBAL WINAPI GlobalFree(HGLOBAL inMemory)
{
    free(BlockOf(inMemory));
    return NULL;
}

LPVOID WINAPI GlobalLock(HGLOBAL inMemory)
{
    return inMemory;
}

BOOL WINAPI GlobalUnlock(HGLOBAL)
{
    SetLastError(NO_ERROR);
    return FALSE;
}

SIZE_T WINAPI GlobalSize(HGLOBAL inMemory)
{
    return inMemory ? BlockOf(inMemory)->mSize : 0;
}

// ---- processes ----
// Opens a document or URL with the desktop's handler (xdg-open); the game opens web pages this way
HINSTANCE WINAPI ShellExecuteA(HWND, LPCSTR inVerb, LPCSTR inFile, LPCSTR, LPCSTR, INT)
{
    if (!inFile || (inVerb && strcasecmp(inVerb, "open") != 0))
        return (HINSTANCE)(ULONG_PTR)SE_ERR_NOASSOC;
    std::string theTarget = strstr(inFile, "://") ? std::string(inFile) : eeport::LinuxPath(inFile);
    char* theArguments[] = {(char*)"xdg-open", (char*)theTarget.c_str(), nullptr};
    pid_t theChild;
    if (posix_spawnp(&theChild, "xdg-open", nullptr, nullptr, theArguments, environ) != 0)
        return (HINSTANCE)(ULONG_PTR)SE_ERR_NOASSOC;
    return (HINSTANCE)(ULONG_PTR)33;
}

// WON starts a web browser or its patcher this way; the Linux build starts no Windows programs
BOOL WINAPI CreateProcessA(LPCSTR, LPSTR inCommandLine, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD,
                           LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION)
{
    eeport::Log("CreateProcessA is not supported: %s", inCommandLine ? inCommandLine : "");
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

DWORD WINAPI WaitForSingleObjectEx(HANDLE inHandle, DWORD inTimeout, BOOL)
{
    return WaitForSingleObject(inHandle, inTimeout);
}

// WON's crash report walks the stack with these; the Linux build writes no such report
BOOL WINAPI GetThreadContext(HANDLE, LPCONTEXT)
{
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

SIZE_T WINAPI VirtualQueryEx(HANDLE, LPCVOID, PMEMORY_BASIC_INFORMATION, SIZE_T)
{
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}

// ---- multimedia timers: one thread each ----
namespace {
struct Timer {
    UINT mID;
    UINT mDelay;
    LPTIMECALLBACK mCallback;
    DWORD_PTR mUser;
    UINT mFlags;
    std::mutex mLock;
    std::condition_variable mChanged;
    bool mStopping = false;
    std::thread mThread;
};
std::mutex gTimersLock;
std::map<UINT, std::shared_ptr<Timer>> gTimers;
UINT gNextTimer = 1;

void RunTimer(std::shared_ptr<Timer> inTimer)
{
    auto theNext = std::chrono::steady_clock::now();
    do {
        theNext += std::chrono::milliseconds(inTimer->mDelay);
        {
            std::unique_lock<std::mutex> theLock(inTimer->mLock);
            if (inTimer->mChanged.wait_until(theLock, theNext, [&] { return inTimer->mStopping; }))
                return;
        }
        if (inTimer->mFlags & TIME_CALLBACK_EVENT_SET)
            SetEvent((HANDLE)inTimer->mCallback);
        else
            inTimer->mCallback(inTimer->mID, 0, inTimer->mUser, 0, 0);
    } while (inTimer->mFlags & TIME_PERIODIC);
    std::lock_guard<std::mutex> theLock(gTimersLock);
    gTimers.erase(inTimer->mID);
    inTimer->mThread.detach();
}
} // namespace

// A timer calls a function or sets an event (TMTimer uses both); the pulse form is not supported
MMRESULT WINAPI timeSetEvent(UINT inDelay, UINT, LPTIMECALLBACK inCallback, DWORD_PTR inUser, UINT inFlags)
{
    if (inDelay == 0 || !inCallback || (inFlags & TIME_CALLBACK_EVENT_PULSE))
        return 0;
    auto theTimer = std::make_shared<Timer>();
    theTimer->mDelay = inDelay;
    theTimer->mCallback = inCallback;
    theTimer->mUser = inUser;
    theTimer->mFlags = inFlags;
    std::lock_guard<std::mutex> theLock(gTimersLock);
    theTimer->mID = gNextTimer++;
    gTimers[theTimer->mID] = theTimer;
    theTimer->mThread = std::thread(RunTimer, theTimer);
    return theTimer->mID;
}

// Stops a timer and waits for a callback in progress, unless the callback itself kills its timer
MMRESULT WINAPI timeKillEvent(UINT inID)
{
    std::shared_ptr<Timer> theTimer;
    {
        std::lock_guard<std::mutex> theLock(gTimersLock);
        auto theEntry = gTimers.find(inID);
        if (theEntry == gTimers.end())
            return MMSYSERR_INVALPARAM;
        theTimer = theEntry->second;
        gTimers.erase(theEntry);
    }
    {
        std::lock_guard<std::mutex> theLock(theTimer->mLock);
        theTimer->mStopping = true;
    }
    theTimer->mChanged.notify_all();
    if (theTimer->mThread.get_id() == std::this_thread::get_id())
        theTimer->mThread.detach();
    else if (theTimer->mThread.joinable())
        theTimer->mThread.join();
    return TIMERR_NOERROR;
}

// ---- COM ----
HRESULT WINAPI CoInitialize(LPVOID)
{
    return S_OK;
}

void WINAPI CoUninitialize(void)
{
}

// CoCreateInstance is DirectMusic.cpp's: DirectMusic's are the only COM classes the game creates

// ---- multimedia devices this build does not have: the game then uses none ----
UINT WINAPI mixerGetNumDevs(void)
{
    return 0;
}

MMRESULT WINAPI mixerGetDevCapsA(UINT_PTR, LPMIXERCAPSA, UINT)
{
    return MMSYSERR_BADDEVICEID;
}

MMRESULT WINAPI mixerGetLineInfoA(HMIXEROBJ, LPMIXERLINEA, DWORD)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI mixerGetLineControlsA(HMIXEROBJ, LPMIXERLINECONTROLSA, DWORD)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI mixerGetControlDetailsA(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI mixerSetControlDetails(HMIXEROBJ, LPMIXERCONTROLDETAILS, DWORD)
{
    return MMSYSERR_INVALHANDLE;
}

UINT WINAPI auxGetNumDevs(void)
{
    return 0;
}

MMRESULT WINAPI auxGetDevCapsA(UINT_PTR, LPAUXCAPSA, UINT)
{
    return MMSYSERR_BADDEVICEID;
}

MMRESULT WINAPI auxGetVolume(UINT, LPDWORD)
{
    return MMSYSERR_BADDEVICEID;
}

MMRESULT WINAPI auxSetVolume(UINT, DWORD)
{
    return MMSYSERR_BADDEVICEID;
}

// CD audio: no CD drive
MCIERROR WINAPI mciSendCommandA(MCIDEVICEID, UINT, DWORD_PTR, DWORD_PTR)
{
    return MCIERR_DEVICE_NOT_INSTALLED;
}

MMRESULT WINAPI waveOutGetDevCapsA(UINT_PTR, LPWAVEOUTCAPSA, UINT)
{
    return MMSYSERR_BADDEVICEID;
}

MMRESULT WINAPI waveOutOpen(LPHWAVEOUT outDevice, UINT, LPCWAVEFORMATEX, DWORD_PTR, DWORD_PTR, DWORD)
{
    if (outDevice)
        *outDevice = NULL;
    return MMSYSERR_NODRIVER;
}

MMRESULT WINAPI waveOutClose(HWAVEOUT)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT, LPWAVEHDR, UINT)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT, LPWAVEHDR, UINT)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI waveOutWrite(HWAVEOUT, LPWAVEHDR, UINT)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI waveOutReset(HWAVEOUT)
{
    return MMSYSERR_INVALHANDLE;
}

MMRESULT WINAPI waveOutSetVolume(HWAVEOUT, DWORD)
{
    return MMSYSERR_INVALHANDLE;
}
