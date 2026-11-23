// The food-target half of Grille's citizen_plan FEED (dll/ai/citizens.c cz_feed and cz_targets at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0). From the Copper Age the food target gets the gatherers that
// pay the citizens a producer still has to train. Upstream takes those gatherers from wood above its late target and
// from metal targets above the miners; this reduced rule protects wood down to its floor of four and the metals down
// to the miners. It changes targets only: upstream's cz_take of actual workers, the mine-slot pass and the goal
// hook are absent here.
#ifndef AGEOFAI_FEED_TARGETS_RULES_H
#define AGEOFAI_FEED_TARGETS_RULES_H

namespace AgeOfAI {
namespace FeedTargetsRules {

const long kCitizensPerProducer = 10;   // a finished Town Center or Capitol makes about ten citizens in five minutes
const long kMinTrainMs = 5000;          // upstream's floor on [autotest] citizen_train_ms
const float kMinPerWorker = 5.0f;       // upstream's floor on the measured food per worker-minute
const long kWoodFloor = 4;              // upstream's wood floor when no late wood target applies

// The gatherers food needs to keep the finished producers training the missing citizens without a pause. One producer
// serves ten citizens, so extra producers do not raise the need. The food cost and the train time are per citizen.
inline long Feed(long inMissing, long inProducers, float inPerWorker, long inFoodCost, long inTrainMs)
{
    if (inMissing <= 0 || inProducers <= 0 || inFoodCost <= 0)
        return 0;
    float thePerWorker = inPerWorker < kMinPerWorker ? kMinPerWorker : inPerWorker;
    float theTrainMs = inTrainMs < kMinTrainMs ? (float)kMinTrainMs : (float)inTrainMs;
    long theNeed = (inMissing + kCitizensPerProducer - 1) / kCitizensPerProducer;
    long theProducers = inProducers < theNeed ? inProducers : theNeed;
    float thePerMinute = (float)theProducers * (float)inFoodCost * 60000.0f / theTrainMs;
    long theFeed = (long)(thePerMinute / thePerWorker + 0.99f);
    return theFeed > 0 ? theFeed : 0;
}

// One gatherer from the target with the largest surplus over its floor, in [inFirst, inLast]. -1: none can give.
inline int Take(unsigned long ioTarget[7], const long inFloor[7], int inFirst, int inLast)
{
    int theFrom = -1;
    for (int r = inFirst; r <= inLast; r++) {
        if ((long)ioTarget[r] <= inFloor[r])
            continue;
        if (theFrom < 0 || (long)ioTarget[r] - inFloor[r] > (long)ioTarget[theFrom] - inFloor[theFrom])
            theFrom = r;
    }
    if (theFrom >= 0)
        ioTarget[theFrom]--;
    return theFrom;
}

// Raise the food target to inFeed, taking from the other targets. Returns the number of gatherers moved and, when
// asked, how many came from wood and from the metals.
inline long RaiseFood(unsigned long ioTarget[7], const long inFloor[7], long inFeed,
                      long* outFromWood = NULL, long* outFromMetals = NULL)
{
    long theMoved = 0;
    while ((long)ioTarget[2] < inFeed) {
        int theFrom = Take(ioTarget, inFloor, 3, 6);
        if (theFrom < 0)
            break;
        if (outFromWood != NULL && theFrom == 3)
            ++*outFromWood;
        if (outFromMetals != NULL && theFrom >= 4)
            ++*outFromMetals;
        ioTarget[2]++;
        theMoved++;
    }
    return theMoved;
}

}  // namespace FeedTargetsRules
}  // namespace AgeOfAI

#endif
