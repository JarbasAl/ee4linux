// The rules of counter-fire (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICounterFire.h): which enemy is a shooter, and how
// many soldiers go at it. The expected values follow upstream's loop in counterfire.c, worked out by hand.
#include <cstdio>
#include <string>
#include <vector>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICounterFire.h"

using namespace AgeOfAI::CounterFireRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void CheckTake(int inGot, int inWant, const std::string& inWhat)
{
    Check(inGot == inWant, inWhat + " (got " + std::to_string(inGot) + ", want " + std::to_string(inWant) + ")");
}

static void TestShooter()
{
    Check(IsShooter(5.0f, 3.0f, 17), "range 5 at a victim of range 3, 17 tiles squared: beyond 4 squared, a shooter");
    Check(!IsShooter(5.0f, 3.0f, 16), "the same at 16 tiles squared: within reach, no shooter");
    Check(!IsShooter(4.9f, 0.0f, 100), "range 4.9: too short to be a shooter, however far");
    Check(IsShooter(5.0f, 0.0f, 2), "a victim of range 0 reaches 1 tile: 2 tiles squared is beyond");
    Check(IsShooter(12.0f, 6.0f, 50), "a mortar at 12 against a tank of range 6, 50 > 49: a shooter");
    Check(!IsShooter(12.0f, 6.0f, 49), "the same at 49: the tank reaches it");
}

static void TestTake()
{
    std::vector<int> theCalls;
    int theTake = Take(20, 0, 0.0f, [&](int inTaken) { theCalls.push_back(inTaken); return 0.3f * inTaken; });
    CheckTake(theTake, 8, "ratio 0.3 per soldier: 4 give 1.2, 8 give 2.4");
    Check(theCalls == std::vector<int>({4, 8}), "judged after each four, at 4 and 8");

    CheckTake(Take(6, 0, 0.0f, [](int inTaken) { return 0.2f * inTaken; }), -1,
              "six candidates at 0.2 each reach 1.2: too weak, nobody goes");
    theCalls.clear();
    Take(6, 0, 0.0f, [&](int inTaken) { theCalls.push_back(inTaken); return 0.2f * inTaken; });
    Check(theCalls == std::vector<int>({4, 6}), "the last step takes the two left");

    CheckTake(Take(10, 3, 1.5f, [](int) { return 9.0f; }), 0, "those already on it win by 1.5: nobody more");
    CheckTake(Take(0, 0, 1.0f, [](int) { return 9.0f; }), -1, "no candidates and short of 1.5: too weak");
    CheckTake(Take(10, 60, 1.0f, [](int) { return 9.0f; }), -1, "60 already on it: nobody more, still short");
    CheckTake(Take(20, 58, 0.0f, [](int inTaken) { return 0.5f * inTaken; }), 4,
              "58 on it: the cap is checked before a step, so four more go (62)");
    CheckTake(Take(20, 58, 0.0f, [](int inTaken) { return 0.2f * inTaken; }), -1,
              "58 on it and four more reach 0.8: the cap stops the next step, too weak");
}

int main()
{
    TestShooter();
    TestTake();
    printf("ageofai counter-fire: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
