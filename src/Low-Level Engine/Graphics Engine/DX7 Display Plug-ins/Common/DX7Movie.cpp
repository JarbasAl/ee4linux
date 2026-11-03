// C:\EEX\Low-Level Engine\Graphics Engine\DX7 Display Plug-ins\Common\DX7Movie.cpp
// Unit flags (hypothesis, docs/CALIBRATION.md "dx7hr"): -O1 -G6 -MD -GR
//
// 0x10001f51..0x1000295d: the U2D key init chains, DX7Movie's header-inline members and implicit copy members, then
// the members (the two ctors first, the rest alphabetical), then the template-static init chain head and atexit
// registrars. A Bink file plays on its own thread (PlayBink) into a system-memory frame buffer; RenderFrame copies the
// latest frame to the screen's back buffer. The ESVERIFYs record __LINE__ 183..186.
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "dx7/DX7Rasterizer.h"
#include "dx7/DX7Surface.h"
#include "dx7/DX7Movie.h"
#include "bink/bink.h"

// The U2D key init chains, then DX7Movie's header-inline members (GetFilename, GetIsPlaying, GetIsReady,
// GetJustStarted), its implicit copy ctor and operator= (dllexport class) and the vector deleting dtor:
// SYNTHETIC: DX7HR 0x10001f51
// SYNTHETIC: DX7HR 0x10001f5b
// SYNTHETIC: DX7HR 0x10001f6c
// SYNTHETIC: DX7HR 0x10001f78
// SYNTHETIC: DX7HR 0x10001f83
// SYNTHETIC: DX7HR 0x10001f8d
// SYNTHETIC: DX7HR 0x10001f9e
// SYNTHETIC: DX7HR 0x10001faa
// SYNTHETIC: DX7HR 0x10001fb5
// SYNTHETIC: DX7HR 0x10001fbf
// SYNTHETIC: DX7HR 0x10001fd0
// SYNTHETIC: DX7HR 0x10001fdc
// SYNTHETIC: DX7HR 0x10001fe7
// SYNTHETIC: DX7HR 0x10001ff1
// SYNTHETIC: DX7HR 0x10002002
// SYNTHETIC: DX7HR 0x1000200e
// SYNTHETIC: DX7HR 0x10002019
// SYNTHETIC: DX7HR 0x10002023
// SYNTHETIC: DX7HR 0x10002034
// SYNTHETIC: DX7HR 0x10002040
// SYNTHETIC: DX7HR 0x1000204b
// SYNTHETIC: DX7HR 0x10002055
// SYNTHETIC: DX7HR 0x10002066
// SYNTHETIC: DX7HR 0x10002072
// SYNTHETIC: DX7HR 0x1000207d
// SYNTHETIC: DX7HR 0x10002087
// SYNTHETIC: DX7HR 0x10002098
// SYNTHETIC: DX7HR 0x100020a4
// SYNTHETIC: DX7HR 0x100020af
// SYNTHETIC: DX7HR 0x100020c4
// SYNTHETIC: DX7HR 0x100020c8
// SYNTHETIC: DX7HR 0x100020cc
// SYNTHETIC: DX7HR 0x100020d0
// SYNTHETIC: DX7HR 0x10002172
// SYNTHETIC: DX7HR 0x10002218

TSCriticalSection* DX7Movie::sMovieCriticalSection;
IDirectSound* DX7Movie::sDirectSoundInterface = NULL;

// FUNCTION: DX7HR 0x10002274
// Creates a movie shown over the whole back buffer and starts its Bink playback thread
DX7Movie::DX7Movie(const FSFileSpec& inFile, unsigned long inScreen)
{
    if (DX7Movie::sMovieCriticalSection == NULL)
        TSScheduler::CreateCriticalSection(DX7Movie::sMovieCriticalSection);
    this->Initialize(inFile, inScreen, false);
}

// FUNCTION: DX7HR 0x100022ef
// Creates a movie drawn into the given rectangle, optionally looping, and starts its Bink playback thread
DX7Movie::DX7Movie(const FSFileSpec& inFile, unsigned long inScreen, GERectangle inRectangle, bool inLoop)
{
    if (DX7Movie::sMovieCriticalSection == NULL)
        TSScheduler::CreateCriticalSection(DX7Movie::sMovieCriticalSection);
    this->mHasRectangle = true;
    this->mDestinationRectangle = inRectangle;
    this->Initialize(inFile, inScreen, inLoop);
}

