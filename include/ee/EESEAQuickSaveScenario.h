// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAQuickSaveScenario.h (RECONSTRUCTION; file name inferred). The
// editor's "quick save scenario" action (RTTI EESEAQuickSaveScenario > EEAction; vtable 0x445fc0, sizeof 0x10): it
// posts the quick save/load action it holds (referenced). The inline ctor 0x1af8ad and ??_G 0x1af8cd
// are emitted by their first user. Requires ee/EEAQuickSaveLoad.h.
#ifndef EE_EESEAQUICKSAVESCENARIO_H
#define EE_EESEAQUICKSAVESCENARIO_H

class EESEAQuickSaveScenario : public EEAction {
public:
    EESEAQuickSaveScenario(EEAQuickSaveLoad* inAction) : mQuickSaveLoad(inAction)
    {
        inAction->Reference();
    }
    virtual ~EESEAQuickSaveScenario()
    {
        this->mQuickSaveLoad->Dereference();
    }
    virtual void Virtual4();                // +0x04 (0x1aa723)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)

    EEAQuickSaveLoad* mQuickSaveLoad;       // +0x0c
};

#endif
