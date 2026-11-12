// Tests of the native build's Win32 layer (src/Port/Linux), against the behaviour Windows documents.
// Built and run by tools/test_port_linux.py: clang++ -m32 with the layer's objects, in a scratch folder.
#include <windows.h>
#include <process.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

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

static void TestEvents()
{
    HANDLE theAuto = CreateEventA(NULL, FALSE, FALSE, NULL);
    HANDLE theManual = CreateEventA(NULL, TRUE, TRUE, NULL);
    CHECK(WaitForSingleObject(theAuto, 0) == WAIT_TIMEOUT);
    DWORD theStart = GetTickCount();
    CHECK(WaitForSingleObject(theAuto, 50) == WAIT_TIMEOUT);
    DWORD theWaited = GetTickCount() - theStart;
    CHECK(theWaited >= 45 && theWaited < 1000);
    SetEvent(theAuto);
    CHECK(WaitForSingleObject(theAuto, 0) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(theAuto, 0) == WAIT_TIMEOUT);        // an auto-reset event resets on a wait
    CHECK(WaitForSingleObject(theManual, 0) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(theManual, 0) == WAIT_OBJECT_0);     // a manual-reset event stays set
    ResetEvent(theManual);
    CHECK(WaitForSingleObject(theManual, 0) == WAIT_TIMEOUT);

    HANDLE theBoth[2] = {theAuto, theManual};
    SetEvent(theManual);
    CHECK(WaitForMultipleObjects(2, theBoth, FALSE, 0) == WAIT_OBJECT_0 + 1);
    CHECK(WaitForMultipleObjects(2, theBoth, TRUE, 0) == WAIT_TIMEOUT);
    SetEvent(theAuto);
    CHECK(WaitForMultipleObjects(2, theBoth, FALSE, 0) == WAIT_OBJECT_0);   // the lowest index wins
    SetEvent(theAuto);
    CHECK(WaitForMultipleObjects(2, theBoth, TRUE, 0) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(theAuto, 0) == WAIT_TIMEOUT);        // the wait-all reset the auto event
    CHECK(CloseHandle(theAuto));
    CHECK(CloseHandle(theManual));
    CHECK(!CloseHandle(NULL));
    CHECK(WaitForSingleObject(NULL, 0) == WAIT_FAILED);
}

static DWORD WINAPI SetLater(LPVOID inEvent)
{
    Sleep(30);
    SetEvent(static_cast<HANDLE>(inEvent));
    return 7;
}

static HANDLE sMutex;
static volatile LONG sInside;
static volatile LONG sOverlaps;

static unsigned int __attribute__((stdcall)) HoldMutex(void*)
{
    for (int i = 0; i < 200; ++i) {
        WaitForSingleObject(sMutex, INFINITE);
        WaitForSingleObject(sMutex, INFINITE);     // a mutex is recursive for its owner
        if (InterlockedIncrement(&sInside) != 1)
            InterlockedIncrement(&sOverlaps);
        InterlockedDecrement(&sInside);
        ReleaseMutex(sMutex);
        ReleaseMutex(sMutex);
    }
    return 3;
}