// FUNCTION: DX7HR 0x10002389
// Destroys the movie's events, playback thread and frame lock
DX7Movie::~DX7Movie()
{
    TSScheduler::DestroyEvent(this->mMovieStopped);
    TSScheduler::DestroyEvent(this->mFrameReadyEvent);
    TSScheduler::DestroyThread(this->mThread);
    TSScheduler::DestroyCriticalSection(this->mFrameAccess);
}

// FUNCTION: DX7HR 0x100023d2
// Creates the system-memory frame buffer at the movie's size in the back buffer's pixel format; on success sets the
// source rectangle (the destination defaults to the movie's size)
long DX7Movie::CreateFrameBuffer(BINK* inBink)
{
    GESurfaceDescription theDescription;
    GEScreen* theScreen = GERasterizer::GetActiveRasterizer()->GetScreen(this->mScreen);
    GESurface* theTarget = GERasterizer::GetActiveRasterizer()->GetSecondarySurface(this->mScreen);
    theDescription.mType = kSurfaceSystemMemory;
    theDescription.mPixelFormat = theTarget->GetPixelFormat();
    theDescription.mPixelDepth = theTarget->GetPixelDepth();
    theDescription.mSurfaceVolatility = 0;
    theDescription.mWidth = inBink->Width;
    theDescription.mHeight = inBink->Height;
    theDescription.m14 = 0;
    theDescription.mMipLevelCount = 0;
    theDescription.mIs3DRenderable = false;
    theDescription.mIsTransparencyEnabled = false;
    if (!this->mHasRectangle)
        this->mDestinationRectangle = GERectangle(0.0f, 0.0f, (float)theDescription.mWidth, (float)theDescription.mHeight);
    long theResult = theScreen->CreateSurface(theDescription, &this->mFrameBuffer);
    if (theResult == kErrorOK) {
        this->mSourceRectangle.SetLeft(0.0f);
        this->mSourceRectangle.SetTop(0.0f);
        this->mSourceRectangle.SetRight((float)(long)inBink->Width);
        this->mSourceRectangle.SetBottom((float)(long)inBink->Height);
    }
    return theResult;
}

// FUNCTION: DX7HR 0x100024c4
// Returns whether the movie loops
bool DX7Movie::GetIsLooping() const
{
    return this->mPlayBinkParameter.mIsLooping;
}

// FUNCTION: DX7HR 0x100024c8
// Sets up a movie: builds the file name, takes a reference, creates the frame lock, the playback thread and its events
void DX7Movie::Initialize(const FSFileSpec& inFile, unsigned long inScreen, bool inLoop)
{
    this->mFrameBuffer = NULL;
    this->mIsPlaying = false;
    this->mStopRequested = false;
    this->mIsFinished = false;
    this->mIsReady = false;
    this->mHasRectangle = false;
    this->mScreen = inScreen;
    this->mPlayBinkParameter.mMovie = this;
    this->mPlayBinkParameter.mIsLooping = inLoop;
    this->mJustStarted = true;
    this->mThread = NULL;
    sprintf(this->mFilename, "%s%s%s", (const char*)inFile.GetVolume(), (const char*)inFile.GetDirectory(),
            (const char*)inFile.GetFile());
    this->Reference();
    ESVERIFY_AT(TSScheduler::CreateCriticalSection(this->mFrameAccess) == keseOK, 183);
    ESVERIFY_AT(TSScheduler::CreateThread(this->mThread, DX7Movie::PlayBink, (void*)&(this->mPlayBinkParameter)) == keseOK, 184);
    ESVERIFY_AT(TSScheduler::CreateEvent(this->mFrameReadyEvent, false) == keseOK, 185);
    ESVERIFY_AT(TSScheduler::CreateEvent(this->mMovieStopped, false) == keseOK, 186);
    this->mMovieStopped->Clear();
}

