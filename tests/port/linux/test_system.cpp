// Tests of the native build's system calls (src/Port/Linux/Win32System.cpp): conversion buffer rules, wsprintf's
// limit, INI files, global memory, multimedia timers, the clock and the version and volume queries.
#include <windows.h>
#include <mmsystem.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <string>
#include <thread>

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

static std::atomic<int> sTicks;
static UINT sSelfKillID;
static std::atomic<int> sSelfKillResult{-1};

static void CALLBACK OnTick(UINT, UINT, DWORD_PTR inUser, DWORD_PTR, DWORD_PTR)
{
    if (inUser == 7)
        ++sTicks;
}

static void CALLBACK OnSelfKill(UINT inID, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR)
{
    sSelfKillResult = timeKillEvent(inID);
}

int main()
{
    // Conversion sizes: -1 counts the terminator, 0 asks for the size, a short buffer fails
    WCHAR theWide[8];
    CHECK(MultiByteToWideChar(CP_ACP, 0, "abc", -1, NULL, 0) == 4);
    CHECK(MultiByteToWideChar(CP_ACP, 0, "abc", 3, theWide, 8) == 3 && theWide[2] == 'c');
    SetLastError(0);
    CHECK(MultiByteToWideChar(CP_ACP, 0, "abcdef", -1, theWide, 3) == 0);
    CHECK(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    char theNarrow[8];
    BOOL theDefault = FALSE;
    const WCHAR kCastle[] = {'a', 0x0416, 'b', 0};       // Cyrillic Zhe has no Windows-1252 byte
    CHECK(WideCharToMultiByte(CP_ACP, 0, kCastle, -1, theNarrow, 8, "#", &theDefault) == 4);
    CHECK(strcmp(theNarrow, "a#b") == 0 && theDefault);
    wchar_t theLong[8];
    CHECK(MultiByteToWideChar(CP_ACP, 0, "\x80x", 2, theLong, 8) == 2 && theLong[0] == 0x20ac);

    // wsprintf stops at 1024 bytes
    static char theBig[2048];
    std::string theText(1500, 'x');
    CHECK(wsprintfA(theBig, "%s", theText.c_str()) == 1023 && strlen(theBig) == 1023);
    CHECK(wsprintfA(theBig, "%d-%s", 42, "z") == 4 && strcmp(theBig, "42-z") == 0);
    char theCopy[4];
    CHECK(strcmp(lstrcpynA(theCopy, "abcdef", 4), "abc") == 0);

    // INI files: names without case, quotes, defaults, the key list
    std::string theIni = std::string(getenv("EE_PORT_TEST_DIR")) + "/Skin.ini";
    std::ofstream(theIni) << "; skin\r\n[Colours]\r\nBack = 1 2 3\r\nText=\"4 5 6\"\r\n[Other]\r\nBack=9\r\n";
    std::string theWindowsIni = "Z:" + theIni;
    for (char& c : theWindowsIni)
        if (c == '/')
            c = '\\';
    char theValue[64];
    CHECK(GetPrivateProfileStringA("colours", "BACK", "", theValue, 64, theWindowsIni.c_str()) == 5);
    CHECK(strcmp(theValue, "1 2 3") == 0);
    CHECK(GetPrivateProfileStringA("Colours", "Text", "", theValue, 64, theWindowsIni.c_str()) == 5);
    CHECK(strcmp(theValue, "4 5 6") == 0);
    CHECK(GetPrivateProfileStringA("Colours", "Missing", "dflt", theValue, 64, theWindowsIni.c_str()) == 4);
    CHECK(strcmp(theValue, "dflt") == 0);
    CHECK(GetPrivateProfileStringA("Other", "Back", "", theValue, 64, theWindowsIni.c_str()) == 1);
    CHECK(GetPrivateProfileStringA("Colours", "Back", "", theValue, 3, theWindowsIni.c_str()) == 2);
    CHECK(strcmp(theValue, "1 ") == 0);
    CHECK(GetPrivateProfileStringA("Colours", NULL, "", theValue, 64, theWindowsIni.c_str()) == 10);
    CHECK(memcmp(theValue, "Back\0Text\0\0", 11) == 0);
    CHECK(GetPrivateProfileStringA("Colours", "Back", "none", theValue, 64, "Z:\\no\\such.ini") == 4);

    // Global memory
    HGLOBAL theMemory = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, 100);
    CHECK(theMemory != NULL && GlobalSize(theMemory) == 100);
    unsigned char* theBytes = (unsigned char*)GlobalLock(theMemory);
    bool theZero = true;
    for (int i = 0; i < 100; ++i)
        theZero = theZero && theBytes[i] == 0;
    CHECK(theZero);
    GlobalUnlock(theMemory);
    CHECK(GlobalFree(theMemory) == NULL);

    // Multimedia timers: a periodic function, a one-shot event, a callback that kills its own timer
    UINT thePeriodic = timeSetEvent(20, 0, OnTick, 7, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
    CHECK(thePeriodic != 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(210));
    CHECK(timeKillEvent(thePeriodic) == TIMERR_NOERROR);
    int theTicks = sTicks;
    CHECK(theTicks >= 8 && theTicks <= 11);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    CHECK(sTicks == theTicks);                          // no tick after the kill returned
    CHECK(timeKillEvent(thePeriodic) == MMSYSERR_INVALPARAM);

    HANDLE theEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    DWORD theStart = timeGetTime();
    CHECK(timeSetEvent(50, 0, (LPTIMECALLBACK)theEvent, 0, TIME_ONESHOT | TIME_CALLBACK_EVENT_SET) != 0);
    CHECK(WaitForSingleObject(theEvent, 1000) == WAIT_OBJECT_0);
    DWORD theElapsed = timeGetTime() - theStart;
    CHECK(theElapsed >= 45 && theElapsed < 200);
    CHECK(WaitForSingleObject(theEvent, 150) == WAIT_TIMEOUT);   // a one-shot timer fires once
    CloseHandle(theEvent);

    sSelfKillID = timeSetEvent(10, 0, OnSelfKill, 0, TIME_PERIODIC | TIME_CALLBACK_FUNCTION);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    CHECK(sSelfKillResult == TIMERR_NOERROR);
    CHECK(timeSetEvent(10, 0, (LPTIMECALLBACK)1, 0, TIME_CALLBACK_EVENT_PULSE) == 0);

    // Clock, version, volume, folders
    FILETIME theTime;
    GetSystemTimeAsFileTime(&theTime);
    unsigned long long theTicks100 = ((unsigned long long)theTime.dwHighDateTime << 32) | theTime.dwLowDateTime;
    long long theUnix = (long long)(theTicks100 / 10000000ULL) - 11644473600LL;
    CHECK(llabs(theUnix - (long long)time(NULL)) <= 2);
    CHECK(LOBYTE(LOWORD(GetVersion())) == 5 && HIBYTE(LOWORD(GetVersion())) == 1);
    DWORD theSectors, theBytesPerSector, theFree, theTotal;
    CHECK(GetDiskFreeSpaceA(NULL, &theSectors, &theBytesPerSector, &theFree, &theTotal));
    unsigned long long theFreeBytes = (unsigned long long)theSectors * theBytesPerSector * theFree;
    CHECK(theFreeBytes > 0 && theFreeBytes <= 0x7fffffffULL && theFree <= theTotal);
    DWORD theSerial1 = 0, theSerial2 = 1;
    CHECK(GetVolumeInformationA("C:\\", NULL, 0, &theSerial1, NULL, NULL, NULL, 0));
    CHECK(GetVolumeInformationA("C:\\", NULL, 0, &theSerial2, NULL, NULL, NULL, 0) && theSerial1 == theSerial2);
    char thePath[MAX_PATH];
    CHECK(GetModuleFileNameA(NULL, thePath, MAX_PATH) > 3 && thePath[0] == 'Z' && thePath[1] == ':');
    CHECK(strstr(thePath, "test_system") != NULL);
    char theLocale[8];
    CHECK(GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, theLocale, 8) == 5);
    CHECK(strcmp(theLocale, "1252") == 0);

    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
