// The worker plan before farms (AgeOfAIWorkers.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/econ.c and
// dll/ai/food.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include "AgeOfAIWorkers.h"

namespace AgeOfAI {
namespace Workers {

void Needs(const long inDemand[7], const long inStock[7], const long inReserve[7], long inMineralMargin, long inTowerStone,
           long inCitizensMore, long inWoodCeiling, long outNeed[7])
{
    for (int r = 0; r < 7; r++)
        outNeed[r] = 0;
    outNeed[2] = inReserve[2] + 300 - inStock[2];
    for (int r = 3; r < 7; r++) {
        long theDemand = inDemand[r];
        if (r >= 4 && inMineralMargin >= 0 && theDemand > inReserve[r] + inMineralMargin)
            theDemand = inReserve[r] + inMineralMargin;
        outNeed[r] = theDemand - inStock[r];
        if (r >= 4 && inReserve[r] - inStock[r] > outNeed[r])
            outNeed[r] = inReserve[r] - inStock[r];
    }
    outNeed[4] += inTowerStone;
    long theMore = inCitizensMore > kFoodPlanCitizens ? kFoodPlanCitizens : inCitizensMore;
    if (theMore > 0)
        outNeed[2] += kFoodPerCitizen * theMore;
    if (inWoodCeiling >= 0) {
        long theRoom = inWoodCeiling - inStock[3];
        if (outNeed[3] > theRoom)
            outNeed[3] = theRoom > 0 ? theRoom : 0;
    }
}

bool Split(const long inNeed[7], const float inPerWorker[7], int inTotal, bool inReservePlan, int inCopperPlan,
           int inFoodFloor, unsigned long ioTarget[7])
{
    long theNeed[7];
    float theWeight[7] = {0};
    float theWeightSum = 0;
    long theSum = 0;
    for (int r = 0; r < 7; r++)
        theNeed[r] = inNeed[r] < 0 ? 0 : inNeed[r];
    for (int r = kFirst; r <= kLast; r++) {
        theSum += theNeed[r];
        float thePerWorker = inPerWorker[r];
        theWeight[r] = inReservePlan ? (float)theNeed[r] / (thePerWorker > 1.0f ? thePerWorker : 1.0f) : (float)theNeed[r];
        theWeightSum += theWeight[r];
    }
    if (theSum <= 0 || theWeightSum <= 0 || inTotal < 6)
        return false;
    const int theFloor[7] = {0, 0, 2, 2, 0, 0, 0};   // a few on food (growth) and on wood (building)
    int theRest = inTotal - 4, theGiven = 0;
    for (int r = kFirst; r <= kLast; r++) {
        int theCount = theFloor[r] + (int)((float)theRest * theWeight[r] / theWeightSum);
        ioTarget[r] = (unsigned long)theCount;
        theGiven += theCount;
    }
    for (int r = kFirst; theGiven < inTotal; r = r == kLast ? kFirst : r + 1)   // the rounding rest to the largest needs
        if (theNeed[r] > 0) {
            ioTarget[r]++;
            theGiven++;
        }
    if (inCopperPlan == 2) {   // the citizens' own food: half the workers on food, up to the floor
        int theFoodFloor = inTotal / 2 > inFoodFloor ? inFoodFloor : inTotal / 2;
        while ((int)ioTarget[2] < theFoodFloor) {
            int theFrom = -1;
            for (int r = 3; r <= kLast; r++)
                if ((int)ioTarget[r] > 2 && (theFrom < 0 || ioTarget[r] > ioTarget[theFrom]))
                    theFrom = r;
            if (theFrom < 0)
                break;
            ioTarget[theFrom]--;
            ioTarget[2]++;
        }
    }
    return true;
}

void Visible(const long inStock[7], const long inReserve[7], bool inCitizen, int inCopperPlan, long inWorkforce,
             long inMinWorkforce, long outVisible[7])
{
    for (int r = 0; r < 7; r++)
        outVisible[r] = inStock[r];
    for (int r = kFirst; r <= kLast; r++) {
        long theVisible = inStock[r] - inReserve[r];
        if (inCitizen && inCopperPlan != 0) {
            if (r != 2 || inWorkforce < 10 || inWorkforce < inMinWorkforce)
                theVisible = inStock[r];
        }
        outVisible[r] = theVisible < 0 ? 0 : theVisible > inStock[r] ? inStock[r] : theVisible;
    }
}

}
}
