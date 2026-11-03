// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGGCivBuilder.h (RECONSTRUCTION; file name inferred).
// The in-game form's civilization builder group (RTTI EEUIFMGGCivBuilder > EEUIFMGGroup, vtable 0x44abf4; EEUIFMGGCivBuilder.cpp 0x267bb6..
// 0x267bf5: ctor, ??_G, dtor). The main game form news it (8 bytes) into +0x500 (0x265252). Slots +04/+08/+0c are
// empty (ICF-folded `ret` 0x1e123d), +0x14 returns false (folded 0x27c0b4: not the base's 0xac9f3, so overridden).
// Requires ee/EEUIFMGMainGroup.h.
#ifndef EE_EEUIFMGGCIVBUILDER_H
#define EE_EEUIFMGGCIVBUILDER_H

class EEUIFMGGCivBuilder : public EEUIFMGGroup {
public:
    EEUIFMGGCivBuilder(EEUIFMainGame* inOwner);   // 0x267bb6
    virtual ~EEUIFMGGCivBuilder();                // +0x00 (??_G 0x267bce, dtor 0x267bea)
    virtual void Refresh();                 // +0x04 (empty)
    virtual void Virtual08();               // +0x08 (empty)
    virtual void Select();                  // +0x0c (empty)
    virtual bool WantsRefresh();            // +0x14 (false)
};

#endif
