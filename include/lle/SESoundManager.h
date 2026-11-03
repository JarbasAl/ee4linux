// C:\EEX\Low-Level Engine\Sound Engine\SESoundManager.h (RECONSTRUCTION; SESoundManager.cpp 0x4f08e..0x532d9).
// Loads sounds into categories and plays instances of them on a mixer's channels; a scavenger thread retires
// finished instances. Member and type names are provisional unless an export or assert names them.
// Requires lle/LowLevelEngine.h.
#ifndef LLE_SESOUNDMANAGER_H
#define LLE_SESOUNDMANAGER_H

#include <vector>
#include <map>
#include "lle/UMemoryPool.h"
#include "lle/SESample.h"
#include "lle/SESMInstanceNode.h"

class FSPath;
class TSThread;
class TSEvent;

struct SESMSound {                          // proven sizeof 0x18 (mSounds: index * 0x18)
    UString mName;                          // +0x00 (vector<SESMSound>::_Tidy 0x4e201 destroys a UString per element)
    std::vector<SESMInstanceNode*>* mInstances; // +0x10 (0: free slot)
    unsigned long mLoadCount;               // +0x14 (Load: 1 on first load, ++ on reload)
};

struct SESMCategory {                       // (provisional name)
    float mPercentage;                      // +0x00 (SetCategoryPercentage)
    unsigned long mMaxInstances;            // +0x04
    unsigned long mNumberOfInstances;       // +0x08
    unsigned long mPriority;                // +0x0c
};

struct SESMOneShotSound {                   // proven sizeof 0xc (vector at +0x10); (provisional names)
    SESample* mSample;                      // +0x00 (UpdateOneShotSounds: its mixer channel still playing?)
    unsigned long mSampleID;                // +0x04 (Unload)
    unsigned long mInstanceID;              // +0x08 (Stop)
};

class LLE_API SESoundManager {              // proven sizeof 0x98 (implicit copy ctor 0x4c54d)
    friend class SESMInstanceNode;          // LastDereference returns the node to sSoundsPool
    friend struct SESMPausedOrder;          // Scavenger's sort predicate (SESoundManager.cpp)
public:
    SESoundManager(FSPath* inPath, SEMixer* inMixer, unsigned long inCategoryCount, unsigned long inChannelCount);
    ~SESoundManager();

