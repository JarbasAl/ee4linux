// (RECONSTRUCTION; file path inferred). The action that removes a
// player (RTTI EEARemovePlayer > EEAction > UReferenceCounter; vtable 0x43e42c; sizeof 0x10; ctor inline: the
// lobby's Cancel 0x1da547 builds it with the server's own slot 0xfe). Requires ee/EEAction.h.
#ifndef EE_EEAREMOVEPLAYER_H
#define EE_EEAREMOVEPLAYER_H

class EEARemovePlayer : public EEAction {
public:
    EEARemovePlayer(unsigned char inPlayer) : mB0D(false), mIsExiting(false) { this->mPlayer = inPlayer; }
    // (EEServer::ProcessSystemMessage: a client's disconnection notification; +0x0c and +0x0e after the vptr)
    EEARemovePlayer(unsigned char inPlayer, bool inExiting) : mB0D(false) { this->mPlayer = inPlayer; this->mIsExiting = inExiting; }
    virtual void Virtual4();                // +0x04
    virtual void Virtual8();                // +0x08

    unsigned char mPlayer;                  // +0x0c
    bool mB0D;                              // +0x0d
    bool mIsExiting;                        // +0x0e
};

#endif
