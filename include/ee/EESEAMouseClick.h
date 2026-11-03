// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAMouseClick.h (RECONSTRUCTION; file name inferred). The
// editor's mouse click action (RTTI EESEAMouseClick > EEAMouseClick; vtable 0x44468c, sizeof 0x10; ctor 0x19176b and
// ??_G 0x191951 emitted by their first user). Requires ee/EEAMouseClick.h.
#ifndef EE_EESEAMOUSECLICK_H
#define EE_EESEAMOUSECLICK_H

class EESEAMouseClick : public EEAMouseClick {
public:
    EESEAMouseClick() {}
    virtual ~EESEAMouseClick() {}   // user-declared empty (its ??_G stores the vptr first; EESETSObject.cpp)
    virtual void Virtual4();                // +0x04 (0x1a747d)
    virtual void Execute(float inX, float inY, bool inMove, unsigned long inModifiers, bool inIsRotating, float inFacing, bool inForceMove);
};

#endif
