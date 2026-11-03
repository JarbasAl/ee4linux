// EEVCWonder.h (RECONSTRUCTION; file name inferred). The wonder victory condition (RTTI EEVCWonder >
// EEVictoryCondition, vtable 0x442064, sizeof 0xc): EETeam's ctor 0x13d013 news it with the team (inline ctor: +4 =
// 0, +8 = the team); ??_G 0x13d059. EEVCWonder.cpp 0x1372a6..0x137b3f. Requires ee/EEVictoryCondition.h.
#ifndef EE_EEVCWONDER_H
#define EE_EEVCWONDER_H

class EEVCWonder : public EEVictoryCondition {
public:
    EEVCWonder(EETeam* inTeam) : EEVictoryCondition(inTeam) {}   // inline (EETeam's ctors 0x13d013..)
    virtual long GetVictoryState();         // +0x04 0x1374c1 (1: a wonder has stood long enough)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x08 0x13744b
};

#endif
