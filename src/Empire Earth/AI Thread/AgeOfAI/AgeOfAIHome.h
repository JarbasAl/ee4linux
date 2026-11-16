// The rules of the idle citizens' way home (CitizensHome in AgeOfAIDanger.cpp) that need no game types: which citizen
// goes, and where it stops.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/econ.c
// (citizens_home) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_HOME_H
#define AGEOFAI_HOME_H

#include <math.h>

namespace AgeOfAI {
namespace HomeRules {

const long kTownReach = 20;    // tiles: the base belongs to a town this near, and a town's buildings are this near it
const long kIdleFar = 15;      // tiles from the nearest finished town past which an idle citizen goes home
const long kBaseNear = 3;      // tiles: a walk that ends this near the base is a walk to the base
const float kTownCloser = 10.0f;   // tiles a town must be nearer than the base to take a citizen walking to the base
const float kSpot = 4.0f;      // tiles from the town's tile, towards the citizen, where it stops

// An idle citizen whose squared distance to the nearest finished town is past kIdleFar
inline bool IdleFar(long inNearestD2)
{
    return inNearestD2 > kIdleFar * kIdleFar;
}

// A walk whose end lies within kBaseNear of the base (inDX, inDY: the end less the base)
inline bool HeadingToBase(long inDX, long inDY)
{
    return inDX * inDX + inDY * inDY <= kBaseNear * kBaseNear;
}

// A citizen walking to the base goes to the town instead only when that town is kTownCloser tiles nearer
inline bool TownCloser(float inTownDistance, float inBaseDistance)
{
    return inTownDistance + kTownCloser <= inBaseDistance;
}

// Where the citizen stops: kSpot tiles from the middle of the town's tile, on the citizen's side (east when it stands
// on the town's tile)
inline void Spot(long inTownX, long inTownY, long inX, long inY, float& outX, float& outY)
{
    float theDX = (float)(inX - inTownX), theDY = (float)(inY - inTownY);
    float theLength = sqrtf(theDX * theDX + theDY * theDY);
    outX = (float)inTownX + 0.5f + (theLength > 0 ? theDX / theLength * kSpot : kSpot);
    outY = (float)inTownY + 0.5f + (theLength > 0 ? theDY / theLength * kSpot : 0.0f);
}

}
}

#endif
