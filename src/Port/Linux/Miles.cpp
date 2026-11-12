// The Miles Sound System 6 API (mss32.dll) that the Miles Sound System Mixer plug-in calls, on SDL audio. A driver
// is one SDL audio device; its callback mixes the driver's playing samples. Samples play RIFF PCM images from the
// game's memory, and MP3 images decoded once through FFmpeg.
//
// The rules marked "Miles" are Miles 6.1's own behaviour as ee-macos (kit-src/dx/mss32.cpp, MIT) measured it from the
// library: the preference defaults that AIL_startup installs, the balance pan law, the loop count that counts down, a
// start that always begins at the top, and positions of a compressed sample counted in its compressed bytes.
#include "Win32Internal.h"

#include <dsound.h>
#include <mss/mss.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
}
#include <SDL.h>

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <memory>
#include <set>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

namespace {

enum Status { kFree = 1, kDone = 2, kPlaying = 4 };

// Miles: the preferences after AIL_startup. The mixer reads the fragment sizes back and divides by them.
const S32 kPreferenceDefaults[64] = {
    131,   64,    127, 120,  8,   127, 1, 0,     2, 0,   1536, 49152, 5, 16,  8192, 0,
    32768, 44100, 0,   4096, 120, 24,  0, 1,     0, 1,   0,    1,     1, 0,   0,    1,
    2048,  0,     8,   96,   0,   0,   0, 32768, 0, 250, 8,    1,     1, 100, 0,    0,
};
const int kDefaultVolume = 2;  // DIG_DEFAULT_VOLUME
const size_t kDecodedBudget = 64u << 20;

// An MP3 image as 16-bit interleaved PCM
struct Decoded {
    std::vector<int16_t> mPCM;
    int mRate = 0;
    int mChannels = 0;
};

struct Driver;

struct Sample {
    Driver* mDriver;
    // the PCM: the game's image, or mDecoded's samples
    const uint8_t* mData = nullptr;
    uint32_t mBytes = 0;
    int mChannels = 0;
    int mBits = 0;
    uint32_t mFileRate = 0;
    std::shared_ptr<const Decoded> mDecoded;
    uint32_t mEncodedBytes = 0;     // a decoded sample's image size: Miles counts its positions in these bytes
    S32 mVolume = 127;
    S32 mPan = 64;
    U32 mLoops = 1;                 // Miles: 0 plays for ever, n plays n passes and counts down
    U32 mRate = 11025;
    bool mPlaying = false;
    uint64_t mCursor = 0;           // while playing: frames, 32.32 fixed point
    uint32_t mPosition = 0;         // while not playing: PCM bytes

