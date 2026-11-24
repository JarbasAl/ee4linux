// The reduced fight-memory rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIFightMemoryRules.h): which remembered
// enemies still count in a lost-fight judgement, and how far their guns reach. The expected values follow cover.c's
// cv_foe_mem at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIFightMemoryRules.h"

using namespace AgeOfAI::FightMemoryRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestConstants()
{
    Check(kMemoryMs == 180000, "a remembered soldier counts for three game minutes");
    Check(kLongRange == 30.0f, "a gun reaching thirty tiles adds no zone reach");
}

static void TestFresh()
{
    Check(Fresh(200000, 50000, 190000), "a soldier seen two and a half minutes ago is fresh");
    Check(Fresh(230000, 50000, 190000), "a soldier exactly three minutes old is still fresh");
    Check(!Fresh(230001, 50000, 190000), "a soldier older than three minutes is forgotten");
    Check(!Fresh(200000, 190000, 190000), "the record of the last scan is somewhere, not at a tile");
    Check(!Fresh(180000, 180001, 190000), "a record from the future does not count");
    Check(Fresh(50000, 40000, 49000), "a soldier seen at the current scan, but not at the last one, counts");
}

static void TestReach()
{
    Check(Reach(0.0f) == 0.0f, "a unit with no gun adds no reach");
    Check(Reach(4.5f) == 4.5f, "a short-range gun adds its reach");
    Check(Reach(29.9f) == 29.9f, "a long gun under thirty still adds its reach");
    Check(Reach(30.0f) == 0.0f, "a gun reaching thirty tiles adds no reach");
    Check(Reach(100.0f) == 0.0f, "a longer gun adds no reach either");
}

int main()
{
    TestConstants();
    TestFresh();
    TestReach();
    printf("ageofai fight memory: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
