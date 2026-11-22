// The reduced pop_follow_human rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIPopulationRules.h): when an AI stops
// following the humans, its cap becomes its own game limit. The expected values follow hook_pop_cap in econ.c,
// worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIPopulationRules.h"

using namespace AgeOfAI::PopulationRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestFollow()
{
    Check(Cap(70, 120, 1) == 70, "following keeps the weighted cap");
    Check(Cap(70, 120, 2) == 70, "any nonzero setting follows the humans");
    Check(Cap(70, 120, -1) == 70, "a negative setting still follows the humans");
    Check(Cap(120, 120, 1) == 120, "following a cap equal to the game limit changes nothing");
    Check(Cap(0, 120, 1) == 0, "following keeps even a zero weighted cap for the caller to rebuild");
}

static void TestOwnLimit()
{
    Check(Cap(70, 120, 0) == 120, "not following uses the game limit");
    Check(Cap(120, 120, 0) == 120, "not following a cap equal to the game limit changes nothing");
    Check(Cap(0, 120, 0) == 120, "not following replaces an unset weighted cap with the game limit");
    Check(Cap(200, 120, 0) == 120, "not following lowers a weighted cap above the game limit");
    Check(Cap(-5, 120, 0) == 120, "not following replaces a negative weighted cap");
}

int main()
{
    TestFollow();
    TestOwnLimit();
    printf("ageofai population: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
