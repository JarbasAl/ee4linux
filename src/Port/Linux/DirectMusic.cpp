// DirectMusic (DirectX 7) for the game's music conductor (SEConductor.cpp), over GothicKit/dmusic
// (third_party/dmusic). The conductor creates a loader, a composer and a performance with CoCreateInstance, gets the
// software synthesizer's port from the performance's IDirectMusic, and from then on loads style-based segments,
// plays them, changes the groove level for the intensity, and composes transitions between grooves.
//
// Reading the segment, style, band and DLS files, choosing patterns by groove level, transitions and the synthesizer
// are dmusic's. This file is the COM surface over it, the file lookup through the layer's paths, and the output stage
// DirectMusic puts behind its port: the port volume and the performance's master volume, the Waves reverb, and a
// resampler for a Miles driver whose rate differs from the synthesizer's. The music plays through the first Miles
// driver opened (Miles.cpp), as DirectMusic plays through the DirectSound object the game hands it.
//
// Adapted from ee-macos kit-src/dx/dmusic.cpp and dmusic_reverb.cpp (MIT, third_party/NOTICE.md). The kit's guest
// memory and object tables become C++ objects on the project's DirectMusic interfaces (include/winsdk/dmusici.h).
//
// What differs from DirectMusic:
// - Time. GetTime's music time runs at 120 beats a minute from Init; the music's own tempo governs its notes but not
//   the time the game reads. PlaySegment and Stop act at the boundary their flags name, never at a given time.
// - Notifications. A handle and types are accepted, and nothing is ever signalled. No game code installs a
//   notification callback (SEConductor::SetNotificationCallback).
// - Secondary segments, motifs, chord maps, tracks and tools do not play; a style has no motifs to enumerate.
// - The synthesizer is dmusic's TinySoundFont, not Microsoft's DLS synthesizer. Instruments a band takes from
//   Windows' General MIDI collection (gm.dls) come from the bank GeneralMidiBank() finds.
#include "Win32Internal.h"

#include <dmusici.h>
#include <dmusic.h>

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <map>
#include <memory>
#include <set>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

