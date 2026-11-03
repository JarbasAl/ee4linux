// EEUCGate (RECONSTRUCTION; file name inferred): the unit class of a wall gate (RTTI EEUCGate > EEUnitClass;
// EEMLockUnlock dynamic_casts a unit's class to it). Names are provisional. Requires ee/EEUCBuilding.h.
#ifndef EE_EEUCGATE_H
#define EE_EEUCGATE_H

class EEUCGate : public EEUCBuilding {   // RTTI EEUCGate > EEUCBuilding > EEUnitClass (hdrcheck)
public:
    EEUCGate(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    // the gate's two wall pieces next to inGate (both return +0x1cc of what 0x211723 finds) (provisional names)
    EEComplexUnit* GetSecondPart(EEComplexUnit* inGate);  // 0x211735
    EEComplexUnit* GetFourthPart(EEComplexUnit* inGate);  // 0x21175c
    bool IsSidePart(EEComplexUnit* inGate);  // 0x10745 (+0x1d0/+0x1cc, then 0x1076f; the attack/build goals) (provisional name)
    bool IsMiddlePart(EEComplexUnit* inGate);  // 0x1076f (thiscall, ret 4; defined with IsSidePart) (provisional name)
    // EEUCGate.cpp's slots and members (vtable 0x448354; provisional names)
    // +0x20 0x2104ef: the gate's other four pieces become inUnit's class (ret 8: two arguments; the base slot
    // EEUnitClass::GateReplaced is the shared `ret 8` 0xac9c8, still declared with one argument)
    virtual void GateReplaced(EEUnit* inGate, EEUnit* inUnit);
    virtual EEUnit* CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator);  // +0x30 0x210e6b
    virtual EEUnit* CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation, EEUnit* inCreator);  // +0x34 0x210e53
    virtual void GetSurroundingTiles(EEComplexUnit* inUnit, std::vector<EETileCoordinate>& outTiles, long, bool);  // +0x44 0x210e86
    virtual bool IsPartLeftOut(EEComplexUnit* inUnit);  // +0x58 0x211370
    virtual void ActionChanged(EEComplexUnit* inUnit);  // +0x74 0x2117f2
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x211905
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x211c35
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x211dc9
    EEComplexUnit* GetPart(EEComplexUnit* inGate, long inIndex);    // 0x21112a (piece inIndex 0..4 of the gate)
    bool AreEnemiesNear(EEComplexUnit* inUnit);  // 0x2113a9 (an enemy near the gate)
    bool IsPasserNear(EEComplexUnit* inUnit);  // 0x211506 (a unit near the gate that may pass)
};

#endif
