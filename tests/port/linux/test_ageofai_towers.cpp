// The tower slots of the defence layout (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITowers.h). The expected tiles follow
// upstream's def_slot in defence.c, worked out by hand: 3 tiles between slots, across the front's direction.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITowers.h"

using namespace AgeOfAI::TowerRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static bool At(const Front inFront[], int inFronts, const long inOut[][2], int inOuts, int i, long inX, long inY)
{
    long x = -1, y = -1;
    return Slot(inFront, inFronts, inOut, inOuts, i, x, y) && x == inX && y == inY;
}

static void TestOneFront()
{
    const Front theEast[1] = {{20, 10, 1.0f, 0.0f}};   // the front 20,10, the enemy to the east
    const long theNone[1][2] = {{0, 0}};
    Check(At(theEast, 1, theNone, 0, 0, 20, 10), "slot 0: the front's middle");
    Check(At(theEast, 1, theNone, 0, 1, 20, 7), "slot 1: 3 tiles to one side, across the direction (20, 7)");
    Check(At(theEast, 1, theNone, 0, 2, 20, 13), "slot 2: 3 tiles to the other side (20, 13)");
    Check(At(theEast, 1, theNone, 0, 3, 20, 4), "slot 3, no outlying town: 6 tiles to the first side (20, 4)");
    Check(At(theEast, 1, theNone, 0, 4, 20, 16), "slot 4: 6 tiles to the other side (20, 16)");
    long x, y;
    Check(!Slot(theEast, 1, theNone, 0, 5, x, y), "one front, no outlying town: 5 slots");
}

static void TestOutlyingTowns()
{
    const Front theEast[1] = {{20, 10, 1.0f, 0.0f}};
    const long theOut[1][2] = {{50, 50}};
    Check(At(theEast, 1, theOut, 1, 3, 50, 50), "the outlying town's spot comes after the first three");
    Check(At(theEast, 1, theOut, 1, 4, 20, 4), "then the two more of the front");
    long x, y;
    Check(!Slot(theEast, 1, theOut, 1, 6, x, y), "one front and one outlying town: 6 slots");
}

static void TestTwoFronts()
{
    const Front theTwo[2] = {{20, 10, 1.0f, 0.0f}, {10, 0, 0.0f, -1.0f}};   // east, and north (y falls)
    const long theNone[1][2] = {{0, 0}};
    Check(At(theTwo, 2, theNone, 0, 3, 10, 0), "slot 3: the second front's middle");
    Check(At(theTwo, 2, theNone, 0, 4, 7, 0), "slot 4: across the north front, -(-1) * 3 * -1 = -3 in x");
    Check(At(theTwo, 2, theNone, 0, 7, 20, 16), "slot 7: the first front's last");
    Check(At(theTwo, 2, theNone, 0, 8, 4, 0), "slot 8: the second front, 6 tiles to the first side, -(-1) * 3 * -2 = -6 in x");
}

static void TestRounding()
{
    // the direction (0.6, 0.8): one slot to the side is (-0.8 * 3 * -1, 0.6 * 3 * -1) = (2.4, -1.8), rounded (2, -2)
    const Front theSlant[1] = {{0, 0, 0.6f, 0.8f}};
    const long theNone[1][2] = {{0, 0}};
    Check(At(theSlant, 1, theNone, 0, 1, 2, -2), "a slanted front rounds each tile to the nearest");
    // the direction (0.8, 0.6): (-0.6 * 3 * -1, 0.8 * 3 * -1) = (1.8, -2.4), rounded (2, -2), not cut to (1, -2)
    const Front theOther[1] = {{0, 0, 0.8f, 0.6f}};
    Check(At(theOther, 1, theNone, 0, 1, 2, -2), "1.8 rounds up to 2");
}

int main()
{
    TestOneFront();
    TestOutlyingTowns();
    TestTwoFronts();
    TestRounding();
    printf("ageofai towers: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
