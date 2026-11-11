// The inline members of EEActiveGroup (ee/EEPlayer.h) that need the goal classes (RECONSTRUCTION; file name
// inferred). Requires ee/EEPlayer.h, ee/EEUnitGoal.h, ee/EEUnit.h.
#ifndef EE_EEACTIVEGROUP_H
#define EE_EEACTIVEGROUP_H

#ifdef EE_PORT
__attribute__((used)) inline void EEActiveGroup::ClearGoal()
#else
inline void EEActiveGroup::ClearGoal()
#endif
{
    if (this->mGoal)
        delete this->mGoal;
    this->mGoal = NULL;
}

#endif
