// Native Linux build only: the entry point, and the GameSpy query-reporting functions that WON calls.
#ifdef EE_PORT_LINUX
#include <windows.h>

#include <cstring>
#include <string>

#include "ee/wonapi/GameSpySDK/gqueryreporting.h"

// Windows starts the game at WinMain with the command line after the program name. The game finds its files from the
// current directory (File System.cpp), so it starts in its own folder.
int main(int argc, char** argv)
{
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
    return WinMain(GetModuleHandleA(NULL), NULL, &theLine[0], SW_SHOWNORMAL);
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
