// C:\EEX\Low-Level Engine\Network Engine\NEPlayer.h (RECONSTRUCTION). A connected player as the synchronous
// communications see it. Every member is inline (exported COMDATs: ctor/Get/SetReadyToPlay at the head of
// NELagLossSimulator.cpp 0x3bc2d, the rest ICF-folded elsewhere). Requires UWideString.
#ifndef LLE_NEPLAYER_H
#define LLE_NEPLAYER_H

class LLE_API NEPlayer {                    // proven sizeof 0xc (no user copy: operator= folded at 0x12ad2)
public:
    NEPlayer(UWideString* inPlayerName, unsigned char inSendToSlot, unsigned long inApplicationStartTime)
    {
        this->mSendToSlot = inSendToSlot;
        this->mPlayerName = new UWideString(*inPlayerName);
        this->mReadyToPlay = false;
        this->mApplicationStartTime = inApplicationStartTime;
    }
    ~NEPlayer() { delete this->mPlayerName; }
    bool GetReadyToPlay() const { return this->mReadyToPlay; }
    void SetReadyToPlay(bool inReadyToPlay) { this->mReadyToPlay = inReadyToPlay; }
    UWideString* GetPlayerName() { return this->mPlayerName; }
    unsigned char GetSendToSlot() { return this->mSendToSlot; }
    unsigned long GetApplicationStartTime() const { return this->mApplicationStartTime; }

    UWideString* mPlayerName;               // +0x00
    unsigned char mSendToSlot;              // +0x04
    bool mReadyToPlay;                      // +0x05
    unsigned long mApplicationStartTime;    // +0x08
};

#endif
