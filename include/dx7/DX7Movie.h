// DX7Movie (RECONSTRUCTION): a GEMovie playing a Bink file on its own thread into a system-memory frame buffer, which
// RenderFrame copies to the screen's back buffer. Member names from DX7Movie.cpp's assert texts where they appear
// (mFrameAccess, mThread, mFrameReadyEvent, mMovieStopped, mPlayBinkParameter); layout from the ctors and the copy
// ctor 0x100020d0: sizeof 0x60. The flags the playback thread writes are volatile (`mov byte [x], 0` stores).
#ifndef DX7_DX7MOVIE_H
#define DX7_DX7MOVIE_H

#include "dx7/DX7Common.h"
#include "lle/GERectangle.h"
#include "lle/GEMovie.h"

struct BINK;
struct IDirectSound;
class TSCriticalSection;
class TSEvent;
class TSThread;
class GESurface;

class DX7_API DX7Movie : public GEMovie {
public:
    // What the playback thread gets (PlayBink's parameter): the movie and whether it loops
    struct PlayBinkParameter {
        DX7Movie* mMovie;
        bool mIsLooping;
    };

    DX7Movie(const FSFileSpec& inFile, unsigned long inScreen);
    DX7Movie(const FSFileSpec& inFile, unsigned long inScreen, GERectangle inRectangle, bool inLoop);

    virtual UString GetFilename() const { return this->mFilename; }
    virtual bool GetIsLooping() const;
    virtual bool GetIsPlaying() const { return this->mIsPlaying; }
    virtual bool GetIsReady() const { return this->mIsReady; }
    virtual bool GetJustStarted() const { return this->mJustStarted; }
    virtual long RenderFrame();
    virtual void Stop();

private:
    virtual ~DX7Movie();
    long CreateFrameBuffer(BINK* inBink);
    void Initialize(const FSFileSpec& inFile, unsigned long inScreen, bool inLoop);
    static unsigned long PlayBink(void* inParameter);
    bool ShouldStop();

    PlayBinkParameter mPlayBinkParameter;   // +0x08
    TSCriticalSection* mFrameAccess;        // +0x10
    TSThread* mThread;                      // +0x14
    TSEvent* mFrameReadyEvent;              // +0x18
    TSEvent* mMovieStopped;                 // +0x1c
    GESurface* mFrameBuffer;                // +0x20
    GERectangle mSourceRectangle;           // +0x24
    GERectangle mDestinationRectangle;      // +0x34
    UString mFilename;                      // +0x44
    unsigned long mScreen;                  // +0x54
    volatile bool mIsPlaying;               // +0x58
    volatile bool mStopRequested;           // +0x59
    volatile bool mIsFinished;              // +0x5a
    volatile bool mIsReady;                 // +0x5b
    volatile bool mJustStarted;             // +0x5c
    bool mHasRectangle;                     // +0x5d

    static TSCriticalSection* sMovieCriticalSection;
    static IDirectSound* sDirectSoundInterface;
};

#endif
