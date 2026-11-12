// Tests of the native build's Miles Sound System (src/Port/Linux/Miles.cpp): the preferences, files, the mix of PCM
// samples with Miles' volume and pan rules, loop counts, positions and their rounding, rates, and the DirectSound
// object. Drivers open without an audio device and mix through eeport::MixMiles, so every mixed value is exact. The
// expected values are worked out by hand in the comments.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <dsound.h>
#include <mss/mss.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <string>
#include <unistd.h>
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

static void Put16(std::vector<unsigned char>& ioImage, unsigned inValue)
{
    ioImage.push_back(inValue & 0xff);
    ioImage.push_back((inValue >> 8) & 0xff);
}

static void Put32(std::vector<unsigned char>& ioImage, unsigned long inValue)
{
    Put16(ioImage, inValue & 0xffff);
    Put16(ioImage, (inValue >> 16) & 0xffff);
}

// A RIFF WAVE image of the given PCM bytes, with a "LIST" chunk of odd size before "fmt " that a reader must skip
static std::vector<unsigned char> Wave(int inChannels, int inBits, unsigned long inRate,
                                       const std::vector<unsigned char>& inPCM, int inTag = 1)
{
    std::vector<unsigned char> theImage = {'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E',
                                           'L', 'I', 'S', 'T', 3, 0, 0, 0, 'a', 'b', 'c', 0};
    theImage.insert(theImage.end(), {'f', 'm', 't', ' ', 16, 0, 0, 0});
    Put16(theImage, inTag);
    Put16(theImage, inChannels);
    Put32(theImage, inRate);
    Put32(theImage, inRate * inChannels * inBits / 8);
    Put16(theImage, inChannels * inBits / 8);
    Put16(theImage, inBits);
    theImage.insert(theImage.end(), {'d', 'a', 't', 'a'});
    Put32(theImage, inPCM.size());
    theImage.insert(theImage.end(), inPCM.begin(), inPCM.end());
    unsigned long theSize = theImage.size() - 8;
    memcpy(&theImage[4], &theSize, 4);
    return theImage;
}

static std::vector<unsigned char> Samples16(const std::vector<int>& inValues)
{
    std::vector<unsigned char> thePCM;
    for (int theValue : inValues)
        Put16(thePCM, unsigned(theValue) & 0xffff);
    return thePCM;
}

static std::vector<short> Mix(HDIGDRIVER inDriver, int inFrames)
{
    std::vector<short> theOut(size_t(inFrames) * 2, 12345);
    eeport::MixMiles(inDriver, theOut.data(), inFrames);
    return theOut;
}

// The left and right values of each frame
static bool Frames(const std::vector<short>& inMix, const std::vector<int>& inExpected)
{
    if (inMix.size() != inExpected.size())
        return false;
    for (size_t i = 0; i < inMix.size(); ++i)
        if (inMix[i] != inExpected[i]) {
            fprintf(stderr, "  value %zu: %d, expected %d\n", i, inMix[i], inExpected[i]);
            return false;
        }
    return true;
}

// One mixed frame of a 16-bit mono sample holding inValue at the given volume, pan and master volume
static std::vector<short> One(HDIGDRIVER inDriver, HSAMPLE inSample, int inValue, S32 inVolume, S32 inPan,
                              S32 inMaster)
{
    static std::vector<unsigned char> sImage;
    sImage = Wave(1, 16, 22050, Samples16({inValue}));
    AIL_init_sample(inSample);
    AIL_set_named_sample_file(inSample, "one.wav", sImage.data(), S32(sImage.size()), 0);
    AIL_set_sample_volume(inSample, inVolume);
    AIL_set_sample_pan(inSample, inPan);
    AIL_set_digital_master_volume(inDriver, inMaster);
    AIL_start_sample(inSample);
    return Mix(inDriver, 1);
}

