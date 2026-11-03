// EECPIArmy.h (RECONSTRUCTION; file name inferred). An army the computer player's intelligence tracks:
// an AI map area (RTTI EECPIArmy > EEAIArea @0; vtable 0x43c1dc, 11 slots: EEAIArea's with its own ??_G 0x8017b). Its
// tiles are its units' tiles; mBounds (EEAIArea +0x50) their bounding rectangle. Ctor 0x8013d; members from the ctor's
// stores (init list: before the vptr) and the bodies. Names are provisional. Requires ee/EEAIArea.h.
#ifndef EE_EECPIARMY_H
#define EE_EECPIARMY_H

class EECPIArmy : public EEAIArea {
public:
    EECPIArmy();                            // 0x8013d (mL64 = 6: the army kind)
    virtual ~EECPIArmy() {}                 // +0x00 (??_G 0x8017b: own vptr, then ~EEAIArea)
    // 0x8019d (ret 4): the unit's tile (its container's when it is inside one) added; mL78 += its class's +0x244
    void AddUnit(EEComplexUnit* inUnit);
    void Merge(EECPIArmy* inOther);         // 0x80221 (ret 4): inOther's tiles, mL78 and mL7C added
    // 0x80296 (ret 4): whether inUnit (or its container) can join: same continent, inside mBounds or within
    // distance^2 mL80 of it
    bool IsNear(EEComplexUnit* inUnit);

    long mL68;                              // +0x68 (ctor 0)
    long mL6C;                              // +0x6c (ctor 0)
    unsigned short mContinentID;            // +0x70 (ctor 0) the continent id (0x80296 compares GetContinentID with it)
    long mL74;                              // +0x74 (ctor 0)
    unsigned long mStrength;                // +0x78 (ctor 0) the summed unit values (Merge adds it)
    long mL7C;                              // +0x7c (ctor 0; Merge adds it)
    long mJoinDistanceSq;                   // +0x80 (= 0x90 in the body) distance^2 limit of IsNear
    bool mB84;                              // +0x84 (ctor false)
};

#endif