    static void CleanupMemoryPoolAndDestroyCriticalSections();
    static void DumpMemoryPoolInfo();
    SESMInstanceNode* GetAndReferenceSoundInstance(unsigned long inInstanceID);
    unsigned long GetAvailableChannels() const { return this->mAvailableChannels; }
    TSCriticalSection* GetCriticalSection() const { return this->mLock; }
    unsigned long GetHighestTimerDelay() const { return this->mMixer->GetHighestTimerDelay(); }
    unsigned long GetInstanceID(SESMInstanceNode* inNode);
    bool GetIsLooping(unsigned long inInstanceID);
    bool GetIsLooping(SESMInstanceNode* inNode);
    bool GetIsMuted() { return this->mIsMuted; }
    bool GetIsPlaying(unsigned long inInstanceID);
    bool GetIsPlaying(SESMInstanceNode* inNode);
    float GetMasterVolume() const { return this->mMixer->GetVolume(); }
    unsigned long GetMaxChannels() const { return this->mMixer->GetMaximumChannelCount(); }
    float GetPan(unsigned long inInstanceID);
    float GetPan(SESMInstanceNode* inNode);
    FSPath* GetPath() const { return this->mPath; }
    float GetPitch(unsigned long inInstanceID);
    float GetPitch(SESMInstanceNode* inNode);
    unsigned long GetSampleID(unsigned long inInstanceID);
    unsigned long GetSampleID(SESMInstanceNode* inNode);
    unsigned long GetSampleRate(unsigned long inSampleID);
    long GetTimeLeftInMilli(unsigned long inInstanceID);
    long GetTotalPlayTimeInMilli(unsigned long inInstanceID);
    float GetVolume(unsigned long inInstanceID);
    static void InitializeSoundsPool(unsigned long inChunkSize, unsigned long inMaxChunks, unsigned long inChunkCount);
    unsigned long Load(const UString& inName, unsigned long inCategory, unsigned long inMaxInstances);
    long MixAhead() { return this->mMixer->MixAhead(); }
    unsigned long Play(unsigned long inSampleID, float inVolume, float inPan, float inPitch, bool inLooping,
                       char* inDebugName);
    SESMInstanceNode* PlayAndReturnReferencedNode(unsigned long inSampleID, float inVolume, float inPan, float inPitch,
                                                  bool inLooping, char* inDebugName);
    long RegisterOneShotSound(unsigned long inSampleID, unsigned long inInterval);
    unsigned long SetCategoryPercentage(unsigned long inCategory, float inPercentage);
    long SetCategoryPriority(unsigned long inCategory, unsigned long inPriority);
    void SetIsMuted(bool inMuted) { this->mIsMuted = inMuted; }
    long SetLatency(unsigned long inMilliseconds) { return this->mMixer->SetLatency(inMilliseconds); }
    long SetMasterVolume(float inVolume);
    long SetMaxChannels(unsigned long inCount);
    long SetPan(unsigned long inInstanceID, float inPan);
    long SetPan(SESMInstanceNode* inNode, float inPan);
    void SetPath(FSPath* inPath);
    long SetPitch(unsigned long inInstanceID, float inPitch);
    long SetPitch(SESMInstanceNode* inNode, float inPitch);
    long SetSoundParameters(unsigned long inInstanceID, float inVolume, float inPan, float inPitch);
    long SetSoundParameters(SESMInstanceNode* inNode, float inVolume, float inPan, float inPitch);
    long SetVolume(unsigned long inInstanceID, float inVolume);
    long SetVolume(SESMInstanceNode* inNode, float inVolume);
    long Stop(unsigned long inInstanceID, bool inImmediately);
    long Stop(SESMInstanceNode* inNode, bool inImmediately);
    long StopAllOneShotSounds();
    long Unload(unsigned long inSampleID);
protected:
    static long CreateSoundPoolLock();
    static void DestroySoundInstance(SESMInstanceNode* inNode);
    unsigned long GetCategoryPriority(unsigned long inCategory);
    static SESMInstanceNode* GetNewSoundInstance();
    float GetVolume2(unsigned long inInstanceID);
    float GetVolume2(SESMInstanceNode* inNode);
private:
    void AddInstanceToPausedQueue(SESMInstanceNode* inNode);
    unsigned long AddNewSampleToPausedQueue(unsigned long inSampleID, float inVolume, float inPan, float inPitch,
                                            SESMInstanceNode*& outNode);
    void AddPrioritizedNodeToPausedQueue(SESMInstanceNode* inNode);
    void DumpChannel(unsigned long inChannel);
    unsigned long DumpLowerPrioritySample(unsigned long inPriority, bool inLooping);
    unsigned long DumpNonPlayingChannels();
    unsigned long GetCategory(unsigned long inSampleID);
    unsigned long GetFreeChannel();
    unsigned long GetMaxCategoryInstances(unsigned long inSampleID);
    unsigned long GetMaxInstances(unsigned long inSampleID);
    unsigned long GetNumberOfCategoryInstances(unsigned long inSampleID);
    unsigned long GetNumberOfInstances(unsigned long inSampleID);
    SESMInstanceNode* GetPausedInstance(unsigned long inInstanceID, unsigned long* outIndex);
    SESMInstanceNode* GetPlayingInstance(unsigned long inInstanceID, unsigned long* outChannel);
    unsigned long GetRandomOffset(SESample* inSample);
    long GetTimeLeftInMilli(SESMInstanceNode* inNode);
    long GetTotalPlayTimeInMilli(SESMInstanceNode* inNode);
    unsigned long ReallyPlayIt(unsigned long inSampleID, unsigned long inChannel, float inVolume, float inPan,
                               float inPitch, bool inLooping, SESMInstanceNode*& outNode);
    unsigned long ReplaceCategoryInstance(unsigned long inSampleID);
    unsigned long ReplaceNonLoopingInstance(unsigned long inSampleID);
    static unsigned long Scavenger(void* inManager);
    long SetPan2(unsigned long inInstanceID, float inPan);
    long SetPan2(SESMInstanceNode* inNode, float inPan);
    long SetPitch2(unsigned long inInstanceID, float inPitch);
    long SetPitch2(SESMInstanceNode* inNode, float inPitch);
    long SetVolume2(unsigned long inInstanceID, float inVolume);
    long SetVolume2(SESMInstanceNode* inNode, float inVolume);
    void UpdateOneShotSounds();

    URandom mRandom;                        // +0x00
    std::vector<SESMOneShotSound> mOneShotSounds;   // +0x10
    std::vector<SESMInstanceNode*> mChannels;       // +0x20 one per available channel (GetFreeChannel)
    std::vector<SESMInstanceNode*> mPaused;         // +0x30
    std::vector<SESMSound> mSounds;                 // +0x40
    std::vector<SESMCategory*> mCategories;         // +0x50
    std::map<unsigned long, SESMInstanceNode*> mPausedInstances;   // +0x60 instance ID -> paused node (GetPausedInstance)
    FSPath* mPath;                          // +0x6c (referenced)
    SEMixer* mMixer;                        // +0x70
    TSThread* mScavengerThread;             // +0x74
    TSEvent* mScavengerThreadFinishedEvent; // +0x78 (ctor ESVERIFY text)
    TSEvent* mScavengerThreadExitEvent;     // +0x7c (ctor ESVERIFY text)
    TSCriticalSection* mLock;               // +0x80 (GetCriticalSection)
    unsigned long mCategoryCount;           // +0x84
    unsigned long mAvailableChannels;       // +0x88 (GetAvailableChannels)
    unsigned long mSoundCount;              // +0x8c (Load: mSounds.size())
    unsigned long mNextInstanceID;          // +0x90
    bool mIsMuted;                          // +0x94

    static UMemoryPool<SESMInstanceNode> sSoundsPool;   // 0x1012de90 (defined in SESoundManager.cpp)
protected:
    static TSCriticalSection* sSoundNodePoolLock;       // 0x1012dec8
};

#endif
