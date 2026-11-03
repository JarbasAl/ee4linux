// C:\EEX\Low-Level Engine\Thread System\TSEvent.h (RECONSTRUCTION; layout proven by TSEvent.cpp 0x53cee..0x53d8b,
// TSScheduler::CreateEvent operator new(8)).
#ifndef LLE_TSEVENT_H
#define LLE_TSEVENT_H

class LLE_API TSEvent {
    friend class TSScheduler;
public:
    long Set();
    long Clear();
    long Wait(unsigned long inTimeout);
    // exported inline members, ICF-folded: GetIsManuallyReset 0x3d783 (in NELagLossSimulator.cpp's inline block,
    // just before TSThread's), operator void* 0x10b8d
    bool GetIsManuallyReset() { return this->mManualReset; }
    operator void*() const { return this->mHandle; }

    HANDLE mHandle;                         // +0x00 (TCP Searcher waits on it directly)
private:
    TSEvent(HANDLE inHandle, bool inManualReset);
    ~TSEvent();

    bool mManualReset;                      // +0x04
};

#endif
