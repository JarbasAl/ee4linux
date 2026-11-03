// EEPGSFormManager.h (RECONSTRUCTION; file name inferred; layout proven: EEPGSFormManager sizeof 0x350 = its new-site
// size, ctor 0x1e44da exact in EEUIEPostGameSummaryEvents.cpp; member names are provisional). The
// post-game summary form's manager: RTTI EEPGSFormManager > UIFormManager, vtable 0x4469c0 (the scalar deleting dtor
// alone), sizeof 0x350 (new at 0x1e3e25). Ctor 0x1e44da, ??_G 0x1e4546 and dtor 0x1e4562 follow the summary's Open
// (their first user) in EEUIEPostGameSummaryEvents.cpp. Nine summary graphs (one series per player, filled by
// 0x1e46b5) and the range of the time line. Requires lle/UIFormManager.h, lle/UGraph.h.
#ifndef EE_EEPGSFORMMANAGER_H
#define EE_EEPGSFORMMANAGER_H

class EEPGSFormManager : public UIFormManager {
public:
    EEPGSFormManager();                     // 0x1e44da
    virtual ~EEPGSFormManager();            // 0x1e4562

    UGraph mEconomyScoreGraph;              // +0x004
    UGraph mTotalCollectedGraph;            // +0x060
    UGraph mTotalSpentGraph;                // +0x0bc
    UGraph mEpochScoreGraph;                // +0x118
    UGraph mMilitaryScoreGraph;             // +0x174
    UGraph mGraph1D0;                       // +0x1d0
    UGraph mSpeedRatingGraph;               // +0x22c
    UGraph mSocietyScoreGraph;              // +0x288
    UGraph mScoreGraph;                     // +0x2e4
    unsigned long mFirstTime;               // +0x340 (the time line's first and last values)
    unsigned long mLastTime;                // +0x344
    long mU348;                             // +0x348 (-1)
    long mU34C;                             // +0x34c (0)
};

#endif
