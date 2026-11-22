// The reduced train_queue rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITrainQueueRules.h): cut a request to what
// keeps each finished producer's one-in-the-making plus queue at the depth. Expected values come from econ.c.
#include <cstdio>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITrainQueueRules.h"

using namespace AgeOfAI::TrainQueueRules;

static int sChecks, sFailures;

static void Check(bool inOk, const char* inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat);
    }
}

static void TestProducerRoom()
{
    Check(ProducerRoom(3, 0) == 3, "an empty producer holds the depth");
    Check(ProducerRoom(3, 1) == 2, "the unit in the making takes one slot");
    Check(ProducerRoom(3, 2) == 1, "one queued unit leaves a slot");
    Check(ProducerRoom(3, 3) == 0, "a full producer has no room");
    Check(ProducerRoom(3, 5) == 0, "an over-deep producer has no room");
    Check(ProducerRoom(0, 0) == 0, "a zero depth gives no room");
}

static void TestDecide()
{
    long theCount = -1;
    Check(Decide(5, 8, true, theCount) == kKeep && theCount == 5, "room for the whole request keeps it");
    Check(Decide(5, 5, true, theCount) == kKeep && theCount == 5, "exactly enough room keeps it");
    Check(Decide(5, 3, true, theCount) == kCut && theCount == 3, "less room cuts to the room");
    Check(Decide(5, 0, true, theCount) == kRefuse && theCount == 0, "no room refuses");
    Check(Decide(5, 0, false, theCount) == kKeep && theCount == 5, "no known producer leaves the game to decide");
    Check(Decide(1, 0, true, theCount) == kRefuse && theCount == 0, "a single unit is refused with no room");
}

int main()
{
    TestProducerRoom();
    TestDecide();
    printf("ageofai train queue: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
