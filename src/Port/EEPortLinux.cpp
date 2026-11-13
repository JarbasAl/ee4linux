// Native Linux build only: the entry point, and the GameSpy query-reporting functions that WON calls.
#ifdef EE_PORT_LINUX
#include <windows.h>

#include <cstring>
#include <string>

#include "ee/wonapi/GameSpySDK/gqueryreporting.h"
#include "platform/PlatSim.h"

int EESimMain(int argc, char** argv);   // src/Port/EESim.cpp
void EESimGameEnded(int inResult);

// Windows starts the game at WinMain with the command line after the program name. The game finds its files from the
// current directory (File System.cpp), so it starts in its own folder. "ee-aoc --sim [options]" runs the headless
// simulation (src/Port/EESim.cpp) instead, with an empty game command line.
int main(int argc, char** argv)
{
    if (argc > 1 && !strcmp(argv[1], "--sim")) {
        int theResult = EESimMain(argc - 1, argv + 1);
        if (theResult != -1)
            return theResult;
        argc = 1;
    }
    std::string theLine;
    for (int i = 1; i < argc; ++i) {
        if (i > 1)
            theLine += ' ';
        bool theQuoted = strchr(argv[i], ' ') != NULL;
        if (theQuoted)
            theLine += '"';
        theLine += argv[i];
        if (theQuoted)
            theLine += '"';
    }
    int theResult = WinMain(GetModuleHandleA(NULL), NULL, &theLine[0], SW_SHOWNORMAL);
    if (gPlatSimMode)
        EESimGameEnded(theResult);   // does not return
    return theResult;
}

// The native build has no GameSpy reporting: qr_init fails as when its socket cannot open, and WON plays on without
// advertising the game to GameSpy's master server
extern "C" {

int qr_init(qr_t* outRecord, const char*, int, const char*, const char*, qr_querycallback_t, qr_querycallback_t,
            qr_querycallback_t, qr_querycallback_t, void*)
{
    if (outRecord)
        *outRecord = NULL;
    return E_GOA_WSOCKERROR;
}

void qr_process_queries(qr_t)
{
}

void qr_send_statechanged(qr_t)
{
}

} // extern "C"

#endif // EE_PORT_LINUX
