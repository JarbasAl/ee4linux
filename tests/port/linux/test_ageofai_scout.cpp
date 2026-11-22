// The reduced towns-scout rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIScoutRules.h): the scan cells, the front
// they must stay behind, the nearest town, the fastest soldier and the turn-back. The expected values follow
// towns.c tn_scout, tn_front and tn_scout_watch, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIScoutRules.h"

using namespace AgeOfAI::ScoutRules;

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
    Check(kCell == 8, "the map is scanned in eight-tile cells");
    Check(kSight == 12, "an enemy twelve tiles away sends the scout home");
    Check(kPause == 90000, "the next scout waits ninety game seconds");
    Check(kHome == 40, "the soldier is picked within forty tiles of the base");
}

static void TestDistances()
{
    Point theA = { 1, 2 }, theB = { 4, 6 };
    Check(Dist2(theA, theB) == 25, "distance is squared");
    Point theC = { -3, -4 }, theD = { 0, 0 };
    Check(Dist2(theC, theD) == 25, "distance works below zero");
    Check(Dist2(theA, theA) == 0, "a point has no distance to itself");
}

static void TestFront()
{
    Point theOwn[1] = { { 0, 0 } };
    long theDF = 0;
    Check(Front(theOwn, 1, NULL, 0, theOwn[0], theDF) == -1 && theDF == 0x7FFFFFFF
          && BeyondFront(-1, theDF), "with no enemy building no cell is held back");

    Point theTowns[2] = { { 0, 0 }, { 20, 0 } };
    Point theFoe[1] = { { 40, 0 } };
    Point theCell = { 10, 0 };
    long theFront = Front(theTowns, 2, theFoe, 1, theCell, theDF);
    Check(theFront == 20 && theDF == 30 && BeyondFront(theFront, theDF),
          "a cell behind the front is allowed");
    theCell.mX = 30;
    theFront = Front(theTowns, 2, theFoe, 1, theCell, theDF);
    Check(theFront == 20 && theDF == 10 && !BeyondFront(theFront, theDF),
          "a cell nearer the enemy than the front town is refused");

    Point theSquare[1] = { { 0, 0 } }, theEnemy[1] = { { 5, 5 } };
    Point theDiagonal = { 10, 10 };
    theFront = Front(theSquare, 1, theEnemy, 1, theDiagonal, theDF);
    Check(theFront == 7 && theDF == 7 && BeyondFront(theFront, theDF),
          "the distances truncate as upstream's square roots do");
    theDiagonal.mX = 7;
    theDiagonal.mY = 7;
    theFront = Front(theSquare, 1, theEnemy, 1, theDiagonal, theDF);
    Check(theFront == 7 && theDF == 2 && !BeyondFront(theFront, theDF),
          "a cell nearer the enemy than the front is refused");
}

static void TestNearest()
{
    Point theTowns[2] = { { 0, 0 }, { 20, 0 } };
    Point theCell = { 12, 0 };
    Check(NearestOwn2(theTowns, 2, theCell) == 64, "the nearer town wins");
    theCell.mX = 10;
    Check(NearestOwn2(theTowns, 2, theCell) == 100, "a tie keeps the distance");
}

static void TestPick()
{
    Point theOwn[2] = { { 4, 4 }, { 28, 28 } };
    Point theFoe[1] = { { 30, 30 } };
    Point theCell = { 0, 0 };
    long theDist = 0;
    bool thePicked = PickCell(32, theOwn, 2, theFoe, 1,
        [](const Point& inP) { return (inP.mX == 12 && inP.mY == 12) || (inP.mX == 20 && inP.mY == 12); },
        [](const Point& inP) { return inP.mX == 12 && inP.mY == 12; },
        theCell, theDist);
    Check(thePicked && theCell.mX == 20 && theCell.mY == 12 && theDist == 320,
          "a threatened cell gives way to the next nearest unseen cell");

    thePicked = PickCell(32, theOwn, 2, theFoe, 1,
        [](const Point&) { return false; }, [](const Point&) { return false; }, theCell, theDist);
    Check(!thePicked, "a fully seen map sends no scout");

    thePicked = PickCell(32, NULL, 0, theFoe, 1,
        [](const Point&) { return true; }, [](const Point&) { return false; }, theCell, theDist);
    Check(!thePicked, "a player with no town sends no scout");

    thePicked = PickCell(0, theOwn, 2, theFoe, 1,
        [](const Point&) { return true; }, [](const Point&) { return false; }, theCell, theDist);
    Check(!thePicked, "an empty map sends no scout");

    Point theNear[1] = { { 0, 0 } }, theFoe2[1] = { { 20, 0 } };
    theCell = { 0, 0 };
    thePicked = PickCell(32, theNear, 1, theFoe2, 1,
        [](const Point& inP) { return inP.mX == 12 && inP.mY == 4; },
        [](const Point&) { return false; }, theCell, theDist);
    Check(!thePicked && theCell.mX == 0, "a cell in front of the front is refused");
}

static void TestSoldierAndWatch()
{
    Check(Faster(6.0f, 5.0f), "the faster soldier is picked");
    Check(!Faster(5.0f, 5.0f), "a tie keeps the first soldier");
    Check(Faster(1.0f, 0.0f), "any positive speed beats a walking soldier");

    Check(TurnBack(0, 0, false), "an enemy on the scout sends it home");
    Check(TurnBack(12, 0, false), "twelve tiles is in sight");
    Check(!TurnBack(13, 0, false), "thirteen tiles is out of sight");
    Check(TurnBack(30, 30, true), "a known defence sends it home from anywhere");

    Check(PauseOver(100000, 0), "a player without a scout can send one");
    Check(!PauseOver(100000, 20000), "a recent scout holds the pause");
    Check(PauseOver(110000, 20000), "the pause ends after ninety game seconds");
    Check(PauseOver(5000, 20000), "a clock reset drops the pause");
}

int main()
{
    TestConstants();
    TestDistances();
    TestFront();
    TestNearest();
    TestPick();
    TestSoldierAndWatch();
    printf("%d checks, %d failures\n", sChecks, sFailures);
    return sFailures != 0;
}
