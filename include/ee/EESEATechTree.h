// C:\EEX\Empire Earth\Scenario Editor Actions\EESEATechTree.h (RECONSTRUCTION; file name inferred). The
// editor's tech tree action (RTTI EESEATechTree > EEAction; vtable 0x442f8c, sizeof 0x1c; ctor inline at its users).
// Requires ee/EEAction.h.
#ifndef EE_EESEATECHTREE_H
#define EE_EESEATECHTREE_H

#include "ee/EmpireEarth.h"
#include "ee/EEUserInterface.h"   // the inline ctor posts itself

class EESEATechTree : public EEAction {
public:
    // inline (EESEBSelect::Select 0x172ffd builds it: the members before the vptr, then it posts itself; needs
    // ee/EEUserInterface.h)
    EESEATechTree(long inFirst, long inLast, long inPlayer, bool inFlag)
        : mFirst(inFirst), mLast(inLast), mPlayer(inPlayer), mIsLeftList(inFlag)
    {
        gUserInterface->PostAction(this);
    }
    virtual void Virtual4();                // +0x04 (0x1aaa78)
    virtual void Virtual8();                // +0x08 (the shared empty slot 0x1720c6)

    long mFirst;                            // +0x0c
    long mLast;                             // +0x10
    long mPlayer;                           // +0x14
    bool mIsLeftList;                       // +0x18 (picks the list box: 0x154 when set, else 0x155)
};

#endif