// The values of Wine's dmusici.h, dmusicc.h and dmksctrl.h, which dxguid.lib holds on Windows
extern "C" {
const GUID CLSID_DirectMusicPerformance = {0xd2ac2881, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID CLSID_DirectMusicSegment = {0xd2ac2882, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID CLSID_DirectMusicComposer = {0xd2ac2890, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID CLSID_DirectMusicLoader = {0xd2ac2892, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_DirectMusicAllTypes = {0xd2ac2893, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_NOTIFICATION_SEGMENT = {0xd2ac2899, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_IDirectMusicStyle = {0xd2ac28a1, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_Download = {0xd2ac28a7, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_Unload = {0xd2ac28a8, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_PerfMasterGrooveLevel = {0xd2ac28b2, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID IID_IDirectMusicComposer = {0xd2ac28bf, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_NOTIFICATION_PERFORMANCE = {0x81f75bc5, 0x4e5d, 0x11d2, {0xbc, 0xc7, 0x00, 0xa0, 0xc9, 0x22, 0xe6, 0xeb}};
const GUID IID_IDirectMusicLoader = {0x2ffaaca2, 0x5dca, 0x11d2, {0xaf, 0xa6, 0x00, 0xaa, 0x00, 0x24, 0xd8, 0xb6}};
const GUID IID_IDirectMusicPerformance2 = {0x6fc2cae0, 0xbc78, 0x11d2, {0xaf, 0xa6, 0x00, 0xaa, 0x00, 0x24, 0xd8, 0xb6}};
const GUID IID_IDirectMusicSegment2 = {0xd38894d1, 0xc052, 0x11d2, {0x87, 0x2f, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID IID_IDirectMusicSegmentState = {0xa3afdcc7, 0xd3ee, 0x11d1, {0xbc, 0x8d, 0x00, 0xa0, 0xc9, 0x22, 0xe6, 0xeb}};
const GUID GUID_PerfAutoDownload = {0xfb09565b, 0x3631, 0x11d2, {0xbc, 0xb8, 0x00, 0xa0, 0xc9, 0x22, 0xe6, 0xeb}};
const GUID IID_IKsControl = {0x28f54685, 0x06fd, 0x11d2, {0xb2, 0x7a, 0x00, 0xa0, 0xc9, 0x22, 0x31, 0x96}};
const GUID GUID_DMUS_PROP_Volume = {0xfedfae25, 0xe46e, 0x11d1, {0xaa, 0xce, 0x00, 0x00, 0xf8, 0x75, 0xac, 0x12}};
const GUID GUID_DMUS_PROP_Effects = {0xcda8d611, 0x684a, 0x11d2, {0x87, 0x1e, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID GUID_DMUS_PROP_WavesReverb = {0x04cb5622, 0x32e5, 0x11d2, {0xaf, 0xa6, 0x00, 0xaa, 0x00, 0x24, 0xd8, 0xb6}};
}

namespace {

// The rest of the DirectX 7 SDK names this file uses, which the project's stand-in header does not declare
const GUID kIID_IDirectMusicPerformance = {0x07d43d03, 0x6523, 0x11d2, {0x87, 0x1d, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID kIID_IDirectMusicSegment = {0xf96029a2, 0x4282, 0x11d2, {0x87, 0x17, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID kIID_IDirectMusic = {0x6536115a, 0x7b2d, 0x11d2, {0xba, 0x18, 0x00, 0x00, 0xf8, 0x75, 0xac, 0x12}};
const GUID kIID_IDirectMusicPort = {0x08f2d8c9, 0x37c2, 0x11d2, {0xb9, 0xf9, 0x00, 0x00, 0xf8, 0x75, 0xac, 0x12}};
const GUID kIID_IDirectMusicStyle = {0xd2ac28bd, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};
const GUID kCLSID_DirectMusicSynth = {0x58c2b4d0, 0x46e7, 0x11d1, {0x89, 0xac, 0x00, 0xa0, 0xc9, 0x05, 0x41, 0x29}};
const GUID kGUID_PerfMasterVolume = {0xd2ac28b1, 0xb39b, 0x11d1, {0x87, 0x04, 0x00, 0x60, 0x08, 0x93, 0xb1, 0xbd}};

// MAKE_HRESULT(1, FACILITY_DIRECTMUSIC, 0x1000 + code)
const HRESULT kNotFound = HRESULT(0x88781193u);              // DMUS_E_NOT_FOUND
const HRESULT kNotInit = HRESULT(0x88781194u);               // DMUS_E_NOT_INIT
const HRESULT kAlreadyInit = HRESULT(0x88781199u);           // DMUS_E_ALREADY_INITED
const HRESULT kUnknownProperty = HRESULT(0x88781122u);       // DMUS_E_UNKNOWN_PROPERTY
const HRESULT kGetUnsupported = HRESULT(0x88781124u);        // DMUS_E_GET_UNSUPPORTED
const HRESULT kBadPath = HRESULT(0x887811a2u);               // DMUS_E_LOADER_BADPATH
const HRESULT kFailedOpen = HRESULT(0x887811a3u);            // DMUS_E_LOADER_FAILEDOPEN
const HRESULT kFormatNotSupported = HRESULT(0x887811a4u);    // DMUS_E_LOADER_FORMATNOTSUPPORTED
const HRESULT kObjectNotFound = HRESULT(0x887811a6u);        // DMUS_E_LOADER_OBJECTNOTFOUND
const HRESULT kNoFileName = HRESULT(0x887811a7u);            // DMUS_E_LOADER_NOFILENAME

const DWORD kObjectFullPath = 1 << 5, kObjectMemory = 1 << 10;    // DMUS_OBJ_FULLPATH, DMUS_OBJ_MEMORY
const DWORD kSegmentGrid = 0x800, kSegmentBeat = 0x1000, kSegmentMeasure = 0x2000;   // DMUS_SEGF_*
const DWORD kComposeImmediate = 0x4, kComposeGrid = 0x8, kComposeBeat = 0x10;          // DMUS_COMPOSEF_*
const DWORD kPortSoftwareSynth = 0x4, kPortDirectSound = 0x80, kPortShareable = 0x100;  // DMUS_PC_*
const DWORD kPortUserModeSynth = 1, kPortSystemMemory = 0x7fffffff;
const DWORD kMaxChannelGroups = 1000;
const DWORD kParamVoices = 0x1, kParamAudioChannels = 0x4, kParamSampleRate = 0x8, kParamEffects = 0x20;
const ULONG kPropertyBasicSupport = 0x200;                   // KSPROPERTY_TYPE_BASICSUPPORT

// Music time: DMUS_PPQ ticks per quarter note at 120 beats a minute
const double kTicksPerSecond = 768.0 * 2.0;
// The synthesizer against Windows': a reference recording of DirectMusic music measured dmusic 2.8 dB louder (range
// 1.4 to 4.7 dB, ee-macos), so its output is taken down by this much
const float kOutputTrimDb = -3.0f;

float GainOf(LONG inMillibels)
{
    return inMillibels <= -9600 ? 0.0f : powf(10.0f, float(inMillibels) / 2000.0f);
}

int64_t Nanoseconds()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

std::string Lower(std::string inText)
{
    for (char& c : inText)
        c = char(tolower(static_cast<unsigned char>(c)));
    return inText;
}

std::string BaseName(const std::string& inPath)
{
    size_t theSlash = inPath.find_last_of("\\/");
    return theSlash == std::string::npos ? inPath : inPath.substr(theSlash + 1);
}

// A WCHAR string as the bytes the game made it from: mbstowcs widens each byte to one character (port_msvcrt.h)
std::string Narrow(const WCHAR* inText, size_t inCapacity)
{
    std::string theText;
    for (size_t i = 0; i < inCapacity && inText[i]; ++i)
        theText += char(inText[i] < 0x100 ? inText[i] : '?');
    return theText;
}

// A whole file in memory that dmusic owns and frees with free(), or NULL
void* ReadFile(const std::string& inPath, size_t* outSize)
{
    FILE* theFile = inPath.empty() ? nullptr : fopen(inPath.c_str(), "rb");
    if (!theFile)
        return nullptr;
    void* theData = nullptr;
    struct stat theStat;
    if (fstat(fileno(theFile), &theStat) == 0 && S_ISREG(theStat.st_mode) && theStat.st_size > 0 &&
        theStat.st_size < 0x40000000) {
        theData = malloc(size_t(theStat.st_size));
        if (theData && fread(theData, 1, size_t(theStat.st_size), theFile) != size_t(theStat.st_size)) {
            free(theData);
            theData = nullptr;
        }
        *outSize = size_t(theStat.st_size);
    }
    fclose(theFile);
    return theData;
}

bool IsDirectory(const std::string& inPath)
{
    struct stat theStat;
    return stat(inPath.c_str(), &theStat) == 0 && S_ISDIR(theStat.st_mode);
}

bool IsFile(const std::string& inPath)
{
    struct stat theStat;
    return stat(inPath.c_str(), &theStat) == 0 && S_ISREG(theStat.st_mode);
}

// The bank for the instruments a band takes from Windows' General MIDI collection, which no one may redistribute:
// EE_GM_BANK when it is set (empty: none), else general-midi.sf2 beside the executable
std::string GeneralMidiBank()
{
    if (const char* theBank = getenv("EE_GM_BANK"))
        return theBank;
    char theExecutable[PATH_MAX];
    ssize_t theLength = readlink("/proc/self/exe", theExecutable, sizeof theExecutable - 1);
    if (theLength <= 0)
        return std::string();
    theExecutable[theLength] = 0;
    std::string theBank = std::string(theExecutable, strrchr(theExecutable, '/') + 1) + "general-midi.sf2";
    return IsFile(theBank) ? theBank : std::string();
}

// dmusic's log: errors by default, each text once; EE_DMUSIC_LOG=warn|info|debug lets more through
void OnDmusicLog(void*, DmLogLevel, const char* inMessage)
{
    static std::mutex sLock;
    static std::set<std::string> sSeen;
    std::lock_guard<std::mutex> theGuard(sLock);
    if (sSeen.size() < 4096 && sSeen.insert(inMessage).second)
        eeport::Log("dmusic: %s", inMessage);
}

DmLogLevel DmusicLogLevel()
{
    const char* theLevel = getenv("EE_DMUSIC_LOG");
    std::string theName = Lower(theLevel ? theLevel : "");
    if (theName == "warn")
        return DmLogLevel_WARN;
    if (theName == "info")
        return DmLogLevel_INFO;
    if (theName == "debug")
        return DmLogLevel_DEBUG;
    return DmLogLevel_ERROR;
}

// ---- the Waves reverb, approximated ----
// The synthesizer's one effect: a reverb licensed from Waves, set by GUID_DMUS_PROP_WavesReverb (input gain and mix
// in dB, decay time in ms, the high frequencies' decay as a ratio). Its algorithm is not published. This is a
// Schroeder-Moorer network in the shape of Jezar's public-domain Freeverb: eight damped feedback combs per channel
// into four Schroeder allpasses, each comb's feedback set from the decay time by Jot's rule, g = 10^(-3 d / (T fs)),
// and a one-pole lowpass in each loop that makes the decay at the Nyquist frequency T * ratio. The wet signal comes
// out at the dry signal's power with the mix at 0 dB; the dry signal passes unchanged.
struct ReverbParameters {
    float mInGainDb = 0.0f;
    float mMixDb = -10.0f;     // DirectX's defaults
    float mTimeMs = 1000.0f;
    float mHighRatio = 0.001f;
};

class Reverb {
public:
    void Configure(double inRate, const ReverbParameters& inParameters);
    // Adds the reverberation of inFrames stereo frames, deinterleaved, to the same buffers
    void Process(float* ioLeft, float* ioRight, uint32_t inFrames);

private:
    struct Comb {
        std::vector<float> mLine;
        uint32_t mPosition = 0;
        float mFeedback = 0.0f, mDamp = 0.0f, mStore = 0.0f;
    };
    struct Allpass {
        std::vector<float> mLine;
        uint32_t mPosition = 0;
    };
    static const int kCombs = 8, kAllpasses = 4;
    Comb mCombs[2][kCombs];
    Allpass mAllpasses[2][kAllpasses];
    double mRate = 0.0;
    float mInputScale = 0.0f;
    float mWet = 0.0f;
};

// Freeverb's tunings in samples at 44.1 kHz: mutually prime, and 23 samples longer on the right
const int kCombTuning[8] = {1116, 1188, 1277, 1356, 1422, 1491, 1557, 1617};
const int kAllpassTuning[4] = {556, 441, 341, 225};
const int kStereoSpread = 23;
const float kAllpassFeedback = 0.5f;

// Keeps a decaying tail out of the denormal range
inline float Flush(float inValue)
{
    return fabsf(inValue) < 1e-25f ? 0.0f : inValue;
}

void Reverb::Configure(double inRate, const ReverbParameters& inParameters)
{
    const bool theResize = inRate != mRate;
    mRate = inRate;
    const double theScale = inRate / 44100.0;
    // the ranges DirectX documents for DMUS_WAVES_REVERB_PARAMS
    const double theTime = fmin(fmax(inParameters.mTimeMs, 1.0), 3000.0) / 1000.0;
    const double theRatio = fmin(fmax(inParameters.mHighRatio, 0.001), 0.999);
    const float theInGain = fminf(fmaxf(inParameters.mInGainDb, -96.0f), 0.0f);
    const float theMix = fminf(fmaxf(inParameters.mMixDb, -96.0f), 0.0f);
    double theSquares = 0.0;
    for (int c = 0; c < 2; ++c) {
        for (int i = 0; i < kCombs; ++i) {
            Comb& theComb = mCombs[c][i];
            const uint32_t theDelay = std::max<uint32_t>(1, uint32_t(lround((kCombTuning[i] + c * kStereoSpread) * theScale)));
            if (theResize || theComb.mLine.size() != theDelay) {
                theComb.mLine.assign(theDelay, 0.0f);
                theComb.mPosition = 0;
                theComb.mStore = 0.0f;
            }
            // Jot's rule: after T seconds the loop has run T fs / d times and lost 60 dB, at DC with g and at the
            // Nyquist frequency with g_hf; the lowpass (1-a)/(1-a z^-1) passes the Nyquist frequency at (1-a)/(1+a)
            const double theLoops = theTime * inRate / theDelay;
            const double theGain = pow(10.0, -3.0 / theLoops);
            const double theHighGain = pow(10.0, -3.0 / (theLoops * theRatio));
            const double theRatioHigh = fmax(theHighGain / theGain, 0.01);
            theComb.mFeedback = float(theGain);
            theComb.mDamp = float((1.0 - theRatioHigh) / (1.0 + theRatioHigh));
            if (c == 0)
                theSquares += theGain * theGain;
        }
        for (int i = 0; i < kAllpasses; ++i) {
            Allpass& theAllpass = mAllpasses[c][i];
            const uint32_t theDelay = std::max<uint32_t>(1, uint32_t(lround((kAllpassTuning[i] + c * kStereoSpread) * theScale)));
            if (theResize || theAllpass.mLine.size() != theDelay) {
                theAllpass.mLine.assign(theDelay, 0.0f);
                theAllpass.mPosition = 0;
            }
        }
    }
    // White noise of power P through a comb with feedback g comes out at P / (1 - g^2), eight combs summed
    // incoherently at 8 P / (1 - g^2), and the allpasses keep the power
    mInputScale = float(sqrt((1.0 - theSquares / kCombs) / kCombs)) * powf(10.0f, theInGain / 20.0f);
    mWet = theMix <= -96.0f ? 0.0f : powf(10.0f, theMix / 20.0f);
}

void Reverb::Process(float* ioLeft, float* ioRight, uint32_t inFrames)
{
    if (mRate <= 0.0 || mWet == 0.0f)
        return;
    for (uint32_t n = 0; n < inFrames; ++n) {
        const float theIn = (ioLeft[n] + ioRight[n]) * 0.5f * mInputScale;
        float theOut[2] = {0.0f, 0.0f};
        for (int c = 0; c < 2; ++c) {
            for (Comb& theComb : mCombs[c]) {
                const float theDelayed = theComb.mLine[theComb.mPosition];
                theComb.mStore = Flush(theDelayed * (1.0f - theComb.mDamp) + theComb.mStore * theComb.mDamp);
                theComb.mLine[theComb.mPosition] = theIn + theComb.mStore * theComb.mFeedback;
                if (++theComb.mPosition == theComb.mLine.size())
                    theComb.mPosition = 0;
                theOut[c] += theDelayed;
            }
            for (Allpass& theAllpass : mAllpasses[c]) {
                const float theDelayed = theAllpass.mLine[theAllpass.mPosition];
                const float theValue = Flush(theOut[c] + kAllpassFeedback * theDelayed);
                theAllpass.mLine[theAllpass.mPosition] = theValue;
                theOut[c] = theDelayed - kAllpassFeedback * theValue;
                if (++theAllpass.mPosition == theAllpass.mLine.size())
                    theAllpass.mPosition = 0;
            }
        }
        ioLeft[n] += theOut[0] * mWet;
        ioRight[n] += theOut[1] * mWet;
    }
}

// ---- the output stage ----
// What the port's KS properties set
struct PortOutput {
    LONG mVolume = 0;                   // hundredths of a dB
    DWORD mEffects = DMUS_EFFECT_REVERB;
    ReverbParameters mReverb;
    uint64_t mVersion = 1;              // counts reverb changes
};

// One performance's synthesizer and the output stage behind it. gOutputLock guards mOutput and mMasterVolume; the
// render fields belong to the audio thread.
struct Engine {
    DmPerformance* mPerformance = nullptr;
    double mRate = 44100.0;
    std::shared_ptr<PortOutput> mOutput = std::make_shared<PortOutput>();
    LONG mMasterVolume = 0;
    // whether the segment last played still plays: mPlays counts plays and stops, mSeen is the last count during
    // which dmusic played, mEnded the last one in which it then stopped
    std::atomic<uint64_t> mPlays{0}, mSeen{0}, mEnded{0};
    // render
    float mGain = 0.0f;
    bool mGainSet = false;
    Reverb mReverb;
    uint64_t mReverbVersion = 0;
    double mReverbRate = 0.0;
    double mPhase = 0.0;                        // the position of the next device frame in mHeld
    std::vector<float> mHeld = {0.0f, 0.0f};    // the source frames from mPhase's frame on, stereo
    std::vector<float> mScratch, mLeft, mRight;

    ~Engine() { DmPerformance_release(mPerformance); }
};

// Lock order: gApiLock, then the mixer's lock (Miles.cpp), then gOutputLock, then dmusic's own locks. The audio
// thread holds the mixer's lock while it renders.
std::recursive_mutex gApiLock;
std::mutex gOutputLock;
std::vector<std::shared_ptr<Engine>> gEngines;

void RenderDmusic(Engine& ioEngine, uint32_t inFrames)
{
    ioEngine.mScratch.assign(size_t(inFrames) * 2, 0.0f);
    if (inFrames)
        DmPerformance_renderPcm(ioEngine.mPerformance, ioEngine.mScratch.data(), size_t(inFrames) * 2,
                                DmRenderOptions(DmRender_FLOAT | DmRender_STEREO));
}

// One engine's block, added into ioStereo: dmusic at its own rate, resampled to the device's when they differ, then
// the gain, then the reverb. gOutputLock held.
void RenderEngine(Engine& ioEngine, float* ioStereo, uint32_t inFrames, double inRate)
{
    ioEngine.mLeft.assign(inFrames, 0.0f);
    ioEngine.mRight.assign(inFrames, 0.0f);
    if (fabs(ioEngine.mRate - inRate) < 0.5) {
        RenderDmusic(ioEngine, inFrames);
        for (uint32_t i = 0; i < inFrames; ++i) {
            ioEngine.mLeft[i] = ioEngine.mScratch[2 * i];
            ioEngine.mRight[i] = ioEngine.mScratch[2 * i + 1];
        }
    } else {
        // linear interpolation between the two source frames around each device frame; the frames a later block
        // still needs stay in mHeld
        std::vector<float>& theHeld = ioEngine.mHeld;
        const double theStep = ioEngine.mRate / inRate;
        const double theEnd = ioEngine.mPhase + inFrames * theStep;
        const size_t theDone = size_t(theEnd);
        const size_t theNeeded = std::max(size_t(ioEngine.mPhase + (inFrames - 1) * theStep) + 2, theDone);
        if (theHeld.size() / 2 < theNeeded) {
            RenderDmusic(ioEngine, uint32_t(theNeeded - theHeld.size() / 2));
            theHeld.insert(theHeld.end(), ioEngine.mScratch.begin(), ioEngine.mScratch.end());
        }
        for (uint32_t i = 0; i < inFrames; ++i) {
            const double thePosition = ioEngine.mPhase + i * theStep;
            const size_t theIndex = size_t(thePosition);
            const float theFraction = float(thePosition - double(theIndex));
            ioEngine.mLeft[i] = theHeld[2 * theIndex] * (1.0f - theFraction) + theHeld[2 * theIndex + 2] * theFraction;
            ioEngine.mRight[i] = theHeld[2 * theIndex + 1] * (1.0f - theFraction) + theHeld[2 * theIndex + 3] * theFraction;
        }
        theHeld.erase(theHeld.begin(), theHeld.begin() + 2 * theDone);
        ioEngine.mPhase = theEnd - double(theDone);
    }

    const uint64_t thePlays = ioEngine.mPlays.load();
    if (DmPerformance_isPlaying(ioEngine.mPerformance))
        ioEngine.mSeen.store(thePlays);
    else if (thePlays && ioEngine.mSeen.load() == thePlays)
        ioEngine.mEnded.store(thePlays);

    // the port volume and the master volume, ramped over about 5 ms so that a fade in steps is not a staircase
    const PortOutput& theOutput = *ioEngine.mOutput;
    const float theTarget = GainOf(theOutput.mVolume) * GainOf(ioEngine.mMasterVolume) * powf(10.0f, kOutputTrimDb / 20.0f);
    if (!ioEngine.mGainSet) {
        ioEngine.mGain = theTarget;
        ioEngine.mGainSet = true;
    }
    const float theRamp = 1.0f - expf(-1.0f / (0.005f * float(inRate)));
    for (uint32_t i = 0; i < inFrames; ++i) {
        ioEngine.mGain += (theTarget - ioEngine.mGain) * theRamp;
        ioEngine.mLeft[i] *= ioEngine.mGain;
        ioEngine.mRight[i] *= ioEngine.mGain;
    }
    if (theOutput.mEffects & DMUS_EFFECT_REVERB) {
        if (ioEngine.mReverbVersion != theOutput.mVersion || ioEngine.mReverbRate != inRate) {
            ioEngine.mReverb.Configure(inRate, theOutput.mReverb);
            ioEngine.mReverbVersion = theOutput.mVersion;
            ioEngine.mReverbRate = inRate;
        }
        ioEngine.mReverb.Process(ioEngine.mLeft.data(), ioEngine.mRight.data(), inFrames);
    }
    for (uint32_t i = 0; i < inFrames; ++i) {
        ioStereo[2 * i] += ioEngine.mLeft[i];
        ioStereo[2 * i + 1] += ioEngine.mRight[i];
    }
}

void Render(float* ioStereo, int inFrames, int inRate)
{
    if (inFrames <= 0 || inRate <= 0)
        return;
    std::lock_guard<std::mutex> theGuard(gOutputLock);
    for (const std::shared_ptr<Engine>& theEngine : gEngines)
        RenderEngine(*theEngine, ioStereo, uint32_t(inFrames), inRate);
}

// ---- COM objects ----
template <class Interface>
class Object : public Interface {
public:
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override
    {
        long theCount = --mReferences;
        if (theCount == 0)
            delete this;
        return ULONG(theCount);
    }

protected:
    virtual ~Object() {}
    HRESULT Answer(REFIID inID, std::initializer_list<const GUID*> inIDs, void** outObject)
    {
        if (!outObject)
            return E_POINTER;
        for (const GUID* theID : inIDs)
            if (IsEqualGUID(inID, *theID) || IsEqualGUID(inID, IID_IUnknown)) {
                this->AddRef();
                *outObject = static_cast<Interface*>(this);
                return S_OK;
            }
        *outObject = nullptr;
        return E_NOINTERFACE;
    }
    std::atomic<long> mReferences{1};
};

// What a loader resolves names with, shared with the segments it loads, which download their bands and styles
// through it later. gApiLock guards it; dmusic calls Resolve only inside calls this file makes under that lock.
struct LoaderData {
    DmLoader* mLoader = nullptr;
    std::string mSearchAll;                        // GUID_DirectMusicAllTypes's directory, a Windows path
    std::map<std::string, std::string> mSearchBy;  // the other classes'
    // a GetObject from memory: the name dmusic is asked for, and the bytes
    std::string mMemoryName;
    std::vector<uint8_t> mMemoryBytes;
    std::string mBank;
    bool mBankLooked = false;
    ~LoaderData() { DmLoader_release(mLoader); }
};

std::string GuidText(REFGUID inID)
{
    char theText[40];
    snprintf(theText, sizeof theText, "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x", unsigned(inID.Data1),
             inID.Data2, inID.Data3, inID.Data4[0], inID.Data4[1], inID.Data4[2], inID.Data4[3], inID.Data4[4],
             inID.Data4[5], inID.Data4[6], inID.Data4[7]);
    return theText;
}

std::string Join(const std::string& inDirectory, const std::string& inName)
{
    if (inDirectory.empty())
        return inName;
    char theLast = inDirectory.back();
    return theLast == '\\' || theLast == '/' ? inDirectory + inName : inDirectory + "\\" + inName;
}

bool IsAbsolute(const std::string& inPath)
{
    return (inPath.size() > 1 && inPath[1] == ':') || (!inPath.empty() && (inPath[0] == '\\' || inPath[0] == '/'));
}

// The loader's resolver: a segment, a style, a band's DLS collection, by name in the search directories
void* Resolve(void* inData, const char* inName, size_t* outSize)
{
    LoaderData& theData = *static_cast<LoaderData*>(inData);
    const std::string theName = inName ? inName : "";
    if (theName.empty())
        return nullptr;
    if (!theData.mMemoryName.empty() && theName == theData.mMemoryName) {
        void* theCopy = malloc(theData.mMemoryBytes.size());
        if (!theCopy)
            return nullptr;
        memcpy(theCopy, theData.mMemoryBytes.data(), theData.mMemoryBytes.size());
        *outSize = theData.mMemoryBytes.size();
        return theCopy;
    }
    std::vector<std::string> theCandidates;
    if (IsAbsolute(theName))
        theCandidates.push_back(theName);
    else {
        if (!theData.mSearchAll.empty())
            theCandidates.push_back(Join(theData.mSearchAll, theName));
        for (const auto& theDirectory : theData.mSearchBy)
            theCandidates.push_back(Join(theDirectory.second, theName));
        theCandidates.push_back(theName);
    }
    for (const std::string& theCandidate : theCandidates)
        if (void* theFile = ReadFile(eeport::LinuxPath(theCandidate.c_str()), outSize))
            return theFile;
    // a collection Windows supplied: dmusic asks for gm.dls, gs.dls or xg.dls by the instrument's flags, and the
    // General MIDI bank answers all three
    const std::string theFile = Lower(BaseName(theName));
    if (theFile == "gm.dls" || theFile == "gs.dls" || theFile == "xg.dls") {
        if (!theData.mBankLooked) {
            theData.mBankLooked = true;
            theData.mBank = GeneralMidiBank();
            if (theData.mBank.empty())
                eeport::Log("DirectMusic: no General MIDI bank (EE_GM_BANK, or general-midi.sf2 beside the "
                            "executable): instruments that bands take from %s are silent", theFile.c_str());
        }
        return ReadFile(theData.mBank, outSize);
    }
    return nullptr;
}

// Whether the bytes are a segment (a RIFF 'DMSG' form, which dmusic does not check), the segment header ('segh')
// and whether a style track ('sttr') exists: what dmusic does not expose
struct SegmentHeader {
    DWORD mRepeats = 0, mLength = 0, mPlayStart = 0, mLoopStart = 0, mLoopEnd = 0, mResolution = 0;
    bool mIsSegment = false, mHasStyle = false;
};

SegmentHeader ReadSegmentHeader(const uint8_t* inData, size_t inSize)
{
    SegmentHeader theHeader;
    auto theWord = [&](size_t inAt) {
        uint32_t theValue = 0;
        if (inAt + 4 <= inSize)
            memcpy(&theValue, inData + inAt, 4);
        return theValue;
    };
    if (inSize < 12 || memcmp(inData, "RIFF", 4) != 0 || memcmp(inData + 8, "DMSG", 4) != 0)
        return theHeader;
    theHeader.mIsSegment = true;
    for (size_t theAt = 12; theAt + 8 <= inSize;) {
        const uint32_t theSize = theWord(theAt + 4);
        if (memcmp(inData + theAt, "segh", 4) == 0 && theSize >= 24) {
            theHeader.mRepeats = theWord(theAt + 8);
            theHeader.mLength = theWord(theAt + 12);
            theHeader.mPlayStart = theWord(theAt + 16);
            theHeader.mLoopStart = theWord(theAt + 20);
            theHeader.mLoopEnd = theWord(theAt + 24);
            theHeader.mResolution = theWord(theAt + 28);
        }
        if (theSize > inSize)
            break;
        theAt += 8 + theSize + (theSize & 1);
    }
    for (size_t i = 0; i + 12 <= inSize; ++i)
        if (memcmp(inData + i, "LIST", 4) == 0 && memcmp(inData + i + 8, "sttr", 4) == 0) {
            theHeader.mHasStyle = true;
            break;
        }
    return theHeader;
}

// A style-based segment's style. dmusic plays neither motifs nor a style on its own, so it has none to give.
class Style final : public Object<IDirectMusicStyle> {
public:
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        return Answer(inID, {&kIID_IDirectMusicStyle}, outObject);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetBand(WCHAR*, IDirectMusicBand** outBand) override
    {
        return NoObject(outBand);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumBand(DWORD, WCHAR*) override { return S_FALSE; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDefaultBand(IDirectMusicBand** outBand) override
    {
        return NoObject(outBand);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumMotif(DWORD, WCHAR*) override { return S_FALSE; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetMotif(WCHAR*, IDirectMusicSegment** outMotif) override
    {
        return NoObject(outMotif);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDefaultChordMap(IDirectMusicChordMap** outMap) override
    {
        return NoObject(outMap);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumChordMap(DWORD, WCHAR*) override { return S_FALSE; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetChordMap(WCHAR*, IDirectMusicChordMap** outMap) override
    {
        return NoObject(outMap);
    }

private:
    template <class T>
    static HRESULT NoObject(T** outObject)
    {
        if (outObject)
            *outObject = nullptr;
        return kNotFound;
    }
};

class Segment final : public Object<IDirectMusicSegment> {
public:
    Segment(DmSegment* inSegment, std::shared_ptr<LoaderData> inLoader, const SegmentHeader& inHeader,
            std::string inName)
        : mSegment(inSegment), mLoader(std::move(inLoader)), mHeader(inHeader), mName(std::move(inName)) {}

    DmSegment* mSegment;
    std::shared_ptr<LoaderData> mLoader;
    SegmentHeader mHeader;
    std::string mName;
    Style* mStyle = nullptr;            // made by GetParam, one reference held
    bool mDownloaded = false;

    // Downloads the bands and the style through the loader that made the segment. A collection that cannot be
    // found leaves its instruments silent, as DirectMusic plays what it has.
    void Download()
    {
        if (mDownloaded)
            return;
        DmResult theResult = DmSegment_download(mSegment, mLoader->mLoader);
        if (theResult != DmResult_SUCCESS)
            eeport::Log("DirectMusic: %s downloaded only in part (dmusic result %d)", mName.c_str(), int(theResult));
        mDownloaded = true;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        return Answer(inID, {&kIID_IDirectMusicSegment, &IID_IDirectMusicSegment2}, outObject);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLength(MUSIC_TIME* outLength) override
    {
        return Put(outLength, MUSIC_TIME(mHeader.mLength));
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetLength(MUSIC_TIME inLength) override
    {
        mHeader.mLength = DWORD(inLength);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRepeats(DWORD* outRepeats) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        return Put(outRepeats, DWORD(DmSegment_getRepeats(mSegment)));
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetRepeats(DWORD inRepeats) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        DmSegment_setRepeats(mSegment, inRepeats);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDefaultResolution(DWORD* outResolution) override
    {
        return Put(outResolution, mHeader.mResolution);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetDefaultResolution(DWORD inResolution) override
    {
        mHeader.mResolution = inResolution;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTrack(REFGUID, DWORD, DWORD, IDirectMusicTrack** outTrack) override
    {
        if (outTrack)
            *outTrack = nullptr;
        return kNotFound;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTrackGroup(IDirectMusicTrack*, DWORD*) override { return kNotFound; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE InsertTrack(IDirectMusicTrack*, DWORD) override { return E_NOTIMPL; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RemoveTrack(IDirectMusicTrack*) override { return E_NOTIMPL; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE InitPlay(IDirectMusicSegmentState** outState, IDirectMusicPerformance*,
                                                            DWORD) override
    {
        if (outState)
            *outState = nullptr;
        return E_NOTIMPL;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetGraph(IDirectMusicGraph** outGraph) override
    {
        if (outGraph)
            *outGraph = nullptr;
        return kNotFound;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetGraph(IDirectMusicGraph*) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AddNotificationType(REFGUID) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RemoveNotificationType(REFGUID) override { return S_OK; }
    // The style of a style-based segment, which is all a game asks a segment for
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetParam(REFGUID inType, DWORD, DWORD, MUSIC_TIME, MUSIC_TIME* outNext,
                                                            void* outParameter) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (outNext)
            *outNext = 0;
        if (!IsEqualGUID(inType, GUID_IDirectMusicStyle))
            return kGetUnsupported;
        if (!outParameter)
            return E_POINTER;
        IDirectMusicStyle*& theStyle = *static_cast<IDirectMusicStyle**>(outParameter);
        theStyle = nullptr;
        if (!mHeader.mHasStyle)
            return kNotFound;
        if (!mStyle)
            mStyle = new Style;
        mStyle->AddRef();
        theStyle = mStyle;
        return S_OK;
    }
    // The downloads. Unloading leaves dmusic's instruments where they are: they belong to the synthesizer, keyed by
    // collection, and cost nothing until they play.
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetParam(REFGUID inType, DWORD, DWORD, MUSIC_TIME, void*) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (IsEqualGUID(inType, GUID_Download))
            Download();
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Clone(MUSIC_TIME, MUSIC_TIME, IDirectMusicSegment** outSegment) override
    {
        if (outSegment)
            *outSegment = nullptr;
        return E_NOTIMPL;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetStartPoint(MUSIC_TIME inStart) override
    {
        mHeader.mPlayStart = DWORD(inStart);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetStartPoint(MUSIC_TIME* outStart) override
    {
        return Put(outStart, MUSIC_TIME(mHeader.mPlayStart));
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetLoopPoints(MUSIC_TIME inStart, MUSIC_TIME inEnd) override
    {
        mHeader.mLoopStart = DWORD(inStart);
        mHeader.mLoopEnd = DWORD(inEnd);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLoopPoints(MUSIC_TIME* outStart, MUSIC_TIME* outEnd) override
    {
        if (!outStart || !outEnd)
            return E_POINTER;
        *outStart = MUSIC_TIME(mHeader.mLoopStart);
        *outEnd = MUSIC_TIME(mHeader.mLoopEnd);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPChannelsUsed(DWORD, DWORD*) override { return S_OK; }

private:
    ~Segment() override
    {
        if (mStyle)
            mStyle->Release();
        DmSegment_release(mSegment);
    }
    template <class T>
    static HRESULT Put(T* outValue, T inValue)
    {
        if (!outValue)
            return E_POINTER;
        *outValue = inValue;
        return S_OK;
    }
};

// One play of a segment
class SegmentState final : public Object<IDirectMusicSegmentState> {
public:
    SegmentState(Segment* inSegment, MUSIC_TIME inStart, uint64_t inPlay)
        : mSegment(inSegment), mRepeats(DmSegment_getRepeats(inSegment->mSegment)), mStart(inStart), mPlay(inPlay)
    {
        mSegment->AddRef();
    }

    Segment* mSegment;
    DWORD mRepeats;
    MUSIC_TIME mStart;
    uint64_t mPlay;     // the engine's play count it belongs to; 0 for a secondary segment, which does not play

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        return Answer(inID, {&IID_IDirectMusicSegmentState}, outObject);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRepeats(DWORD* outRepeats) override
    {
        if (!outRepeats)
            return E_POINTER;
        *outRepeats = mRepeats;
        return S_OK;
    }
    // The interface the loader handed out, with a reference
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetSegment(IDirectMusicSegment** outSegment) override
    {
        if (!outSegment)
            return E_POINTER;
        mSegment->AddRef();
        *outSegment = mSegment;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetStartTime(MUSIC_TIME* outStart) override
    {
        if (!outStart)
            return E_POINTER;
        *outStart = mStart;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetSeek(MUSIC_TIME* outSeek) override
    {
        if (!outSeek)
            return E_POINTER;
        *outSeek = 0;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetStartPoint(MUSIC_TIME* outStart) override
    {
        if (!outStart)
            return E_POINTER;
        *outStart = 0;
        return S_OK;
    }

private:
    ~SegmentState() override { mSegment->Release(); }
};

class Loader final : public Object<IDirectMusicLoader> {
public:
    Loader() : mData(std::make_shared<LoaderData>()) {}
    bool Create()
    {
        if (DmLoader_create(&mData->mLoader, DmLoader_DEFAULT) != DmResult_SUCCESS)
            return false;
        DmLoader_addResolver(mData->mLoader, Resolve, mData.get());
        return true;
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        return Answer(inID, {&IID_IDirectMusicLoader}, outObject);
    }
    // A segment, by file name in the search directory, by full path, or from memory
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetObject(DMUS_OBJECTDESC* inDescription, REFIID inID,
                                                             LPVOID* outObject) override
    {
        if (!outObject)
            return E_POINTER;
        *outObject = nullptr;
        if (!inDescription || inDescription->dwSize < offsetof(DMUS_OBJECTDESC, pStream))
            return E_INVALIDARG;
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        const DWORD theValid = inDescription->dwValidData;
        const bool theSegment = (theValid & DMUS_OBJ_CLASS) ? IsEqualGUID(inDescription->guidClass, CLSID_DirectMusicSegment)
                                                            : IsSegmentID(inID);
        if (!theSegment) {
            eeport::Log("DirectMusic: the loader makes segments only; class %s is not loaded",
                        GuidText(inDescription->guidClass).c_str());
            return kFormatNotSupported;
        }
        if (!IsSegmentID(inID) && !IsEqualGUID(inID, IID_IUnknown))
            return E_NOINTERFACE;

        std::string theName;
        std::vector<uint8_t> theBytes;
        const bool theMemory = (theValid & kObjectMemory) != 0;
        if (theMemory) {
            if (!inDescription->pbMemData || inDescription->llMemLength <= 0 || inDescription->llMemLength > INT_MAX)
                return E_INVALIDARG;
            theBytes.assign(inDescription->pbMemData, inDescription->pbMemData + inDescription->llMemLength);
            char theTag[32];
            snprintf(theTag, sizeof theTag, "<memory %p>", static_cast<void*>(inDescription->pbMemData));
            theName = theTag;
        } else if (theValid & (DMUS_OBJ_FILENAME | kObjectFullPath))
            theName = Narrow(inDescription->wszFileName, DMUS_MAX_FILENAME);
        else if (theValid & (DMUS_OBJ_NAME | DMUS_OBJ_OBJECT)) {
            EE_PORT_UNSUPPORTED("IDirectMusicLoader::GetObject by name or GUID alone");
            return kObjectNotFound;
        }
        if (theName.empty())
            return kNoFileName;

        const std::string theKey = Lower(theName);
        if (mCache && !theMemory) {
            auto theCached = mCached.find(theKey);
            if (theCached != mCached.end()) {
                theCached->second->AddRef();
                *outObject = static_cast<IDirectMusicSegment*>(theCached->second);
                return S_OK;
            }
        }
        if (!theMemory) {
            size_t theSize = 0;
            void* theFile = Resolve(mData.get(), theName.c_str(), &theSize);
            if (!theFile)
                return kFailedOpen;
            theBytes.assign(static_cast<uint8_t*>(theFile), static_cast<uint8_t*>(theFile) + theSize);
            free(theFile);
        }
        // dmusic reads the segment through the resolver, which answers this name with these bytes
        const SegmentHeader theHeader = ReadSegmentHeader(theBytes.data(), theBytes.size());
        if (!theHeader.mIsSegment) {
            eeport::Log("DirectMusic: %s is not a segment", theName.c_str());
            return kFormatNotSupported;
        }
        mData->mMemoryName = theName;
        mData->mMemoryBytes.swap(theBytes);
        DmSegment* theLoaded = nullptr;
        DmResult theResult = DmLoader_getSegment(mData->mLoader, theName.c_str(), &theLoaded);
        mData->mMemoryName.clear();
        mData->mMemoryBytes.clear();
        if (theResult != DmResult_SUCCESS || !theLoaded) {
            eeport::Log("DirectMusic: %s is not a segment dmusic reads (%d)", theName.c_str(), int(theResult));
            return theResult == DmResult_MEMORY_EXHAUSTED ? E_OUTOFMEMORY : kFormatNotSupported;
        }
        Segment* theNew = new Segment(theLoaded, mData, theHeader, BaseName(theName));
        if (mCache && !theMemory) {
            mCached[theKey] = theNew;
            theNew->AddRef();
        }
        *outObject = static_cast<IDirectMusicSegment*>(theNew);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetObject(DMUS_OBJECTDESC*) override
    {
        EE_PORT_UNSUPPORTED("IDirectMusicLoader::SetObject");
        return S_OK;
    }
    // DirectMusic refuses a directory that does not exist
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetSearchDirectory(REFGUID inClass, WCHAR* inPath, BOOL inClear) override
    {
        if (!inPath)
            return E_POINTER;
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        const std::string theDirectory = Narrow(inPath, MAX_PATH);
        if (!IsDirectory(eeport::LinuxPath(theDirectory.c_str())))
            return kBadPath;
        std::string& theSlot = IsEqualGUID(inClass, GUID_DirectMusicAllTypes) ? mData->mSearchAll
                                                                              : mData->mSearchBy[GuidText(inClass)];
        const bool theSame = theSlot == theDirectory;
        theSlot = theDirectory;
        if (inClear)
            ClearAll();
        return theSame ? S_FALSE : S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ScanDirectory(REFGUID, WCHAR*, WCHAR*) override { return S_FALSE; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CacheObject(IDirectMusicObject*) override
    {
        EE_PORT_UNSUPPORTED("IDirectMusicLoader::CacheObject");
        return S_FALSE;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ReleaseObject(IDirectMusicObject* inObject) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        for (auto theCached = mCached.begin(); theCached != mCached.end(); ++theCached)
            if (static_cast<void*>(static_cast<IDirectMusicSegment*>(theCached->second)) == static_cast<void*>(inObject)) {
                theCached->second->Release();
                mCached.erase(theCached);
                return S_OK;
            }
        return S_FALSE;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ClearCache(REFGUID) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        ClearAll();
        return S_OK;
    }
    // Only segments are cached, whatever the class
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnableCache(REFGUID, BOOL inEnable) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        const bool theSame = mCache == bool(inEnable);
        mCache = inEnable != FALSE;
        if (!mCache)
            ClearAll();
        return theSame ? S_FALSE : S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumObject(REFGUID, DWORD, DMUS_OBJECTDESC*) override { return S_FALSE; }

private:
    ~Loader() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        ClearAll();
    }
    static bool IsSegmentID(REFIID inID)
    {
        return IsEqualGUID(inID, kIID_IDirectMusicSegment) || IsEqualGUID(inID, IID_IDirectMusicSegment2);
    }
    void ClearAll()
    {
        std::map<std::string, Segment*> theCached;
        theCached.swap(mCached);
        for (auto& theEntry : theCached)
            theEntry.second->Release();
    }
    std::shared_ptr<LoaderData> mData;
    bool mCache = true;                         // DirectMusic caches every type by default
    std::map<std::string, Segment*> mCached;    // lower-case name to segment, one reference held
};

// The software synthesizer's port. Its KS properties set the output stage of the performance it is added to.
class Port final : public IDirectMusicPort, public IKsControl {
public:
    std::shared_ptr<PortOutput> mOutput = std::make_shared<PortOutput>();
    DWORD mGroups = 1;
    DWORD mVoices = 32, mAudioChannels = 2, mSampleRate = 22050;
    bool mActive = false;

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        if (IsEqualGUID(inID, IID_IUnknown) || IsEqualGUID(inID, kIID_IDirectMusicPort))
            *outObject = static_cast<IDirectMusicPort*>(this);
        else if (IsEqualGUID(inID, IID_IKsControl))
            *outObject = static_cast<IKsControl*>(this);
        else {
            *outObject = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override
    {
        long theCount = --mReferences;
        if (theCount == 0)
            delete this;
        return ULONG(theCount);
    }

    static void PutCaps(DMUS_PORTCAPS& outCaps)
    {
        memset(reinterpret_cast<uint8_t*>(&outCaps) + sizeof outCaps.dwSize, 0, sizeof outCaps - sizeof outCaps.dwSize);
        outCaps.dwFlags = DMUS_PC_DLS | kPortSoftwareSynth | kPortDirectSound | kPortShareable;
        outCaps.guidPort = kCLSID_DirectMusicSynth;
        outCaps.dwClass = DMUS_PC_OUTPUTCLASS;
        outCaps.dwType = kPortUserModeSynth;
        outCaps.dwMemorySize = kPortSystemMemory;
        outCaps.dwMaxChannelGroups = kMaxChannelGroups;
        outCaps.dwMaxVoices = 1000;
        outCaps.dwMaxAudioChannels = 2;
        outCaps.dwEffectFlags = DMUS_EFFECT_REVERB;
        const char* theName = "Software Synthesizer";
        for (size_t i = 0; theName[i]; ++i)
            outCaps.wszDescription[i] = WCHAR(theName[i]);
    }

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE PlayBuffer(IDirectMusicBuffer*) override { return E_NOTIMPL; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetReadNotificationHandle(HANDLE) override { return E_NOTIMPL; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Read(IDirectMusicBuffer*) override { return E_NOTIMPL; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DownloadInstrument(IDirectMusicInstrument*,
                                                                      IDirectMusicDownloadedInstrument** outInstrument,
                                                                      void*, DWORD) override
    {
        if (outInstrument)
            *outInstrument = nullptr;
        return E_NOTIMPL;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE UnloadInstrument(IDirectMusicDownloadedInstrument*) override
    {
        return E_NOTIMPL;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLatencyClock(IReferenceClock** outClock) override
    {
        if (outClock)
            *outClock = nullptr;
        return E_NOTIMPL;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetRunningStats(_DMUS_SYNTHSTATS*) override { return E_NOTIMPL; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Compact() override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCaps(DMUS_PORTCAPS* outCaps) override
    {
        if (!outCaps)
            return E_POINTER;
        if (outCaps->dwSize != sizeof(DMUS_PORTCAPS))
            return E_INVALIDARG;
        PutCaps(*outCaps);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DeviceIoControl(DWORD, void*, DWORD, void*, DWORD, DWORD*,
                                                                   void*) override
    {
        return E_NOTIMPL;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetNumChannelGroups(DWORD inGroups) override
    {
        if (inGroups < 1 || inGroups > kMaxChannelGroups)
            return E_INVALIDARG;
        mGroups = inGroups;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetNumChannelGroups(DWORD* outGroups) override
    {
        if (!outGroups)
            return E_POINTER;
        *outGroups = mGroups;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Activate(BOOL inActive) override
    {
        const bool theSame = mActive == bool(inActive);
        mActive = inActive != FALSE;
        return theSame ? S_FALSE : S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetChannelPriority(DWORD, DWORD, DWORD) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetChannelPriority(DWORD, DWORD, DWORD* outPriority) override
    {
        if (!outPriority)
            return E_POINTER;
        *outPriority = 0x80000000u;     // DAUD_STANDARD_VOICE_PRIORITY
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetDirectSound(IDirectSound*, void*) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetFormat(void* outFormat, DWORD* ioFormatSize, DWORD* outBufferSize) override
    {
        if (!ioFormatSize)
            return E_POINTER;
        if (outFormat) {
            if (*ioFormatSize < sizeof(WAVEFORMATEX))
                return E_INVALIDARG;
            WAVEFORMATEX& theFormat = *static_cast<WAVEFORMATEX*>(outFormat);
            theFormat.wFormatTag = WAVE_FORMAT_PCM;
            theFormat.nChannels = WORD(mAudioChannels);
            theFormat.nSamplesPerSec = mSampleRate;
            theFormat.nAvgBytesPerSec = mSampleRate * 2 * mAudioChannels;
            theFormat.nBlockAlign = WORD(2 * mAudioChannels);
            theFormat.wBitsPerSample = 16;
            theFormat.cbSize = 0;
        }
        *ioFormatSize = sizeof(WAVEFORMATEX);
        if (outBufferSize)
            *outBufferSize = mSampleRate / 10 * 2 * mAudioChannels;   // about 100 ms
        return S_OK;
    }

    // The synthesizer's volume (LONG, hundredths of a dB), its effects (DWORD, bit 0 the reverb) and the reverb's
    // parameters (DMUS_WAVES_REVERB_PARAMS)
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE KsProperty(KSPROPERTY* inProperty, ULONG inPropertyLength, LPVOID ioData,
                                                              ULONG inDataLength, ULONG* outReturned) override
    {
        if (!inProperty || inPropertyLength < sizeof(KSPROPERTY))
            return E_INVALIDARG;
        if (outReturned)
            *outReturned = 0;
        ULONG theSize = 0;
        if (IsEqualGUID(inProperty->Set, GUID_DMUS_PROP_Volume) || IsEqualGUID(inProperty->Set, GUID_DMUS_PROP_Effects))
            theSize = 4;
        else if (IsEqualGUID(inProperty->Set, GUID_DMUS_PROP_WavesReverb))
            theSize = sizeof(DMUS_WAVES_REVERB_PARAMS);
        if (!theSize || inProperty->Id != 0)
            return kUnknownProperty;
        if (inProperty->Flags & kPropertyBasicSupport) {
            if (inDataLength < 4 || !ioData)
                return E_INVALIDARG;
            *static_cast<DWORD*>(ioData) = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET;
            if (outReturned)
                *outReturned = 4;
            return S_OK;
        }
        if (!(inProperty->Flags & (KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_SET)) || inDataLength < theSize || !ioData)
            return E_INVALIDARG;
        const bool theSet = (inProperty->Flags & KSPROPERTY_TYPE_SET) != 0;
        std::lock_guard<std::mutex> theGuard(gOutputLock);
        PortOutput& theOutput = *mOutput;
        if (IsEqualGUID(inProperty->Set, GUID_DMUS_PROP_Volume)) {
            if (theSet)
                theOutput.mVolume = std::min<LONG>(std::max<LONG>(*static_cast<LONG*>(ioData), -20000), 2000);
            else
                *static_cast<LONG*>(ioData) = theOutput.mVolume;
        } else if (IsEqualGUID(inProperty->Set, GUID_DMUS_PROP_Effects)) {
            if (theSet)
                theOutput.mEffects = *static_cast<DWORD*>(ioData) & DMUS_EFFECT_REVERB;
            else
                *static_cast<DWORD*>(ioData) = theOutput.mEffects;
        } else {
            DMUS_WAVES_REVERB_PARAMS& theParameters = *static_cast<DMUS_WAVES_REVERB_PARAMS*>(ioData);
            if (theSet) {
                theOutput.mReverb.mInGainDb = theParameters.fInGain;
                theOutput.mReverb.mMixDb = theParameters.fReverbMix;
                theOutput.mReverb.mTimeMs = theParameters.fReverbTime;
                theOutput.mReverb.mHighRatio = theParameters.fHighFreqRTRatio;
                ++theOutput.mVersion;
            } else {
                theParameters.fInGain = theOutput.mReverb.mInGainDb;
                theParameters.fReverbMix = theOutput.mReverb.mMixDb;
                theParameters.fReverbTime = theOutput.mReverb.mTimeMs;
                theParameters.fHighFreqRTRatio = theOutput.mReverb.mHighRatio;
            }
        }
        if (outReturned)
            *outReturned = theSet ? 0 : theSize;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE KsMethod(KSPROPERTY*, ULONG, LPVOID, ULONG, ULONG*) override
    {
        return kUnknownProperty;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE KsEvent(KSPROPERTY*, ULONG, LPVOID, ULONG, ULONG*) override
    {
        return kUnknownProperty;
    }

private:
    ~Port() {}
    std::atomic<long> mReferences{1};
};

// The port factory
class DirectMusic final : public Object<IDirectMusic> {
public:
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        return Answer(inID, {&kIID_IDirectMusic}, outObject);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumPort(DWORD inIndex, DMUS_PORTCAPS* outCaps) override
    {
        if (!outCaps)
            return E_POINTER;
        if (inIndex != 0)
            return S_FALSE;
        if (outCaps->dwSize != sizeof(DMUS_PORTCAPS))
            return E_INVALIDARG;
        Port::PutCaps(*outCaps);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateMusicBuffer(void*, IDirectMusicBuffer** outBuffer, IUnknown*) override
    {
        if (outBuffer)
            *outBuffer = nullptr;
        return E_NOTIMPL;
    }
    // The software synthesizer, whatever was asked for; the parameters it cannot take are written back with what it
    // took, with S_FALSE, as DirectMusic does
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreatePort(REFCLSID, DMUS_PORTPARAMS* ioParameters,
                                                              IDirectMusicPort** outPort, IUnknown* inOuter) override
    {
        if (!outPort || !ioParameters)
            return E_POINTER;
        *outPort = nullptr;
        if (inOuter)
            return CLASS_E_NOAGGREGATION;
        if (ioParameters->dwSize < offsetof(DMUS_PORTPARAMS, dwFeatures))
            return E_INVALIDARG;
        Port* thePort = new Port;
        const DWORD theValid = ioParameters->dwValidParams;
        bool theChanged = false;
        if (theValid & kParamVoices)
            thePort->mVoices = std::max<DWORD>(1, ioParameters->dwVoices);
        if (theValid & DMUS_PORTPARAMS_CHANNELGROUPS) {
            thePort->mGroups = std::min(std::max<DWORD>(ioParameters->dwChannelGroups, 1), kMaxChannelGroups);
            theChanged |= thePort->mGroups != ioParameters->dwChannelGroups;
            ioParameters->dwChannelGroups = thePort->mGroups;
        }
        if (theValid & kParamAudioChannels) {
            thePort->mAudioChannels = ioParameters->dwAudioChannels == 1 ? 1 : 2;
            theChanged |= thePort->mAudioChannels != ioParameters->dwAudioChannels;
            ioParameters->dwAudioChannels = thePort->mAudioChannels;
        }
        if (theValid & kParamSampleRate)
            thePort->mSampleRate = std::max<DWORD>(8000, std::min<DWORD>(ioParameters->dwSampleRate, 96000));
        if (theValid & kParamEffects)
            thePort->mOutput->mEffects = ioParameters->dwEffectFlags & DMUS_EFFECT_REVERB;
        *outPort = thePort;
        return theChanged ? S_FALSE : S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE EnumMasterClock(DWORD, _DMUS_CLOCKINFO*) override { return S_FALSE; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetMasterClock(GUID* outClock, IReferenceClock** outReference) override
    {
        if (outClock)
            *outClock = GUID();
        if (outReference) {
            *outReference = nullptr;
            return E_NOTIMPL;
        }
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetMasterClock(REFGUID) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Activate(BOOL inActive) override
    {
        const bool theSame = mActive == bool(inActive);
        mActive = inActive != FALSE;
        return theSame ? S_FALSE : S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetDefaultPort(GUID* outPort) override
    {
        if (!outPort)
            return E_POINTER;
        *outPort = kCLSID_DirectMusicSynth;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetDirectSound(IDirectSound*, HWND) override { return S_OK; }

private:
    bool mActive = false;
};

class Performance final : public Object<IDirectMusicPerformance> {
public:
    std::shared_ptr<Engine> mEngine;

    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        return Answer(inID, {&kIID_IDirectMusicPerformance, &IID_IDirectMusicPerformance2}, outObject);
    }
    // The synthesizer, at the rate of the Miles driver it plays through, and the IDirectMusic the ports come from
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Init(IDirectMusic** ioDirectMusic, IDirectSound*, HWND) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (mEngine)
            return kAlreadyInit;
        static bool sLogger = (Dm_setLogger(DmusicLogLevel(), OnDmusicLog, nullptr), true);
        (void)sLogger;
        int theRate = eeport::MusicRate();
        if (theRate <= 0)
            theRate = 44100;
        auto theEngine = std::make_shared<Engine>();
        if (DmPerformance_create(&theEngine->mPerformance, uint32_t(theRate)) != DmResult_SUCCESS)
            return E_OUTOFMEMORY;
        theEngine->mRate = theRate;
        DmPerformance_setMasterGroove(theEngine->mPerformance, mMasterGroove);
        if (ioDirectMusic) {
            if (*ioDirectMusic)
                (*ioDirectMusic)->AddRef();
            else
                *ioDirectMusic = new DirectMusic;
            mDirectMusic = *ioDirectMusic;
            mDirectMusic->AddRef();
        }
        mEngine = theEngine;
        mStart = Nanoseconds();
        eeport::SetMusicSource(Render);
        std::lock_guard<std::mutex> theOutputGuard(gOutputLock);
        gEngines.push_back(theEngine);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE PlaySegment(IDirectMusicSegment* inSegment, DWORD inFlags, __int64 inStart,
                                                               IDirectMusicSegmentState** outState) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (outState)
            *outState = nullptr;
        if (!mEngine)
            return kNotInit;
        if (!inSegment)
            return E_POINTER;
        Segment* theSegment = static_cast<Segment*>(inSegment);
        if (inStart != 0)
            EE_PORT_UNSUPPORTED("IDirectMusicPerformance::PlaySegment at a start time");
        if (inFlags & DMUS_SEGF_SECONDARY) {
            EE_PORT_UNSUPPORTED("DirectMusic secondary segments");
            SegmentState* theState = new SegmentState(theSegment, MusicTime(), 0);
            if (outState)
                *outState = theState;
            else
                theState->Release();
            return S_OK;
        }
        if (inFlags & DMUS_SEGF_QUEUE)
            EE_PORT_UNSUPPORTED("DirectMusic queued segments (one starts at its boundary)");
        theSegment->Download();
        if (DmPerformance_playSegment(mEngine->mPerformance, theSegment->mSegment,
                                      TimingOf(inFlags, theSegment->mHeader.mResolution)) != DmResult_SUCCESS)
            return E_FAIL;
        SegmentState* theState = StartPrimary(theSegment);
        if (outState) {
            theState->AddRef();
            *outState = theState;
        }
        return S_OK;
    }
    // Everything when both are NULL, else the primary segment if it is the one named
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Stop(IDirectMusicSegment* inSegment, IDirectMusicSegmentState* inState,
                                                        MUSIC_TIME, DWORD inFlags) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (!mEngine)
            return kNotInit;
        bool theMatch = !inSegment && !inState;
        if (!theMatch && mPrimary)
            theMatch = inState == mPrimary || inSegment == mPrimary->mSegment;
        if (theMatch)
            StopPrimary(TimingOf(inFlags, 0));
        return S_OK;
    }
    // The primary segment, while it plays
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetSegmentState(IDirectMusicSegmentState** outState, MUSIC_TIME) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (!outState)
            return E_POINTER;
        *outState = nullptr;
        if (!mEngine)
            return kNotInit;
        if (!PrimaryPlaying())
            return kNotFound;
        mPrimary->AddRef();
        *outState = mPrimary;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetPrepareTime(DWORD inMilliseconds) override
    {
        mPrepare = inMilliseconds;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetPrepareTime(DWORD* outMilliseconds) override
    {
        if (!outMilliseconds)
            return E_POINTER;
        *outMilliseconds = mPrepare;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetBumperLength(DWORD inMilliseconds) override
    {
        mBumper = inMilliseconds;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetBumperLength(DWORD* outMilliseconds) override
    {
        if (!outMilliseconds)
            return E_POINTER;
        *outMilliseconds = mBumper;
        return S_OK;
    }
    // The performance owns a message sent to it and frees it
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SendPMsg(DMUS_PMSG* inMessage) override
    {
        EE_PORT_UNSUPPORTED("IDirectMusicPerformance::SendPMsg");
        free(inMessage);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE MusicToReferenceTime(MUSIC_TIME inTime, REFERENCE_TIME* outTime) override
    {
        if (!mEngine)
            return kNotInit;
        if (!outTime)
            return E_POINTER;
        *outTime = mStart / 100 + REFERENCE_TIME(inTime / kTicksPerSecond * 1e7);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ReferenceToMusicTime(REFERENCE_TIME inTime, MUSIC_TIME* outTime) override
    {
        if (!mEngine)
            return kNotInit;
        if (!outTime)
            return E_POINTER;
        *outTime = MUSIC_TIME((inTime - mStart / 100) * 1e-7 * kTicksPerSecond);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE IsPlaying(IDirectMusicSegment* inSegment, IDirectMusicSegmentState* inState) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (!mEngine)
            return kNotInit;
        if (!inSegment && !inState)
            return E_POINTER;
        if (!PrimaryPlaying())
            return S_FALSE;
        return (inState ? inState == mPrimary : inSegment == mPrimary->mSegment) ? S_OK : S_FALSE;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetTime(REFERENCE_TIME* outNow, MUSIC_TIME* outMusicNow) override
    {
        if (!mEngine)
            return kNotInit;
        if (outNow)
            *outNow = Nanoseconds() / 100;
        if (outMusicNow)
            *outMusicNow = MusicTime();
        return S_OK;
    }
    // DMUS_PMSG starts with its size
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AllocPMsg(ULONG inSize, DMUS_PMSG** outMessage) override
    {
        if (!outMessage)
            return E_POINTER;
        *outMessage = nullptr;
        if (inSize < sizeof(DMUS_PMSG))
            return E_INVALIDARG;
        DMUS_PMSG* theMessage = static_cast<DMUS_PMSG*>(calloc(1, inSize));
        if (!theMessage)
            return E_OUTOFMEMORY;
        theMessage->dwSize = inSize;
        *outMessage = theMessage;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE FreePMsg(DMUS_PMSG* inMessage) override
    {
        if (!inMessage)
            return E_POINTER;
        free(inMessage);
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetGraph(IDirectMusicGraph** outGraph) override
    {
        if (outGraph)
            *outGraph = nullptr;
        return kNotFound;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetGraph(IDirectMusicGraph*) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetNotificationHandle(HANDLE inHandle, REFERENCE_TIME) override
    {
        if (inHandle)
            EE_PORT_UNSUPPORTED("DirectMusic notifications (the handle is never signalled)");
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetNotificationPMsg(DMUS_NOTIFICATION_PMSG** outMessage) override
    {
        if (!outMessage)
            return E_POINTER;
        *outMessage = nullptr;
        return S_FALSE;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AddNotificationType(REFGUID) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RemoveNotificationType(REFGUID) override { return S_OK; }
    // The port whose output stage the performance plays through: the first one added. NULL adds a default port.
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AddPort(IDirectMusicPort* inPort) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (!mEngine)
            return kNotInit;
        Port* thePort = inPort ? static_cast<Port*>(inPort) : new Port;
        if (std::find(mPorts.begin(), mPorts.end(), thePort) != mPorts.end())
            return S_FALSE;
        if (inPort)
            thePort->AddRef();
        mPorts.push_back(thePort);
        if (mPorts.size() == 1) {
            std::lock_guard<std::mutex> theOutputGuard(gOutputLock);
            mEngine->mOutput = thePort->mOutput;
        }
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE RemovePort(IDirectMusicPort* inPort) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (!mEngine)
            return kNotInit;
        auto theFound = std::find(mPorts.begin(), mPorts.end(), static_cast<Port*>(inPort));
        if (!inPort || theFound == mPorts.end())
            return E_INVALIDARG;
        mPorts.erase(theFound);
        {
            std::lock_guard<std::mutex> theOutputGuard(gOutputLock);
            mEngine->mOutput = mPorts.empty() ? std::make_shared<PortOutput>() : mPorts.front()->mOutput;
        }
        inPort->Release();
        return S_OK;
    }
    // One synthesizer plays every channel
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AssignPChannelBlock(DWORD, IDirectMusicPort* inPort, DWORD) override
    {
        if (!mEngine)
            return kNotInit;
        return inPort ? S_OK : E_INVALIDARG;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AssignPChannel(DWORD, IDirectMusicPort* inPort, DWORD, DWORD) override
    {
        if (!mEngine)
            return kNotInit;
        return inPort ? S_OK : E_INVALIDARG;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE PChannelInfo(DWORD inChannel, IDirectMusicPort** outPort, DWORD* outGroup,
                                                                DWORD* outMidiChannel) override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        if (!mEngine)
            return kNotInit;
        if (mPorts.empty())
            return E_INVALIDARG;
        if (outPort) {
            mPorts.front()->AddRef();
            *outPort = mPorts.front();
        }
        if (outGroup)
            *outGroup = inChannel / 16 + 1;
        if (outMidiChannel)
            *outMidiChannel = inChannel % 16;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DownloadInstrument(IDirectMusicInstrument*, DWORD,
                                                                      IDirectMusicDownloadedInstrument** outInstrument, void*,
                                                                      DWORD, IDirectMusicPort** outPort, DWORD*, DWORD*) override
    {
        if (outInstrument)
            *outInstrument = nullptr;
        if (outPort)
            *outPort = nullptr;
        return E_NOTIMPL;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Invalidate(MUSIC_TIME, DWORD) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetParam(REFGUID, DWORD, DWORD, MUSIC_TIME, MUSIC_TIME* outNext, void*) override
    {
        if (outNext)
            *outNext = 0;
        return kNotFound;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetParam(REFGUID, DWORD, DWORD, MUSIC_TIME, void*) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetGlobalParam(REFGUID inType, void* outValue, DWORD inSize) override
    {
        if (!outValue || !inSize)
            return E_POINTER;
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        auto theFound = mGlobals.find(GuidText(inType));
        if (theFound != mGlobals.end()) {
            memcpy(outValue, theFound->second.data(), std::min<size_t>(inSize, theFound->second.size()));
            return S_OK;
        }
        // the defaults of the ones that have them
        if (IsEqualGUID(inType, GUID_PerfMasterGrooveLevel) || IsEqualGUID(inType, GUID_PerfAutoDownload) ||
            IsEqualGUID(inType, kGUID_PerfMasterVolume)) {
            memset(outValue, 0, inSize);
            return S_OK;
        }
        return E_INVALIDARG;
    }
    // The auto-download (BOOL), the master groove level (char) and the master volume (LONG, hundredths of a dB) mean
    // something; every parameter is kept for GetGlobalParam, as DirectMusic keeps them
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetGlobalParam(REFGUID inType, void* inValue, DWORD inSize) override
    {
        if (!inValue || !inSize)
            return E_POINTER;
        if (inSize > 0x10000)
            return E_INVALIDARG;
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        const uint8_t* theBytes = static_cast<const uint8_t*>(inValue);
        mGlobals[GuidText(inType)].assign(theBytes, theBytes + inSize);
        if (IsEqualGUID(inType, GUID_PerfMasterGrooveLevel)) {
            mMasterGroove = int8_t(theBytes[0]);
            if (mEngine)
                DmPerformance_setMasterGroove(mEngine->mPerformance, mMasterGroove);
        } else if (IsEqualGUID(inType, kGUID_PerfMasterVolume) && inSize >= 4 && mEngine) {
            LONG theVolume;
            memcpy(&theVolume, theBytes, 4);
            std::lock_guard<std::mutex> theOutputGuard(gOutputLock);
            mEngine->mMasterVolume = std::min<LONG>(std::max<LONG>(theVolume, -20000), 2000);
        }
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetLatencyTime(REFERENCE_TIME* outTime) override
    {
        if (!outTime)
            return E_POINTER;
        *outTime = Nanoseconds() / 100 + 20 * 10000;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetQueueTime(REFERENCE_TIME* outTime) override
    {
        if (!outTime)
            return E_POINTER;
        *outTime = Nanoseconds() / 100 + (20 + REFERENCE_TIME(mBumper)) * 10000;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AdjustTime(REFERENCE_TIME) override { return S_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CloseDown() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        Close();
        return S_OK;
    }

    // AutoTransition: a transition pattern from the playing style at the boundary, then the new segment, or silence
    HRESULT Transition(Segment* inSegment, DmEmbellishmentType inEmbellishment, DmTiming inTiming,
                       IDirectMusicSegmentState** outState)
    {
        if (!mEngine)
            return kNotInit;
        if (!inSegment) {
            StopPrimary(inTiming);
            return S_OK;
        }
        inSegment->Download();
        if (DmPerformance_playTransition(mEngine->mPerformance, inSegment->mSegment, inEmbellishment, inTiming) !=
            DmResult_SUCCESS)
            return E_FAIL;
        SegmentState* theState = StartPrimary(inSegment);
        if (outState) {
            theState->AddRef();
            *outState = theState;
        }
        return S_OK;
    }

private:
    ~Performance() override
    {
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        Close();
    }
    MUSIC_TIME MusicTime() const
    {
        return MUSIC_TIME(int64_t(double(Nanoseconds() - mStart) * 1e-9 * kTicksPerSecond));
    }
    // DMUS_SEGF_DEFAULT takes the segment's default resolution
    static DmTiming TimingOf(DWORD inFlags, DWORD inResolution)
    {
        if (inFlags & DMUS_SEGF_DEFAULT)
            inFlags = inResolution;
        if (inFlags & kSegmentMeasure)
            return DmTiming_MEASURE;
        if (inFlags & kSegmentBeat)
            return DmTiming_BEAT;
        if (inFlags & kSegmentGrid)
            return DmTiming_GRID;
        return DmTiming_INSTANT;
    }
    // A new state for a segment, which becomes the primary segment
    SegmentState* StartPrimary(Segment* inSegment)
    {
        SegmentState* theState = new SegmentState(inSegment, MusicTime(), ++mEngine->mPlays);
        if (mPrimary)
            mPrimary->Release();
        mPrimary = theState;
        return theState;
    }
    void StopPrimary(DmTiming inTiming)
    {
        DmPerformance_playSegment(mEngine->mPerformance, nullptr, inTiming);
        ++mEngine->mPlays;
        if (mPrimary)
            mPrimary->Release();
        mPrimary = nullptr;
    }
    // Whether the primary segment still plays, as the audio thread last saw it
    bool PrimaryPlaying()
    {
        if (!mPrimary)
            return false;
        if (mEngine->mPlays.load() != mPrimary->mPlay || mEngine->mEnded.load() == mPrimary->mPlay) {
            mPrimary->Release();
            mPrimary = nullptr;
        }
        return mPrimary != nullptr;
    }
    void Close()
    {
        if (!mEngine)
            return;
        DmPerformance_playSegment(mEngine->mPerformance, nullptr, DmTiming_INSTANT);
        {
            std::lock_guard<std::mutex> theOutputGuard(gOutputLock);
            gEngines.erase(std::remove(gEngines.begin(), gEngines.end(), mEngine), gEngines.end());
        }
        mEngine.reset();
        if (mPrimary)
            mPrimary->Release();
        mPrimary = nullptr;
        for (Port* thePort : mPorts)
            thePort->Release();
        mPorts.clear();
        if (mDirectMusic)
            mDirectMusic->Release();
        mDirectMusic = nullptr;
    }

    IDirectMusic* mDirectMusic = nullptr;   // one reference held
    std::vector<Port*> mPorts;              // one reference each
    SegmentState* mPrimary = nullptr;       // the primary segment's state, one reference held
    int8_t mMasterGroove = 0;
    std::map<std::string, std::vector<uint8_t>> mGlobals;
    DWORD mPrepare = 1000, mBumper = 50;
    int64_t mStart = 0;
};

class Composer final : public Object<IDirectMusicComposer> {
public:
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        return Answer(inID, {&IID_IDirectMusicComposer}, outObject);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ComposeSegmentFromTemplate(IDirectMusicStyle*, IDirectMusicSegment*, WORD,
                                                                              IDirectMusicChordMap*,
                                                                              IDirectMusicSegment** outSegment) override
    {
        return NotComposed(outSegment);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ComposeSegmentFromShape(IDirectMusicStyle*, WORD, WORD, WORD, BOOL, BOOL,
                                                                           IDirectMusicChordMap*,
                                                                           IDirectMusicSegment** outSegment) override
    {
        return NotComposed(outSegment);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ComposeTransition(IDirectMusicSegment*, IDirectMusicSegment*, MUSIC_TIME,
                                                                     WORD, DWORD, IDirectMusicChordMap*,
                                                                     IDirectMusicSegment** outSegment) override
    {
        return NotComposed(outSegment);
    }
    // DMUS_COMMANDT_GROOVE, FILL, INTRO, BREAK, END, ENDANDINTRO choose the embellishment; the DMUS_COMPOSEF_* flags
    // choose the boundary, a measure unless they name another. A modulating or long transition plays as a
    // one-measure transition, and the transition segment is not handed out.
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE AutoTransition(IDirectMusicPerformance* inPerformance,
                                                                  IDirectMusicSegment* inSegment, WORD inCommand,
                                                                  DWORD inFlags, IDirectMusicChordMap*,
                                                                  IDirectMusicSegment** outTransition,
                                                                  IDirectMusicSegmentState** outState,
                                                                  IDirectMusicSegmentState** outTransitionState) override
    {
        for (IDirectMusicSegmentState** theOut : {outState, outTransitionState})
            if (theOut)
                *theOut = nullptr;
        if (outTransition)
            *outTransition = nullptr;
        if (!inPerformance)
            return E_POINTER;
        static const DmEmbellishmentType kCommands[6] = {DmEmbellishment_GROOVE, DmEmbellishment_FILL,
                                                         DmEmbellishment_INTRO,  DmEmbellishment_BREAK,
                                                         DmEmbellishment_END,    DmEmbellishment_END_AND_INTRO};
        DmTiming theTiming = DmTiming_MEASURE;
        if (inFlags & kComposeImmediate)
            theTiming = DmTiming_INSTANT;
        else if (inFlags & kComposeGrid)
            theTiming = DmTiming_GRID;
        else if (inFlags & kComposeBeat)
            theTiming = DmTiming_BEAT;
        std::lock_guard<std::recursive_mutex> theGuard(gApiLock);
        return static_cast<Performance*>(inPerformance)
            ->Transition(static_cast<Segment*>(inSegment), inCommand < 6 ? kCommands[inCommand] : DmEmbellishment_GROOVE,
                         theTiming, outState);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ComposeTemplateFromShape(WORD, WORD, BOOL, BOOL, WORD,
                                                                            IDirectMusicSegment** outSegment) override
    {
        return NotComposed(outSegment);
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE ChangeChordMap(IDirectMusicSegment*, BOOL, IDirectMusicChordMap*) override
    {
        EE_PORT_UNSUPPORTED("IDirectMusicComposer::ChangeChordMap");
        return E_NOTIMPL;
    }

private:
    static HRESULT NotComposed(IDirectMusicSegment** outSegment)
    {
        EE_PORT_UNSUPPORTED("IDirectMusicComposer composition (it composes transitions only)");
        if (outSegment)
            *outSegment = nullptr;
        return E_NOTIMPL;
    }
};

// A new object of a class, asked for an interface it answers
template <class T>
HRESULT Make(T* inObject, REFIID inID, LPVOID* outObject)
{
    HRESULT theResult = inObject->QueryInterface(inID, outObject);
    inObject->Release();
    return theResult;
}

} // namespace

// DirectMusic's classes are the only COM classes the game creates
HRESULT WINAPI CoCreateInstance(REFCLSID inClass, LPUNKNOWN inOuter, DWORD, REFIID inID, LPVOID* outObject)
{
    if (!outObject)
        return E_POINTER;
    *outObject = NULL;
    if (inOuter)
        return CLASS_E_NOAGGREGATION;
    if (IsEqualGUID(inClass, CLSID_DirectMusicLoader)) {
        Loader* theLoader = new Loader;
        if (!theLoader->Create()) {
            theLoader->Release();
            return E_OUTOFMEMORY;
        }
        return Make(theLoader, inID, outObject);
    }
    if (IsEqualGUID(inClass, CLSID_DirectMusicPerformance))
        return Make(new Performance, inID, outObject);
    if (IsEqualGUID(inClass, CLSID_DirectMusicComposer))
        return Make(new Composer, inID, outObject);
    eeport::Log("CoCreateInstance: class %s is not registered", GuidText(inClass).c_str());
    return REGDB_E_CLASSNOTREG;
}
