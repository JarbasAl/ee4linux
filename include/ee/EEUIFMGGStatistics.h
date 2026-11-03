// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGGStatistics.h (RECONSTRUCTION; file name inferred). The in-game
// form's statistics group (RTTI EEUIFMGGStatistics > EEUIFMGGroup, vtable 0x44ac64; EEUIFMGGStatistics.cpp
// 0x267ca8..0x267ec8). The main game form news it (8 bytes) into +0x4f0 (0x2652d5). It writes
// the form's text labels 0x5f (population), 0x60, 0x61 (epoch). Slot names are provisional. Requires
// ee/EEUIFMGMainGroup.h, ee/EEPlayer.h.
#ifndef EE_EEUIFMGGSTATISTICS_H
#define EE_EEUIFMGGSTATISTICS_H
#include "ee/EETechTreeTechID.h"

class EEUIFMGGStatistics : public EEUIFMGGroup {
public:
    EEUIFMGGStatistics(EEUIFMainGame* inOwner);  // 0x267ca8
    virtual ~EEUIFMGGStatistics();          // +0x00 (??_G 0x267cc0, dtor 0x267cdc)
    virtual void Refresh();                 // +0x04 0x267ce7 (label 0x5f: population / limit, when it changed)
    virtual void Virtual08();               // +0x08 (empty)
    virtual void Select();                  // +0x0c 0x267db1 (labels 0x60, 0x61, then Virtual04)
    virtual bool WantsRefresh();            // +0x14 (false)
};

// EETechTree's (declared in ee/EEPlayer.h; an in-class body there shifts register allocation in other includers)
inline long EETechTree::GetShownEpoch() const
{
    long theEpoch = this->mStartEpoch + 2 > this->mEndEpoch ? this->mEndEpoch : this->mStartEpoch + 2;
    return theEpoch > kTechEpochBronze ? theEpoch : 3;
}

#endif
