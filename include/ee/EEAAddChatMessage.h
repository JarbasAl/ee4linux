// C:\EEX\Empire Earth\Actions\EEAAddChatMessage.h (RECONSTRUCTION; file name inferred). Puts a chat line into the
// Layout proven: EEAAddChatMessage sizeof 0x24 = its new-site size, ctor 0xbdb42 exact in EEMessage.cpp. Member names
// are provisional.
// chat log on the UI thread (RTTI EEAAddChatMessage > EEAction; vtable 0x43e340; sizeof 0x24). Posted by
// EEMChat::Execute (0xbcd63), which refuses the inline ctor: the out-of-line copy 0xbdb42 and ??_G 0xbdc00 sit in
// EEMessage.cpp's range. Names are provisional. Requires ee/EEAction.h.
#ifndef EE_EEAADDCHATMESSAGE_H
#define EE_EEAADDCHATMESSAGE_H

class EEAAddChatMessage : public EEAction {
public:
    EEAAddChatMessage(const UWideString& inText, bool inFromLocalPlayer)
    {
        this->mText = inText;
        this->mFromLocalPlayer = inFromLocalPlayer;
    }
    virtual ~EEAAddChatMessage() {}         // +0x00 (??_G 0xbdc00 stores the own vptr first: user-declared)
    virtual void Virtual4();                // +0x04 0x15c9
    virtual void Virtual8();                // +0x08 (ICF-folded empty `ret` 0x1720c6)

    UWideString mText;                      // +0x0c
    bool mFromLocalPlayer;                  // +0x20 (the sender's slot is the local one)
};

#endif
