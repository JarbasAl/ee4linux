// The inline members of EEActiveGroup (ee/EEPlayer.h) that need the goal classes (RECONSTRUCTION; file name
// inferred). Requires ee/EEPlayer.h, ee/EEUnitGoal.h, ee/EEUnit.h.
#ifndef EE_EEACTIVEGROUP_H
#define EE_EEACTIVEGROUP_H

inline void EEActiveGroup::ClearGoal()
{
    if (this->mGoal)
        delete this->mGoal;
    this->mGoal = NULL;
}

#endif
