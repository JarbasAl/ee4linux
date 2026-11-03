// C:\EEX\Empire Earth\...\EEAIHill.h (RECONSTRUCTION; file path inferred). An AI map area
// (RTTI EEAIHill > EEAIArea @0; vtable 0x439f18, 11 slots). Only the base is declared: EECPILocation::Update 0x80a57 reads the
// EEAIArea serial of what EEAIContinentManager's lookup returns. Requires ee/EEAIArea.h.
#ifndef EE_EEAIHILL_H
#define EE_EEAIHILL_H

class EEAIHill : public EEAIArea {
public:
    // inline (EEAIAStarHillGoal::CreateHill 0x16d04 expands it: EEAIArea(), +0x68/+0x6a = 0, the vptr)
    EEAIHill() : mContinentID(0), mID(0) {}
    // inline, user-declared empty (??_G 0x1706b stores the own vptr, then ~EEAIArea)
    virtual ~EEAIHill() {}

    unsigned short mContinentID;            // +0x68 (the hill goal's +0x56: every tile's continent)
    unsigned short mID;                     // +0x6a (EEAIContinent::AddHillID 0x26e13 reads it)
};

#endif
