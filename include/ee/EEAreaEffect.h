// EEAreaEffect.h (RECONSTRUCTION; file name inferred). A timed effect on a unit or an area (RTTI EEAreaEffect >
// UReferenceCounter; vtable 0x44be24; EEAEManager.cpp's family: EEAECloaking, EEAEMorale, ...) and a unit's list of
// them (RTTI EEUnitAreaEffectList > UReferenceContainer<EEAreaEffect*>; vtable 0x446e6c). Only what the decompiled
// units call; provisional names. Requires lle/LowLevelEngine.h.
#ifndef EE_EEAREAEFFECT_H
#define EE_EEAREAEFFECT_H

#include "ee/UReferenceContainer.h"
#include <stdio.h>

class EEAreaEffectTemplate;
class EEComplexUnit;
class EEUnit;
#include "lle/GE2DPoint.h"              // mLocation by value (every includer already includes it)

// From the AE units' own code: ctor 0x27bf4b (ret 4: the template, referenced; mLocation = (0, 0), mUnit = 0, mPlayer =
// 0, then Virtual28(false)), dtor 0x27bf9f (dereferences the template and the unit, then Virtual40),
// ??_G 0x27bf83. Base vtable 0x44be24 has 19 slots (+0x00..+0x48). The derived kinds (EEAECloaking 0x44ba94,
// EEAEHealing 0x44baec, EEAEEconomy 0x44bcd0, EEAEEconomyFromUnits 0x44bb80, EEAEConversionImmunity 0x44bbd8,
// EEAECalamityImmunity 0x44bc28, EEAEMorale 0x44bc78 and its kinds) override them. Slot names are provisional.
class EEAreaEffect : public UReferenceCounter {
public:
    EEAreaEffect(EEAreaEffectTemplate* inTemplate);   // 0x27bf4b
    virtual ~EEAreaEffect();                // +0x00 0x27bf9f (??_G 0x27bf83)
    // +0x04 (empty: ICF `ret 4`; EEAECloaking 0x27aa51 calls the unit's SetCloakableChain(true))
    virtual void AppliedTo(EEComplexUnit* inUnit);
    // +0x08 0x27bfcd: mTemplate->CoversUnit(the three arguments, GetScale()) (EEUnitClass 0x631f1e: owner player id,
    // created unit, owner position; AL tested)
    virtual bool Covers(unsigned long inPlayer, EEComplexUnit* inUnit, const GE2DPoint& inPoint);
    // +0x0c 0x27bff1: the template's graphic effect (+0x14) on inUnit (gEffectManager->CreateUnitEffect, no handle)
    virtual void ShowOn(EEUnit* inUnit);
    virtual void Dump(FILE* inFile);        // +0x10 0x27c014: the out-of-synch dump (4 fprintf)
    // +0x14 0x27c073 (EEComplexUnit::GetChecksum 0x1fa07f sums it into the checksum; 13.2): mLocation's
    // coordinates as their bit patterns + the unit's id (0 without one) + mPlayer
    virtual long Checksum();
    virtual long GetStrength();             // +0x18 (0: ICF 0xfca98; EEAECloaking: the template's +0x3c)
    virtual float GetRadius();              // +0x1c 0x27c0a0: GetScale() * mTemplate->mRadius (the cloaking calamity spreads over twice this)
    virtual bool IsImmunity();              // +0x20 (false: ICF 0x27c0b4)
    virtual void SetUnit(EEComplexUnit* inUnit);   // +0x24 0x27c0cb (referenced) (provisional name)
    virtual void LoadMedia(bool inIsOnDemand);  // +0x28 0x27c0dc: mTemplate->Load(inArg)
    // +0x2c/+0x30/+0x34 (false: ICF 0x27acf9, ret 0xc): a modifier of inValue for inKind into outValue (EEAEMorale
    // 0x27b2e3 overrides +0x30 for kind 1, EEAECloaking 0x27ab3a for kind 3) (provisional names)
    virtual bool Virtual2C(long inKind, long inValue, long& outValue);
    virtual bool Modify(long inKind, long inValue, long& outValue);
    virtual bool Virtual34(long inKind, long inValue, long& outValue);
    // +0x38 (empty: ICF `ret 4`; EEComplexUnit::RemoveAffectingAreaEffects calls it with the unit; EEAECloaking
    // 0x27ab53)
    virtual void RemovedFrom(EEComplexUnit* inUnit);
    virtual void MarkDirty();               // +0x3c (empty: ICF `ret`; EEAEMoraleFromBuildings 0x27b956 sets its +0x1c)
    virtual void UnloadMedia();             // +0x40 0x27c0eb: mTemplate->Unload()
    virtual void Recount(long inArg);       // +0x44 (empty: ICF `ret 4`; EEAEMoraleFromBuildings 0x27b95b; type inferred)
    virtual void Tick(EEComplexUnit* inUnit, float inElapsed);        // +0x48 (empty: ICF `ret 8`; EEAEHealing 0x27ad56)

    // 0x27b480 (ret 4; EEAEManager.cpp's range, EEAEManager::FindEffect): the template's range test of inPoint around
    // mLocation (provisional name)
    bool IsInRange(const GE2DPoint& inPoint);
    float GetScale();                       // 0x27c0b7: the unit's +0x34 object's +0x84, 1.0 without a unit (provisional name)

    // +0x08 (= 0, 0; dumped as "Area Effect Position Location X/Y"; EEUnitAreaEffectList::MoveAndUpdate assigns a GE2DPoint
    // to it, IsInRange passes its address as one)
    GE2DPoint mLocation;
    // +0x10 (referenced; EEE2DMarchingIcons 0xe3b1d/0xe4132: its kind and icon count; ee/EEAreaEffectTemplate.h)
    EEAreaEffectTemplate* mTemplate;
    EEComplexUnit* mUnit;                   // +0x14 (referenced; EEUnitClass 0x631ede: the owner unit, its +0x13c/+0x14c position, +0x18 player)
    long mPlayer;                           // +0x18 (= 0; dumped as "Area Effect Player Number", summed into Virtual14)
};

// sizeof 0x14 (EEComplexUnit holds two, at +0xe4 and +0xf8; the ctor stores vtable 0x446e6c at both). The
// container's members do it all: no members of its own.
class EEUnitAreaEffectList : public UReferenceContainer<EEAreaEffect*> {
public:
    // Original27c6b9: vector walk, effect filter and owner callback; ret16. No body credit.
    void ApplyModifiers(long inKind, const long& inDamage, long& outDamage, EEComplexUnit* inUnit);
    void Add(EEAreaEffect* inEffect);       // 0x27c607 (keeps a reference)
    // 0x27c623 (ret 4): inEffect, replacing an effect of the same kind (kinds 2, 5 and 6 count as one) whose Virtual18
    // is not larger; nothing when the old one's is at least as large (provisional name)
    void AddAreaEffect(EEAreaEffect* inEffect);
    void Tick(EEComplexUnit* inUnit, float inElapsed);       // 0x27c7c9: each effect's Virtual48 (provisional name)
    bool Remove(long inKind);               // 0x27c6f3: the first effect of that kind
    void KeepStrongest(); // actual original noarg list filter; no body credit
    void MarkDirty();                       // 0x27c768: each effect's Virtual3C (EEPSuggestRecomputeAllAreaEffects) (provisional name)
    void MoveAndUpdate(const GE2DPoint& inPoint, float inValue);  // 0x27c783 (EEComplexUnit::UpdateAreaEffects) (provisional name)
};

#endif
