// EEUCNaval.h (RECONSTRUCTION from RTTI/vtables/ctors; one header per class; provisional names). Classes: EEUCNaval.
// Names, argument/return types and member types inferred (signatures from `ret N` and the bodies, ctor
// arguments from their stores, members from layout.py); PLACEHOLDER members cover objects not decoded yet.
// Requires the engine declarations of: GE3DPoint, GETransformation.
#ifndef EE_EEUCNAVAL_H
#define EE_EEUCNAVAL_H

#include "ee/EEUnit.h"

class EEComplexUnit;
class EEUnit;
class EEUnitClass;

// vtable 0x449350; bases EEUCNaval > EEUnitClass > UReferenceCounter; sizeof 0x32c; ctor inline; ??_G 0x236248; dtor 0x2305ee
class EEUCNaval : public EEUnitClass {
public:
    EEUCNaval();                            // inline
    EEUCNaval(EEUnitClassRecord* inRecord);   // inline: defined in EEUnitClass.cpp (the class factory 0x2358b1)
    virtual ~EEUCNaval() {}                 // +0x00 dtor 0x2305ee
    virtual void GroundLocation(GE3DPoint& ioLocation); // +0x04 0x218a45 (ICF: EEUCNavalCitizen, EEUCNavalTransport)
    virtual void LocationToTile(const GE3DPoint& inLocation, long& outX, long& outY); // +0x08 0x218490 (ICF: EEUCNavalBuilding, EEUCNavalCitizen...)
    virtual void AdjustPath(EEComplexUnit* inUnit, std::vector<GE3DPoint>& ioPath, bool inSmooth); // +0x0c 0x217c6d (ICF: EEUCNavalTransport)
    virtual bool CanAttackMove();           // +0x10 0x18aa9 trivial: mov al, 1; ret (ICF: ?$EEAIAStarGoalBase@VEEAIAStarBoundedStorage@@VEEMap@@, ?$EEAIAStarGoalBase@VEEAIAStarChokePointStorage@@VEEMap@@...)
    virtual EEUnit* CreateUnit(long inX, long inY, long inPlayer, bool inIsFoundation, EEUnit* inCreator); // +0x30 0x218605 (ICF: EEUCNavalBuilding, EEUCNavalCitizen...)
    virtual EEUnit* CreateUnitAt(GETransformation& inTransformation, long inPlayer, bool inIsFoundation, EEUnit* inCreator); // +0x34 0x218909 (ICF: EEUCNavalBuilding, EEUCNavalCitizen...)
    virtual void ActionChanged(EEComplexUnit* inUnit); // +0x74 0x217c84
    virtual void UnitCreated(EEComplexUnit* inUnit);  // +0x78 0x217d6a
    virtual void AddToTile(EEUnit* inUnit);   // +0x84 0x21aaa9 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual bool HasArrived(EEComplexUnit* inUnit, float inValue);  // +0x88 0x217dcf
    virtual void RemoveFromTile(EEUnit*);  // +0x98 0x21aaf5 (ICF: EEUCAirAnimal, EEUCAirplane...)
    virtual long EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0xa0 0x217eb2 (ICF: EEUCNavalTransport)
    virtual void Update(EEComplexUnit* inUnit);      // +0xb8 0x218055
    virtual void UpdateEffects(EEComplexUnit*); // +0xbc 0x20d336 (ICF: EEUCBasicCitizen, EEUCBasicPriest...)
    // 0x217fb5 (ret 4; reads no member): VirtualB8 for class type 0x13b (burning time to hit points, then the
    // timer) (provisional name)
    void UpdateResourceUnit(EEComplexUnit* inUnit);
};

#endif
