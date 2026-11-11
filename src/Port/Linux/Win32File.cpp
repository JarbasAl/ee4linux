// Win32 files, folders, finds and file mappings on POSIX, by Windows path (Win32Path.cpp)
#include "Win32Internal.h"

#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fnmatch.h>
#include <map>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace eeport {

struct File : KernelObject {
    explicit File(int inDescriptor) : KernelObject(kFile), mDescriptor(inDescriptor) {}
    ~File() override { close(this->mDescriptor); }
    const int mDescriptor;
};

struct Mapping : KernelObject {
    Mapping() : KernelObject(kMapping) {}
    ~Mapping() override
    {
        if (this->mDescriptor >= 0)
            close(this->mDescriptor);
    }
    int mDescriptor = -1;       // a duplicate of the file's, or -1 for memory alone
    DWORD mProtect = 0;
    size_t mSize = 0;
};

struct Find : KernelObject {
    Find() : KernelObject(kFind) {}
    std::string mFolder;        // the Linux folder, with a trailing slash
    std::vector<std::string> mNames;
    size_t mNext = 0;
};

static DWORD ErrorOf(int inErrno)
{
    switch (inErrno) {
    case ENOENT: return ERROR_FILE_NOT_FOUND;
    case ENOTDIR: return ERROR_PATH_NOT_FOUND;
    case EACCES:
    case EPERM:
    case EROFS: return ERROR_ACCESS_DENIED;
    case EEXIST: return ERROR_FILE_EXISTS;
    case ENOSPC: return ERROR_DISK_FULL;
    case EMFILE:
    case ENFILE: return ERROR_TOO_MANY_OPEN_FILES;
    case ENOTEMPTY: return ERROR_DIR_NOT_EMPTY;
    case EBADF: return ERROR_INVALID_HANDLE;
    default: return ERROR_GEN_FAILURE;
    }
}

static void SetErrorFromErrno() { SetLastError(ErrorOf(errno)); }

static File* FileOf(HANDLE inHandle)
{
    KernelObject* theObject = ObjectOf(inHandle);
    return theObject && theObject->mKind == KernelObject::kFile ? static_cast<File*>(theObject) : NULL;
}

// 100 ns intervals since 1601, from a POSIX time
static FILETIME FileTimeOf(const timespec& inTime)
{
    unsigned long long theTicks = ((unsigned long long)inTime.tv_sec + 11644473600ull) * 10000000ull +
                                  (unsigned long long)inTime.tv_nsec / 100ull;
    FILETIME theResult;
    theResult.dwLowDateTime = (DWORD)theTicks;
    theResult.dwHighDateTime = (DWORD)(theTicks >> 32);
    return theResult;
}

static DWORD AttributesOf(const std::string& inPath, const struct stat& inInfo)
{
    DWORD theAttributes = 0;
    if (S_ISDIR(inInfo.st_mode))
        theAttributes |= FILE_ATTRIBUTE_DIRECTORY;
    if (access(inPath.c_str(), W_OK) != 0)
        theAttributes |= FILE_ATTRIBUTE_READONLY;
    return theAttributes ? theAttributes : FILE_ATTRIBUTE_NORMAL;
}

// The Windows wildcard match: * and ?, without regard to case; "*.*" matches every name
static bool MatchesPattern(const std::string& inPattern, const char* inName)
{
    if (inPattern == "*.*" || inPattern == "*")
        return true;
    return fnmatch(inPattern.c_str(), inName, FNM_CASEFOLD | FNM_NOESCAPE) == 0;
}

static bool FillFind(Find* ioFind, LPWIN32_FIND_DATAA outData)
{
    while (ioFind->mNext < ioFind->mNames.size()) {
        const std::string& theName = ioFind->mNames[ioFind->mNext++];
        std::string thePath = ioFind->mFolder + theName;
        struct stat theInfo;
        if (stat(thePath.c_str(), &theInfo) != 0)
            continue;
        memset(outData, 0, sizeof *outData);
        outData->dwFileAttributes = AttributesOf(thePath, theInfo);
        outData->ftCreationTime = FileTimeOf(theInfo.st_ctim);
        outData->ftLastAccessTime = FileTimeOf(theInfo.st_atim);
        outData->ftLastWriteTime = FileTimeOf(theInfo.st_mtim);
        unsigned long long theSize = S_ISDIR(theInfo.st_mode) ? 0 : (unsigned long long)theInfo.st_size;
        outData->nFileSizeHigh = (DWORD)(theSize >> 32);
        outData->nFileSizeLow = (DWORD)theSize;
        snprintf(outData->cFileName, sizeof outData->cFileName, "%s", theName.c_str());
        return true;
    }
    return false;
}

