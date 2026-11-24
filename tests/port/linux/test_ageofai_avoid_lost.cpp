// The reduced lost-fight rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIAvoidLostRules.h): which cells a refresh
// marks, and the scorer's reach exception. The expected values follow cover.c and attack.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIAvoidLostRules.h"

using namespace AgeOfAI::AvoidLostRules;

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
    Check(kCell == 8, "a fight is judged in eight-tile cells");
    Check(kFoe == 10, "enemy fighters count within ten tiles");
    Check(kReachCap == 15, "the enemy reach is capped at fifteen tiles");
    Check(kRefreshMs == 4000, "the cells are judged again every four seconds");
    Check(kReachSlack == 2.0f, "a soldier fights inside its range plus two");
}

static void TestZoneRadius()
{
    Check(ZoneRadius(0.0f) == 10.0f, "no enemy reach gives a ten-tile zone");
    Check(ZoneRadius(4.0f) == 14.0f, "a four-tile reach widens the zone");
    Check(ZoneRadius(15.0f) == 25.0f, "the reach cap gives a twenty-five-tile zone");
    Check(ZoneRadius(23.0f) == 25.0f, "a longer reach stays at the cap");
    Check(ZoneRadius(-1.0f) == 10.0f, "a negative reach does not shrink the zone");
    Check(OwnRadius(ZoneRadius(15.0f)) == 29.0f, "own fighters count four tiles past the zone edge");
}

static void TestJudgement()
{
    Check(IsLost(0.999f), "a ratio below one is a lost fight");
    Check(!IsLost(1.0f), "a ratio of one holds the fight");
    Check(!IsLost(1.5f), "a winning fight is not lost");
    Check(IsLost(0.0f), "no own fighters makes the fight lost");
}

int main()
{
    TestConstants();
    TestZoneRadius();
    TestJudgement();
    printf("ageofai avoid lost: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
