// The tower slots of the defence layout (DefenceSpot in AgeOfAIDanger.cpp): where on each front the towers go, and in
// which order.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/defence.c
// (def_slot) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_TOWERS_H
#define AGEOFAI_TOWERS_H

#include <math.h>

namespace AgeOfAI {
namespace TowerRules {

const float kSpacing = 3.0f;   // tiles between two slots across a front

// A front: its middle tile, defence_distance out from the base towards an enemy, and that direction as a unit vector
struct Front {
    long mFX, mFY;
    float mDX, mDY;
};

// Slot i in build order: on each front its middle, then one slot to the left and one to the right; then the spots by the
// outlying towns (inOut); then two more on each front, two slots out on each side. False when there is no slot i.
inline bool Slot(const Front inFront[], int inFronts, const long inOut[][2], int inOuts, int i, long& outX, long& outY)
{
    static const int kFirst[3] = {0, -1, 1}, kMore[2] = {-2, 2};
    for (int k = 0; k < inFronts; k++, i -= 3)
        if (i < 3) {
            outX = inFront[k].mFX + (long)lroundf(-inFront[k].mDY * kSpacing * (float)kFirst[i]);
            outY = inFront[k].mFY + (long)lroundf(inFront[k].mDX * kSpacing * (float)kFirst[i]);
            return true;
        }
    if (i < inOuts) {
        outX = inOut[i][0];
        outY = inOut[i][1];
        return true;
    }
    i -= inOuts;
    for (int k = 0; k < inFronts; k++, i -= 2)
        if (i < 2) {
            outX = inFront[k].mFX + (long)lroundf(-inFront[k].mDY * kSpacing * (float)kMore[i]);
            outY = inFront[k].mFY + (long)lroundf(inFront[k].mDX * kSpacing * (float)kMore[i]);
            return true;
        }
    return false;
}

}
}

#endif
