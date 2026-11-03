// C:\EEX\Empire Earth\Actions\EEASaveGame.h (RECONSTRUCTION; file name inferred). The save-game action
// (RTTI EEASaveGame > EEAction > UReferenceCounter; vtable 0x439060; sizeof 0x30): ctor 0x4613, Virtual4 0x4672,
// ??_G 0x46c9, dtor 0x46e5 (EEASaveGame.cpp 0x4613..0x470f). The ctor assigns its members in the body (default ctors,
// then operator=). Names are provisional. Requires ee/EEAction.h, lle/UString.h.
#ifndef EE_EEASAVEGAME_H
#define EE_EEASAVEGAME_H

class EEASaveGame : public EEAction {
public:
    EEASaveGame(const UString& inFileName, bool inFlag, UString inDescription);
    virtual ~EEASaveGame();
    virtual void Virtual4();                // +0x04 (0x4672)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)

    UString mFileName;                      // +0x0c
    UString mDescription;                   // +0x1c
    bool mFlag;                             // +0x2c
};

#endif
