// Windows paths on Linux, and the C runtime's file calls that take one (port_msvcrt.h)
#include "Win32Internal.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

namespace eeport {

// The folder a drive letter names: Z: is the root, as in Wine; another letter is the link of that name in the Wine
// prefix's dosdevices folder (WINEPREFIX, else ~/.wine), so the paths of a Wine install of the game keep working.
static std::string DriveRoot(char inLetter)
{
    char theLetter = (char)tolower((unsigned char)inLetter);
    if (theLetter == 'z')
        return "";
    const char* thePrefix = getenv("WINEPREFIX");
    std::string theRoot = thePrefix && *thePrefix ? thePrefix : std::string(getenv("HOME") ? getenv("HOME") : "") + "/.wine";
    return theRoot + "/dosdevices/" + theLetter + ":";
}

// The entry of inFolder whose name equals inName without regard to case, or inName itself when there is none
static std::string MatchName(const std::string& inFolder, const std::string& inName)
{
    DIR* theFolder = opendir(inFolder.empty() ? "." : inFolder.c_str());
    if (!theFolder)
        return inName;
    std::string theMatch = inName;
    while (dirent* theEntry = readdir(theFolder))
        if (strcasecmp(theEntry->d_name, inName.c_str()) == 0) {
            theMatch = theEntry->d_name;
            break;
        }
    closedir(theFolder);
    return theMatch;
}

std::string LinuxPath(const char* inWindowsPath)
{
    std::string thePath = inWindowsPath ? inWindowsPath : "";
    for (char& c : thePath)
        if (c == '\\')
            c = '/';
    std::string theRoot;
    if (thePath.size() >= 2 && thePath[1] == ':' && isalpha((unsigned char)thePath[0])) {
        theRoot = DriveRoot(thePath[0]);
        thePath.erase(0, 2);
    }
    std::string theResult = theRoot;
    struct stat theInfo;
    if (stat((theRoot + thePath).c_str(), &theInfo) == 0)
        return theRoot + thePath;
    // Match each component that does not exist as written
    size_t thePosition = 0;
    if (!thePath.empty() && thePath[0] == '/') {
        theResult += "/";
        thePosition = 1;
    }
    while (thePosition <= thePath.size()) {
        size_t theEnd = thePath.find('/', thePosition);
        if (theEnd == std::string::npos)
            theEnd = thePath.size();
        std::string theName = thePath.substr(thePosition, theEnd - thePosition);
        if (!theName.empty()) {
            std::string theFolder = theResult;
            if (theName != "." && theName != "..") {
                std::string theWritten = theFolder + theName;
                if (stat(theWritten.c_str(), &theInfo) != 0)
                    theName = MatchName(theFolder, theName);
            }
            theResult = theFolder + theName;
            if (theEnd < thePath.size())
                theResult += "/";
        }
        thePosition = theEnd + 1;
    }
    return theResult;
}

std::string WindowsPath(const std::string& inLinuxPath)
{
    std::string thePath = inLinuxPath;
    for (char& c : thePath)
        if (c == '/')
            c = '\\';
    if (!thePath.empty() && thePath[0] == '\\')
        thePath = "Z:" + thePath;
    return thePath;
}

// ---- text-mode reads: CR before LF is dropped, as MSVC's runtime does ----
struct TextStream {
    FILE* mFile;
    int mPendingCR;     // a CR read at the end of the last chunk, not yet known to be followed by LF
};

static ssize_t TextRead(void* inCookie, char* outData, size_t inSize)
{
    TextStream* theStream = static_cast<TextStream*>(inCookie);
    size_t theCount = 0;
    while (theCount < inSize) {
        int c = fgetc(theStream->mFile);
        if (theStream->mPendingCR) {
            theStream->mPendingCR = 0;
            if (c != '\n') {
                outData[theCount++] = '\r';
                if (c == EOF)
                    break;
                if (theCount == inSize) {
                    ungetc(c, theStream->mFile);
                    break;
                }
            }
        }
        if (c == EOF)
            break;
        if (c == '\r') {
            theStream->mPendingCR = 1;
            continue;
        }
        outData[theCount++] = (char)c;
        if (c == '\n')
            break;      // a line at a time keeps the stream's position close to the file's
    }
    return (ssize_t)theCount;
}

static ssize_t TextWrite(void* inCookie, const char* inData, size_t inSize)
{
    return (ssize_t)fwrite(inData, 1, inSize, static_cast<TextStream*>(inCookie)->mFile);
}

static int TextSeek(void* inCookie, off64_t* ioOffset, int inWhence)
{
    TextStream* theStream = static_cast<TextStream*>(inCookie);
    theStream->mPendingCR = 0;
    if (fseeko64(theStream->mFile, *ioOffset, inWhence) != 0)
        return -1;
    *ioOffset = ftello64(theStream->mFile);
    return 0;
}

static int TextClose(void* inCookie)
{
    TextStream* theStream = static_cast<TextStream*>(inCookie);
    int theResult = fclose(theStream->mFile);
    delete theStream;
    return theResult;
}

} // namespace eeport

