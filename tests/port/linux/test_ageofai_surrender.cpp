// The reduced surrender rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISurrenderRules.h): when an AI with no citizens
// gives up. The expected values follow diplo.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISurrenderRules.h"

using namespace AgeOfAI::SurrenderRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestTiming()
{
    Check(kNoTrainMs == 60000, "a minute without a town centre");
    Check(kAnyNoCitizenMs == 180000, "three minutes without citizens in any case");
}

static void TestAtWar()
{
    Check(!ShouldResign(0, false, true), "a player with citizens does not give up");
    Check(!ShouldResign(kNoTrainMs - 1, false, true), "a minute is needed without a town centre");
    Check(ShouldResign(kNoTrainMs, false, true), "a minute without a town centre ends it");
    Check(ShouldResign(kNoTrainMs + 1, false, true), "longer without a town centre still ends it");
    Check(!ShouldResign(kAnyNoCitizenMs - 1, true, true), "a town centre holds it under three minutes");
    Check(ShouldResign(kAnyNoCitizenMs, true, true), "three minutes ends it even with a town centre");
}

static void TestAtPeace()
{
    Check(!ShouldResign(kAnyNoCitizenMs, true, false), "at peace a town centre holds it past three minutes");
    Check(!ShouldResign(kAnyNoCitizenMs - 1, true, false), "at peace a town centre holds it");
    Check(!ShouldResign(kNoTrainMs - 1, false, false), "at peace a minute is still needed");
    Check(ShouldResign(kNoTrainMs, false, false), "at peace no town centre after a minute ends it");
}

int main()
{
    TestTiming();
    TestAtWar();
    TestAtPeace();
    printf("ageofai surrender: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
