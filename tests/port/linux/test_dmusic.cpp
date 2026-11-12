// Tests of the native build's DirectMusic (src/Port/Linux/DirectMusic.cpp): the COM classes, the port and its
// properties, the loader with Empire Earth's own music (EE_MUSIC, the base game's Data/Music folder), playback through
// the Miles mixer, the volume, the reverb, stopping, transitions and the General MIDI bank (EE_GM_BANK). Drivers open
// without an audio device and mix through eeport::MixMiles. dmusic draws its variations from rand(), so a run seeded
// with srand(1) on a fresh performance is repeatable, and two runs that differ only in volume differ only in scale.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <dmusici.h>
#include <dsound.h>
#include <mss/mss.h>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

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

// The values of the DirectX 7 SDK, written out here rather than taken from a header
static const GUID kIID_IDirectMusicPerformance = {0x07d43d03, 0x6523, 0x11d2, {0x87, 0x1d, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
static const GUID kIID_IDirectMusicPort = {0x08f2d8c9, 0x37c2, 0x11d2, {0xb9, 0xf9, 0x00, 0x00, 0xf8, 0x75, 0xac, 0x12}};
static const GUID kCLSID_DirectMusicSynth = {0x58c2b4d0, 0x46e7, 0x11d1, {0x89, 0xac, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29}};
static const GUID kGUID_PerfMasterVolume = {0xd2ac28b1, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
static const GUID kSomeParameter = {0x12345678, 0x1234, 0x1234, {1, 2, 3, 4, 5, 6, 7, 8}};
static const HRESULT kNotFound = HRESULT(0x88781193u);
static const HRESULT kNotInit = HRESULT(0x88781194u);
static const HRESULT kAlreadyInit = HRESULT(0x88781199u);
static const HRESULT kUnknownProperty = HRESULT(0x88781122u);
static const HRESULT kGetUnsupported = HRESULT(0x88781124u);
static const HRESULT kBadPath = HRESULT(0x887811a2u);
static const HRESULT kFailedOpen = HRESULT(0x887811a3u);
static const HRESULT kFormatNotSupported = HRESULT(0x887811a4u);
static const DWORD kObjectFullPath = 1 << 5, kObjectMemory = 1 << 10;
static const DWORD kComposeImmediate = 0x4;
static const ULONG kBasicSupport = 0x200;

static const char* const kSongs[] = {"Balance", "Enmity", "Fallen", "Honor", "Pawns", "Rivers", "Shadow2", "Time"};
static const int kRate = 22050;

static void Wide(WCHAR* outText, const std::string& inText)
{
    size_t i = 0;
    for (; i < inText.size(); ++i)
        outText[i] = WCHAR(static_cast<unsigned char>(inText[i]));
    outText[i] = 0;
}

static DMUS_OBJECTDESC Describe(const std::string& inFile, DWORD inHow = DMUS_OBJ_FILENAME)
{
    DMUS_OBJECTDESC theDescription;
    memset(&theDescription, 0, sizeof theDescription);
    theDescription.dwSize = sizeof theDescription;
    theDescription.guidClass = CLSID_DirectMusicSegment;
    theDescription.dwValidData = DMUS_OBJ_CLASS | inHow;
    Wide(theDescription.wszFileName, inFile);
    return theDescription;
}

static IDirectMusicLoader* NewLoader(const std::string& inFolder)
{
    IDirectMusicLoader* theLoader = nullptr;
    CoCreateInstance(CLSID_DirectMusicLoader, nullptr, CLSCTX_INPROC, IID_IDirectMusicLoader, (void**)&theLoader);
    WCHAR theFolder[MAX_PATH];
    Wide(theFolder, inFolder);
    theLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, theFolder, FALSE);
    return theLoader;
}

static IDirectMusicSegment* Load(IDirectMusicLoader* inLoader, const std::string& inFile, HRESULT* outResult = nullptr)
{
    DMUS_OBJECTDESC theDescription = Describe(inFile);
    IDirectMusicSegment* theSegment = nullptr;
    HRESULT theResult = inLoader->GetObject(&theDescription, IID_IDirectMusicSegment2, (void**)&theSegment);
    if (outResult)
        *outResult = theResult;
    return theSegment;
}

// A performance set up as SEConductor sets it up: Init, the default port with eight channel groups, the channel blocks
struct Player {
    IDirectMusicPerformance* mPerformance = nullptr;
    IDirectMusic* mDirectMusic = nullptr;
    IDirectMusicPort* mPort = nullptr;
    IKsControl* mControl = nullptr;

    Player()
    {
        CoCreateInstance(CLSID_DirectMusicPerformance, nullptr, CLSCTX_INPROC, IID_IDirectMusicPerformance2,
                         (void**)&mPerformance);
        mPerformance->Init(&mDirectMusic, nullptr, nullptr);
        GUID thePortClass;
        mDirectMusic->GetDefaultPort(&thePortClass);
        DMUS_PORTPARAMS theParameters;
        memset(&theParameters, 0, sizeof theParameters);
        theParameters.dwSize = sizeof theParameters;
        theParameters.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
        theParameters.dwChannelGroups = 8;
        mDirectMusic->CreatePort(thePortClass, &theParameters, &mPort, nullptr);
        mPort->Activate(TRUE);
        mPerformance->AddPort(mPort);
        for (DWORD i = 0; i < 8; ++i)
            mPerformance->AssignPChannelBlock(i, mPort, i + 1);
        mPort->QueryInterface(IID_IKsControl, (void**)&mControl);
    }
    ~Player()
    {
        mPerformance->Stop(nullptr, nullptr, 0, 0);
        mPerformance->CloseDown();
        mControl->Release();
        mPort->Release();
        mDirectMusic->Release();
        mPerformance->Release();
    }
    HRESULT Property(const GUID& inSet, ULONG inFlags, void* ioData, ULONG inSize, ULONG* outReturned = nullptr)
    {
        KSPROPERTY theProperty;
        theProperty.Set = inSet;
        theProperty.Id = 0;
        theProperty.Flags = inFlags;
        ULONG theReturned = 0;
        HRESULT theResult = mControl->KsProperty(&theProperty, sizeof theProperty, ioData, inSize, &theReturned);
        if (outReturned)
            *outReturned = theReturned;
        return theResult;
    }
};

static std::vector<int16_t> Mix(HDIGDRIVER inDriver, int inRate, double inSeconds, int inBlock = 0)
{
    std::vector<int16_t> theSamples(size_t(inRate * inSeconds) * 2);
    const int theBlock = inBlock ? inBlock : inRate / 50;
    for (size_t theFrame = 0; theFrame < theSamples.size() / 2; theFrame += theBlock) {
        const int theFrames = int(std::min<size_t>(theBlock, theSamples.size() / 2 - theFrame));
        eeport::MixMiles(inDriver, theSamples.data() + 2 * theFrame, theFrames);
    }
    return theSamples;
}

static double Rms(const std::vector<int16_t>& inSamples, size_t inFrom = 0, size_t inTo = SIZE_MAX)
{
    inTo = std::min(inTo, inSamples.size());
    double theSum = 0;
    for (size_t i = inFrom; i < inTo; ++i)
        theSum += double(inSamples[i]) * inSamples[i];
    return inTo > inFrom ? sqrt(theSum / double(inTo - inFrom)) : 0.0;
}

static int Peak(const std::vector<int16_t>& inSamples)
{
    int thePeak = 0;
    for (int16_t theSample : inSamples)
        thePeak = std::max(thePeak, abs(int(theSample)));
    return thePeak;
}

// RMS of a minus b over RMS of b
static double Difference(const std::vector<int16_t>& inA, const std::vector<int16_t>& inB)
{
    double theDifference = 0, theReference = 0;
    for (size_t i = 0; i < inA.size() && i < inB.size(); ++i) {
        theDifference += double(inA[i] - inB[i]) * (inA[i] - inB[i]);
        theReference += double(inB[i]) * inB[i];
    }
    return theReference > 0 ? sqrt(theDifference / theReference) : 0.0;
}

// One play of a song from the start on a fresh performance, with the given setup before it plays
template <class Setup>
static std::vector<int16_t> Play(HDIGDRIVER inDriver, IDirectMusicSegment* inSegment, double inSeconds, Setup inSetup)
{
    srand(1);
    Player thePlayer;
    inSetup(thePlayer);
    inSegment->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
    thePlayer.mPerformance->PlaySegment(inSegment, DMUS_SEGF_DEFAULT, 0, nullptr);
    return Mix(inDriver, kRate, inSeconds);
}

int main()
{
    const char* theMusic = getenv("EE_MUSIC");
    const char* theBank = getenv("EE_GM_BANK");
    if (!theMusic || !*theMusic || !theBank || !*theBank) {
        printf("EE_MUSIC (the base game's Data/Music) and EE_GM_BANK (a General MIDI SoundFont) must be set\n");
        return 1;
    }
    const std::string theFolder = theMusic;
    eeport::MilesWithoutDevice();
    (AIL_startup)();
    // the music plays through the first driver opened
    HDIGDRIVER theDriver = AIL_open_digital_driver(kRate, 16, 2, 0);
    HDIGDRIVER theSecond = AIL_open_digital_driver(44100, 16, 2, 0);
    CHECK(theDriver && theSecond);
    CHECK(eeport::MusicRate() == kRate);

    // ---- the classes ----
    {
        IUnknown* theObject = reinterpret_cast<IUnknown*>(1);
        CHECK(CoCreateInstance(kCLSID_DirectMusicSynth, nullptr, CLSCTX_INPROC, IID_IUnknown, (void**)&theObject) ==
              REGDB_E_CLASSNOTREG);
        CHECK(theObject == nullptr);
        CHECK(CoCreateInstance(CLSID_DirectMusicLoader, reinterpret_cast<IUnknown*>(8), CLSCTX_INPROC, IID_IUnknown,
                               (void**)&theObject) == CLASS_E_NOAGGREGATION);
        CHECK(CoCreateInstance(CLSID_DirectMusicLoader, nullptr, CLSCTX_INPROC, IID_IDirectMusicLoader, nullptr) ==
              E_POINTER);
        // a class asked for an interface it does not have
        theObject = reinterpret_cast<IUnknown*>(1);
        CHECK(CoCreateInstance(CLSID_DirectMusicLoader, nullptr, CLSCTX_INPROC, IID_IDirectMusicComposer,
                               (void**)&theObject) == E_NOINTERFACE);
        CHECK(theObject == nullptr);

        IDirectMusicPerformance* thePerformance = nullptr;
        CHECK(CoCreateInstance(CLSID_DirectMusicPerformance, nullptr, CLSCTX_INPROC, kIID_IDirectMusicPerformance,
                               (void**)&thePerformance) == S_OK && thePerformance);
        IDirectMusicPerformance* theSecondName = nullptr;
        CHECK(thePerformance->QueryInterface(IID_IDirectMusicPerformance2, (void**)&theSecondName) == S_OK &&
              theSecondName == thePerformance);
        IUnknown* theUnknown = nullptr;
        CHECK(thePerformance->QueryInterface(IID_IUnknown, (void**)&theUnknown) == S_OK && theUnknown);
        // before Init
        CHECK(thePerformance->PlaySegment(nullptr, 0, 0, nullptr) == kNotInit);
        IDirectMusicSegmentState* theState = reinterpret_cast<IDirectMusicSegmentState*>(1);
        CHECK(thePerformance->GetSegmentState(&theState, 0) == kNotInit && theState == nullptr);
        CHECK(thePerformance->AddPort(nullptr) == kNotInit);
        // Init makes an IDirectMusic when given a NULL one, and refuses a second Init
        IDirectMusic* theDirectMusic = nullptr;
        CHECK(thePerformance->Init(&theDirectMusic, nullptr, nullptr) == S_OK && theDirectMusic);
        CHECK(thePerformance->Init(nullptr, nullptr, nullptr) == kAlreadyInit);
        // the reference counts: ours and the performance's
        CHECK(theDirectMusic->AddRef() == 3 && theDirectMusic->Release() == 2);
        CHECK(theUnknown->Release() == 2 && theSecondName->Release() == 1);
        CHECK(thePerformance->CloseDown() == S_OK);
        CHECK(theDirectMusic->Release() == 0);
        CHECK(thePerformance->Release() == 0);

        IDirectMusicComposer* theComposer = nullptr;
        CHECK(CoCreateInstance(CLSID_DirectMusicComposer, nullptr, CLSCTX_INPROC, IID_IDirectMusicComposer,
                               (void**)&theComposer) == S_OK && theComposer);
        CHECK(theComposer->AutoTransition(nullptr, nullptr, 0, 0, nullptr, nullptr, nullptr, nullptr) == E_POINTER);
        CHECK(theComposer->Release() == 0);
    }

    // ---- the port ----
    {
        Player thePlayer;
        DMUS_PORTCAPS theCaps;
        memset(&theCaps, 0xff, sizeof theCaps);
        theCaps.dwSize = sizeof theCaps;
        CHECK(thePlayer.mDirectMusic->EnumPort(0, &theCaps) == S_OK);
        CHECK(IsEqualGUID(theCaps.guidPort, kCLSID_DirectMusicSynth));
        CHECK(theCaps.dwClass == DMUS_PC_OUTPUTCLASS);
        CHECK((theCaps.dwFlags & DMUS_PC_DLS) && theCaps.dwEffectFlags == DMUS_EFFECT_REVERB);
        CHECK(theCaps.dwMaxChannelGroups == 1000 && theCaps.dwMaxAudioChannels == 2);
        std::string theName;
        for (int i = 0; theCaps.wszDescription[i]; ++i)
            theName += char(theCaps.wszDescription[i]);
        CHECK(theName == "Software Synthesizer");
        CHECK(thePlayer.mDirectMusic->EnumPort(1, &theCaps) == S_FALSE);
        theCaps.dwSize = 4;
        CHECK(thePlayer.mDirectMusic->EnumPort(0, &theCaps) == E_INVALIDARG);
        GUID theDefault;
        CHECK(thePlayer.mDirectMusic->GetDefaultPort(&theDefault) == S_OK &&
              IsEqualGUID(theDefault, kCLSID_DirectMusicSynth));

        // the port takes what it can and writes back what it took
        DMUS_PORTPARAMS theParameters;
        memset(&theParameters, 0, sizeof theParameters);
        theParameters.dwSize = sizeof theParameters;
        theParameters.dwValidParams = DMUS_PORTPARAMS_CHANNELGROUPS;
        theParameters.dwChannelGroups = 2000;
        IDirectMusicPort* thePort = nullptr;
        CHECK(thePlayer.mDirectMusic->CreatePort(theDefault, &theParameters, &thePort, nullptr) == S_FALSE && thePort);
        CHECK(theParameters.dwChannelGroups == 1000);
        DWORD theGroups = 0;
        CHECK(thePort->GetNumChannelGroups(&theGroups) == S_OK && theGroups == 1000);
        CHECK(thePort->SetNumChannelGroups(0) == E_INVALIDARG);
        IUnknown* thePortAgain = nullptr;
        CHECK(thePort->QueryInterface(kIID_IDirectMusicPort, (void**)&thePortAgain) == S_OK && thePortAgain == thePort);
        thePortAgain->Release();
        CHECK(thePort->Activate(TRUE) == S_OK && thePort->Activate(TRUE) == S_FALSE);
        CHECK(thePort->Release() == 0);
        CHECK(thePlayer.mDirectMusic->CreatePort(theDefault, &theParameters, &thePort,
                                                 reinterpret_cast<IUnknown*>(8)) == CLASS_E_NOAGGREGATION);
        // a port added twice is added once
        CHECK(thePlayer.mPerformance->AddPort(thePlayer.mPort) == S_FALSE);
        IDirectMusicPort* theChannelPort = nullptr;
        DWORD theGroup = 0, theChannel = 0;
        CHECK(thePlayer.mPerformance->PChannelInfo(37, &theChannelPort, &theGroup, &theChannel) == S_OK);
        CHECK(theChannelPort == thePlayer.mPort && theGroup == 3 && theChannel == 5);
        theChannelPort->Release();

        // the properties: volume, effects, the reverb's parameters
        LONG theVolume = 1;
        ULONG theReturned = 0;
        CHECK(thePlayer.Property(GUID_DMUS_PROP_Volume, KSPROPERTY_TYPE_GET, &theVolume, 4, &theReturned) == S_OK);
        CHECK(theVolume == 0 && theReturned == 4);
        theVolume = -600;
        CHECK(thePlayer.Property(GUID_DMUS_PROP_Volume, KSPROPERTY_TYPE_SET, &theVolume, 4) == S_OK);
        theVolume = 0;
        CHECK(thePlayer.Property(GUID_DMUS_PROP_Volume, KSPROPERTY_TYPE_GET, &theVolume, 4) == S_OK && theVolume == -600);
        DWORD theEffects = 0;
        CHECK(thePlayer.Property(GUID_DMUS_PROP_Effects, KSPROPERTY_TYPE_GET, &theEffects, 4) == S_OK);
        CHECK(theEffects == DMUS_EFFECT_REVERB);
        DMUS_WAVES_REVERB_PARAMS theReverb = {-1.5f, -4.0f, 2500.0f, 0.25f};
        CHECK(thePlayer.Property(GUID_DMUS_PROP_WavesReverb, KSPROPERTY_TYPE_SET, &theReverb, sizeof theReverb) == S_OK);
        memset(&theReverb, 0, sizeof theReverb);
        CHECK(thePlayer.Property(GUID_DMUS_PROP_WavesReverb, KSPROPERTY_TYPE_GET, &theReverb, sizeof theReverb,
                                 &theReturned) == S_OK);
        CHECK(theReverb.fInGain == -1.5f && theReverb.fReverbMix == -4.0f && theReverb.fReverbTime == 2500.0f &&
              theReverb.fHighFreqRTRatio == 0.25f && theReturned == sizeof theReverb);
        CHECK(thePlayer.Property(GUID_DMUS_PROP_WavesReverb, KSPROPERTY_TYPE_GET, &theReverb, 4) == E_INVALIDARG);
        DWORD theSupport = 0;
        CHECK(thePlayer.Property(GUID_DMUS_PROP_Volume, kBasicSupport, &theSupport, 4) == S_OK);
        CHECK(theSupport == (KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET));
        CHECK(thePlayer.Property(kSomeParameter, KSPROPERTY_TYPE_GET, &theSupport, 4) == kUnknownProperty);

        // the global parameters: the defaults, the master groove level, and any other kept as it was given
        char theGroove = 9;
        CHECK(thePlayer.mPerformance->GetGlobalParam(GUID_PerfMasterGrooveLevel, &theGroove, 1) == S_OK && theGroove == 0);
        theGroove = -7;
        CHECK(thePlayer.mPerformance->SetGlobalParam(GUID_PerfMasterGrooveLevel, &theGroove, 1) == S_OK);
        theGroove = 0;
        CHECK(thePlayer.mPerformance->GetGlobalParam(GUID_PerfMasterGrooveLevel, &theGroove, 1) == S_OK && theGroove == -7);
        DWORD theValue = 0;
        CHECK(thePlayer.mPerformance->GetGlobalParam(kSomeParameter, &theValue, 4) == E_INVALIDARG);
        theValue = 0xdeadbeef;
        CHECK(thePlayer.mPerformance->SetGlobalParam(kSomeParameter, &theValue, 4) == S_OK);
        theValue = 0;
        CHECK(thePlayer.mPerformance->GetGlobalParam(kSomeParameter, &theValue, 4) == S_OK && theValue == 0xdeadbeef);

        // notifications are never signalled; messages are allocated with their size
        DMUS_NOTIFICATION_PMSG* theNotification = reinterpret_cast<DMUS_NOTIFICATION_PMSG*>(1);
        CHECK(thePlayer.mPerformance->GetNotificationPMsg(&theNotification) == S_FALSE && theNotification == nullptr);
        DMUS_PMSG* theMessage = nullptr;
        CHECK(thePlayer.mPerformance->AllocPMsg(sizeof(DMUS_PMSG) - 1, &theMessage) == E_INVALIDARG && !theMessage);
        CHECK(thePlayer.mPerformance->AllocPMsg(sizeof(DMUS_PMSG) + 16, &theMessage) == S_OK && theMessage);
        CHECK(theMessage && theMessage->dwSize == sizeof(DMUS_PMSG) + 16);
        CHECK(thePlayer.mPerformance->FreePMsg(theMessage) == S_OK);
    }

    // ---- the loader ----
    IDirectMusicLoader* theLoader = NewLoader(theFolder);
    std::vector<IDirectMusicSegment*> theSegments;
    {
        WCHAR thePath[MAX_PATH];
        Wide(thePath, theFolder + "/no such folder");
        CHECK(theLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, thePath, FALSE) == kBadPath);
        Wide(thePath, theFolder);
        CHECK(theLoader->SetSearchDirectory(GUID_DirectMusicAllTypes, thePath, FALSE) == S_FALSE);
        HRESULT theResult = S_OK;
        CHECK(Load(theLoader, "No Such Song.sgt", &theResult) == nullptr && theResult == kFailedOpen);
        for (const char* theSong : kSongs) {
            IDirectMusicSegment* theSegment = Load(theLoader, std::string(theSong) + ".sgt", &theResult);
            CHECK(theSegment && theResult == S_OK);
            theSegments.push_back(theSegment);
            if (!theSegment) {
                fprintf(stderr, "%s.sgt: %08x\n", theSong, unsigned(theResult));
                continue;
            }
            // every song is style based
            IDirectMusicStyle* theStyle = nullptr;
            CHECK(theSegment->GetParam(GUID_IDirectMusicStyle, 0xffffffff, 0, 0, nullptr, &theStyle) == S_OK && theStyle);
            if (theStyle) {
                WCHAR theMotif[64];
                CHECK(theStyle->EnumMotif(0, theMotif) == S_FALSE);
                theStyle->Release();
            }
        }
        // the cache answers a name in any case with the same object
        IDirectMusicSegment* theAgain = Load(theLoader, "BALANCE.SGT");
        CHECK(theAgain && theAgain == theSegments[0]);
        if (theAgain)
            theAgain->Release();
        // a full path, and the same bytes from memory, each a new object
        DMUS_OBJECTDESC theDescription = Describe(theFolder + "/Balance.sgt", kObjectFullPath);
        IDirectMusicSegment* theFull = nullptr;
        CHECK(theLoader->GetObject(&theDescription, IID_IDirectMusicSegment2, (void**)&theFull) == S_OK && theFull);
        FILE* theFile = fopen((theFolder + "/Balance.sgt").c_str(), "rb");
        std::vector<BYTE> theBytes(1 << 20);
        theBytes.resize(theFile ? fread(theBytes.data(), 1, theBytes.size(), theFile) : 0);
        if (theFile)
            fclose(theFile);
        theDescription = Describe("");
        theDescription.dwValidData = DMUS_OBJ_CLASS | kObjectMemory;
        theDescription.pbMemData = theBytes.data();
        theDescription.llMemLength = LONGLONG(theBytes.size());
        IDirectMusicSegment* theMemory = nullptr;
        CHECK(theLoader->GetObject(&theDescription, IID_IDirectMusicSegment2, (void**)&theMemory) == S_OK && theMemory);
        CHECK(theMemory != theSegments[0]);
        MUSIC_TIME theLength = 0, theOther = 0, theThird = 0;
        CHECK(theSegments[0] && theSegments[0]->GetLength(&theLength) == S_OK && theLength > 0);
        CHECK(theMemory && theMemory->GetLength(&theOther) == S_OK && theOther == theLength);
        CHECK(theFull && theFull->GetLength(&theThird) == S_OK && theThird == theLength);
        if (theFull)
            theFull->Release();
        if (theMemory)
            theMemory->Release();
        // the loader makes segments only
        theDescription = Describe("Balance.sty");
        theDescription.guidClass = GUID_IDirectMusicStyle;
        IUnknown* theObject = reinterpret_cast<IUnknown*>(1);
        CHECK(theLoader->GetObject(&theDescription, IID_IUnknown, (void**)&theObject) == kFormatNotSupported);
        CHECK(theObject == nullptr);
        // a segment's parameters other than its style are not there
        DWORD theValue = 0;
        CHECK(theSegments[0] && theSegments[0]->GetParam(kSomeParameter, 0, 0, 0, nullptr, &theValue) == kGetUnsupported);
        // the repeats round trip
        DWORD theRepeats = 0;
        CHECK(theSegments[1] && theSegments[1]->SetRepeats(5) == S_OK && theSegments[1]->GetRepeats(&theRepeats) == S_OK &&
              theRepeats == 5);
        // the collections Windows supplies are the General MIDI bank: found, and not a segment
        for (const char* theCollection : {"gm.dls", "GS.DLS", "xg.dls"}) {
            CHECK(Load(theLoader, theCollection, &theResult) == nullptr && theResult == kFormatNotSupported);
            if (theResult != kFormatNotSupported)
                fprintf(stderr, "%s: %08x\n", theCollection, unsigned(theResult));
        }
        CHECK(Load(theLoader, "other.dls", &theResult) == nullptr && theResult == kFailedOpen);
    }

    // ---- playback ----
    for (size_t i = 0; i < theSegments.size(); ++i) {
        if (!theSegments[i])
            continue;
        theSegments[i]->SetRepeats(100000);
        std::vector<int16_t> theSamples = Play(theDriver, theSegments[i], 10.0, [](Player&) {});
        // audible every second, and at full scale for under 0.1% of the samples: the synthesizer's level against
        // Windows' is known only to a few dB, and Honor's loudest passages reach full scale
        int theQuietSeconds = 0;
        for (int s = 0; s < 10; ++s)
            theQuietSeconds += Rms(theSamples, size_t(s) * kRate * 2, size_t(s + 1) * kRate * 2) < 100;
        const int thePeak = Peak(theSamples);
        size_t theClipped = 0;
        for (int16_t theSample : theSamples)
            theClipped += theSample == 32767 || theSample == -32768;
        printf("%s: rms %.0f, peak %d, clipped %zu, quiet seconds %d\n", kSongs[i], Rms(theSamples), thePeak, theClipped,
               theQuietSeconds);
        CHECK(Rms(theSamples) > 1000 && theClipped * 1000 < theSamples.size() && theQuietSeconds == 0);
    }

    IDirectMusicSegment* theBalance = theSegments[0];
    IDirectMusicSegment* theEnmity = theSegments[1];
    if (theBalance && theEnmity) {
        // a run repeats itself, so the runs below differ only by what they change
        std::vector<int16_t> theFirst = Play(theDriver, theBalance, 4.0, [](Player&) {});
        std::vector<int16_t> theAgain = Play(theDriver, theBalance, 4.0, [](Player&) {});
        CHECK(theFirst == theAgain);
        CHECK(Rms(theFirst) > 1000);

        // -6 dB on the port, then on the performance: 10^(-600/2000) = 0.501 of the level
        std::vector<int16_t> thePort = Play(theDriver, theBalance, 4.0, [](Player& ioPlayer) {
            LONG theVolume = -600;
            ioPlayer.Property(GUID_DMUS_PROP_Volume, KSPROPERTY_TYPE_SET, &theVolume, 4);
        });
        std::vector<int16_t> theMaster = Play(theDriver, theBalance, 4.0, [](Player& ioPlayer) {
            LONG theVolume = -600;
            ioPlayer.mPerformance->SetGlobalParam(kGUID_PerfMasterVolume, &theVolume, 4);
        });
        const double theExpected = pow(10.0, -0.3);
        const double thePortRatio = Rms(thePort) / Rms(theFirst), theMasterRatio = Rms(theMaster) / Rms(theFirst);
        printf("volume -600: port %.4f, master %.4f of the level, against %.4f\n", thePortRatio, theMasterRatio, theExpected);
        CHECK(fabs(thePortRatio - theExpected) < 0.002 && fabs(theMasterRatio - theExpected) < 0.002);
        // scaled sample by sample, not only in power
        std::vector<int16_t> theScaled(theFirst.size());
        for (size_t i = 0; i < theFirst.size(); ++i)
            theScaled[i] = int16_t(lrint(theFirst[i] * theExpected));
        CHECK(Difference(thePort, theScaled) < 0.01);

        // the reverb is the only effect; with it off the music is the dry signal, and the default mix of -10 dB adds a
        // wet signal at about a tenth of the dry power
        std::vector<int16_t> theDry = Play(theDriver, theBalance, 4.0, [](Player& ioPlayer) {
            DWORD theEffects = 0;
            ioPlayer.Property(GUID_DMUS_PROP_Effects, KSPROPERTY_TYPE_SET, &theEffects, 4);
        });
        const double theWet = Difference(theFirst, theDry);
        printf("reverb: wet over dry %.3f in amplitude\n", theWet);
        CHECK(theWet > 0.15 && theWet < 0.6);
        std::vector<int16_t> theLouder = Play(theDriver, theBalance, 4.0, [](Player& ioPlayer) {
            DMUS_WAVES_REVERB_PARAMS theReverb = {0.0f, 0.0f, 1000.0f, 0.001f};
            ioPlayer.Property(GUID_DMUS_PROP_WavesReverb, KSPROPERTY_TYPE_SET, &theReverb, sizeof theReverb);
        });
        const double theLouderWet = Difference(theLouder, theDry);
        printf("reverb at a mix of 0 dB: wet over dry %.3f\n", theLouderWet);
        CHECK(fabs(theLouderWet / theWet - pow(10.0, 0.5)) < 0.1 * pow(10.0, 0.5));

        // stopping: the state, then silence once the notes and the reverb have died away
        {
            srand(1);
            Player thePlayer;
            theBalance->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
            IDirectMusicSegmentState* theState = nullptr;
            CHECK(thePlayer.mPerformance->PlaySegment(theBalance, DMUS_SEGF_DEFAULT, 0, &theState) == S_OK && theState);
            Mix(theDriver, kRate, 1.0);
            IDirectMusicSegmentState* theNow = nullptr;
            CHECK(thePlayer.mPerformance->GetSegmentState(&theNow, 0) == S_OK && theNow == theState);
            IDirectMusicSegment* theSegment = nullptr;
            CHECK(theNow && theNow->GetSegment(&theSegment) == S_OK && theSegment == theBalance);
            if (theSegment)
                theSegment->Release();
            if (theNow)
                theNow->Release();
            CHECK(thePlayer.mPerformance->IsPlaying(theBalance, nullptr) == S_OK);
            CHECK(thePlayer.mPerformance->IsPlaying(nullptr, theState) == S_OK);
            CHECK(thePlayer.mPerformance->IsPlaying(theEnmity, nullptr) == S_FALSE);
            // another segment's stop leaves the primary segment playing
            CHECK(thePlayer.mPerformance->Stop(theEnmity, nullptr, 0, 0) == S_OK);
            CHECK(thePlayer.mPerformance->IsPlaying(theBalance, nullptr) == S_OK);
            CHECK(thePlayer.mPerformance->Stop(nullptr, nullptr, 0, 0) == S_OK);
            CHECK(thePlayer.mPerformance->IsPlaying(theBalance, nullptr) == S_FALSE);
            theNow = reinterpret_cast<IDirectMusicSegmentState*>(1);
            CHECK(thePlayer.mPerformance->GetSegmentState(&theNow, 0) == kNotFound && theNow == nullptr);
            std::vector<int16_t> theAfter = Mix(theDriver, kRate, 6.0);
            const double theTail = Rms(theAfter, 0, kRate * 2), theEnd = Rms(theAfter, size_t(kRate) * 2 * 5);
            printf("after Stop: rms %.1f in the first second, %.1f in the sixth\n", theTail, theEnd);
            CHECK(theEnd == 0.0);
            theState->Release();
        }

        // AutoTransition: the new segment becomes the primary segment; a NULL segment stops the music
        {
            srand(1);
            Player thePlayer;
            IDirectMusicComposer* theComposer = nullptr;
            CoCreateInstance(CLSID_DirectMusicComposer, nullptr, CLSCTX_INPROC, IID_IDirectMusicComposer,
                             (void**)&theComposer);
            theBalance->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
            theEnmity->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
            thePlayer.mPerformance->PlaySegment(theBalance, DMUS_SEGF_DEFAULT, 0, nullptr);
            Mix(theDriver, kRate, 1.0);
            IDirectMusicSegmentState* theState = nullptr;
            CHECK(theComposer->AutoTransition(thePlayer.mPerformance, theEnmity, DMUS_COMMANDT_FILL, kComposeImmediate,
                                              nullptr, nullptr, &theState, nullptr) == S_OK && theState);
            IDirectMusicSegment* theSegment = nullptr;
            CHECK(theState && theState->GetSegment(&theSegment) == S_OK && theSegment == theEnmity);
            if (theSegment)
                theSegment->Release();
            CHECK(thePlayer.mPerformance->IsPlaying(theEnmity, nullptr) == S_OK);
            CHECK(thePlayer.mPerformance->IsPlaying(theBalance, nullptr) == S_FALSE);
            std::vector<int16_t> theTransition = Mix(theDriver, kRate, 3.0);
            CHECK(Rms(theTransition) > 1000);
            CHECK(theComposer->AutoTransition(thePlayer.mPerformance, nullptr, DMUS_COMMANDT_END, kComposeImmediate,
                                              nullptr, nullptr, nullptr, nullptr) == S_OK);
            CHECK(thePlayer.mPerformance->IsPlaying(theEnmity, nullptr) == S_FALSE);
            std::vector<int16_t> theAfter = Mix(theDriver, kRate, 6.0);
            CHECK(Rms(theAfter, size_t(kRate) * 2 * 5) == 0.0);
            if (theState)
                theState->Release();
            theComposer->Release();
        }

        // a reverb change while the music plays takes effect: the same run with the mix raised to 0 dB after two
        // seconds is the same for two seconds and differs after
        {
            auto theRun = [&](bool inChange) {
                srand(1);
                Player thePlayer;
                theBalance->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
                thePlayer.mPerformance->PlaySegment(theBalance, DMUS_SEGF_DEFAULT, 0, nullptr);
                std::vector<int16_t> theSamples = Mix(theDriver, kRate, 2.0);
                if (inChange) {
                    DMUS_WAVES_REVERB_PARAMS theReverb = {0.0f, 0.0f, 1000.0f, 0.001f};
                    thePlayer.Property(GUID_DMUS_PROP_WavesReverb, KSPROPERTY_TYPE_SET, &theReverb, sizeof theReverb);
                }
                std::vector<int16_t> theLater = Mix(theDriver, kRate, 2.0);
                theSamples.insert(theSamples.end(), theLater.begin(), theLater.end());
                return theSamples;
            };
            std::vector<int16_t> theSame = theRun(false), theChanged = theRun(true);
            const size_t theHalf = theSame.size() / 2;
            CHECK(std::equal(theSame.begin(), theSame.begin() + theHalf, theChanged.begin()));
            CHECK(Rms(theChanged, theHalf) > Rms(theSame, theHalf) * 1.05);
        }

        // a segment ends after its last repeat: the state goes, and silence follows
        {
            srand(1);
            Player thePlayer;
            theEnmity->SetRepeats(0);
            theEnmity->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
            thePlayer.mPerformance->PlaySegment(theEnmity, DMUS_SEGF_DEFAULT, 0, nullptr);
            int theSeconds = 0;
            while (theSeconds < 600 && thePlayer.mPerformance->IsPlaying(theEnmity, nullptr) == S_OK) {
                Mix(theDriver, kRate, 1.0);
                ++theSeconds;
            }
            IDirectMusicSegmentState* theNow = reinterpret_cast<IDirectMusicSegmentState*>(1);
            CHECK(thePlayer.mPerformance->GetSegmentState(&theNow, 0) == kNotFound && theNow == nullptr);
            std::vector<int16_t> theAfter = Mix(theDriver, kRate, 6.0);
            printf("Enmity without repeats: ended after %d s, rms %.1f in the sixth second after\n", theSeconds,
                   Rms(theAfter, size_t(kRate) * 2 * 5));
            // Enmity's tempo track holds one tempo, 120 beats a minute, so its length lasts length / 768 / 2 seconds
            MUSIC_TIME theLength = 0;
            theEnmity->GetLength(&theLength);
            CHECK(theLength > 0 && abs(theSeconds - int(lround(theLength / 768.0 / 2.0))) <= 1);
            CHECK(Rms(theAfter, size_t(kRate) * 2 * 5) == 0.0);
            theEnmity->SetRepeats(100000);
        }

        // the music goes to the first driver only; when that driver closes it goes to one still open, which runs at
        // another rate
        {
            srand(1);
            Player thePlayer;
            theBalance->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
            thePlayer.mPerformance->PlaySegment(theBalance, DMUS_SEGF_DEFAULT, 0, nullptr);
            CHECK(Rms(Mix(theSecond, 44100, 1.0)) == 0.0);
            CHECK(Rms(Mix(theDriver, kRate, 1.0)) > 1000);
            AIL_close_digital_driver(theDriver);
            theDriver = nullptr;
            CHECK(eeport::MusicRate() == 44100);
            std::vector<int16_t> theMoved = Mix(theSecond, 44100, 2.0);
            printf("after the first driver closed: rms %.0f at 44100 Hz\n", Rms(theMoved));
            CHECK(Rms(theMoved) > 1000);
        }
    }

    // ---- the General MIDI bank: Pawns takes instruments from gm.dls and gs.dls, Enmity none ----
    {
        HDIGDRIVER theMusicDriver = theDriver ? theDriver : theSecond;
        const int theMusicRate = theDriver ? kRate : 44100;
        auto theRun = [&](const char* inSong, const char* inBank) {
            setenv("EE_GM_BANK", inBank, 1);
            IDirectMusicLoader* theOwn = NewLoader(theFolder);
            IDirectMusicSegment* theSegment = Load(theOwn, inSong);
            std::vector<int16_t> theSamples;
            if (theSegment) {
                srand(1);
                Player thePlayer;
                theSegment->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
                thePlayer.mPerformance->PlaySegment(theSegment, DMUS_SEGF_DEFAULT, 0, nullptr);
                theSamples = Mix(theMusicDriver, theMusicRate, 20.0);
                theSegment->Release();
            }
            theOwn->Release();
            return theSamples;
        };
        const std::string theBankPath = theBank;
        std::vector<int16_t> thePawns = theRun("Pawns.sgt", theBankPath.c_str());
        std::vector<int16_t> thePawnsBare = theRun("Pawns.sgt", "");
        std::vector<int16_t> theEnmityRun = theRun("Enmity.sgt", theBankPath.c_str());
        std::vector<int16_t> theEnmityBare = theRun("Enmity.sgt", "");
        printf("bank: Pawns rms %.0f with it, %.0f without; Enmity %s\n", Rms(thePawns), Rms(thePawnsBare),
               theEnmityRun == theEnmityBare ? "identical" : "differs");
        CHECK(!thePawns.empty() && thePawns != thePawnsBare && Rms(thePawns) > Rms(thePawnsBare));
        CHECK(!theEnmityRun.empty() && theEnmityRun == theEnmityBare);
        // without a bank the collections are not found
        IDirectMusicLoader* theOwn = NewLoader(theFolder);
        HRESULT theResult = S_OK;
        CHECK(Load(theOwn, "gm.dls", &theResult) == nullptr && theResult == kFailedOpen);
        theOwn->Release();
        setenv("EE_GM_BANK", theBankPath.c_str(), 1);
    }

    if (theDriver)
        AIL_close_digital_driver(theDriver);
    AIL_close_digital_driver(theSecond);
    CHECK(eeport::MusicRate() == 0);

    // ---- resampling: a performance made for a 22050 Hz driver, which then plays through a 44100 Hz one, sounds the
    // same whatever the size of the blocks the mixer asks for, down to one frame at a time ----
    if (theBalance) {
        auto theRun = [&](int inBlock) {
            HDIGDRIVER theFirst = AIL_open_digital_driver(kRate, 16, 2, 0);
            HDIGDRIVER theOther = AIL_open_digital_driver(44100, 16, 2, 0);
            srand(1);
            std::vector<int16_t> theSamples;
            {
                Player thePlayer;
                theBalance->SetParam(GUID_Download, 0xffffffff, 0, 0, thePlayer.mPerformance);
                thePlayer.mPerformance->PlaySegment(theBalance, DMUS_SEGF_DEFAULT, 0, nullptr);
                Mix(theFirst, kRate, 0.5);
                AIL_close_digital_driver(theFirst);
                theSamples = Mix(theOther, 44100, 2.0, inBlock);
            }
            AIL_close_digital_driver(theOther);
            return theSamples;
        };
        std::vector<int16_t> theReference = theRun(882);
        std::vector<int16_t> theOdd = theRun(1001), theSingle = theRun(1);
        printf("resampling: rms %.0f; blocks of 1001 frames differ by %.4f, single frames by %.4f\n", Rms(theReference),
               Difference(theOdd, theReference), Difference(theSingle, theReference));
        CHECK(Rms(theReference) > 1000);
        CHECK(Difference(theOdd, theReference) < 0.05 && Difference(theSingle, theReference) < 0.05);
        CHECK(eeport::MusicRate() == 0);
    }

    for (IDirectMusicSegment* theSegment : theSegments)
        if (theSegment)
            theSegment->Release();
    CHECK(theLoader->Release() == 0);
    AIL_shutdown();

    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