    explicit Sample(Driver* inDriver) : mDriver(inDriver) {}
};

struct Driver {
    U32 mRate;
    S32 mBits;
    S32 mChannels;
    S32 mMaster = 127;
    int mDeviceRate;                // the rate the mixer produces
    SDL_AudioDeviceID mDevice = 0;
    std::vector<Sample*> mSamples;
};

// gLock guards every driver and sample; the audio callback takes it to mix
std::mutex gLock;
std::set<Driver*> gDrivers;
std::set<Sample*> gSamples;
S32 gPreferences[64];
bool gPreferencesSet;
char gLastError[256];
char gRedistDirectory[MAX_PATH + 2];
bool gWithoutDevice;

std::mutex gDecodeLock;
struct CacheEntry {
    uint64_t mHash;
    uint32_t mBytes;
    uint64_t mUsed;
    std::shared_ptr<const Decoded> mDecoded;
};
std::vector<CacheEntry> gDecodeCache;
uint64_t gDecodeClock;

void SetError(const char* inText)
{
    snprintf(gLastError, sizeof gLastError, "%s", inText);
}

S32* Preferences()
{
    if (!gPreferencesSet) {
        std::copy(std::begin(kPreferenceDefaults), std::end(kPreferenceDefaults), gPreferences);
        gPreferencesSet = true;
    }
    return gPreferences;
}

Sample* SampleOf(HSAMPLE inHandle)
{
    Sample* theSample = reinterpret_cast<Sample*>(inHandle);
    return gSamples.count(theSample) ? theSample : nullptr;
}

Driver* DriverOf(HDIGDRIVER inHandle)
{
    Driver* theDriver = reinterpret_cast<Driver*>(inHandle);
    return gDrivers.count(theDriver) ? theDriver : nullptr;
}

uint32_t FrameBytes(const Sample& inSample)
{
    return std::max(1, inSample.mChannels * (inSample.mBits / 8));
}

uint32_t FrameCount(const Sample& inSample)
{
    return inSample.mBytes / FrameBytes(inSample);
}

// Miles: the bytes a position moves in, a PCM frame or one byte of a compressed image
uint32_t Granularity(const Sample& inSample)
{
    return inSample.mDecoded ? 1 : FrameBytes(inSample);
}

uint32_t ToMiles(const Sample& inSample, uint32_t inPCM)
{
    if (!inSample.mDecoded || !inSample.mBytes)
        return inPCM;
    return uint32_t(uint64_t(inPCM) * inSample.mEncodedBytes / inSample.mBytes);
}

uint32_t FromMiles(const Sample& inSample, uint32_t inPosition)
{
    uint64_t thePCM = inPosition;
    if (inSample.mDecoded && inSample.mEncodedBytes)
        thePCM = uint64_t(inPosition) * inSample.mBytes / inSample.mEncodedBytes;
    thePCM = std::min<uint64_t>(thePCM, inSample.mBytes);
    return uint32_t(thePCM - thePCM % FrameBytes(inSample));
}

// Where the sample stands, in PCM bytes
uint32_t Cursor(const Sample& inSample)
{
    if (!inSample.mPlaying)
        return inSample.mPosition;
    return std::min<uint64_t>((inSample.mCursor >> 32) * FrameBytes(inSample), inSample.mBytes);
}

void Play(Sample& ioSample, uint32_t inFrom)
{
    ioSample.mPlaying = true;
    ioSample.mCursor = uint64_t(inFrom / FrameBytes(ioSample)) << 32;
}

void Stop(Sample& ioSample)
{
    ioSample.mPosition = Cursor(ioSample);
    ioSample.mPlaying = false;
}

// Miles: a pass reached the end. The sample goes round again, counting the loops down, or finishes at the end.
// Returns whether it plays on.
bool PassEnded(Sample& ioSample)
{
    if (ioSample.mLoops == 0)
        return true;
    if (ioSample.mLoops > 1) {
        --ioSample.mLoops;
        return true;
    }
    ioSample.mPlaying = false;
    ioSample.mPosition = ioSample.mBytes;
    return false;
}

void Unload(Sample& ioSample)
{
    ioSample.mPlaying = false;
    ioSample.mPosition = 0;
    ioSample.mData = nullptr;
    ioSample.mBytes = 0;
    ioSample.mChannels = 0;
    ioSample.mBits = 0;
    ioSample.mFileRate = 0;
    ioSample.mDecoded.reset();
    ioSample.mEncodedBytes = 0;
}

// Miles: AIL_init_sample's defaults, DIG_DEFAULT_VOLUME, centred on a stereo driver, one pass, 11025 Hz until a file
// states its own rate
void Initialize(Sample& ioSample)
{
    Unload(ioSample);
    ioSample.mVolume = std::min<S32>(std::max<S32>(Preferences()[kDefaultVolume], 0), 127);
    ioSample.mPan = ioSample.mDriver->mChannels == 1 ? 0 : 64;
    ioSample.mLoops = 1;
    ioSample.mRate = 11025;
}

int32_t Frame(const Sample& inSample, uint32_t inFrame, int inChannel)
{
    uint32_t theIndex = inFrame * inSample.mChannels + std::min(inChannel, inSample.mChannels - 1);
    if (inSample.mBits == 8)
        return (int32_t(inSample.mData[theIndex]) - 128) * 256;
    int16_t theValue;
    memcpy(&theValue, inSample.mData + theIndex * 2, 2);
    return theValue;
}

// Miles: balance, not power. Each side keeps full level until the sound moves away from it, then fades linearly; a
// side's scale out of 128 is twice the distance from the far end, saturating at 63, so 63 and 64 are both centre.
int PanScale(int inDistance)
{
    return inDistance < 63 ? 2 * inDistance : 128;
}

void MixSample(Sample& ioSample, int32_t* ioMix, int inFrames, int inDeviceRate)
{
    uint32_t theFrames = FrameCount(ioSample);
    if (!theFrames || !inDeviceRate) {
        ioSample.mPlaying = false;
        return;
    }
    U32 theRate = ioSample.mRate ? ioSample.mRate : ioSample.mFileRate;
    uint64_t theStep = (uint64_t(theRate) << 32) / unsigned(inDeviceRate);
    // Miles: the master volume scales the sample's in integer steps of 1/127
    int64_t theVolume = int64_t(ioSample.mDriver->mMaster) * std::min<S32>(std::max<S32>(ioSample.mVolume, 0), 127) /
                        127;
    S32 thePan = std::min<S32>(std::max<S32>(ioSample.mPan, 0), 127);
    bool theMono = ioSample.mDriver->mChannels == 1;
    int64_t theLeft = theVolume * (theMono ? 128 : PanScale(127 - thePan));
    int64_t theRight = theVolume * (theMono ? 128 : PanScale(thePan));
    const uint64_t theLength = uint64_t(theFrames) << 32;
    for (int i = 0; i < inFrames; ++i) {
        while (ioSample.mCursor >= theLength) {
            if (!PassEnded(ioSample))
                return;
            ioSample.mCursor -= theLength;
        }
        uint32_t theIndex = uint32_t(ioSample.mCursor >> 32);
        uint32_t theNext = theIndex + 1 < theFrames ? theIndex + 1 : ioSample.mLoops == 1 ? theIndex : 0;
        int64_t theFraction = (ioSample.mCursor >> 16) & 0xffff;
        for (int c = 0; c < 2; ++c) {
            int64_t theA = Frame(ioSample, theIndex, c), theB = Frame(ioSample, theNext, c);
            int64_t theValue = theA + (((theB - theA) * theFraction) >> 16);
            ioMix[i * 2 + c] += int32_t(theValue * (c ? theRight : theLeft) / (127 * 128));
        }
        ioSample.mCursor += theStep;
    }
}

void Mix(Driver& ioDriver, int16_t* outSamples, int inFrames)
{
    std::vector<int32_t> theMix(size_t(inFrames) * 2);
    for (Sample* theSample : ioDriver.mSamples)
        if (theSample->mPlaying)
            MixSample(*theSample, theMix.data(), inFrames, ioDriver.mDeviceRate);
    for (size_t i = 0; i < theMix.size(); ++i)
        outSamples[i] = int16_t(std::min(std::max(theMix[i], -32768), 32767));
}

void SDLCALL AudioCallback(void* inDriver, Uint8* outStream, int inBytes)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Mix(*static_cast<Driver*>(inDriver), reinterpret_cast<int16_t*>(outStream), inBytes / 4);
}