// FUNCTION: DX7HR 0x100025f9
// Waits for a decoded frame and copies it to the back buffer (stretched to the destination); 7 once playback has ended
long DX7Movie::RenderFrame()
{
    long theResult = kErrorOK;
    GERectangle theDestination;
    if (!this->mJustStarted && !this->mIsPlaying)
        theResult = 7;
    else {
        this->mFrameReadyEvent->Wait(INFINITE);
        this->mFrameAccess->Enter();
        GESurface* theTarget = GERasterizer::GetActiveRasterizer()->GetSecondarySurface(this->mScreen);
        GESurface* theFrame;
        if (this->mIsPlaying && (theFrame = this->mFrameBuffer) != NULL) {
            theFrame->Reference();
            if (!this->mHasRectangle) {
                theDestination.SetLeft(0.0f);
                theDestination.SetTop(0.0f);
                theDestination.SetRight((float)(long)theTarget->GetWidth());
                theDestination.SetBottom((float)(long)theTarget->GetHeight());
            } else
                theDestination = this->mDestinationRectangle;
            theResult = theFrame->CopyTo(theTarget, this->mSourceRectangle, theDestination, false);
            theFrame->Dereference();
        }
        this->mFrameAccess->Leave();
    }
    return theResult;
}

// FUNCTION: DX7HR 0x100026f8
// Playback thread (one movie at a time): decodes the Bink file frame by frame into the frame buffer until stopped or
// finished, then frees the buffer, releases the movie's reference and signals that it stopped
unsigned long DX7Movie::PlayBink(void* inParameter)
{
    DX7Movie::sMovieCriticalSection->Enter();
    SEMixer* theMixer = SEMixer::GetActiveMixer();
    IDirectSound* theDirectSound = theMixer != NULL ? theMixer->GetDirectSoundInterface() : NULL;
    DX7Movie* theMovie = static_cast<PlayBinkParameter*>(inParameter)->mMovie;
    bool theLoop = static_cast<PlayBinkParameter*>(inParameter)->mIsLooping;
    theMovie->mThread->SetPriority(0);
    UMSetFloatingPointPrecision(0x10000);
    if (theMixer != NULL && theDirectSound != DX7Movie::sDirectSoundInterface) {
        BinkSoundUseDirectSound(theDirectSound);
        DX7Movie::sDirectSoundInterface = theDirectSound;
    }
    HBINK theBink = BinkOpen(theMovie->mFilename, 0);
    if (theBink != NULL) {
        if (theMovie->CreateFrameBuffer(theBink) != kErrorOK)
            theMovie->mIsFinished = true;
        theMovie->mIsPlaying = true;
        theMovie->mJustStarted = false;
        while (!theMovie->ShouldStop()) {
            if (UMGetFloatingPointPrecision() != 0x10000)
                UMSetFloatingPointPrecision(0x10000);
            if (UMGetFloatingPointRoundingMode() != 0)
                UMSetFloatingPointRoundingMode(0);
            while (BinkWait(theBink))
                Sleep(1);
            BinkDoFrame(theBink);
            theMovie->mFrameAccess->Enter();
            if (theMovie->mFrameBuffer->Lock() == kErrorOK) {
                BinkCopyToBuffer(theBink, theMovie->mFrameBuffer->GetMemoryAddress(),
                                 theMovie->mFrameBuffer->GetBytesPerRow(), theBink->Height, 0, 0,
                                 BinkDDSurfaceType(*static_cast<DX7Surface*>(theMovie->mFrameBuffer)));
                theMovie->mIsReady = true;
                theMovie->mFrameReadyEvent->Set();
                theMovie->mFrameBuffer->Unlock();
            }
            theMovie->mFrameAccess->Leave();
            if (!theMovie->ShouldStop() && !theLoop)
                theMovie->mIsFinished = theBink->FrameNum == theBink->Frames;
            if (!theMovie->ShouldStop())
                BinkNextFrame(theBink);
        }
        theMovie->mFrameAccess->Enter();
        theMovie->mIsPlaying = false;
        theMovie->mFrameReadyEvent->Set();
        if (theMovie->mFrameBuffer != NULL) {
            theMovie->mFrameBuffer->Dereference();
            theMovie->mFrameBuffer = NULL;
        }
        theMovie->mFrameAccess->Leave();
        BinkClose(theBink);
        Sleep(100);
    }
    theMovie->mJustStarted = false;
    theMovie->Dereference();
    DX7Movie::sMovieCriticalSection->Leave();
    theMovie->mMovieStopped->Set();
    return 0;
}

