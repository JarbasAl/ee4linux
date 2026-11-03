// C:\EEX\Low-Level Engine\Error System\ESProfile.h (file name inferred): named timing records. In this release build
// the profiling calls are empty inline bodies (0x37f0, 0xc16a, 0x58f8e); only the history store remains.
// ESProfile.cpp 0x36c5..0x4020. Requires <map> and UWideString.
#ifndef LLE_ESPROFILE_H
#define LLE_ESPROFILE_H

#include <map>

// Implicit copy ctor (0x36c5), operator= (0x3758) and dtor (0x37eb) are emitted at the head of ESProfile.cpp.
// Times are __int64 (signed __alldiv by 1000); field names are provisional, grouped by what Clear/Convert* do.
struct LLE_API ESProfileHistory {           // sizeof 0x70 (pair value at node +0x28)
    ESProfileHistory();                     // 0x3859: Clear()
    void Clear();                           // 0x386c
    void ConvertMicrosecondsToMilliseconds();   // 0x38b0
    void ConvertMillisecondsToMicroseconds();   // 0x3953

    UWideString mName;                      // +0x00
    __int64 mTotalTime;                     // +0x18 converted
    __int64 mLastTime;                      // +0x20 converted
    __int64 mAverageTime;                   // +0x28 converted
    __int64 mMaximumTime;                   // +0x30 converted (last)
    __int64 mFirstTime;                     // +0x38 converted
    __int64 mMinimumTime;                   // +0x40 -1 = none (converted only when set)
    __int64 mRecentTime;                    // +0x48 converted
    __int64 mCallCount;                     // +0x50 not converted
    __int64 mRecursionCount;                // +0x58 not converted
    __int64 mSampleCount;                   // +0x60 not converted
    bool mInMicroseconds;                   // +0x68
};

class LLE_API ESProfile {
public:
    // every member inline and empty in this build (ICF-folded): both ctors 0x37f0, dtor/StartProfile 0xc16a,
    // EndProfile 0x58f8e; implicit operator= 0x37fb
    ESProfile(const char* inName, bool inRecursive, bool inReport, long inThreshold) {}   // names are provisional
    ESProfile(const UWideString& inName, bool inRecursive, bool inReport, long inThreshold) {}
    ~ESProfile() {}
    void StartProfile() {}
    void EndProfile(bool inRecord) {}

    static const std::map<UWideString, ESProfileHistory>& GetProfileHistory() { return ESProfile::sProfileHistory; }  // 0x37f5
    static void ClearAllRecords();                                              // 0x39f7
    static bool GetProfileInformation(const UWideString& inName, ESProfileHistory& outHistory);   // 0x3a01

private:
    static std::map<UWideString, ESProfileHistory> sProfileHistory;            // 0x1010f6f0
};

#endif
