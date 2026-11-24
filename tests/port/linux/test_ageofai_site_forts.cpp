// The reduced site-fort rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISiteFortsRules.h): the mine a fort goes
// round, how far forts keep apart, its exposure bonus and the four corner towers. Expected values follow defence.c
// sf_pick / sf_corner / sf_build, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISiteFortsRules.h"

using namespace AgeOfAI::FortRules;

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
    Check(kMaxForts == 6, "up to six forts per player");
    Check(kTowers == 4, "a fort has four corner towers");
    Check(kRadius == 4, "corner towers stand four tiles from the mine");
    Check(kMinWorkers == 2, "a mine needs two gatherers");
    Check(kExposedBonus == 4, "exposure adds four to the score");
    Check(kSeaRadius == 9, "a ship reaches nine tiles");
    Check(kTickMs == 20000, "the defence tick is every twenty seconds");
    Check(kBusyMs == 1800000, "a fort is built within thirty minutes");
}

static void TestMineCenter()
{
    Check(MineCenter(10, 1) == 10 && MineCenter(10, 0) == 10, "a one-tile mine uses its own tile");
    Check(MineCenter(10, 3) == 11 && MineCenter(10, 4) == 12, "a wide mine uses half its size");
    Check(MineCenter(10, 7) == 13, "a seven-tile mine uses three tiles");
    Check(MineCenter(10, 8) == 10, "a mine of eight tiles or more is not centred");
    Check(MineCenter(-4, 3) == -3, "a negative tile gets the same offset");
}

static void TestSeaAndScore()
{
    Check(InSeaReach(9, 0) && InSeaReach(0, -9), "nine tiles straight is in a ship's reach");
    Check(InSeaReach(6, 6) && !InSeaReach(7, 7), "the reach is a circle, not a square");
    Check(!InSeaReach(10, 0) && !InSeaReach(-1, 9), "past the reach is out");
    Check(Score(2, false) == 2 && Score(2, true) == 6, "exposure adds four to the gatherers");
    Check(Score(9, true) > Score(10, false), "exposure beats one extra gatherer");
}

static void TestSpacing()
{
    Check(FortTaken(20, 20, 20, 20), "a mine already holding a fort is taken");
    Check(FortTaken(20, 20, 27, 20) && FortTaken(20, 20, 20, 13), "seven tiles on an axis is too near");
    Check(!FortTaken(20, 20, 28, 20) && !FortTaken(20, 20, 20, 12), "eight tiles on an axis is far enough");
    Check(FortTaken(20, 20, 24, 24), "a diagonal inside the square is too near on both axes");
    Check(!FortTaken(20, 20, 28, 28), "eight tiles on both axes is far enough");
    Check(FortTaken(-20, 20, -13, 20) && !FortTaken(-20, 20, -12, 20), "spacing works on negative tiles too");
}

static void TestCorners()
{
    long theX = 0, theY = 0;
    Corner(20, 30, 0, theX, theY);
    Check(theX == 16 && theY == 26, "corner zero is top left");
    Corner(20, 30, 1, theX, theY);
    Check(theX == 24 && theY == 26, "corner one is top right");
    Corner(20, 30, 2, theX, theY);
    Check(theX == 24 && theY == 34, "corner two is bottom right");
    Corner(20, 30, 3, theX, theY);
    Check(theX == 16 && theY == 34, "corner three is bottom left");
}

static void TestRing()
{
    long theOffsets[9][2] = {};
    int theCount = Ring(theOffsets);
    Check(theCount == 9, "the corner and its eight neighbours are tried");
    Check(theOffsets[0][0] == 0 && theOffsets[0][1] == 0, "the corner itself comes first");
    bool theSeen[3][3] = {};
    bool theAll = true;
    for (int k = 0; k < theCount; k++) {
        long theAX = theOffsets[k][0], theAY = theOffsets[k][1];
        if (theAX < -1 || theAX > 1 || theAY < -1 || theAY > 1 || theSeen[theAX + 1][theAY + 1])
            theAll = false;
        else
            theSeen[theAX + 1][theAY + 1] = true;
    }
    Check(theAll, "each neighbour is tried once, and none is farther than one tile");
}

int main()
{
    TestConstants();
    TestMineCenter();
    TestSeaAndScore();
    TestSpacing();
    TestCorners();
    TestRing();
    printf("%d checks, %d failures\n", sChecks, sFailures);
    return sFailures != 0;
}