// Opens the driver's device and starts it. Without a sound device, SDL's dummy driver plays to nothing, so samples
// still advance and finish.
void OpenDevice(Driver& ioDriver)
{
    SDL_AudioSpec theWanted{}, theHave{};
    theWanted.freq = int(ioDriver.mRate);
    theWanted.format = AUDIO_S16SYS;
    theWanted.channels = 2;
    theWanted.samples = 512;
    theWanted.callback = AudioCallback;
    theWanted.userdata = &ioDriver;
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) == 0)
        ioDriver.mDevice = SDL_OpenAudioDevice(nullptr, 0, &theWanted, &theHave, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    if (!ioDriver.mDevice) {
        eeport::Log("Miles: no sound device (%s); sound plays to SDL's dummy driver", SDL_GetError());
        SDL_AudioQuit();
        if (SDL_AudioInit("dummy") == 0)
            ioDriver.mDevice = SDL_OpenAudioDevice(nullptr, 0, &theWanted, &theHave, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    }
    if (!ioDriver.mDevice) {
        eeport::Log("Miles: SDL's dummy sound driver does not open either (%s)", SDL_GetError());
        return;
    }
    ioDriver.mDeviceRate = theHave.freq;
    SDL_PauseAudioDevice(ioDriver.mDevice, 0);
}

uint64_t Hash(const uint8_t* inData, size_t inSize)
{
    uint64_t theHash = 0xcbf29ce484222325ull;
    for (size_t i = 0; i < inSize; ++i)
        theHash = (theHash ^ inData[i]) * 0x100000001b3ull;
    return theHash;
}

uint32_t Read32(const uint8_t* inData)
{
    return inData[0] | inData[1] << 8 | inData[2] << 16 | uint32_t(inData[3]) << 24;
}

uint16_t Read16(const uint8_t* inData)
{
    return uint16_t(inData[0] | inData[1] << 8);
}

enum RiffResult { kNotRiff, kRiffPCM, kRiffUnsupported };

// A RIFF WAVE image: PCM with 1 or 2 channels of 8 or 16 bits is played from the image itself
RiffResult ParseRiff(Sample& ioSample, const uint8_t* inImage, uint32_t inSize)
{
    if (inSize < 12 || memcmp(inImage, "RIFF", 4) != 0 || memcmp(inImage + 8, "WAVE", 4) != 0)
        return kNotRiff;
    const uint8_t* theFormat = nullptr;
    const uint8_t* theData = nullptr;
    uint32_t theDataBytes = 0;
    for (uint32_t theOffset = 12; theOffset + 8 <= inSize;) {
        uint32_t theChunk = Read32(inImage + theOffset + 4);
        uint32_t theAvailable = inSize - theOffset - 8;
        if (memcmp(inImage + theOffset, "fmt ", 4) == 0 && theChunk >= 16 && theAvailable >= 16)
            theFormat = inImage + theOffset + 8;
        else if (memcmp(inImage + theOffset, "data", 4) == 0 && !theData) {
            theData = inImage + theOffset + 8;
            theDataBytes = std::min(theChunk, theAvailable);
        }
        if (theChunk > theAvailable)
            break;
        theOffset += 8 + theChunk + (theChunk & 1);
    }
    if (!theFormat || !theData)
        return kRiffUnsupported;
    int theChannels = Read16(theFormat + 2), theBits = Read16(theFormat + 14);
    if (Read16(theFormat) != WAVE_FORMAT_PCM || theChannels < 1 || theChannels > 2 || (theBits != 8 && theBits != 16))
        return kRiffUnsupported;
    ioSample.mData = theData;
    ioSample.mChannels = theChannels;
    ioSample.mBits = theBits;
    ioSample.mBytes = theDataBytes - theDataBytes % FrameBytes(ioSample);
    ioSample.mFileRate = Read32(theFormat + 4);
    return kRiffPCM;
}

bool Receive(AVCodecContext* inContext, AVFrame* ioFrame, SwrContext** ioResampler, Decoded& ioDecoded)
{
    while (avcodec_receive_frame(inContext, ioFrame) == 0) {
        if (!*ioResampler) {
            int theChannels = ioFrame->ch_layout.nb_channels;
            if (theChannels < 1 || theChannels > 2 || ioFrame->sample_rate <= 0)
                return false;
            ioDecoded.mChannels = theChannels;
            ioDecoded.mRate = ioFrame->sample_rate;
            if (swr_alloc_set_opts2(ioResampler, &ioFrame->ch_layout, AV_SAMPLE_FMT_S16, ioFrame->sample_rate,
                                    &ioFrame->ch_layout, AVSampleFormat(ioFrame->format), ioFrame->sample_rate, 0,
                                    nullptr) < 0 || swr_init(*ioResampler) < 0)
                return false;
        }
        if (ioFrame->sample_rate != ioDecoded.mRate || ioFrame->ch_layout.nb_channels != ioDecoded.mChannels)
            return false;
        size_t theStart = ioDecoded.mPCM.size();
        int theRoom = swr_get_out_samples(*ioResampler, ioFrame->nb_samples);
        ioDecoded.mPCM.resize(theStart + size_t(theRoom) * ioDecoded.mChannels);
        uint8_t* theOut = reinterpret_cast<uint8_t*>(ioDecoded.mPCM.data() + theStart);
        int theCount = swr_convert(*ioResampler, &theOut, theRoom, const_cast<const uint8_t**>(ioFrame->extended_data),
                                   ioFrame->nb_samples);
        if (theCount < 0)
            return false;
        ioDecoded.mPCM.resize(theStart + size_t(theCount) * ioDecoded.mChannels);
    }
    return true;
}

// MPEG audio frames, after an ID3v2 tag if there is one, decoded whole
std::shared_ptr<Decoded> DecodeMPEG(const uint8_t* inImage, uint32_t inSize)
{
    uint32_t theSkip = 0;
    if (inSize >= 10 && memcmp(inImage, "ID3", 3) == 0)
        theSkip = std::min<uint32_t>(inSize, 10 + ((inImage[6] & 0x7f) << 21 | (inImage[7] & 0x7f) << 14 |
                                                   (inImage[8] & 0x7f) << 7 | (inImage[9] & 0x7f)));
    std::vector<uint8_t> theInput(inImage + theSkip, inImage + inSize);
    size_t theSize = theInput.size();
    theInput.resize(theSize + AV_INPUT_BUFFER_PADDING_SIZE);

    auto theDecoded = std::make_shared<Decoded>();
    const AVCodec* theCodec = avcodec_find_decoder(AV_CODEC_ID_MP3);
    AVCodecParserContext* theParser = theCodec ? av_parser_init(theCodec->id) : nullptr;
    AVCodecContext* theContext = theCodec ? avcodec_alloc_context3(theCodec) : nullptr;
    AVPacket* thePacket = av_packet_alloc();
    AVFrame* theFrame = av_frame_alloc();
    SwrContext* theResampler = nullptr;
    bool theGood = theParser && theContext && thePacket && theFrame && avcodec_open2(theContext, theCodec, nullptr) == 0;
    const uint8_t* theCursor = theInput.data();
    size_t theLeft = theSize;
    while (theGood) {
        int theUsed = av_parser_parse2(theParser, theContext, &thePacket->data, &thePacket->size, theCursor,
                                       int(theLeft), AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
        if (theUsed < 0)
            break;
        theCursor += theUsed;
        theLeft -= size_t(theUsed);
        if (thePacket->size) {
            // a frame the decoder refuses is skipped, as a player does with a damaged frame
            if (avcodec_send_packet(theContext, thePacket) == 0)
                theGood = Receive(theContext, theFrame, &theResampler, *theDecoded);
        } else if (!theLeft) {
            break;
        }
        if (theDecoded->mPCM.size() * 2 > kDecodedBudget)
            theGood = false;
    }
    if (theGood && avcodec_send_packet(theContext, nullptr) == 0)
        theGood = Receive(theContext, theFrame, &theResampler, *theDecoded);
    swr_free(&theResampler);
    av_frame_free(&theFrame);
    av_packet_free(&thePacket);
    avcodec_free_context(&theContext);
    av_parser_close(theParser);
    if (!theGood || theDecoded->mPCM.empty())
        return nullptr;
    return theDecoded;
}

// The decoded PCM of an image, from the cache when the same bytes were decoded before. The cache keeps at most
// kDecodedBudget bytes of PCM that no sample plays.
std::shared_ptr<const Decoded> Decode(const uint8_t* inImage, uint32_t inSize)
{
    uint64_t theHash = Hash(inImage, inSize);
    {
        std::lock_guard<std::mutex> theGuard(gDecodeLock);
        for (CacheEntry& theEntry : gDecodeCache)
            if (theEntry.mHash == theHash && theEntry.mBytes == inSize) {
                theEntry.mUsed = ++gDecodeClock;
                return theEntry.mDecoded;
            }
    }
    std::shared_ptr<const Decoded> theDecoded = DecodeMPEG(inImage, inSize);
    if (!theDecoded)
        return nullptr;
    std::lock_guard<std::mutex> theGuard(gDecodeLock);
    gDecodeCache.push_back({theHash, inSize, ++gDecodeClock, theDecoded});
    size_t theTotal = 0;
    for (const CacheEntry& theEntry : gDecodeCache)
        theTotal += theEntry.mDecoded->mPCM.size() * 2;
    while (theTotal > kDecodedBudget) {
        auto theVictim = gDecodeCache.end();
        for (auto it = gDecodeCache.begin(); it != gDecodeCache.end(); ++it)
            if (it->mDecoded.use_count() == 1 && (theVictim == gDecodeCache.end() || it->mUsed < theVictim->mUsed))
                theVictim = it;
        if (theVictim == gDecodeCache.end())
            break;
        theTotal -= theVictim->mDecoded->mPCM.size() * 2;
        gDecodeCache.erase(theVictim);
    }
    return theDecoded;
}

// The DirectSound object that AIL_get_DirectSound_info gives. The mixer reads its capabilities and hands it to
// DirectMusic; the layer's own mixer plays every sound, so it makes no buffers.
class DirectSound final : public IDirectSound {
public:
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE QueryInterface(REFIID inID, void** outObject) override
    {
        if (!outObject)
            return E_POINTER;
        if (!IsEqualGUID(inID, IID_IUnknown) && !IsEqualGUID(inID, IID_IDirectSound)) {
            *outObject = nullptr;
            return E_NOINTERFACE;
        }
        AddRef();
        *outObject = this;
        return S_OK;
    }
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE AddRef() override { return ULONG(++mReferences); }
    // The object lives as long as the program: DirectMusic may keep it after the driver closes
    COM_DECLSPEC_NOTHROW ULONG STDMETHODCALLTYPE Release() override { return ULONG(std::max<long>(--mReferences, 1)); }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE CreateSoundBuffer(LPCDSBUFFERDESC, LPLPDIRECTSOUNDBUFFER outBuffer, IUnknown*) override
    {
        EE_PORT_UNSUPPORTED("IDirectSound::CreateSoundBuffer");
        if (outBuffer)
            *outBuffer = nullptr;
        return DSERR_UNSUPPORTED;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetCaps(LPDSCAPS outCaps) override
    {
        if (!outCaps || outCaps->dwSize < sizeof(DSCAPS))
            return DSERR_INVALIDPARAM;
        memset(outCaps, 0, sizeof(DSCAPS));
        outCaps->dwSize = sizeof(DSCAPS);
        outCaps->dwFlags = DSCAPS_PRIMARYMONO | DSCAPS_PRIMARYSTEREO | DSCAPS_PRIMARY8BIT | DSCAPS_PRIMARY16BIT |
                           DSCAPS_CONTINUOUSRATE | DSCAPS_SECONDARYMONO | DSCAPS_SECONDARYSTEREO |
                           DSCAPS_SECONDARY8BIT | DSCAPS_SECONDARY16BIT;
        outCaps->dwMinSecondarySampleRate = DSBFREQUENCY_MIN;
        outCaps->dwMaxSecondarySampleRate = DSBFREQUENCY_MAX;
        outCaps->dwPrimaryBuffers = 1;
        // no hardware mixing: the mixer then uses 16 channels
        return DS_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE DuplicateSoundBuffer(LPDIRECTSOUNDBUFFER, LPLPDIRECTSOUNDBUFFER outBuffer) override
    {
        EE_PORT_UNSUPPORTED("IDirectSound::DuplicateSoundBuffer");
        if (outBuffer)
            *outBuffer = nullptr;
        return DSERR_UNSUPPORTED;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetCooperativeLevel(HWND, DWORD) override { return DS_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Compact() override { return DS_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE GetSpeakerConfig(LPDWORD outConfig) override
    {
        if (!outConfig)
            return DSERR_INVALIDPARAM;
        *outConfig = DSSPEAKER_STEREO;
        return DS_OK;
    }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE SetSpeakerConfig(DWORD) override { return DS_OK; }
    COM_DECLSPEC_NOTHROW HRESULT STDMETHODCALLTYPE Initialize(LPCGUID) override { return DSERR_ALREADYINITIALIZED; }

private:
    std::atomic<long> mReferences{1};
};

DirectSound gDirectSound;

void CloseDriver(Driver* inDriver, std::unique_lock<std::mutex>& ioLock)
{
    gDrivers.erase(inDriver);
    if (inDriver->mDevice) {
        // the callback takes gLock, and closing waits for it to return
        ioLock.unlock();
        SDL_CloseAudioDevice(inDriver->mDevice);
        ioLock.lock();
    }
    for (Sample* theSample : inDriver->mSamples) {
        gSamples.erase(theSample);
        delete theSample;
    }
    delete inDriver;
}

} // namespace

void eeport::MilesWithoutDevice()
{
    std::lock_guard<std::mutex> theGuard(gLock);
    gWithoutDevice = true;
}

void eeport::MixMiles(void* inDriver, int16_t* outSamples, int inFrames)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Driver* theDriver = DriverOf(static_cast<HDIGDRIVER>(inDriver)))
        Mix(*theDriver, outSamples, inFrames);
}

extern "C" {

// The parentheses keep mss.h's AIL_startup macro from expanding
S32 AILCALL (AIL_startup)(void)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    gPreferencesSet = false;
    Preferences();
    return 1;
}

void AILCALL AIL_shutdown(void)
{
    std::unique_lock<std::mutex> theLock(gLock);
    while (!gDrivers.empty())
        CloseDriver(*gDrivers.begin(), theLock);
}

// Miles keeps a copy that ends in a backslash
char* AILCALL AIL_set_redist_directory(const char* inDirectory)
{
    std::string theDirectory = inDirectory ? inDirectory : "";
    if (theDirectory.size() > MAX_PATH)
        theDirectory.resize(MAX_PATH);
    if (!theDirectory.empty() && theDirectory.back() != '\\' && theDirectory.back() != ':')
        theDirectory += '\\';
    std::lock_guard<std::mutex> theGuard(gLock);
    snprintf(gRedistDirectory, sizeof gRedistDirectory, "%s", theDirectory.c_str());
    return gRedistDirectory;
}

char* AILCALL AIL_last_error(void)
{
    return gLastError;
}

// Returns the value the preference had
S32 AILCALL AIL_set_preference(U32 inNumber, S32 inValue)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (inNumber >= 64)
        return 0;
    S32 thePrevious = Preferences()[inNumber];
    Preferences()[inNumber] = inValue;
    return thePrevious;
}

S32 AILCALL AIL_get_preference(U32 inNumber)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    return inNumber < 64 ? Preferences()[inNumber] : 0;
}

// The SDL callback mixes; there is no Miles timer to serve or to be late
void AILCALL AIL_serve(void) {}

S32 AILCALL AIL_get_timer_highest_delay(void)
{
    return 0;
}

// Miles allocates zeroed memory
void* AILCALL AIL_mem_alloc_lock(U32 inSize)
{
    void* theMemory = calloc(1, std::max<U32>(inSize, 1));
    if (!theMemory) {
        std::lock_guard<std::mutex> theGuard(gLock);
        SetError("Out of memory.");
    }
    return theMemory;
}

void AILCALL AIL_mem_free_lock(void* inMemory)
{
    free(inMemory);
}

// The whole file, into inDestination or, when that is NULL (or Miles' FILE_READ_WITH_SIZE, -1), into memory from
// AIL_mem_alloc_lock
void* AILCALL AIL_file_read(const char* inName, void* inDestination)
{
    int theFile = inName ? open(eeport::LinuxPath(inName).c_str(), O_RDONLY | O_CLOEXEC) : -1;
    struct stat theStat;
    if (theFile < 0 || fstat(theFile, &theStat) != 0 || !S_ISREG(theStat.st_mode) || theStat.st_size <= 0 ||
        theStat.st_size > INT32_MAX) {
        if (theFile >= 0)
            close(theFile);
        std::lock_guard<std::mutex> theGuard(gLock);
        SetError("Unable to open file.");
        return nullptr;
    }
    size_t theSize = size_t(theStat.st_size);
    bool theAllocate = !inDestination || inDestination == reinterpret_cast<void*>(-1);
    uint8_t* theOut = static_cast<uint8_t*>(theAllocate ? AIL_mem_alloc_lock(U32(theSize)) : inDestination);
    size_t theRead = 0;
    while (theOut && theRead < theSize) {
        ssize_t theCount = read(theFile, theOut + theRead, theSize - theRead);
        if (theCount < 0 && errno == EINTR)
            continue;
        if (theCount <= 0)
            break;
        theRead += size_t(theCount);
    }
    close(theFile);
    if (!theOut || theRead != theSize) {
        if (theAllocate)
            free(theOut);
        std::lock_guard<std::mutex> theGuard(gLock);
        SetError(theOut ? "Unable to read file." : "Out of memory.");
        return nullptr;
    }
    return theOut;
}

// The size of a file, or -1
S32 AILCALL AIL_file_size(const char* inName)
{
    struct stat theStat;
    if (!inName || stat(eeport::LinuxPath(inName).c_str(), &theStat) != 0 || !S_ISREG(theStat.st_mode) ||
        theStat.st_size > INT32_MAX) {
        std::lock_guard<std::mutex> theGuard(gLock);
        SetError("Unable to open file.");
        return -1;
    }
    return S32(theStat.st_size);
}

// Miles: a rate of 0 is 22050, any width but 8 bits is 16, any channel count but 1 is 2. Without the flag 1
// (DIG_USE_WAVEOUT's), DIG_USE_WAVEOUT goes back to 0.
HDIGDRIVER AILCALL AIL_open_digital_driver(U32 inRate, S32 inBits, S32 inChannels, U32 inFlags)
{
    Driver* theDriver = new Driver;
    theDriver->mRate = inRate ? inRate : 22050;
    theDriver->mBits = inBits != 8 ? 16 : 8;
    theDriver->mChannels = inChannels != 1 ? 2 : 1;
    theDriver->mDeviceRate = int(theDriver->mRate);
    bool theWithoutDevice;
    {
        std::lock_guard<std::mutex> theGuard(gLock);
        if (!(inFlags & 1))
            Preferences()[DIG_USE_WAVEOUT] = 0;
        theWithoutDevice = gWithoutDevice;
    }
    if (!theWithoutDevice)
        OpenDevice(*theDriver);
    std::lock_guard<std::mutex> theGuard(gLock);
    gDrivers.insert(theDriver);
    return reinterpret_cast<HDIGDRIVER>(theDriver);
}

void AILCALL AIL_close_digital_driver(HDIGDRIVER inDriver)
{
    std::unique_lock<std::mutex> theLock(gLock);
    if (Driver* theDriver = DriverOf(inDriver))
        CloseDriver(theDriver, theLock);
}

S32 AILCALL AIL_set_DirectSound_HWND(HDIGDRIVER, HWND)
{
    return 0;
}

// The DirectSound object, for the mixer and DirectMusic. No sample has a DirectSound buffer.
void AILCALL AIL_get_DirectSound_info(HSAMPLE, AILLPDIRECTSOUND* outDirectSound, AILLPDIRECTSOUNDBUFFER* outBuffer)
{
    if (outDirectSound)
        *outDirectSound = static_cast<IDirectSound*>(&gDirectSound);
    if (outBuffer)
        *outBuffer = nullptr;
}

S32 AILCALL AIL_digital_CPU_percent(HDIGDRIVER)
{
    return 0;
}

void AILCALL AIL_set_digital_master_volume(HDIGDRIVER inDriver, S32 inVolume)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Driver* theDriver = DriverOf(inDriver))
        theDriver->mMaster = std::min<S32>(std::max<S32>(inVolume, 0), 127);
}

