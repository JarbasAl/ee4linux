// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGGDiplomacy.h (RECONSTRUCTION; file name inferred).
// The in-game form's diplomacy group (RTTI EEUIFMGGDiplomacy > EEUIFMGGroup, vtable 0x44ac10; EEUIFMGGDiplomacy.cpp 0x267bf5..
// 0x267c34: ctor, ??_G, dtor). The main game form news it (8 bytes) into +0x4f8 (0x265295). Slots +04/+08/+0c are
// empty (ICF-folded `ret` 0x1e123d), +0x14 returns false (folded 0x27c0b4: not the base's 0xac9f3, so overridden).
// Requires ee/EEUIFMGMainGroup.h.
#ifndef EE_EEUIFMGGDIPLOMACY_H
#define EE_EEUIFMGGDIPLOMACY_H

class EEUIFMGGDiplomacy : public EEUIFMGGroup {
public:
    EEUIFMGGDiplomacy(EEUIFMainGame* inOwner);   // 0x267bf5
    virtual ~EEUIFMGGDiplomacy();                // +0x00 (??_G 0x267c0d, dtor 0x267c29)
    virtual void Refresh();                 // +0x04 (empty)
    virtual void Virtual08();               // +0x08 (empty)
    virtual void Select();                  // +0x0c (empty)
    virtual bool WantsRefresh();            // +0x14 (false)
};

#endif