int main()
{
    // ---- preferences ----
    // before AIL_startup the table already holds the defaults, so a fragment size is never 0
    CHECK(AIL_get_preference(DIG_DS_FRAGMENT_SIZE) == 8);
    CHECK(AIL_get_preference(DIG_DS_FRAGMENT_CNT) == 96);
    CHECK(AIL_get_preference(DIG_DS_MIX_FRAGMENT_CNT) == 8);
    CHECK(AIL_get_preference(DIG_MIXER_CHANNELS) == 64);
    CHECK(AIL_get_preference(2) == 127);
    CHECK(AIL_get_preference(64) == 0);
    CHECK(AIL_set_preference(DIG_DS_FRAGMENT_SIZE, 0) == 8);
    CHECK(AIL_get_preference(DIG_DS_FRAGMENT_SIZE) == 0);
    CHECK((AIL_startup)() == 1);
    CHECK(AIL_get_preference(DIG_DS_FRAGMENT_SIZE) == 8);
    CHECK(AIL_set_preference(DIG_MIXER_CHANNELS, 16) == 64);
    CHECK(AIL_get_preference(DIG_MIXER_CHANNELS) == 16);
    AIL_set_preference(DIG_MIXER_CHANNELS, 64);

    // ---- the redistribution directory ends in a backslash ----
    CHECK(strcmp(AIL_set_redist_directory("C:\\Game\\mss"), "C:\\Game\\mss\\") == 0);
    CHECK(strcmp(AIL_set_redist_directory("C:\\Game\\"), "C:\\Game\\") == 0);
    CHECK(strcmp(AIL_set_redist_directory("C:"), "C:") == 0);
    CHECK(strcmp(AIL_set_redist_directory(NULL), "") == 0);

    // ---- files ----
    std::string theFolder = getenv("EE_PORT_TEST_DIR") ? getenv("EE_PORT_TEST_DIR") : ".";
    {
        std::ofstream theStream(theFolder + "/Sound.wav", std::ios::binary);
        theStream << "0123456789";
    }
    std::string theWindowsName = eeport::WindowsPath(theFolder + "/sound.WAV");
    CHECK(AIL_file_size(theWindowsName.c_str()) == 10);
    char* theRead = static_cast<char*>(AIL_file_read(theWindowsName.c_str(), NULL));
    CHECK(theRead && memcmp(theRead, "0123456789", 10) == 0);
    AIL_mem_free_lock(theRead);
    char theBuffer[12] = "xxxxxxxxxxx";
    CHECK(AIL_file_read(theWindowsName.c_str(), theBuffer) == theBuffer && memcmp(theBuffer, "0123456789x", 11) == 0);
    std::string theMissing = eeport::WindowsPath(theFolder + "/missing.wav");
    CHECK(AIL_file_size(theMissing.c_str()) == -1);
    CHECK(AIL_file_read(theMissing.c_str(), NULL) == NULL);
    CHECK(strlen(AIL_last_error()) > 0);
    unsigned char* theZeroed = static_cast<unsigned char*>(AIL_mem_alloc_lock(64));
    bool theAllZero = theZeroed != NULL;
    for (int i = 0; theZeroed && i < 64; ++i)
        theAllZero = theAllZero && theZeroed[i] == 0;
    CHECK(theAllZero);
    AIL_mem_free_lock(theZeroed);

    // ---- a driver on an SDL device: the device's callback plays 0.1 s of sound in about 0.1 s ----
    setenv("SDL_AUDIODRIVER", "dummy", 1);
    HDIGDRIVER theDevice = AIL_open_digital_driver(22050, 16, 2, 0);
    HSAMPLE theTimed = AIL_allocate_sample_handle(theDevice);
    std::vector<unsigned char> theTenth = Wave(1, 16, 22050, std::vector<unsigned char>(4410));
    CHECK(AIL_set_named_sample_file(theTimed, "tenth.wav", theTenth.data(), S32(theTenth.size()), 0) == 1);
    DWORD theStart = GetTickCount();
    AIL_start_sample(theTimed);
    while (AIL_sample_status(theTimed) == 4 && GetTickCount() - theStart < 5000)
        usleep(1000);
    DWORD theElapsed = GetTickCount() - theStart;
    CHECK(AIL_sample_status(theTimed) == 2 && AIL_sample_position(theTimed) == 4410);
    CHECK(theElapsed >= 80 && theElapsed < 1000);
    AIL_close_digital_driver(theDevice);

    // ---- the driver ----
    eeport::MilesWithoutDevice();
    AIL_set_preference(DIG_USE_WAVEOUT, 1);
    HDIGDRIVER theKept = AIL_open_digital_driver(22050, 16, 2, 1);
    CHECK(theKept != NULL && AIL_get_preference(DIG_USE_WAVEOUT) == 1);
    AIL_close_digital_driver(theKept);
    // a rate of 0 is 22050 Hz, any width but 8 is 16 bits, any count but 1 is stereo; flag 1 clear resets waveOut
    HDIGDRIVER theDriver = AIL_open_digital_driver(0, 12, 5, 0);
    CHECK(theDriver != NULL && AIL_get_preference(DIG_USE_WAVEOUT) == 0);
    CHECK(AIL_digital_CPU_percent(theDriver) == 0);
    CHECK(AIL_get_timer_highest_delay() == 0);
    AIL_serve();

    HSAMPLE theSample = AIL_allocate_sample_handle(theDriver);
    CHECK(theSample != NULL);
    // AIL_init_sample's defaults: 11025 Hz, and nothing to play
    CHECK(AIL_sample_playback_rate(theSample) == 11025);
    CHECK(AIL_sample_status(theSample) == 2);

    // ---- a 16-bit mono sample at the driver's rate mixes to the same values on both sides ----
    std::vector<unsigned char> theFour = Wave(1, 16, 22050, Samples16({1000, 2000, -3000, 4000}));
    CHECK(AIL_set_named_sample_file(theSample, "four.wav", theFour.data(), S32(theFour.size()), 0) == 1);
    CHECK(AIL_sample_granularity(theSample) == 2);
    CHECK(AIL_sample_playback_rate(theSample) == 22050);
    CHECK(AIL_sample_status(theSample) == 2);
    AIL_start_sample(theSample);
    CHECK(AIL_sample_status(theSample) == 4);
    CHECK(Frames(Mix(theDriver, 4), {1000, 1000, 2000, 2000, -3000, -3000, 4000, 4000}));
    // the pass ends: silence, the status is done and the position is the end of the data, 8 bytes
    CHECK(Frames(Mix(theDriver, 2), {0, 0, 0, 0}));
    CHECK(AIL_sample_status(theSample) == 2);
    CHECK(AIL_sample_position(theSample) == 8);

    // ---- loop counts: 2 plays two passes; 0 plays for ever ----
    AIL_set_sample_loop_count(theSample, 2);
    AIL_start_sample(theSample);
    CHECK(Frames(Mix(theDriver, 9), {1000, 1000, 2000, 2000, -3000, -3000, 4000, 4000, 1000, 1000, 2000, 2000,
                                     -3000, -3000, 4000, 4000, 0, 0}));
    CHECK(AIL_sample_status(theSample) == 2);
    AIL_set_sample_loop_count(theSample, 0);
    AIL_start_sample(theSample);
    std::vector<short> theForever = Mix(theDriver, 103);
    // frame 102 is frame 102 mod 4 = 2 of the data: -3000
    CHECK(theForever[204] == -3000 && theForever[205] == -3000);
    CHECK(AIL_sample_status(theSample) == 4);
    AIL_end_sample(theSample);
    CHECK(AIL_sample_status(theSample) == 2);
    // the end stops the sample where it was: 103 frames in, at frame 103 mod 4 = 3, which is 6 bytes
    CHECK(AIL_sample_position(theSample) == 6);
    CHECK(Frames(Mix(theDriver, 1), {0, 0}));
    AIL_set_sample_loop_count(theSample, 1);

    // ---- positions: rounded to the nearest frame of 2 bytes ----
    AIL_set_sample_position(theSample, 3);
    CHECK(AIL_sample_position(theSample) == 4);
    AIL_set_sample_position(theSample, 1);
    CHECK(AIL_sample_position(theSample) == 2);
    AIL_set_sample_position(theSample, 5);
    CHECK(AIL_sample_position(theSample) == 6);
    AIL_set_sample_position(theSample, 100);
    CHECK(AIL_sample_position(theSample) == 8);
    // a start begins at the top, whatever position was set
    AIL_set_sample_position(theSample, 4);
    AIL_start_sample(theSample);
    CHECK(Frames(Mix(theDriver, 1), {1000, 1000}));
    // a playing sample moves at once: byte 4 is frame 2
    AIL_set_sample_position(theSample, 4);
    CHECK(AIL_sample_position(theSample) == 4);
    CHECK(Frames(Mix(theDriver, 2), {-3000, -3000, 4000, 4000}));
    // moving a playing sample to the end ends the pass
    AIL_start_sample(theSample);
    AIL_set_sample_position(theSample, 8);
    CHECK(AIL_sample_status(theSample) == 2);
    CHECK(AIL_sample_position(theSample) == 8);

    // ---- rates: 11025 Hz on a 22050 Hz driver takes two output frames per frame, in a straight line between ----
    std::vector<unsigned char> theRamp = Wave(1, 16, 11025, Samples16({0, 1000}));
    AIL_init_sample(theSample);
    CHECK(AIL_set_named_sample_file(theSample, "ramp.wav", theRamp.data(), S32(theRamp.size()), 0) == 1);
    CHECK(AIL_sample_playback_rate(theSample) == 11025);
    AIL_start_sample(theSample);
    // frame 0, half way to frame 1, frame 1, and frame 1 held at the end of a last pass
    CHECK(Frames(Mix(theDriver, 5), {0, 0, 500, 500, 1000, 1000, 1000, 1000, 0, 0}));
    // twice the driver's rate skips every other frame
    AIL_init_sample(theSample);
    AIL_set_named_sample_file(theSample, "four.wav", theFour.data(), S32(theFour.size()), 0);
    AIL_set_sample_playback_rate(theSample, 44100);
    CHECK(AIL_sample_playback_rate(theSample) == 44100);
    AIL_start_sample(theSample);
    CHECK(Frames(Mix(theDriver, 3), {1000, 1000, -3000, -3000, 0, 0}));

    // ---- milliseconds: 22050 frames of 2 bytes at 22050 Hz are 1000 ms ----
    std::vector<unsigned char> theSecond = Wave(1, 16, 22050, std::vector<unsigned char>(44100));
    AIL_init_sample(theSample);
    AIL_set_named_sample_file(theSample, "second.wav", theSecond.data(), S32(theSecond.size()), 0);
    S32 theTotal = -1, theCurrent = -1;
    AIL_sample_ms_position(theSample, &theTotal, &theCurrent);
    CHECK(theTotal == 1000 && theCurrent == 0);
    AIL_start_sample(theSample);
    Mix(theDriver, 11025);
    AIL_sample_ms_position(theSample, &theTotal, &theCurrent);
    CHECK(theTotal == 1000 && theCurrent == 500);
    // at twice the rate the same bytes last half as long
    AIL_set_sample_playback_rate(theSample, 44100);
    AIL_sample_ms_position(theSample, &theTotal, &theCurrent);
    CHECK(theTotal == 500 && theCurrent == 250);
    AIL_end_sample(theSample);

    // ---- volume: the master scales the sample's in whole steps of 1/127 ----
    // volume 64, master 127: 127 * 64 / 127 = 64; 1000 * 64 / 127 = 503.9, which truncates to 503
    CHECK(Frames(One(theDriver, theSample, 1000, 64, 64, 127), {503, 503}));
    CHECK(Frames(One(theDriver, theSample, 1000, 127, 64, 64), {503, 503}));
    // volume 100, master 100: 100 * 100 / 127 = 78.7, truncated to 78; 1000 * 78 / 127 = 614.2 (not 1000 * 0.62)
    CHECK(Frames(One(theDriver, theSample, 1000, 100, 64, 100), {614, 614}));
    CHECK(Frames(One(theDriver, theSample, 1000, 0, 64, 127), {0, 0}));
    // out of range is clamped to 0..127
    CHECK(Frames(One(theDriver, theSample, 1000, 500, 64, 900), {1000, 1000}));

    // ---- pan: balance; a side keeps full level until the sound moves away from it ----
    CHECK(Frames(One(theDriver, theSample, 1000, 127, 0, 127), {1000, 0}));
    CHECK(Frames(One(theDriver, theSample, 1000, 127, 127, 127), {0, 1000}));
    CHECK(Frames(One(theDriver, theSample, 1000, 127, 63, 127), {1000, 1000}));
    // pan 96: the left scale is 2 * (127 - 96) = 62 of 128, and 1000 * 62 / 128 = 484.4
    CHECK(Frames(One(theDriver, theSample, 1000, 127, 96, 127), {484, 1000}));
    // pan 32: the right scale is 2 * 32 = 64 of 128, so 500
    CHECK(Frames(One(theDriver, theSample, 1000, 127, 32, 127), {1000, 500}));
    CHECK(Frames(One(theDriver, theSample, -1000, 127, 32, 127), {-1000, -500}));

    // ---- 8-bit unsigned samples: 128 is silence ----
    std::vector<unsigned char> theBytes = Wave(1, 8, 22050, {128, 255, 0});
    AIL_init_sample(theSample);
    CHECK(AIL_set_named_sample_file(theSample, "bytes.wav", theBytes.data(), S32(theBytes.size()), 0) == 1);
    CHECK(AIL_sample_granularity(theSample) == 1);
    AIL_start_sample(theSample);
    CHECK(Frames(Mix(theDriver, 3), {0, 0, 32512, 32512, -32768, -32768}));

    // ---- a 16-bit stereo sample keeps its sides, and positions move by 4 bytes ----
    std::vector<unsigned char> theStereo = Wave(2, 16, 22050, Samples16({100, -200, 300, -400}));
    AIL_init_sample(theSample);
    CHECK(AIL_set_named_sample_file(theSample, "stereo.wav", theStereo.data(), S32(theStereo.size()), 0) == 1);
    CHECK(AIL_sample_granularity(theSample) == 4);
    AIL_set_sample_position(theSample, 5);
    CHECK(AIL_sample_position(theSample) == 4);
    AIL_set_sample_position(theSample, 6);
    CHECK(AIL_sample_position(theSample) == 8);
    AIL_start_sample(theSample);
    CHECK(Frames(Mix(theDriver, 2), {100, -200, 300, -400}));

    // ---- two samples add, and the sum is clamped ----
    std::vector<unsigned char> theLoud = Wave(1, 16, 22050, Samples16({30000, -30000}));
    HSAMPLE theOther = AIL_allocate_sample_handle(theDriver);
    AIL_init_sample(theSample);
    AIL_set_named_sample_file(theSample, "loud.wav", theLoud.data(), S32(theLoud.size()), 0);
    AIL_set_named_sample_file(theOther, "loud.wav", theLoud.data(), S32(theLoud.size()), 0);
    AIL_start_sample(theSample);
    AIL_start_sample(theOther);
    CHECK(Frames(Mix(theDriver, 2), {32767, 32767, -32768, -32768}));

    // ---- images that do not play ----
    // format tag 2 (Microsoft ADPCM) with a width the PCM reader would take
    std::vector<unsigned char> theADPCM = Wave(1, 16, 22050, {1, 2, 3, 4}, 2);
    CHECK(AIL_set_named_sample_file(theOther, "adpcm.wav", theADPCM.data(), S32(theADPCM.size()), 0) == 0);
    CHECK(strlen(AIL_last_error()) > 0);
    unsigned char theNoise[64] = {1, 2, 3};
    CHECK(AIL_set_named_sample_file(theOther, "noise.mp3", theNoise, sizeof theNoise, 0) == 0);
    AIL_start_sample(theOther);
    CHECK(AIL_sample_status(theOther) == 2);
    AIL_release_sample_handle(theOther);
    CHECK(AIL_sample_status(theOther) == 1);

    // ---- the DirectSound object ----
    AILLPDIRECTSOUND theObject = NULL;
    AIL_get_DirectSound_info(theSample, &theObject, NULL);
    IDirectSound* theDirectSound = static_cast<IDirectSound*>(theObject);
    CHECK(theDirectSound != NULL);
    if (theDirectSound) {
        // {279AFA83-4981-11CE-A521-0020AF0BE560} as Microsoft documents IID_IDirectSound
        const GUID theID = {0x279afa83, 0x4981, 0x11ce, {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60}};
        const GUID theOtherID = {0x279afa84, 0x4981, 0x11ce, {0xa5, 0x21, 0x00, 0x20, 0xaf, 0x0b, 0xe5, 0x60}};
        void* theFound = NULL;
        CHECK(theDirectSound->QueryInterface(theID, &theFound) == S_OK && theFound == theDirectSound);
        CHECK(theDirectSound->QueryInterface(theOtherID, &theFound) == E_NOINTERFACE && theFound == NULL);
        ULONG theCount = theDirectSound->AddRef();
        CHECK(theDirectSound->Release() == theCount - 1);
        DSCAPS theCaps;
        memset(&theCaps, 0xff, sizeof theCaps);
        theCaps.dwSize = sizeof theCaps;
        CHECK(theDirectSound->GetCaps(&theCaps) == DS_OK);
        // no hardware mixing, so the mixer uses 16 channels
        CHECK(theCaps.dwFreeHwMixingAllBuffers == 0 && theCaps.dwMaxHwMixingAllBuffers == 0);
        CHECK(theCaps.dwFlags & DSCAPS_SECONDARY16BIT);
        theCaps.dwSize = 4;
        CHECK(theDirectSound->GetCaps(&theCaps) == E_INVALIDARG);
        DWORD theSpeakers = 0;
        CHECK(theDirectSound->GetSpeakerConfig(&theSpeakers) == DS_OK && theSpeakers == DSSPEAKER_STEREO);
    }

    // ---- closing the driver frees its samples ----
    AIL_close_digital_driver(theDriver);
    CHECK(AIL_sample_status(theSample) == 1);
    AIL_shutdown();

    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
