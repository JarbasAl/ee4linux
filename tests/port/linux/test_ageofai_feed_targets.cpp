// The reduced food-target rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIFeedTargetsRules.h): the target food
// gatherers that pay the missing citizens, and the donors that target takes from. Expected values come from citizens.c.
#include <cstdio>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIFeedTargetsRules.h"

using namespace AgeOfAI::FeedTargetsRules;

static int sChecks, sFailures;

static void Check(bool inOk, const char* inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat);
    }
}

static void TestFeed()
{
    // One producer pays 50 food every 28 seconds, 107 a minute; at ten food a worker-minute that is eleven gatherers.
    Check(Feed(10, 1, 10.0f, 50, 28000) == 11, "one producer needs eleven food gatherers");
    Check(Feed(9, 3, 10.0f, 50, 28000) == 11, "three producers still serve one batch of nine");
    Check(Feed(19, 3, 10.0f, 50, 28000) == 22, "two batches need two producers");
    Check(Feed(21, 3, 10.0f, 50, 28000) == 33, "three batches need three producers");
    Check(Feed(21, 1, 10.0f, 50, 28000) == 11, "one producer does not pay three batches");
    Check(Feed(10, 1, 1.0f, 50, 28000) == 22, "the rate floor is five food a worker-minute");
    Check(Feed(10, 1, 10.0f, 50, 1000) == 60, "the train-time floor is five seconds");
    Check(Feed(0, 1, 10.0f, 50, 28000) == 0, "no missing citizens need no food gatherers");
    Check(Feed(10, 0, 10.0f, 50, 28000) == 0, "no producer needs no food gatherers");
}

static void TestTake()
{
    unsigned long theTarget[7] = {0, 0, 4, 20, 8, 14, 6};
    const long theFloor[7] = {0, 0, 0, 4, 2, 2, 2};
    Check(Take(theTarget, theFloor, 3, 6) == 3 && theTarget[3] == 19, "wood has the largest surplus");
    Check(Take(theTarget, theFloor, 4, 6) == 5 && theTarget[5] == 13, "gold has the largest remaining surplus");
    Check(Take(theTarget, theFloor, 4, 4) == 4 && theTarget[4] == 7, "a single range takes from that range");
    theTarget[4] = 2;
    Check(Take(theTarget, theFloor, 4, 4) == -1, "a target at its floor gives none");
    Check(theTarget[2] == 4 && theTarget[6] == 6, "the food and untouched targets stand");
}

static void TestRaise()
{
    unsigned long theTarget[7] = {0, 0, 4, 10, 5, 8, 3};
    const long theFloor[7] = {0, 0, 0, kWoodFloor, 2, 2, 2};
    Check(RaiseFood(theTarget, theFloor, 7) == 3 && theTarget[2] == 7, "three moves raise food by three");
    Check(theTarget[3] == 8 && theTarget[4] == 5 && theTarget[5] == 7 && theTarget[6] == 3,
          "the largest surplus gives first, ties going to the lower resource");
    Check(RaiseFood(theTarget, theFloor, 7) == 0 && theTarget[2] == 7, "a target already met moves none");
    Check(RaiseFood(theTarget, theFloor, 20) == 13, "the donors give all their surplus");
    Check(theTarget[2] == 20 && theTarget[3] == 4, "food takes wood down to its floor");
    Check(theTarget[4] == 2 && theTarget[5] == 2 && theTarget[6] == 2, "the metals stop at their floors");
    Check(RaiseFood(theTarget, theFloor, 30) == 0 && theTarget[2] == 20, "no donor leaves the target short");
}

int main()
{
    TestFeed();
    TestTake();
    TestRaise();
    printf("ageofai feed targets: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
