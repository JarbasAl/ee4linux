// Aiming ahead of a moving target (AgeOfAILead.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/aim.c
// at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include <cmath>

#include "AgeOfAILead.h"

namespace AgeOfAI {
namespace Lead {

float LeadAim(float inTargetX, float inTargetY, float inAimX, float inAimY, bool inCentred, float inVX, float inVY,
              float inFlightTime, const std::function<float(float, float)>& inFlight, float& outX, float& outY)
{
    float theTime = inFlightTime;
    outX = inAimX;
    outY = inAimY;
    for (int i = 0; i < kIterations; i++) {
        if (theTime > kMaxFlight)
            theTime = kMaxFlight;
        float theX = inCentred ? inTargetX + inVX * theTime : inAimX + inVX * theTime;
        float theY = inCentred ? inTargetY + inVY * theTime : inAimY + inVY * theTime;
        if (inCentred) {
            theX = floorf(theX) + 0.5f;
            theY = floorf(theY) + 0.5f;
        }
        float theNext = inFlight(theX, theY);
        if (theNext < 0)
            break;
        outX = theX;
        outY = theY;
        theTime = theNext;
    }
    return theTime;
}

}
}