static void TestThreads()
{
    // The calling thread has a thread object as well, though the layer did not start it: GetCurrentThread's pseudo
    // handle answers for the thread, and a duplicate of it is a handle to the same thread, as on Windows
    HANDLE theSelf = NULL;
    CHECK(DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(), &theSelf,
                          THREAD_ALL_ACCESS, FALSE, DUPLICATE_SAME_ACCESS));
    CHECK(theSelf != NULL && theSelf != GetCurrentThread());
    DWORD theSelfCode = 0;
    CHECK(GetExitCodeThread(theSelf, &theSelfCode) && theSelfCode == STILL_ACTIVE);
    CHECK(GetExitCodeThread(GetCurrentThread(), &theSelfCode) && theSelfCode == STILL_ACTIVE);
    CHECK(WaitForSingleObject(theSelf, 0) == WAIT_TIMEOUT);     // the calling thread has not exited
    CHECK(GetThreadPriority(theSelf) == THREAD_PRIORITY_NORMAL);
    CHECK(SetThreadPriority(theSelf, THREAD_PRIORITY_ABOVE_NORMAL));
    CHECK(GetThreadPriority(GetCurrentThread()) == THREAD_PRIORITY_ABOVE_NORMAL);
    CHECK(SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL));
    CHECK(CloseHandle(theSelf));
    CHECK(GetExitCodeThread(GetCurrentThread(), &theSelfCode));  // the pseudo handle outlives the duplicate
    CHECK(GetThreadPriority(GetCurrentThread()) == THREAD_PRIORITY_NORMAL);

    HANDLE theEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    DWORD theID = 0;
    HANDLE theThread = CreateThread(NULL, 0, SetLater, theEvent, 0, &theID);
    CHECK(theThread != NULL);
    CHECK(theID != 0 && theID != GetCurrentThreadId());
    CHECK(WaitForSingleObject(theEvent, 5000) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(theThread, 5000) == WAIT_OBJECT_0);   // a finished thread is signalled
    DWORD theCode = 0;
    CHECK(GetExitCodeThread(theThread, &theCode) && theCode == 7);
    CloseHandle(theThread);

    // A suspended thread does not run until it is resumed
    theThread = CreateThread(NULL, 0, SetLater, theEvent, CREATE_SUSPENDED, NULL);
    CHECK(WaitForSingleObject(theEvent, 100) == WAIT_TIMEOUT);
    CHECK(ResumeThread(theThread) == 1);
    CHECK(WaitForSingleObject(theEvent, 5000) == WAIT_OBJECT_0);
    CHECK(WaitForSingleObject(theThread, 5000) == WAIT_OBJECT_0);
    CloseHandle(theThread);

    // A mutex lets one owner in at a time, and a second owner waits
    sMutex = CreateMutexA(NULL, FALSE, NULL);
    HANDLE theWorkers[3];
    for (HANDLE& theWorker : theWorkers)
        theWorker = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, HoldMutex, NULL, 0, NULL));
    CHECK(WaitForMultipleObjects(3, theWorkers, TRUE, 10000) == WAIT_OBJECT_0);
    CHECK(sOverlaps == 0);
    CHECK(!ReleaseMutex(sMutex));       // not the owner
    for (HANDLE theWorker : theWorkers) {
        CHECK(GetExitCodeThread(theWorker, &theCode) && theCode == 3);
        CloseHandle(theWorker);
    }
    CloseHandle(sMutex);
    CloseHandle(theEvent);

    CRITICAL_SECTION theSection;
    InitializeCriticalSection(&theSection);
    EnterCriticalSection(&theSection);
    EnterCriticalSection(&theSection);
    CHECK(theSection.RecursionCount == 2);
    LeaveCriticalSection(&theSection);
    LeaveCriticalSection(&theSection);
    CHECK(theSection.RecursionCount == 0);
    DeleteCriticalSection(&theSection);
}

static void TestClocks()
{
    LARGE_INTEGER theFrequency, theFirst, theSecond;
    CHECK(QueryPerformanceFrequency(&theFrequency) && theFrequency.QuadPart > 0);
    QueryPerformanceCounter(&theFirst);
    Sleep(20);
    QueryPerformanceCounter(&theSecond);
    double theSeconds = double(theSecond.QuadPart - theFirst.QuadPart) / double(theFrequency.QuadPart);
    CHECK(theSeconds >= 0.019 && theSeconds < 1.0);
    DWORD theTick = GetTickCount();
    DWORD theTime = timeGetTime();
    CHECK(theTime - theTick < 5);
}

static std::string sRoot;   // a scratch folder, as a Linux path

static void WriteText(const std::string& inLinuxPath, const char* inText)
{
    FILE* theFile = fopen(inLinuxPath.c_str(), "wb");       // a Linux path is a Windows path relative to Z:
    fputs(inText, theFile);
    fclose(theFile);
}

static std::string WindowsName(const std::string& inLinuxPath)
{
    std::string theName = "Z:" + inLinuxPath;
    for (char& c : theName)
        if (c == '/')
            c = '\\';
    return theName;
}