HSAMPLE AILCALL AIL_allocate_sample_handle(HDIGDRIVER inDriver)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Driver* theDriver = DriverOf(inDriver);
    if (!theDriver) {
        SetError("Invalid driver.");
        return nullptr;
    }
    Sample* theSample = new Sample(theDriver);
    Initialize(*theSample);
    theDriver->mSamples.push_back(theSample);
    gSamples.insert(theSample);
    return reinterpret_cast<HSAMPLE>(theSample);
}

void AILCALL AIL_release_sample_handle(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    if (!theSample)
        return;
    std::vector<Sample*>& theSamples = theSample->mDriver->mSamples;
    theSamples.erase(std::find(theSamples.begin(), theSamples.end(), theSample));
    gSamples.erase(theSample);
    delete theSample;
}

void AILCALL AIL_init_sample(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Sample* theSample = SampleOf(inSample))
        Initialize(*theSample);
}

// Miles chooses a decoder by the name's extension. This reads the image instead: RIFF PCM plays from the image, which
// the game keeps while the sample plays; anything else is decoded as MPEG audio. Returns 1, or 0 for an image it
// cannot play.
S32 AILCALL AIL_set_named_sample_file(HSAMPLE inSample, const char*, const void* inImage, S32 inSize, S32)
{
    const uint8_t* theImage = static_cast<const uint8_t*>(inImage);
    uint32_t theSize = inSize > 0 ? uint32_t(inSize) : 0;
    {
        std::lock_guard<std::mutex> theGuard(gLock);
        Sample* theSample = SampleOf(inSample);
        if (!theSample)
            return 0;
        Unload(*theSample);
        if (!theImage || !theSize) {
            SetError("Invalid file image.");
            return 0;
        }
        switch (ParseRiff(*theSample, theImage, theSize)) {
        case kRiffPCM:
            theSample->mRate = theSample->mFileRate;
            return 1;
        case kRiffUnsupported:
            Unload(*theSample);
            SetError("Unsupported wave file format.");
            return 0;
        case kNotRiff:
            break;
        }
    }
    // decoding takes long: the audio callback keeps mixing meanwhile
    std::shared_ptr<const Decoded> theDecoded = Decode(theImage, theSize);
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    if (!theSample)
        return 0;
    if (!theDecoded) {
        SetError("Unknown file type.");
        return 0;
    }
    Unload(*theSample);
    theSample->mDecoded = theDecoded;
    theSample->mData = reinterpret_cast<const uint8_t*>(theDecoded->mPCM.data());
    theSample->mBytes = uint32_t(theDecoded->mPCM.size() * 2);
    theSample->mChannels = theDecoded->mChannels;
    theSample->mBits = 16;
    theSample->mFileRate = U32(theDecoded->mRate);
    theSample->mEncodedBytes = theSize;
    theSample->mRate = theSample->mFileRate;
    return 1;
}

