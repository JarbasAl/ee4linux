// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGGOptions.h (RECONSTRUCTION; file name inferred).
// The in-game form's options group (RTTI EEUIFMGGOptions > EEUIFMGGroup, vtable 0x44ac2c; EEUIFMGGOptions.cpp 0x267c34..
// 0x267c73: ctor, ??_G, dtor). The main game form news it (8 bytes) into +0x4fc (0x265275). Slots +04/+08/+0c are
// empty (ICF-folded `ret` 0x1e123d), +0x14 returns false (folded 0x27c0b4: not the base's 0xac9f3, so overridden).
// Requires ee/EEUIFMGMainGroup.h.
#ifndef EE_EEUIFMGGOPTIONS_H
#define EE_EEUIFMGGOPTIONS_H

class EEUIFMGGOptions : public EEUIFMGGroup {
public:
    EEUIFMGGOptions(EEUIFMainGame* inOwner);   // 0x267c34
    virtual ~EEUIFMGGOptions();                // +0x00 (??_G 0x267c4c, dtor 0x267c68)
    virtual void Refresh();                 // +0x04 (empty)
    virtual void Virtual08();               // +0x08 (empty)
    virtual void Select();                  // +0x0c (empty)
    virtual bool WantsRefresh();            // +0x14 (false)
};

#endif
