// EEAEMorale.h (RECONSTRUCTION; file name inferred). The morale area effects (RTTI EEAEMorale > EEAreaEffect;
// vtable 0x44bc78, ??_G 0x27b335 shared with its kinds) and its kinds EEAEMoraleFromBuildings (vtable 0x44bd6c) and
// EEAEMoraleFromMilitaryHeroes (vtable 0x44bdc8). EEAEMorale adds slots +0x4c (the morale level) and +0x50 (0x27b2d7,
// ICF with its +0x18: Virtual4C()). Names are provisional. Requires ee/EEAreaEffect.h and
// ee/EEAreaEffectTemplate.h.
#ifndef EE_EEAEMORALE_H
#define EE_EEAEMORALE_H

class EEAEMorale : public EEAreaEffect {
public:
    // inline: EEAEMoraleFromBuildings's ctor 0x27b911 calls EEAreaEffect's ctor directly
    EEAEMorale(EEAreaEffectTemplate* inTemplate) : EEAreaEffect(inTemplate) {}
    virtual ~EEAEMorale() {}                // +0x00 (inline: ??_G 0x27b335 stores the vptr, then the base dtor)
    // all inline: the slots and ??_G 0x27b335 are emitted in EEAEManager.cpp's range (CreateAreaEffect builds one)
    virtual long GetStrength() { return this->GetLevel(); }  // +0x18 0x27b2d7 (ICF with +0x50)
    // +0x30 0x27b2e3: for kind 1, outValue = inValue less a tenth per morale level (Virtual4C clamped to 0..10)
    virtual bool Modify(long inKind, long inValue, long& outValue)
    {
        bool theResult = false;
        if (inKind == 1) {
            long theLevel = this->GetLevel();
            theLevel = 0 > (theLevel > 10 ? 10 : theLevel) ? 0 : (theLevel > 10 ? 10 : theLevel);
            outValue = inValue - UMFloatToLong(inValue * (theLevel / 10.0f));
            theResult = true;
        }
        return theResult;
    }
    virtual long GetLevel() { return this->mTemplate->mLevel; }  // +0x4c 0x27b2dc
    virtual long Virtual50() { return this->GetLevel(); }  // +0x50 0x27b2d7
};

// EEAEMoraleFromBuildings (sizeof 0x28; ctor 0x27b911, made by EEAEManager::CreateAreaEffect 0x27b23f): the morale
// from the buildings around the effect, recounted (Virtual44) when Virtual3C flags it (EECPResourceCenter::GetMoraleFill
// 0x48cd05 calls +0x3c, +0x44(0), +0x4c through a dynamic_cast).
class EEAEMoraleFromBuildings : public EEAEMorale {
public:
    EEAEMoraleFromBuildings(EEAreaEffectTemplate* inTemplate);   // 0x27b911
    virtual void MarkDirty();               // +0x3c 0x27b956: mDirty = true
    virtual void Recount(long inArg);       // +0x44 0x27b95b (inArg unread): mCount from the buildings in range
    virtual long GetLevel();                // +0x4c 0x27b949: the smaller of mCount and mMax

    bool mDirty;                            // +0x1c (= true)
    long mMax;                              // +0x20 (= the template's +0x3c + the game variant's +0x74)
    long mCount;                            // +0x24 (= 0; Virtual44 clamps it to 0..mMax)
};

// EEAEMoraleFromMilitaryHeroes (sizeof 0x1c; ctor 0x27bf0a out of line, +0x4c 0x27bf22: the unit's morale from its
// class upgrade's +0xdc while alive; EEAEMoraleFromMilitaryHeroes.cpp) declared for
// EEAEManager::CreateAreaEffect
class EEAEMoraleFromMilitaryHeroes : public EEAEMorale {
public:
    EEAEMoraleFromMilitaryHeroes(EEAreaEffectTemplate* inTemplate);   // 0x27bf0a
    virtual long GetLevel();                // +0x4c 0x27bf22
};

#endif