// Miles: a start always begins at the top
void AILCALL AIL_start_sample(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    if (theSample && FrameCount(*theSample))
        Play(*theSample, 0);
}

void AILCALL AIL_end_sample(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Sample* theSample = SampleOf(inSample))
        if (theSample->mPlaying)
            Stop(*theSample);
}

U32 AILCALL AIL_sample_status(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    return !theSample ? kFree : theSample->mPlaying ? kPlaying : kDone;
}

S32 AILCALL AIL_sample_granularity(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    return theSample ? S32(Granularity(*theSample)) : 0;
}

S32 AILCALL AIL_sample_playback_rate(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    return theSample ? S32(theSample->mRate) : 0;
}

// Bytes into the sample's data: PCM bytes, or the image's bytes for a decoded sample
U32 AILCALL AIL_sample_position(HSAMPLE inSample)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    return theSample ? ToMiles(*theSample, Cursor(*theSample)) : 0;
}

// Miles: the length and position in milliseconds, at the data's byte rate: a PCM frame per sample at the playback
// rate, or the image's bytes spread over the decoded length
void AILCALL AIL_sample_ms_position(HSAMPLE inSample, S32* outTotal, S32* outCurrent)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    if (!theSample)
        return;
    double thePerSecond;
    uint32_t theLength = theSample->mBytes, theAt = Cursor(*theSample);
    if (theSample->mDecoded) {
        double theSeconds = theSample->mFileRate ? double(FrameCount(*theSample)) / theSample->mFileRate : 0.0;
        thePerSecond = theSeconds > 0 ? theSample->mEncodedBytes / theSeconds : 0.0;
        theLength = theSample->mEncodedBytes;
        theAt = ToMiles(*theSample, theAt);
    } else {
        thePerSecond = double(FrameBytes(*theSample)) * theSample->mRate;
    }
    auto theMilliseconds = [thePerSecond](uint32_t inBytes) {
        return thePerSecond > 0 ? S32(std::nearbyint(inBytes * 1000.0 / thePerSecond)) : 0;
    };
    if (outTotal)
        *outTotal = theMilliseconds(theLength);
    if (outCurrent)
        *outCurrent = theMilliseconds(theAt);
}

