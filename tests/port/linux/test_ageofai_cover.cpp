// The reduced tower-cover rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICoverRules.h): when a unit has enemies
// near, when the fight is lost, where it stops and when it goes back. The expected values follow cover.c, worked out
// by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICoverRules.h"

using namespace AgeOfAI::CoverRules;

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
    Check(kCell == 8, "enemies are counted in eight-tile cells");
    Check(kLocalRadius == 10, "the fight is judged ten tiles from the unit");
    Check(kHoldAtMs == 20000, "a unit rests twenty seconds at cover");
    Check(kHoldMaxMs == 90000, "a unit rests at most ninety seconds");
}

static void TestEnemyNear()
{
    Check(EnemyNear(20, 20, 20, 20), "an enemy in the unit's own cell is near");
    Check(EnemyNear(20, 20, 24, 25), "an enemy in the cell next door is near");
    Check(EnemyNear(20, 20, 15, 15), "an enemy in the diagonal cell is near");
    Check(!EnemyNear(20, 20, 36, 20), "an enemy two cells away is not near");
    Check(!EnemyNear(20, 20, 20, 33), "an enemy two cells down is not near");
    Check(EnemyNear(-1, -1, -7, -7), "negative tiles fall in the same cell");
}

static void TestLostFight()
{
    Check(LostFight(0.49f, 0.5f), "a fight estimated below the ratio is lost");
    Check(!LostFight(0.5f, 0.5f), "the ratio itself is not lost");
    Check(!LostFight(1.0f, 0.5f), "a fight won is not lost");
    Check(LostFight(0.0f, 0.5f), "no own force at all is lost");
    Check(!LostFight(1e6f, 0.5f), "no enemy to fight is not lost");
}

static void TestCover()
{
    Check(AtCover(0, 0), "a unit standing on the cover fights there");
    Check(AtCover(4, 0), "four tiles from cover is close enough");
    Check(!AtCover(5, 0), "five tiles from cover is not");
    Check(!AtCover(3, 3), "a diagonal of three tiles each way is too far");
    long theX = 0, theY = 0;
    CoverTarget(30, 20, 10, 20, true, 20, 20, theX, theY);
    Check(theX == 12 && theY == 20, "the stop is two tiles toward the base");
    CoverTarget(30, 10, 10, 10, true, 18, 16, theX, theY);
    Check(theX == 12 && theY == 11, "a diagonal base rounds each tile of the stop");
    CoverTarget(30, 20, 10, 20, false, 0, 0, theX, theY);
    Check(theX == 10 && theY == 20, "with no base the unit stops on the cover");
}

static void TestHold()
{
    Check(!HoldDone(false, 20000, 0, 0), "twenty seconds is not up before arrival");
    Check(!HoldDone(true, 19999, 0, 0), "nineteen seconds at cover: still held");
    Check(HoldDone(true, 20000, 0, 0), "twenty seconds at cover: let go");
    Check(!HoldDone(true, 90000, 89000, 0), "ninety seconds exactly: still held");
    Check(HoldDone(false, 90001, 89000, 0), "past ninety seconds: let go even away from cover");
    Check(!HoldDone(true, 50000, 40000, 0), "ten seconds at cover: still held");
    Check(!HoldDone(true, 40000, 50000, 0), "the clock cannot go backwards");
}

int main()
{
    TestConstants();
    TestEnemyNear();
    TestLostFight();
    TestCover();
    TestHold();
    printf("ageofai cover: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
