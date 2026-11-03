// C:\EEX\Empire Earth\Scenario Editor Actions\EESEAQuickSaveLoad.h (RECONSTRUCTION; file name inferred).
// The editor's quick save/load action (RTTI EESEAQuickSaveLoad > EEAQuickSaveLoad; vtable 0x44516c, sizeof 0x2c; ctor
// inline in EEUIFScenarioEditor.cpp, ??_G 0x19f4df). Requires ee/EEAQuickSaveLoad.h.
#ifndef EE_EESEAQUICKSAVELOAD_H
#define EE_EESEAQUICKSAVELOAD_H

class EESEAQuickSaveLoad : public EEAQuickSaveLoad {
public:
    EESEAQuickSaveLoad(const UString& inName) : EEAQuickSaveLoad(inName) {}
    virtual ~EESEAQuickSaveLoad() {}        // user-declared (??_G 0x19f4df stores this vtable first)
    virtual void Virtual4();                // +0x04 (0x1aa70e)
};

#endif