static void TestPaths()
{
    mkdir((sRoot + "/Data").c_str(), 0755);
    mkdir((sRoot + "/Data/Scenarios").c_str(), 0755);
    WriteText(sRoot + "/Data/Scenarios/Test Map.scn", "line one\r\nline two\r\nbare\rcr\r\n");
    WriteText(sRoot + "/Data/Scenarios/other.SCN", "x");
    WriteText(sRoot + "/Data/Scenarios/notes.txt", "y");

    // A Windows path in another case finds the file, through the port's fopen
    std::string theUpper = WindowsName(sRoot) + "\\DATA\\SCENARIOS\\TEST MAP.SCN";
    FILE* theFile = fopen(theUpper.c_str(), "rb");
    CHECK(theFile != NULL);
    char theLine[64] = {};
    if (theFile) {
        size_t theCount = fread(theLine, 1, sizeof theLine - 1, theFile);
        CHECK(theCount == 29);          // binary mode keeps every CR
        fclose(theFile);
    }

    // Text mode drops a CR only before LF, as MSVC's runtime does
    theFile = fopen(theUpper.c_str(), "r");
    CHECK(theFile != NULL);
    if (theFile) {
        CHECK(fgets(theLine, sizeof theLine, theFile) && strcmp(theLine, "line one\n") == 0);
        CHECK(fgets(theLine, sizeof theLine, theFile) && strcmp(theLine, "line two\n") == 0);
        CHECK(fgets(theLine, sizeof theLine, theFile) && strcmp(theLine, "bare\rcr\n") == 0);
        CHECK(fgets(theLine, sizeof theLine, theFile) == NULL);
        fclose(theFile);
    }

    // A file that does not exist yet is made under the matched folders, with its name as written
    std::string theNew = WindowsName(sRoot) + "\\data\\scenarios\\New File.txt";
    theFile = fopen(theNew.c_str(), "w");
    CHECK(theFile != NULL);
    if (theFile)
        fclose(theFile);
    struct stat theInfo;
    CHECK(stat((sRoot + "/Data/Scenarios/New File.txt").c_str(), &theInfo) == 0);

    // CreateFile, its size, a seek and a read
    HANDLE theHandle = CreateFileA(theUpper.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    CHECK(theHandle != INVALID_HANDLE_VALUE);
    CHECK(GetFileSize(theHandle, NULL) == 29);
    CHECK(SetFilePointer(theHandle, 10, NULL, FILE_BEGIN) == 10);
    DWORD theRead = 0;
    char theBytes[8] = {};
    CHECK(ReadFile(theHandle, theBytes, 4, &theRead, NULL) && theRead == 4 && memcmp(theBytes, "line", 4) == 0);
    CHECK(ReadFile(theHandle, theBytes, 8, &theRead, NULL) && theRead == 8);
    CHECK(SetFilePointer(theHandle, 0, NULL, FILE_END) == 29);
    CHECK(ReadFile(theHandle, theBytes, 8, &theRead, NULL) && theRead == 0);   // at the end: success, nothing read
    CloseHandle(theHandle);

    std::string theMissing = WindowsName(sRoot) + "\\Data\\nothing.dat";
    CHECK(CreateFileA(theMissing.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL) == INVALID_HANDLE_VALUE);
    CHECK(GetLastError() == ERROR_FILE_NOT_FOUND);
    CHECK(GetFileAttributesA(theMissing.c_str()) == INVALID_FILE_ATTRIBUTES);
    CHECK(GetFileAttributesA((WindowsName(sRoot) + "\\data").c_str()) & FILE_ATTRIBUTE_DIRECTORY);

    // CREATE_NEW refuses an existing file; OPEN_ALWAYS on one says it existed
    CHECK(CreateFileA(theUpper.c_str(), GENERIC_WRITE, 0, NULL, CREATE_NEW, 0, NULL) == INVALID_HANDLE_VALUE);
    CHECK(GetLastError() == ERROR_FILE_EXISTS);
    theHandle = CreateFileA(theUpper.c_str(), GENERIC_READ, 0, NULL, OPEN_ALWAYS, 0, NULL);
    CHECK(theHandle != INVALID_HANDLE_VALUE && GetLastError() == ERROR_ALREADY_EXISTS);
    CloseHandle(theHandle);

    // A find with a wildcard matches without regard to case; "*.*" lists everything
    WIN32_FIND_DATAA theFound;
    std::string thePattern = WindowsName(sRoot) + "\\Data\\Scenarios\\*.scn";
    HANDLE theFind = FindFirstFileA(thePattern.c_str(), &theFound);
    CHECK(theFind != INVALID_HANDLE_VALUE);
    int theMatches = 0;
    bool theSawMap = false, theSawOther = false;
    if (theFind != INVALID_HANDLE_VALUE) {
        do {
            ++theMatches;
            theSawMap |= strcmp(theFound.cFileName, "Test Map.scn") == 0 && theFound.nFileSizeLow == 29;
            theSawOther |= strcmp(theFound.cFileName, "other.SCN") == 0;
        } while (FindNextFileA(theFind, &theFound));
        CHECK(GetLastError() == ERROR_NO_MORE_FILES);
        FindClose(theFind);
    }
    CHECK(theMatches == 2 && theSawMap && theSawOther);
    thePattern = WindowsName(sRoot) + "\\Data\\Scenarios\\*.*";
    theFind = FindFirstFileA(thePattern.c_str(), &theFound);
    theMatches = 0;
    if (theFind != INVALID_HANDLE_VALUE) {
        do
            ++theMatches;
        while (FindNextFileA(theFind, &theFound));
        FindClose(theFind);
    }
    CHECK(theMatches == 6);     // ".", "..", and four files
    thePattern = WindowsName(sRoot) + "\\Data\\Scenarios\\*.wav";
    CHECK(FindFirstFileA(thePattern.c_str(), &theFound) == INVALID_HANDLE_VALUE);
    CHECK(GetLastError() == ERROR_FILE_NOT_FOUND);

    // _splitpath, as the MSVC runtime splits
    char theDrive[_MAX_DRIVE], theFolder[_MAX_DIR], theName[_MAX_FNAME], theExtension[_MAX_EXT];
    _splitpath("C:\\Games\\EE\\data.ssa", theDrive, theFolder, theName, theExtension);
    CHECK(strcmp(theDrive, "C:") == 0 && strcmp(theFolder, "\\Games\\EE\\") == 0);
    CHECK(strcmp(theName, "data") == 0 && strcmp(theExtension, ".ssa") == 0);
    char theFull[MAX_PATH];
    CHECK(_fullpath(theFull, "C:\\Games\\EE\\..\\.\\Data\\x.ssa", sizeof theFull) &&
          strcmp(theFull, "C:\\Games\\Data\\x.ssa") == 0);
    CHECK(_fullpath(theFull, "C:/a/../../b", sizeof theFull) && strcmp(theFull, "C:\\b") == 0);
    std::string theRelative = WindowsName(sRoot) + "\\Data";
    SetCurrentDirectoryA(theRelative.c_str());
    CHECK(_fullpath(theFull, "Scenarios\\..\\x.txt", sizeof theFull) &&
          strcmp(theFull, (theRelative + "\\x.txt").c_str()) == 0);
    CHECK(_fullpath(theFull, "C:\\x", 4) == NULL);        // too long for the buffer
    _splitpath("file", theDrive, theFolder, theName, theExtension);
    CHECK(theDrive[0] == 0 && theFolder[0] == 0 && strcmp(theName, "file") == 0 && theExtension[0] == 0);
}

static void TestRuntime()
{
    char theText[40];
    CHECK(strcmp(_itoa(-42, theText, 10), "-42") == 0);
    CHECK(strcmp(_itoa(255, theText, 16), "ff") == 0);
    CHECK(strcmp(_itoa(-1, theText, 16), "ffffffff") == 0);     // other radixes are unsigned
    CHECK(strcmp(_itoa(0, theText, 2), "0") == 0);
    CHECK(_stricmp("Data.SSA", "data.ssa") == 0 && _stricmp("a", "B") < 0);
    CHECK(_mbsnbcnt((const unsigned char*)"abc", 10) == 3 && _mbsnbcnt((const unsigned char*)"abc", 2) == 2);
    const unsigned char* theString = (const unsigned char*)"hello";
    CHECK(_mbsninc(theString, 3) == theString + 3 && _mbsninc(theString, 9) == theString + 5);

    // The x87 control word: precision and rounding round-trip through the MSVC form
    unsigned int theSaved = _controlfp(0, 0);
    _controlfp(_PC_24, _MCW_PC);
    CHECK((_controlfp(0, 0) & _MCW_PC) == _PC_24);
    _controlfp(_PC_53, _MCW_PC);
    CHECK((_controlfp(0, 0) & _MCW_PC) == _PC_53);
    volatile double theThird = 1.0;
    theThird /= 3.0;
    _controlfp(_RC_CHOP, _MCW_RC);
    CHECK((_controlfp(0, 0) & _MCW_RC) == _RC_CHOP);
    volatile float theValue = 2.7f;
    long theRounded;
    __asm__ __volatile__("fistpl %0" : "=m"(theRounded) : "t"((double)theValue) : "st");
    CHECK(theRounded == 2);         // chop rounds toward zero in the FPU itself
    _controlfp(_RC_NEAR, _MCW_RC);
    __asm__ __volatile__("fistpl %0" : "=m"(theRounded) : "t"((double)theValue) : "st");
    CHECK(theRounded == 3);
    _controlfp(theSaved, _MCW_PC | _MCW_RC);
    (void)theThird;
}

int main()
{
    char theFolder[] = "/tmp/ee-port-test-XXXXXX";
    const char* theScratch = getenv("EE_PORT_TEST_DIR");
    sRoot = theScratch ? theScratch : mkdtemp(theFolder);
    TestEvents();
    TestThreads();
    TestClocks();
    TestPaths();
    TestRuntime();
    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