using namespace eeport;

extern "C" {

// The C runtime's fopen: every image of the port links with --wrap=fopen, this layer's too
FILE* __real_fopen(const char* inPath, const char* inMode);

// The game's fopen (--wrap=fopen): a Windows path, and MSVC's text mode when the mode has no b
FILE* __wrap_fopen(const char* inPath, const char* inMode)
{
    std::string theMode;
    bool theBinary = false;
    for (const char* p = inMode; *p; ++p) {
        if (*p == 'b')
            theBinary = true;
        else if (*p != 't')
            theMode += *p;
    }
    FILE* theFile = __real_fopen(LinuxPath(inPath).c_str(), theMode.c_str());
    if (!theFile || theBinary || theMode.find('r') == std::string::npos)
        return theFile;
    TextStream* theStream = new TextStream{theFile, 0};
    cookie_io_functions_t theFunctions = {TextRead, TextWrite, TextSeek, TextClose};
    FILE* theText = fopencookie(theStream, theMode.c_str(), theFunctions);
    if (!theText) {
        fclose(theFile);
        delete theStream;
    }
    return theText;
}

int ee_port_stat(const char* inPath, struct stat* outStat)
{
    return stat(LinuxPath(inPath).c_str(), outStat);
}

// MSVC's modes: _S_IWRITE (0x80) makes the file writable, _S_IREAD (0x100) alone makes it read-only
int _chmod(const char* inPath, int inMode)
{
    std::string thePath = LinuxPath(inPath);
    struct stat theInfo;
    if (stat(thePath.c_str(), &theInfo) != 0)
        return -1;
    mode_t theMode = theInfo.st_mode & 07777;
    theMode = (inMode & 0x80) ? (theMode | S_IWUSR) : (theMode & ~(S_IWUSR | S_IWGRP | S_IWOTH));
    return chmod(thePath.c_str(), theMode);
}

char* _getcwd(char* outPath, int inSize)
{
    char theFolder[4096];
    if (!getcwd(theFolder, sizeof theFolder))
        return NULL;
    std::string thePath = WindowsPath(theFolder);
    if (!outPath)
        return strdup(thePath.c_str());
    if ((int)thePath.size() >= inSize) {
        errno = ERANGE;
        return NULL;
    }
    strcpy(outPath, thePath.c_str());
    return outPath;
}

char* _fullpath(char* outPath, const char* inPath, size_t inSize)
{
    std::string thePath = inPath;
    for (char& c : thePath)
        if (c == '/')
            c = '\\';
    std::string theBase;
    if (thePath.size() >= 2 && thePath[1] == ':') {
        theBase = thePath.substr(0, 2);
        thePath.erase(0, 2);
    } else {
        char theFolder[4096];
        theBase = getcwd(theFolder, sizeof theFolder) ? WindowsPath(theFolder) : "Z:";
        if (!thePath.empty() && thePath[0] == '\\')
            theBase = theBase.substr(0, 2);         // the current drive's root
        else
            thePath = theBase.substr(2) + "\\" + thePath;
        theBase = theBase.substr(0, 2);
    }
    // thePath is now absolute on theBase's drive: resolve . and .. by component
    std::string theResult;
    size_t thePosition = 0;
    while (thePosition <= thePath.size()) {
        size_t theEnd = thePath.find('\\', thePosition);
        if (theEnd == std::string::npos)
            theEnd = thePath.size();
        std::string theName = thePath.substr(thePosition, theEnd - thePosition);
        if (theName == "..") {
            size_t theCut = theResult.find_last_of('\\');
            theResult.erase(theCut == std::string::npos ? 0 : theCut);
        } else if (!theName.empty() && theName != ".")
            theResult += "\\" + theName;
        thePosition = theEnd + 1;
    }
    theResult = theBase + (theResult.empty() ? "\\" : theResult);
    if (!outPath)
        return strdup(theResult.c_str());
    if (theResult.size() + 1 > inSize)
        return NULL;
    strcpy(outPath, theResult.c_str());
    return outPath;
}

int _utime(const char* inPath, struct utimbuf* inTimes)
{
    return utime(LinuxPath(inPath).c_str(), inTimes);
}

} // extern "C"
