// C:\EEX\Empire Earth\...\EEAction.h (RECONSTRUCTION; file path inferred). The base of every game action posted
// to the UI thread (EEUserInterface::PostAction): RTTI EEAction > UReferenceCounter, vtable 0x438a90 (slot 0 the
// scalar deleting destructor, shared 0x1e1b69 by every action that adds no destructible member; slots 4/8 pure).
// slot 0 is the inline virtual destructor (0x1e1b69 stores ??_7EEAction then the imported
// ??_7UReferenceCounter: a user-declared empty inline dtor); the pilot's `Virtual0` was that slot.
#ifndef EE_EEACTION_H
#define EE_EEACTION_H

#include "lle/LowLevelEngine.h"

class EEAction : public UReferenceCounter {
public:
    // inline; out-of-line copy 0x1a6695 (count, then +8, then the vptr: an init-list member)
    EEAction(bool inFlag) : mIsUndoable(inFlag) {}
    // inline: EEUIEHKPlayerEvent::ProcessEvent (0x1e952b) builds an EEAPlayerEvent with this
    // expanded (and byte [+8], 0 before the vptr store)
    EEAction() : mIsUndoable(false) {}
    virtual ~EEAction() {}              // +0x00 (??_G 0x1e1b69)
    virtual void Virtual4() = 0;        // +0x04
    virtual void Virtual8() = 0;        // +0x08
    bool mIsUndoable;                   // +0x08
};

#endif
