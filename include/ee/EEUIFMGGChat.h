// C:\EEX\Empire Earth\User Interface Thread\EEUIFMGGChat.h (RECONSTRUCTION; file name inferred).
// The in-game form's chat group (RTTI EEUIFMGGChat > EEUIFMGGroup, vtable 0x44abd8; EEUIFMGGChat.cpp 0x267b77..
// 0x267bb6: ctor, ??_G, dtor). The main game form news it (8 bytes) into +0x4f4 (0x2652b5). Slots +04/+08/+0c are
// empty (ICF-folded `ret` 0x1e123d), +0x14 returns false (folded 0x27c0b4: not the base's 0xac9f3, so overridden).
// Requires ee/EEUIFMGMainGroup.h.
#ifndef EE_EEUIFMGGCHAT_H
#define EE_EEUIFMGGCHAT_H

class EEUIFMGGChat : public EEUIFMGGroup {
public:
    EEUIFMGGChat(EEUIFMainGame* inOwner);   // 0x267b77
    virtual ~EEUIFMGGChat();                // +0x00 (??_G 0x267b8f, dtor 0x267bab)
    virtual void Refresh();                 // +0x04 (empty)
    virtual void Virtual08();               // +0x08 (empty)
    virtual void Select();                  // +0x0c (empty)
    virtual bool WantsRefresh();            // +0x14 (false)
};

#endif
