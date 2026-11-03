// EEEffectsTable.h (RECONSTRUCTION; file name inferred). The effects per map region: RTTI
// EEEffectsTable > RTSRegionTable<EEEffect*> (vtable 0x441cf0; ??_G 0x11e1b6). EEMap::Fn11BF4A / Load make it with
// a region size of 10 (its inline ctor stores the vptr after the base ctor 0x11e2ce); the destructor is implicit.
// Requires ee/RTSRegionTable.h.
#ifndef EE_EEEFFECTSTABLE_H
#define EE_EEEFFECTSTABLE_H

class EEEffect;

class EEEffectsTable : public RTSRegionTable<EEEffect*> {
public:
    EEEffectsTable(EEMap* inMap, long inRegionSize) : RTSRegionTable<EEEffect*>(inMap, inRegionSize) {}
    virtual ~EEEffectsTable() {}            // +0x00 (user-declared: ??_G 0x11e1b6 stores the class's own vptr first)
};

#endif