// Miles rounds to the nearest whole granule. A playing sample moves there at once, and at or past the end that is
// the end of a pass; a sample that is not playing keeps it, though its next start begins at the top.
void AILCALL AIL_set_sample_position(HSAMPLE inSample, U32 inPosition)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    Sample* theSample = SampleOf(inSample);
    if (!theSample)
        return;
    uint64_t theGranule = Granularity(*theSample);
    uint64_t theRounded = (uint64_t(inPosition) + theGranule / 2) / theGranule * theGranule;
    uint32_t thePCM = FromMiles(*theSample, uint32_t(std::min<uint64_t>(theRounded, UINT32_MAX)));
    if (!theSample->mPlaying)
        theSample->mPosition = thePCM;
    else if (thePCM < theSample->mBytes)
        Play(*theSample, thePCM);
    else if (PassEnded(*theSample))
        Play(*theSample, 0);
}

void AILCALL AIL_set_sample_loop_count(HSAMPLE inSample, S32 inCount)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Sample* theSample = SampleOf(inSample))
        theSample->mLoops = U32(inCount);
}

void AILCALL AIL_set_sample_pan(HSAMPLE inSample, S32 inPan)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Sample* theSample = SampleOf(inSample))
        theSample->mPan = std::min<S32>(std::max<S32>(inPan, 0), 127);
}

void AILCALL AIL_set_sample_volume(HSAMPLE inSample, S32 inVolume)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Sample* theSample = SampleOf(inSample))
        theSample->mVolume = std::min<S32>(std::max<S32>(inVolume, 0), 127);
}

// A rate set while the sample plays retunes it from where it is
void AILCALL AIL_set_sample_playback_rate(HSAMPLE inSample, S32 inRate)
{
    std::lock_guard<std::mutex> theGuard(gLock);
    if (Sample* theSample = SampleOf(inSample))
        theSample->mRate = U32(std::max<S32>(inRate, 0));
}

} // extern "C"
