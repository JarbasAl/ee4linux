// C:\EEX\Empire Earth\Scenario Editor Actions\EESEADeleteUnit.h (RECONSTRUCTION; file name inferred). The editor's
// "delete units" action (RTTI EESEADeleteUnit > EEAction; vtable 0x445958, sizeof 0x18; ctor 0x1a6b81 posts
// itself; Virtual4 0x1a6c4c). EESEADeleteUnit.cpp 0x1a6b81..0x1a7331. Names are provisional.
// Requires ee/EEAction.h.
#ifndef EE_EESEADELETEUNIT_H
#define EE_EESEADELETEUNIT_H

class EESEADeleteUnit : public EEAction {
public:
    // 0x1a6b81: inUnitId -1 deletes the selection (provisional name)
    EESEADeleteUnit(unsigned long inUnitId);
    // 0x1a6b45: the selection of player inPlayer (EESEUnit::UpdateKeys: the delete key, inGroup with shift)
    // (provisional name)
    EESEADeleteUnit(long inPlayer, bool inGroup);
    virtual void Virtual4();                // +0x04 0x1a6c4c
    virtual void Virtual8() {}              // +0x08 (the shared empty slot 0x1720c6)
    // 0x1a6bcf: deletes inUnit and the rest of its +0x1d0 chain (provisional name)
    void DeleteComplexUnit(EEComplexUnit* inUnit);
    // 0x1a6e33: takes inUnit out of its container's cargo (provisional name)
    void RemoveFromContainer(EEComplexUnit* inUnit);


    unsigned long mUnitId;                  // +0x0c
    long mPlayer;                           // +0x10 the unit's player (its player +0x45c)
    bool mIsGroup;                          // +0x14
    bool mUpdateSelection;                  // +0x15
};

#endif
