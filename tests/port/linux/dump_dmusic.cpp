// Loads one segment from a folder through the native build's DirectMusic, downloads it to a performance and plays
// one second of it, for tools/test_port_linux.py: dump_dmusic <folder> <file>. With EE_DMUSIC_LOG=warn the log names
// every DLS collection a band asks for and does not get.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <dmusici.h>
#include <mss/mss.h>

#include <cstdio>
#include <string>
#include <vector>

static void Wide(WCHAR* outText, const std::string& inText)
{
    size_t i = 0;
    for (; i < inText.size(); ++i)
        outText[i] = WCHAR(static_cast<unsigned char>(inText[i]));
    outText[i] = 0;
}

int main(int argc, char** argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: dump_dmusic <folder> <file>\n");
        return 2;
    }
    eeport::MilesWithoutDevice();
    (AIL_startup)();
    HDIGDRIVER theDriver = AIL_open_digital_driver(22050, 16, 2, 0);
    IDirectMusicLoader* theLoader = nullptr;
    IDirectMusicPerformance* thePerformance = nullptr;
    CoCreateInstance(CLSID_DirectMusicLoader, nullptr, CLSCTX_INPROC, IID_IDirectMusicLoader, (void**)&theLoader);
    CoCreateInstance(CLSID_DirectMusicPerformance, nullptr, CLSCTX_INPROC, IID_IDirectMusicPerformance2,
                     (void**)&thePerformance);
    if (!theDriver || !theLoader || !thePerformance || thePerformance->Init(nullptr, nullptr, nullptr) != S_OK) {
        fprintf(stderr, "dump_dmusic: no driver, loader or performance\n");
        return 1;
    }
    thePerformance->AddPort(nullptr);
    WCHAR theFolder[MAX_PATH];
    Wide(theFolder, argv[1]);
    theLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, theFolder, FALSE);
    DMUS_OBJECTDESC theDescription = {};
    theDescription.dwSize = sizeof theDescription;
    theDescription.guidClass = CLSID_DirectMusicSegment;
    theDescription.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
    Wide(theDescription.wszFileName, argv[2]);
    IDirectMusicSegment* theSegment = nullptr;
    HRESULT theResult = theLoader->GetObject(&theDescription, IID_IDirectMusicSegment2, (void**)&theSegment);
    if (!theSegment) {
        fprintf(stderr, "dump_dmusic: %s: %08x\n", argv[2], unsigned(theResult));
        return 1;
    }
    theSegment->SetParam(GUID_Download, 0xffffffff, 0, 0, thePerformance);
    thePerformance->PlaySegment(theSegment, DMUS_SEGF_DEFAULT, 0, nullptr);
    std::vector<int16_t> theSamples(22050 * 2);
    eeport::MixMiles(theDriver, theSamples.data(), 22050);
    thePerformance->CloseDown();
    theSegment->Release();
    thePerformance->Release();
    theLoader->Release();
    AIL_close_digital_driver(theDriver);
    AIL_shutdown();
    return 0;
}
