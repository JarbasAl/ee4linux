// The rules of counter-siege and the citizen militia (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIDefence.h): when the
// citizens fight and when the soldiers go. The expected values follow upstream's militia_answer and counter_siege in
// defence.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIDefence.h"

using namespace AgeOfAI::DefenceRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestLimits()
{
    Check(kMilitiaMostEnemies == 6, "the citizens take on 6 enemies at most");
    Check(kMilitiaPerEnemy == 3, "3 citizens per enemy at most");
    Check(kCounterSiegeMost == 5, "5 soldiers on a siege threat at most");
}

static void TestCitizensStand()
{
    Check(PowerRatio(300.0f, 100.0f) >= kMilitiaStand, "300 against 100: 3 times, the citizens go");
    Check(PowerRatio(299.0f, 100.0f) < kMilitiaStand, "299 against 100: under 3 times, they stay");
    Check(PowerRatio(5.0f, 0.0f) >= kMilitiaStand, "no enemy strength (a foundation's builders gone): they go");
}

static void TestEarlyMilitia()
{
    // 2 damage per second for 10 seconds on 4 citizens of 100 hit points in all: 2 * 10 * 4 / 100 = 0.8 citizens
    Check(CitizensLost(2.0f, 10.0f, 4, 100.0f) < 1.0f, "0.8 citizens lost: they go");
    // 5 * 10 * 4 / 200 = 1.0
    Check(CitizensLost(5.0f, 10.0f, 4, 200.0f) >= 1.0f, "1 citizen lost: refused");
    Check(CitizensLost(5.0f, 10.0f, 4, 200.0f) == 1.0f, "5 * 10 * 4 / 200 is 1");
}

static void TestCaught()
{
    Check(CaughtFight(true, 1.2f, 1.0f, 0.5f, 0.5f), "attacked by soldiers 1.2 times as fast, a share of 0.5: fight");
    Check(!CaughtFight(true, 1.1f, 1.0f, 0.5f, 0.5f), "1.1 times as fast: the citizens can run");
    Check(!CaughtFight(false, 2.0f, 1.0f, 1.0f, 0.5f), "the soldiers do not attack: no fight");
    Check(!CaughtFight(true, 2.0f, 1.0f, 0.49f, 0.5f), "a share under the minimum: no fight");
}

static void TestCounterSiege()
{
    // 2 enemy soldiers near the threat: 2 * 2 + 3 = 7 of the player's soldiers needed
    Check(SoldiersGo(5, 2, 2), "5 free and 2 already there against 2 guards: 7, they go");
    Check(!SoldiersGo(4, 2, 2), "6 against 2 guards: too few");
    Check(SoldiersGo(3, 0, 0), "no guards: 3 soldiers go");
    Check(!SoldiersGo(2, 0, 0), "no guards: 2 soldiers are too few");
}

int main()
{
    TestLimits();
    TestCitizensStand();
    TestEarlyMilitia();
    TestCaught();
    TestCounterSiege();
    printf("ageofai defence: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
