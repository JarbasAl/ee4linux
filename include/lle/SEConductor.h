// C:\EEX\Low-Level Engine\Sound Engine\SEConductor.h (RECONSTRUCTION; SEConductor.cpp 0x4bbce..0x4e8aa). DirectMusic
// score playback. Every SEMixer embeds one (SEMixer::GetConductor). Requires lle/LowLevelEngine.h.
#ifndef LLE_SECONDUCTOR_H
#define LLE_SECONDUCTOR_H

#include <vector>

struct IDirectMusic;
struct IDirectMusicComposer;
struct IDirectMusicLoader;
struct IDirectMusicPerformance;
struct IDirectMusicPort;
struct IDirectMusicSegment;
class SEMixer;
class SEScore;
class TSCriticalSection;
class TSEvent;
class TSThread;

// One loaded score (SEConductor::mScores). Names are provisional.
struct SEConductorScore {                   // proven sizeof 0x20 (vector: size >> 5; UWideString dtor per element)
    UWideString mName;                      // +0x00
    SEScore* mScore;                        // +0x14
    long mID;                               // +0x18
    unsigned long mLoadCount;               // +0x1c (LoadScore: 1, ++ per repeated load; UnloadScore)
};

class LLE_API SEConductor {                 // proven sizeof 0x84 (SEMixer::mModule at +0x84)
    friend class SEMixer;
    friend class SEScore;                   // ~SEScore unloads from the active mixer's performance
public:
    // inline members in declaration order (SEConductor.cpp emits them first, 0x4bbce..0x4bc09; the folded ones
    // sit at other addresses)
    unsigned long GetChannelGroupCount() { return this->mChannelGroupCount; }
    unsigned long GetConductorState() const { return this->mConductorState; }
    unsigned long GetIntensity() const { return this->mIntensity; }
    const UWideString& GetMusicFolder() const { return this->mMusicFolder; }
    unsigned long GetReverbState() const { return this->mReverbState; }
    float GetVolume() const { return this->mVolume; }
    long LoadScore(const UWideString& inName, long& outID) { return this->LoadScore(inName, outID, false); }
    void SetNotificationCallback(unsigned long inEvent, void (*inCallback)(SEConductor&, long))
        { this->mCallbacks[inEvent] = inCallback; }

    long GetCurrentlyPlayingScore();
    SEScore* GetScore(long inID) const;
    long GetScoreID(SEScore* inScore) const;
    long Play(long inID);
    long Queue(long inID);
    long SetChannelGroupCount(unsigned long inCount);
    long SetConductorState(unsigned long inState);
    long SetIntensity(unsigned long inIntensity, bool inImmediate);
    long SetMusicFolder(const UWideString& inFolder);
    long SetReverb(unsigned long inReverb);
    long SetReverbStyle(float inGain, float inHighFreqRTRatio, float inMix, float inTime);
    long SetVolume(float inVolume);
    long Stop();
    long TransitionTo(long inID, unsigned long inFlags, unsigned long inEmbellishment);
    long UnloadScore(long inID);
private:
    IDirectMusicPerformance* GetPerformance() const { return this->mPerformance; }
    void SetMixer(SEMixer* inMixer) { this->mMixer = inMixer; }

    SEConductor();
    virtual ~SEConductor();
    static unsigned long EventHandler(void* inConductor);
    long InitializeConductor();
    long InitializePorts(IDirectMusicPerformance* inPerformance, IDirectMusic* inDirectMusic,
                         IDirectMusicPort*& outPort);
    long LoadScore(const UWideString& inName, long& outID, bool inSecondary);
    long SetDirectMusicFolder(const UWideString& inFolder);
    long UninitializeConductor();

    std::vector<SEConductorScore> mScores;  // +0x04
    UWideString mMusicFolder;               // +0x14 (capacity 0x104)
    void (*mCallbacks[6])(SEConductor&, long);   // +0x28 SetNotificationCallback
    TSThread* mEventHandler;                // +0x40 (assert text; created by InitializeConductor)
    TSCriticalSection* mLock;               // +0x44 (assert text "this->mLock")
    TSEvent* mNotificationEvent;            // +0x48 (assert text)
    TSEvent* mEventHandlerExit;             // +0x4c (assert text)
    TSEvent* mEventHandlerThreadDone;       // +0x50 (assert text)
    SEMixer* mMixer;                        // +0x54
    IDirectMusicPerformance* mPerformance;  // +0x58 (GetPerformance)
    IDirectMusicComposer* mComposer;        // +0x5c (CoCreateInstance(CLSID_DirectMusicComposer), TransitionTo)
    IDirectMusicLoader* mLoader;            // +0x60
    IDirectMusic* mDirectMusic;             // +0x64 (IDirectMusicPerformance::Init)
    IDirectMusicPort* mPort;                // +0x68 (InitializePorts; IKsControl for volume/reverb)
    float mVolume;                          // +0x6c (1.0)
    unsigned long mConductorState;          // +0x70
    unsigned long mReverbState;             // +0x74 (1)
    long mNextScoreID;                      // +0x78 (LoadScore; wraps -1 to 0)
    unsigned long mIntensity;               // +0x7c
    unsigned long mChannelGroupCount;       // +0x80 (1000)
};

// SEConductor::TransitionTo inCommand: the DirectMusic DMUS_COMMANDT_* the conductor composes with (SEConductor.cpp)
enum SETransitionCommand {
    kMusicTransitionGroove = 0,     // TransitionTo case -> DMUS_COMMANDT_GROOVE (default)
    kMusicTransitionFill = 1,       // TransitionTo case -> DMUS_COMMANDT_FILL
    kMusicTransitionBreak = 2,      // TransitionTo case -> DMUS_COMMANDT_BREAK
    kMusicTransitionIntro = 3,      // TransitionTo case -> DMUS_COMMANDT_INTRO
    kMusicTransitionEnd = 4,        // TransitionTo case -> DMUS_COMMANDT_END
    kMusicTransitionEndAndIntro = 5,  // TransitionTo case -> DMUS_COMMANDT_ENDANDINTRO
};

// SEConductor::TransitionTo inFlags: the DirectMusic DMUS_COMPOSEF_* boundary
enum SETransitionFlags {
    kTransitionFlagMeasure = 0,  // TransitionTo case -> DMUS_COMPOSEF_MEASURE (default)
    kTransitionFlagModulate = 1,  // TransitionTo case -> DMUS_COMPOSEF_MODULATE
    kTransitionFlagLong = 2,    // TransitionTo case -> DMUS_COMPOSEF_LONG
};

#endif