// The views of file mappings, by address: UnmapViewOfFile needs the length
static std::mutex sViewsLock;
static std::map<void*, size_t> sViews;

} // namespace eeport

using namespace eeport;

extern "C" {

HANDLE WINAPI CreateFileA(LPCSTR inName, DWORD inAccess, DWORD inShare, LPSECURITY_ATTRIBUTES inSecurity,
                          DWORD inDisposition, DWORD inFlags, HANDLE inTemplate)
{
    (void)inShare, (void)inSecurity, (void)inFlags, (void)inTemplate;
    std::string thePath = LinuxPath(inName);
    int theFlags = (inAccess & GENERIC_WRITE) ? ((inAccess & GENERIC_READ) ? O_RDWR : O_WRONLY) : O_RDONLY;
    struct stat theInfo;
    bool theExists = stat(thePath.c_str(), &theInfo) == 0;
    switch (inDisposition) {
    case CREATE_NEW: theFlags |= O_CREAT | O_EXCL; break;
    case CREATE_ALWAYS: theFlags |= O_CREAT | O_TRUNC; break;
    case OPEN_ALWAYS: theFlags |= O_CREAT; break;
    case TRUNCATE_EXISTING: theFlags |= O_TRUNC; break;
    default: break;     // OPEN_EXISTING
    }
    if (theExists && S_ISDIR(theInfo.st_mode)) {
        SetLastError(ERROR_ACCESS_DENIED);
        return INVALID_HANDLE_VALUE;
    }
    int theDescriptor = open(thePath.c_str(), theFlags | O_CLOEXEC, 0644);
    if (theDescriptor < 0) {
        SetErrorFromErrno();
        if (errno == ENOENT && inDisposition != OPEN_EXISTING && inDisposition != TRUNCATE_EXISTING)
            SetLastError(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    SetLastError(theExists && (inDisposition == CREATE_ALWAYS || inDisposition == OPEN_ALWAYS) ? ERROR_ALREADY_EXISTS : 0);
    return NewHandle(new File(theDescriptor));
}

BOOL WINAPI ReadFile(HANDLE inFile, LPVOID outData, DWORD inSize, LPDWORD outRead, LPOVERLAPPED inOverlapped)
{
    (void)inOverlapped;
    File* theFile = FileOf(inFile);
    if (outRead)
        *outRead = 0;
    if (!theFile) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    DWORD theDone = 0;
    while (theDone < inSize) {
        ssize_t theCount = read(theFile->mDescriptor, static_cast<char*>(outData) + theDone, inSize - theDone);
        if (theCount < 0 && errno == EINTR)
            continue;
        if (theCount < 0) {
            SetErrorFromErrno();
            return FALSE;
        }
        if (theCount == 0)
            break;
        theDone += (DWORD)theCount;
    }
    if (outRead)
        *outRead = theDone;
    return TRUE;
}

BOOL WINAPI WriteFile(HANDLE inFile, LPCVOID inData, DWORD inSize, LPDWORD outWritten, LPOVERLAPPED inOverlapped)
{
    (void)inOverlapped;
    File* theFile = FileOf(inFile);
    if (outWritten)
        *outWritten = 0;
    if (!theFile) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    DWORD theDone = 0;
    while (theDone < inSize) {
        ssize_t theCount = write(theFile->mDescriptor, static_cast<const char*>(inData) + theDone, inSize - theDone);
        if (theCount < 0 && errno == EINTR)
            continue;
        if (theCount < 0) {
            SetErrorFromErrno();
            if (outWritten)
                *outWritten = theDone;
            return FALSE;
        }
        theDone += (DWORD)theCount;
    }
    if (outWritten)
        *outWritten = theDone;
    return TRUE;
}

DWORD WINAPI SetFilePointer(HANDLE inFile, LONG inDistance, PLONG ioDistanceHigh, DWORD inMethod)
{
    File* theFile = FileOf(inFile);
    if (!theFile) {
        SetLastError(ERROR_INVALID_HANDLE);
        return INVALID_SET_FILE_POINTER;
    }
    long long theDistance = ioDistanceHigh ? ((long long)*ioDistanceHigh << 32) | (unsigned long)inDistance : inDistance;
    int theWhence = inMethod == FILE_CURRENT ? SEEK_CUR : inMethod == FILE_END ? SEEK_END : SEEK_SET;
    off64_t thePosition = lseek64(theFile->mDescriptor, theDistance, theWhence);
    if (thePosition < 0) {
        SetLastError(ERROR_NEGATIVE_SEEK);
        return INVALID_SET_FILE_POINTER;
    }
    if (ioDistanceHigh)
        *ioDistanceHigh = (LONG)(thePosition >> 32);
    SetLastError(0);
    return (DWORD)thePosition;
}

DWORD WINAPI GetFileSize(HANDLE inFile, LPDWORD outSizeHigh)
{
    File* theFile = FileOf(inFile);
    struct stat theInfo;
    if (!theFile || fstat(theFile->mDescriptor, &theInfo) != 0) {
        SetLastError(ERROR_INVALID_HANDLE);
        return INVALID_FILE_SIZE;
    }
    if (outSizeHigh)
        *outSizeHigh = (DWORD)((unsigned long long)theInfo.st_size >> 32);
    return (DWORD)theInfo.st_size;
}

BOOL WINAPI SetEndOfFile(HANDLE inFile)
{
    File* theFile = FileOf(inFile);
    if (!theFile) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    off64_t thePosition = lseek64(theFile->mDescriptor, 0, SEEK_CUR);
    if (thePosition < 0 || ftruncate64(theFile->mDescriptor, thePosition) != 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI GetFileTime(HANDLE inFile, LPFILETIME outCreation, LPFILETIME outAccess, LPFILETIME outWrite)
{
    File* theFile = FileOf(inFile);
    struct stat theInfo;
    if (!theFile || fstat(theFile->mDescriptor, &theInfo) != 0) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (outCreation)
        *outCreation = FileTimeOf(theInfo.st_ctim);
    if (outAccess)
        *outAccess = FileTimeOf(theInfo.st_atim);
    if (outWrite)
        *outWrite = FileTimeOf(theInfo.st_mtim);
    return TRUE;
}

INT WINAPI CompareFileTime(const FILETIME* inFirst, const FILETIME* inSecond)
{
    unsigned long long a = ((unsigned long long)inFirst->dwHighDateTime << 32) | inFirst->dwLowDateTime;
    unsigned long long b = ((unsigned long long)inSecond->dwHighDateTime << 32) | inSecond->dwLowDateTime;
    return a < b ? -1 : a > b ? 1 : 0;
}

DWORD WINAPI GetFileAttributesA(LPCSTR inName)
{
    std::string thePath = LinuxPath(inName);
    struct stat theInfo;
    if (stat(thePath.c_str(), &theInfo) != 0) {
        SetErrorFromErrno();
        return INVALID_FILE_ATTRIBUTES;
    }
    return AttributesOf(thePath, theInfo);
}

BOOL WINAPI GetFileAttributesExA(LPCSTR inName, GET_FILEEX_INFO_LEVELS inLevel, LPVOID outInformation)
{
    (void)inLevel;
    std::string thePath = LinuxPath(inName);
    struct stat theInfo;
    if (stat(thePath.c_str(), &theInfo) != 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    WIN32_FILE_ATTRIBUTE_DATA* theData = static_cast<WIN32_FILE_ATTRIBUTE_DATA*>(outInformation);
    theData->dwFileAttributes = AttributesOf(thePath, theInfo);
    theData->ftCreationTime = FileTimeOf(theInfo.st_ctim);
    theData->ftLastAccessTime = FileTimeOf(theInfo.st_atim);
    theData->ftLastWriteTime = FileTimeOf(theInfo.st_mtim);
    unsigned long long theSize = S_ISDIR(theInfo.st_mode) ? 0 : (unsigned long long)theInfo.st_size;
    theData->nFileSizeHigh = (DWORD)(theSize >> 32);
    theData->nFileSizeLow = (DWORD)theSize;
    return TRUE;
}

BOOL WINAPI SetFileAttributesA(LPCSTR inName, DWORD inAttributes)
{
    std::string thePath = LinuxPath(inName);
    struct stat theInfo;
    if (stat(thePath.c_str(), &theInfo) != 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    mode_t theMode = theInfo.st_mode & 07777;
    theMode = (inAttributes & FILE_ATTRIBUTE_READONLY) ? (theMode & ~(S_IWUSR | S_IWGRP | S_IWOTH)) : (theMode | S_IWUSR);
    if (chmod(thePath.c_str(), theMode) != 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI DeleteFileA(LPCSTR inName)
{
    if (unlink(LinuxPath(inName).c_str()) != 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI CreateDirectoryA(LPCSTR inName, LPSECURITY_ATTRIBUTES inSecurity)
{
    (void)inSecurity;
    if (mkdir(LinuxPath(inName).c_str(), 0755) != 0) {
        SetLastError(errno == EEXIST ? ERROR_ALREADY_EXISTS : ErrorOf(errno));
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI RemoveDirectoryA(LPCSTR inName)
{
    if (rmdir(LinuxPath(inName).c_str()) != 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI CopyFileA(LPCSTR inSource, LPCSTR inTarget, BOOL inFailIfExists)
{
    std::string theSource = LinuxPath(inSource), theTarget = LinuxPath(inTarget);
    int theIn = open(theSource.c_str(), O_RDONLY | O_CLOEXEC);
    if (theIn < 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    int theOut = open(theTarget.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC | (inFailIfExists ? O_EXCL : 0), 0644);
    if (theOut < 0) {
        SetErrorFromErrno();
        close(theIn);
        return FALSE;
    }
    char theBuffer[65536];
    ssize_t theCount;
    bool theOK = true;
    while ((theCount = read(theIn, theBuffer, sizeof theBuffer)) > 0)
        if (write(theOut, theBuffer, (size_t)theCount) != theCount) {
            theOK = false;
            break;
        }
    if (theCount < 0)
        theOK = false;
    if (!theOK)
        SetErrorFromErrno();
    close(theIn);
    close(theOut);
    return theOK;
}

HANDLE WINAPI FindFirstFileA(LPCSTR inPattern, LPWIN32_FIND_DATAA outData)
{
    std::string thePattern = inPattern;
    size_t theSeparator = thePattern.find_last_of("\\/");
    std::string theFolder = theSeparator == std::string::npos ? "." : LinuxPath(thePattern.substr(0, theSeparator + 1).c_str());
    std::string theName = theSeparator == std::string::npos ? thePattern : thePattern.substr(theSeparator + 1);
    if (theFolder.empty() || theFolder.back() != '/')
        theFolder += "/";
    DIR* theDirectory = opendir(theFolder.c_str());
    if (!theDirectory) {
        SetLastError(ERROR_PATH_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    Find* theFind = new Find;
    theFind->mFolder = theFolder;
    while (dirent* theEntry = readdir(theDirectory))
        if (MatchesPattern(theName, theEntry->d_name))
            theFind->mNames.push_back(theEntry->d_name);
    closedir(theDirectory);
    if (!FillFind(theFind, outData)) {
        delete theFind;
        SetLastError(ERROR_FILE_NOT_FOUND);
        return INVALID_HANDLE_VALUE;
    }
    return NewHandle(theFind);
}

BOOL WINAPI FindNextFileA(HANDLE inFind, LPWIN32_FIND_DATAA outData)
{
    KernelObject* theObject = ObjectOf(inFind);
    if (!theObject || theObject->mKind != KernelObject::kFind) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    if (!FillFind(static_cast<Find*>(theObject), outData)) {
        SetLastError(ERROR_NO_MORE_FILES);
        return FALSE;
    }
    return TRUE;
}

BOOL WINAPI FindClose(HANDLE inFind)
{
    return CloseHandle(inFind);
}

UINT WINAPI GetCurrentDirectoryA(UINT inSize, LPSTR outPath)
{
    char theFolder[4096];
    if (!getcwd(theFolder, sizeof theFolder))
        return 0;
    std::string thePath = WindowsPath(theFolder);
    if (thePath.size() + 1 > inSize)
        return (UINT)thePath.size() + 1;
    strcpy(outPath, thePath.c_str());
    return (UINT)thePath.size();
}

BOOL WINAPI SetCurrentDirectoryA(LPCSTR inPath)
{
    if (chdir(LinuxPath(inPath).c_str()) != 0) {
        SetErrorFromErrno();
        return FALSE;
    }
    return TRUE;
}

DWORD WINAPI GetTempPathA(DWORD inSize, LPSTR outPath)
{
    const char* theFolder = getenv("TMPDIR");
    std::string thePath = WindowsPath(std::string(theFolder && *theFolder ? theFolder : "/tmp")) + "\\";
    if (thePath.size() + 1 > inSize)
        return (DWORD)thePath.size() + 1;
    strcpy(outPath, thePath.c_str());
    return (DWORD)thePath.size();
}

// A name <folder>\<prefix><hex>.TMP; with inUnique 0 the file is made, as Windows does
UINT WINAPI GetTempFileNameA(LPCSTR inFolder, LPCSTR inPrefix, UINT inUnique, LPSTR outName)
{
    std::string theFolder = inFolder;
    if (!theFolder.empty() && theFolder.back() != '\\' && theFolder.back() != '/')
        theFolder += "\\";
    std::string thePrefix = std::string(inPrefix ? inPrefix : "").substr(0, 3);
    UINT theNumber = inUnique ? inUnique : (UINT)(GetTickCount() & 0xffff);
    for (int theTry = 0; theTry < 65536; ++theTry) {
        char theName[MAX_PATH];
        snprintf(theName, sizeof theName, "%s%s%X.TMP", theFolder.c_str(), thePrefix.c_str(), theNumber & 0xffff);
        if (inUnique) {
            strcpy(outName, theName);
            return inUnique;
        }
        int theDescriptor = open(LinuxPath(theName).c_str(), O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0644);
        if (theDescriptor >= 0) {
            close(theDescriptor);
            strcpy(outName, theName);
            return theNumber & 0xffff;
        }
        if (errno != EEXIST)
            break;
        theNumber = (theNumber + 1) & 0xffff ? theNumber + 1 : 1;
    }
    SetErrorFromErrno();
    return 0;
}

HANDLE WINAPI CreateFileMappingA(HANDLE inFile, LPSECURITY_ATTRIBUTES inSecurity, DWORD inProtect, DWORD inSizeHigh,
                                 DWORD inSizeLow, LPCSTR inName)
{
    (void)inSecurity;
    if (inName)
        Log("CreateFileMappingA(\"%s\"): named mappings are not shared between processes here", inName);
    Mapping* theMapping = new Mapping;
    theMapping->mProtect = inProtect;
    theMapping->mSize = ((size_t)inSizeHigh << 32) | inSizeLow;
    if (File* theFile = FileOf(inFile)) {
        theMapping->mDescriptor = dup(theFile->mDescriptor);
        struct stat theInfo;
        if (theMapping->mSize == 0 && fstat(theMapping->mDescriptor, &theInfo) == 0)
            theMapping->mSize = (size_t)theInfo.st_size;
    }
    SetLastError(0);
    return NewHandle(theMapping);
}

LPVOID WINAPI MapViewOfFile(HANDLE inMapping, DWORD inAccess, DWORD inOffsetHigh, DWORD inOffsetLow, SIZE_T inSize)
{
    KernelObject* theObject = ObjectOf(inMapping);
    if (!theObject || theObject->mKind != KernelObject::kMapping) {
        SetLastError(ERROR_INVALID_HANDLE);
        return NULL;
    }
    Mapping* theMapping = static_cast<Mapping*>(theObject);
    off64_t theOffset = ((off64_t)inOffsetHigh << 32) | inOffsetLow;
    size_t theSize = inSize ? inSize : theMapping->mSize - (size_t)theOffset;
    int theProtect = PROT_READ | ((inAccess & FILE_MAP_WRITE) ? PROT_WRITE : 0);
    int theFlags = (inAccess & FILE_MAP_COPY) ? MAP_PRIVATE : MAP_SHARED;
    void* theView = theMapping->mDescriptor >= 0
                        ? mmap64(NULL, theSize, theProtect, theFlags, theMapping->mDescriptor, theOffset)
                        : mmap(NULL, theSize, theProtect, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (theView == MAP_FAILED) {
        SetErrorFromErrno();
        return NULL;
    }
    std::lock_guard<std::mutex> theLock(sViewsLock);
    sViews[theView] = theSize;
    return theView;
}

BOOL WINAPI UnmapViewOfFile(LPCVOID inView)
{
    size_t theSize;
    {
        std::lock_guard<std::mutex> theLock(sViewsLock);
        auto theEntry = sViews.find(const_cast<void*>(inView));
        if (theEntry == sViews.end()) {
            SetLastError(ERROR_INVALID_ADDRESS);
            return FALSE;
        }
        theSize = theEntry->second;
        sViews.erase(theEntry);
    }
    return munmap(const_cast<void*>(inView), theSize) == 0;
}

} // extern "C"
