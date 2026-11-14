// The places of the early game (AgeOfAIEarlyGeometry.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/early.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include <cmath>

#include "AgeOfAIEarlyGeometry.h"

namespace AgeOfAI {
namespace Early {

void FrontToCentre(int inBaseX, int inBaseY, int inMapSize, float& outDX, float& outDY)
{
    float theDX = (float)(inMapSize / 2 - inBaseX), theDY = (float)(inMapSize / 2 - inBaseY);
    float theLength = sqrtf(theDX * theDX + theDY * theDY);
    if (theLength < 1) {
        outDX = 1;
        outDY = 0;
    } else {
        outDX = theDX / theLength;
        outDY = theDY / theLength;
    }
}

void CoreSlot(int inBaseX, int inBaseY, float inDX, float inDY, int inSlot, float inSumX, float inSumY, int& outX, int& outY)
{
    float theDX = inDX, theDY = inDY;
    if (inSlot == 1) {
        float thePX = -inDY, thePY = inDX;
        if (thePX * inSumX + thePY * inSumY < 0) {
            thePX = -thePX;
            thePY = -thePY;
        }
        theDX = thePX;
        theDY = thePY;
    }
    outX = inBaseX + (int)lroundf(theDX * kCoreDistance);
    outY = inBaseY + (int)lroundf(theDY * kCoreDistance);
}

void BaitSpot(int inTowerX, int inTowerY, int inEnemyX, int inEnemyY, float& outX, float& outY)
{
    float theX = (float)(inTowerX - inEnemyX), theY = (float)(inTowerY - inEnemyY);
    float theLength = sqrtf(theX * theX + theY * theY);
    outX = (float)inTowerX + 0.5f + (theLength > 0.5f ? theX / theLength * 1.5f : 0.0f);
    outY = (float)inTowerY + 0.5f + (theLength > 0.5f ? theY / theLength * 1.5f : 0.0f);
}

}
}
