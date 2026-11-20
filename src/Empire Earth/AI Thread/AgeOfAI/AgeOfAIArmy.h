// The rules of the outdated units' attack (AgeOfAIArmy.cpp) that need no game types: which unit is outdated, and which
// enemy building the units attack.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/army.c
// (ou_outdated, outdated_attack) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_ARMY_H
#define AGEOFAI_ARMY_H

#include <cstddef>

namespace AgeOfAI {
namespace ArmyRules {

const unsigned long kOutdatedEvery = 300000;   // game milliseconds between two looks
const int kOutdatedLeast = 8;                   // free outdated units at least, or none go
const int kOutdatedMost = 256;                  // units in one attack at most
const int kGroupsMost = 64;                     // attack groups walked for their members
const int kMembersMost = 4096;                  // members of attack subgroups noted at most

const unsigned long kCitizenEvery = 5000;       // game milliseconds between two citizen-attack scans
const int kCitizenMembersMost = 1000;           // members counted in one subgroup, as upstream's n < 1000

const unsigned long kRetargetEvery = 10000;     // game milliseconds between two attack-group retarget scans

// A land unit (class 5, not a citizen) whose type's tech has its last epoch behind the player's: no line upgrade reaches it
// again
inline bool IsOutdated(unsigned long inCategory, bool inCitizen, long inLastEpoch, long inEpoch)
{
    const unsigned long kLand = 5;
    return inCategory == kLand && !inCitizen && inLastEpoch < inEpoch;
}

// The enemy building nearest the units' middle that is not a defence; the nearest defence (a tower or a fortress) only when
// no other is left. Offer each candidate in turn: the first of equal distances stays.
template <class T>
class TargetChoice {
public:
    void Offer(T* inUnit, long inDist2, bool inDefence)
    {
        if (inDefence) {
            if (this->mDefence == NULL || inDist2 < this->mDefenceDist2) {
                this->mDefence = inUnit;
                this->mDefenceDist2 = inDist2;
            }
        } else if (this->mBest == NULL || inDist2 < this->mBestDist2) {
            this->mBest = inUnit;
            this->mBestDist2 = inDist2;
        }
    }
    T* Chosen() const { return this->mBest != NULL ? this->mBest : this->mDefence; }

private:
    T* mBest = NULL;
    T* mDefence = NULL;
    long mBestDist2 = 0, mDefenceDist2 = 0;
};

}
}

#endif