// FUNCTION: DX7HR 0x100028e5
// Returns whether a stop was requested or the movie has finished
bool DX7Movie::ShouldStop()
{
    return this->mStopRequested || this->mIsFinished;
}

// FUNCTION: DX7HR 0x100028f5
// Asks the playback thread to stop and waits until it has
void DX7Movie::Stop()
{
    this->mStopRequested = true;
    this->mMovieStopped->Wait(INFINITE);
}

// The template-static init chain head and atexit registrars (four pools: GELOD x3 and U2DSparseArray):
// SYNTHETIC: DX7HR 0x10002905
// SYNTHETIC: DX7HR 0x1000292d
// SYNTHETIC: DX7HR 0x10002939
// SYNTHETIC: DX7HR 0x10002945
// SYNTHETIC: DX7HR 0x10002951

// DX7HRTnLDisplay.dll (the T&L twin builds this unit unchanged): the same functions at their twin addresses.
// SYNTHETIC: DX7HRTNL 0x10001fbe
// SYNTHETIC: DX7HRTNL 0x10001fc8
// SYNTHETIC: DX7HRTNL 0x10001fd9
// SYNTHETIC: DX7HRTNL 0x10001fe5
// SYNTHETIC: DX7HRTNL 0x10001ff0
// SYNTHETIC: DX7HRTNL 0x10001ffa
// SYNTHETIC: DX7HRTNL 0x1000200b
// SYNTHETIC: DX7HRTNL 0x10002017
// SYNTHETIC: DX7HRTNL 0x10002022
// SYNTHETIC: DX7HRTNL 0x1000202c
// SYNTHETIC: DX7HRTNL 0x1000203d
// SYNTHETIC: DX7HRTNL 0x10002049
// SYNTHETIC: DX7HRTNL 0x10002054
// SYNTHETIC: DX7HRTNL 0x1000205e
// SYNTHETIC: DX7HRTNL 0x1000206f
// SYNTHETIC: DX7HRTNL 0x1000207b
// SYNTHETIC: DX7HRTNL 0x10002086
// SYNTHETIC: DX7HRTNL 0x10002090
// SYNTHETIC: DX7HRTNL 0x100020a1
// SYNTHETIC: DX7HRTNL 0x100020ad
// SYNTHETIC: DX7HRTNL 0x100020b8
// SYNTHETIC: DX7HRTNL 0x100020c2
// SYNTHETIC: DX7HRTNL 0x100020d3
// SYNTHETIC: DX7HRTNL 0x100020df
// SYNTHETIC: DX7HRTNL 0x100020ea
// SYNTHETIC: DX7HRTNL 0x100020f4
// SYNTHETIC: DX7HRTNL 0x10002105
// SYNTHETIC: DX7HRTNL 0x10002111
// SYNTHETIC: DX7HRTNL 0x1000211c
// SYNTHETIC: DX7HRTNL 0x10002131
// SYNTHETIC: DX7HRTNL 0x10002135
// SYNTHETIC: DX7HRTNL 0x10002139
// SYNTHETIC: DX7HRTNL 0x1000213d
// SYNTHETIC: DX7HRTNL 0x100021df
// SYNTHETIC: DX7HRTNL 0x10002285
// FUNCTION: DX7HRTNL 0x100022e1
// FUNCTION: DX7HRTNL 0x1000235c
// FUNCTION: DX7HRTNL 0x100023f6
// FUNCTION: DX7HRTNL 0x1000243f
// FUNCTION: DX7HRTNL 0x10002531
// FUNCTION: DX7HRTNL 0x10002535
// FUNCTION: DX7HRTNL 0x10002666
// FUNCTION: DX7HRTNL 0x10002765
// FUNCTION: DX7HRTNL 0x10002952
// FUNCTION: DX7HRTNL 0x10002962
// SYNTHETIC: DX7HRTNL 0x10002984
// SYNTHETIC: DX7HRTNL 0x100029ac
// SYNTHETIC: DX7HRTNL 0x100029b8
// SYNTHETIC: DX7HRTNL 0x100029c4
// SYNTHETIC: DX7HRTNL 0x100029d0
// TEMPLATE: DX7HRTNL 0x10002972
