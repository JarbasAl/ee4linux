// The Bink API (binkw32.dll) that the DX7 display plug-ins call, on FFmpeg. It plays the game's Bink 1 files and,
// in their place, a replacement movie of the same name in a modern format (an upscaled intro, for example). The
// replacement may have any size: BinkOpen reports the file's own size, the plug-in makes its frame buffer that size,
// and its RenderFrame stretches the frame onto the back buffer.
//
// A movie Movies\Name.bik is looked for, in this order, as Name with each extension of kReplacements in the folder of
// EE_PORT_MOVIES, then in the movie's own folder, and last as the .bik file itself.
//
// Sound goes to its own SDL audio device; BinkSetSoundSystem and BinkOpenDirectSound only satisfy the plug-in.
#include "Win32Internal.h"

#include <ddraw.h>
#include <bink/bink.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
#include <libswscale/swscale.h>
}
#include <SDL.h>

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <string>
#include <unistd.h>

namespace {

const char* const kReplacements[] = {".webm", ".mkv", ".mp4", ".mov", ".avi", ".mpg"};

// The surface types of BinkCopyToBuffer (RAD's BINKSURFACE* values) that the plug-in's surfaces can have
enum SurfaceType { kSurface24 = 1, kSurface32 = 3, kSurface32A = 5, kSurface555 = 9, kSurface565 = 10 };

struct Movie {
    BINK mPublic;                       // first: the plug-in reads Width, Height, Frames and FrameNum
    AVFormatContext* mFormat = nullptr;
    AVCodecContext* mVideo = nullptr;
    AVCodecContext* mAudio = nullptr;
    int mVideoStream = -1;
    int mAudioStream = -1;
    AVPacket* mPacket = nullptr;
    AVFrame* mFrame = nullptr;          // the frame BinkDoFrame decoded: frame FrameNum
    AVFrame* mNext = nullptr;           // the frame after it, decoded ahead to know whether FrameNum is the last
    AVFrame* mScratch = nullptr;
    bool mHasFrame = false;
    bool mHasNext = false;
    bool mPrimed = false;               // mNext holds the first frame, or the video has none
    double mFramesPerSecond = 0;
    SwsContext* mScaler = nullptr;
    int mScalerFormat = -1;
    SwrContext* mResampler = nullptr;
    SDL_AudioDeviceID mDevice = 0;
    std::chrono::steady_clock::time_point mStart;
    bool mStarted = false;
};

bool Exists(const std::string& inPath)
{
    return !inPath.empty() && access(inPath.c_str(), R_OK) == 0;
}

// The file to play for the plug-in's movie name
std::string MovieFile(const char* inName)
{
    std::string theName = inName;
    size_t theSlash = theName.find_last_of("\\/");
    size_t theDot = theName.rfind('.');
    if (theDot == std::string::npos || (theSlash != std::string::npos && theDot < theSlash))
        theDot = theName.size();
    std::string theStem = theName.substr(0, theDot);
    std::string theBase = theStem.substr(theSlash == std::string::npos ? 0 : theSlash + 1);
    if (const char* theFolder = getenv("EE_PORT_MOVIES")) {
        for (const char* theExtension : kReplacements) {
            std::string thePath = eeport::LinuxPath((std::string(theFolder) + "/" + theBase + theExtension).c_str());
            if (Exists(thePath))
                return thePath;
        }
    }
    for (const char* theExtension : kReplacements) {
        std::string thePath = eeport::LinuxPath((theStem + theExtension).c_str());
        if (Exists(thePath))
            return thePath;
    }
    std::string thePath = eeport::LinuxPath(inName);
    return Exists(thePath) ? thePath : std::string();
}

AVCodecContext* OpenDecoder(AVFormatContext* inFormat, int inStream)
{
    const AVCodecParameters* theParameters = inFormat->streams[inStream]->codecpar;
    const AVCodec* theCodec = avcodec_find_decoder(theParameters->codec_id);
    if (!theCodec)
        return nullptr;
    AVCodecContext* theContext = avcodec_alloc_context3(theCodec);
    theContext->thread_count = 0;
    if (avcodec_parameters_to_context(theContext, theParameters) < 0 || avcodec_open2(theContext, theCodec, nullptr) < 0)
        avcodec_free_context(&theContext);
    return theContext;
}

void OpenSound(Movie* inMovie)
{
    if (!inMovie->mAudio || SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
        return;
    SDL_AudioSpec theWanted{}, theHave{};
    theWanted.freq = inMovie->mAudio->sample_rate;
    theWanted.format = AUDIO_S16SYS;
    theWanted.channels = 2;
    theWanted.samples = 2048;
    inMovie->mDevice = SDL_OpenAudioDevice(nullptr, 0, &theWanted, &theHave, 0);
    if (!inMovie->mDevice) {
        eeport::Log("Bink: no sound device: %s", SDL_GetError());
        return;
    }
    AVChannelLayout theStereo = AV_CHANNEL_LAYOUT_STEREO;
    if (swr_alloc_set_opts2(&inMovie->mResampler, &theStereo, AV_SAMPLE_FMT_S16, theHave.freq,
                            &inMovie->mAudio->ch_layout, inMovie->mAudio->sample_fmt, inMovie->mAudio->sample_rate, 0,
                            nullptr) < 0 || swr_init(inMovie->mResampler) < 0) {
        SDL_CloseAudioDevice(inMovie->mDevice);
        inMovie->mDevice = 0;
    }
}

void QueueSound(Movie* inMovie)
{
    while (avcodec_receive_frame(inMovie->mAudio, inMovie->mScratch) == 0) {
        if (inMovie->mDevice) {
            int theCount = swr_get_out_samples(inMovie->mResampler, inMovie->mScratch->nb_samples);
            uint8_t* theSamples = nullptr;
            av_samples_alloc(&theSamples, nullptr, 2, theCount, AV_SAMPLE_FMT_S16, 0);
            theCount = swr_convert(inMovie->mResampler, &theSamples, theCount,
                                   (const uint8_t**)inMovie->mScratch->extended_data, inMovie->mScratch->nb_samples);
            if (theCount > 0)
                SDL_QueueAudio(inMovie->mDevice, theSamples, theCount * 4);
            av_freep(&theSamples);
        }
        av_frame_unref(inMovie->mScratch);
    }
}

// Decodes the next video frame into outFrame, passing sound packets on; false at the end of the video
bool DecodeVideo(Movie* inMovie, AVFrame* outFrame)
{
    for (;;) {
        int theResult = avcodec_receive_frame(inMovie->mVideo, outFrame);
        if (theResult == 0)
            return true;
        if (theResult != AVERROR(EAGAIN))
            return false;
        if (av_read_frame(inMovie->mFormat, inMovie->mPacket) < 0) {
            avcodec_send_packet(inMovie->mVideo, nullptr);  // drain the frames the decoder holds
            continue;
        }
        if (inMovie->mPacket->stream_index == inMovie->mVideoStream)
            avcodec_send_packet(inMovie->mVideo, inMovie->mPacket);
        else if (inMovie->mPacket->stream_index == inMovie->mAudioStream) {
            if (avcodec_send_packet(inMovie->mAudio, inMovie->mPacket) == 0)
                QueueSound(inMovie);
        }
        av_packet_unref(inMovie->mPacket);
    }
}

void Rewind(Movie* inMovie)
{
    av_seek_frame(inMovie->mFormat, -1, 0, AVSEEK_FLAG_BACKWARD);
    avcodec_flush_buffers(inMovie->mVideo);
    if (inMovie->mAudio)
        avcodec_flush_buffers(inMovie->mAudio);
    if (inMovie->mDevice) {
        SDL_PauseAudioDevice(inMovie->mDevice, 1);
        SDL_ClearQueuedAudio(inMovie->mDevice);
    }
    av_frame_unref(inMovie->mNext);
    inMovie->mHasNext = false;
    inMovie->mPrimed = false;
    inMovie->mStarted = false;
    inMovie->mPublic.FrameNum = 1;
}

AVPixelFormat PixelFormatOf(S32 inType, int* outBytes)
{
    switch (inType & 0xff) {
    case kSurface24:
        *outBytes = 3;
        return AV_PIX_FMT_BGR24;
    case kSurface32:
    case kSurface32A:
        *outBytes = 4;
        return AV_PIX_FMT_BGRA;
    case kSurface555:
        *outBytes = 2;
        return AV_PIX_FMT_RGB555LE;
    case kSurface565:
        *outBytes = 2;
        return AV_PIX_FMT_RGB565LE;
    default:
        *outBytes = 0;
        return AV_PIX_FMT_NONE;
    }
}

} // namespace

extern "C" {

HBINK __stdcall BinkOpen(const char* inName, U32)
{
    std::string thePath = MovieFile(inName);
    if (thePath.empty()) {
        eeport::Log("Bink: no movie for %s", inName);
        return NULL;
    }
    Movie* theMovie = new Movie;
    if (avformat_open_input(&theMovie->mFormat, thePath.c_str(), nullptr, nullptr) < 0 ||
        avformat_find_stream_info(theMovie->mFormat, nullptr) < 0) {
        eeport::Log("Bink: cannot read %s", thePath.c_str());
        avformat_close_input(&theMovie->mFormat);
        delete theMovie;
        return NULL;
    }
    theMovie->mVideoStream = av_find_best_stream(theMovie->mFormat, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    theMovie->mAudioStream = av_find_best_stream(theMovie->mFormat, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    if (theMovie->mVideoStream < 0 || !(theMovie->mVideo = OpenDecoder(theMovie->mFormat, theMovie->mVideoStream))) {
        eeport::Log("Bink: no video decoder for %s", thePath.c_str());
        BinkClose(&theMovie->mPublic);
        return NULL;
    }
    if (theMovie->mAudioStream >= 0)
        theMovie->mAudio = OpenDecoder(theMovie->mFormat, theMovie->mAudioStream);

    AVStream* theStream = theMovie->mFormat->streams[theMovie->mVideoStream];
    AVRational theRate = av_guess_frame_rate(theMovie->mFormat, theStream, nullptr);
    theMovie->mFramesPerSecond = theRate.num > 0 && theRate.den > 0 ? av_q2d(theRate) : 15.0;
    long long theFrames = theStream->nb_frames;
    if (theFrames <= 0) {
        double theSeconds = theStream->duration != AV_NOPTS_VALUE ? theStream->duration * av_q2d(theStream->time_base)
                                                                  : theMovie->mFormat->duration / (double)AV_TIME_BASE;
        theFrames = std::llround(theSeconds * theMovie->mFramesPerSecond);
    }
    theMovie->mPublic.Width = theMovie->mVideo->width;
    theMovie->mPublic.Height = theMovie->mVideo->height;
    theMovie->mPublic.Frames = theFrames > 0 ? (U32)theFrames : 1;
    theMovie->mPublic.FrameNum = 1;
    theMovie->mPacket = av_packet_alloc();
    theMovie->mFrame = av_frame_alloc();
    theMovie->mNext = av_frame_alloc();
    theMovie->mScratch = av_frame_alloc();
    OpenSound(theMovie);
    eeport::Log("Bink: %s plays %s, %lux%lu, %lu frames at %.3f per second", inName, thePath.c_str(),
                theMovie->mPublic.Width, theMovie->mPublic.Height, theMovie->mPublic.Frames,
                theMovie->mFramesPerSecond);
    return &theMovie->mPublic;
}

void __stdcall BinkClose(HBINK inBink)
{
    if (!inBink)
        return;
    Movie* theMovie = reinterpret_cast<Movie*>(inBink);
    if (theMovie->mDevice)
        SDL_CloseAudioDevice(theMovie->mDevice);
    swr_free(&theMovie->mResampler);
    sws_freeContext(theMovie->mScaler);
    av_frame_free(&theMovie->mFrame);
    av_frame_free(&theMovie->mNext);
    av_frame_free(&theMovie->mScratch);
    av_packet_free(&theMovie->mPacket);
    avcodec_free_context(&theMovie->mVideo);
    avcodec_free_context(&theMovie->mAudio);
    avformat_close_input(&theMovie->mFormat);
    delete theMovie;
}

// Nonzero while it is too early to show frame FrameNum; the clock starts with the first frame
S32 __stdcall BinkWait(HBINK inBink)
{
    Movie* theMovie = reinterpret_cast<Movie*>(inBink);
    if (!theMovie->mStarted)
        return 0;
    double theElapsed = std::chrono::duration<double>(std::chrono::steady_clock::now() - theMovie->mStart).count();
    return theElapsed < (inBink->FrameNum - 1) / theMovie->mFramesPerSecond;
}

// Decodes frame FrameNum. Frames follows the frame decoded ahead: it becomes FrameNum when FrameNum is the last
// frame, and grows past FrameNum while frames remain, so the plug-in sees the end on the last frame even when the
// file's frame count was an estimate. Past the end the last frame stays.
S32 __stdcall BinkDoFrame(HBINK inBink)
{
    Movie* theMovie = reinterpret_cast<Movie*>(inBink);
    if (!theMovie->mStarted) {
        theMovie->mStarted = true;
        theMovie->mStart = std::chrono::steady_clock::now();
        if (theMovie->mDevice)
            SDL_PauseAudioDevice(theMovie->mDevice, 0);
    }
    if (!theMovie->mPrimed) {
        theMovie->mPrimed = true;
        theMovie->mHasNext = DecodeVideo(theMovie, theMovie->mNext);
    }
    if (theMovie->mHasNext) {
        av_frame_unref(theMovie->mFrame);
        av_frame_move_ref(theMovie->mFrame, theMovie->mNext);
        theMovie->mHasFrame = true;
        theMovie->mHasNext = DecodeVideo(theMovie, theMovie->mNext);
    }
    if (!theMovie->mHasNext)
        inBink->Frames = inBink->FrameNum;
    else if (inBink->FrameNum >= inBink->Frames)
        inBink->Frames = inBink->FrameNum + 1;
    return 0;
}

// Moves to the next frame; after the last one, back to the first (the plug-in loops a movie this way)
void __stdcall BinkNextFrame(HBINK inBink)
{
    if (inBink->FrameNum >= inBink->Frames)
        Rewind(reinterpret_cast<Movie*>(inBink));
    else
        ++inBink->FrameNum;
}

// Converts the decoded frame into a surface's memory of the given type, at its full size
S32 __stdcall BinkCopyToBuffer(HBINK inBink, void* outDestination, S32 inPitch, U32 inHeight, U32 inX, U32 inY,
                               U32 inFlags)
{
    Movie* theMovie = reinterpret_cast<Movie*>(inBink);
    int theBytes = 0;
    AVPixelFormat theFormat = PixelFormatOf(inFlags, &theBytes);
    if (!theMovie->mHasFrame || theFormat == AV_PIX_FMT_NONE || !outDestination)
        return 0;
    if (!theMovie->mScaler || theMovie->mScalerFormat != (int)theFormat) {
        sws_freeContext(theMovie->mScaler);
        theMovie->mScaler = sws_getContext(theMovie->mFrame->width, theMovie->mFrame->height,
                                           (AVPixelFormat)theMovie->mFrame->format, theMovie->mFrame->width,
                                           theMovie->mFrame->height, theFormat, SWS_BILINEAR, nullptr, nullptr,
                                           nullptr);
        theMovie->mScalerFormat = theFormat;
    }
    if (!theMovie->mScaler)
        return 0;
    U32 theRows = inHeight < (U32)theMovie->mFrame->height ? inHeight : (U32)theMovie->mFrame->height;
    uint8_t* theTarget[4] = {(uint8_t*)outDestination + inY * inPitch + inX * theBytes};
    int thePitch[4] = {inPitch};
    sws_scale(theMovie->mScaler, theMovie->mFrame->data, theMovie->mFrame->linesize, 0, (int)theRows, theTarget,
              thePitch);
    return 1;
}

// The BINKSURFACE type of a DirectDraw surface, from its pixel format
S32 __stdcall BinkDDSurfaceType(void* inSurface)
{
    IDirectDrawSurface7* theSurface = static_cast<IDirectDrawSurface7*>(inSurface);
    DDPIXELFORMAT theFormat{};
    theFormat.dwSize = sizeof theFormat;
    if (!theSurface || FAILED(theSurface->GetPixelFormat(&theFormat)) || !(theFormat.dwFlags & DDPF_RGB))
        return -1;
    switch (theFormat.dwRGBBitCount) {
    case 32:
        return theFormat.dwRGBAlphaBitMask ? kSurface32A : kSurface32;
    case 24:
        return kSurface24;
    case 16:
        return theFormat.dwGBitMask == 0x07e0 ? kSurface565 : kSurface555;
    default:
        return -1;
    }
}

S32 __stdcall BinkSetSoundSystem(BINKSNDSYSOPEN, U32)
{
    return 1;
}

BINKSNDOPEN __stdcall BinkOpenDirectSound(U32)
{
    return nullptr;
}

} // extern "C"
