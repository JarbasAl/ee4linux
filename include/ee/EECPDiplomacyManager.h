// C:\EEX\Empire Earth\AI Thread\EECPDiplomacyManager.h (RECONSTRUCTION; file name inferred). A computer
// player's diplomacy manager (RTTI EECPDiplomacyManager, no base; vtable 0x43c1a4: the destructor only; sizeof 8):
// ctor 0x7ed03 (called by EEComputerPlayer's ctor 0x2f1c8), ??_G 0x7ed15, dtor 0x7ed31. Names are provisional.
#ifndef EE_EECPDIPLOMACYMANAGER_H
#define EE_EECPDIPLOMACYMANAGER_H

class EEComputerPlayer;

class EECPDiplomacyManager {
public:
    EECPDiplomacyManager(EEComputerPlayer* inComputerPlayer);
    virtual ~EECPDiplomacyManager();
    void Update(unsigned long inTime);      // (EEComputerPlayer::Update 0x3f175 calls 0x157269, an ICF-folded empty `ret 4`) (provisional name)

    EEComputerPlayer* mComputerPlayer;      // +0x04 (assigned in the ctor body: stored after the vptr)
};

#endif
