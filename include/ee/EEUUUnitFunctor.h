// EEUUUnitFunctor.h (RECONSTRUCTION; file name inferred). The base of the unit functors (RTTI EEUUUnitFunctor; vtable
// 0x442b18: the shared ??_G 0x16b555 (it stores this vptr: an empty inline virtual dtor), then two empty `ret 4` slots
// (the ICF body 0x156dd8)). Derived (RTTI, sharing that ??_G): EEPSuggestRecomputeAllAreaEffects
// (EEPlayer.cpp), EEHitPointUpgrader, ApplyGeneratedEffectsToOthersFunctor, SetAffectedByAreaEffectFunctor,
// ClearAreaEffectsFunctor, RebuildAreaEffectRegionListFunctor, ... The walker 0x24672f (cdecl) calls slot +8 with each
// unit of a player (mFirstUnit list) and slot +4 with its complex unit (+0x30), then (inCalamities) the same for each
// GetPendingUnit unit. Slot names are provisional.
// Requires ee/EEUnit.h (EEUnit, EEComplexUnit) or forward declarations.
#ifndef EE_EEUUUNITFUNCTOR_H
#define EE_EEUUUNITFUNCTOR_H

class EEComplexUnit;
class EEPlayer;
class RTSUnit;

class EEUUUnitFunctor {
public:
    virtual ~EEUUUnitFunctor() {}
    virtual void Apply(EEComplexUnit* inUnit) {}       // +0x04 (the unit's complex unit)
    virtual void ApplyToUnit(RTSUnit* inUnit) {}       // +0x08 (each unit)
};

// 0x24672f (cdecl; EEPlayer::SuggestRecomputeAllAreaEffects passes its functor and false) (provisional name)
void EEForEachUnitOf(EEPlayer* inPlayer, EEUUUnitFunctor* inFunctor, bool inCalamities);

#endif
