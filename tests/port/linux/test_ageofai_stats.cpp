// The fight estimate (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIStats.cpp): sides of a few units with stats chosen by hand,
// and the damage, closing times, kill times and ratios worked out by hand from the rules in AgeOfAIStats.h.
#include <cmath>
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIStats.cpp"

using namespace AgeOfAI::Stats;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static bool Near(float inValue, float inExpected)
{
    return fabsf(inValue - inExpected) <= 1e-3f * (fabsf(inExpected) > 1 ? fabsf(inExpected) : 1);
}

static void CheckNear(float inValue, float inExpected, const std::string& inWhat)
{
    char theText[64];
    snprintf(theText, sizeof theText, " (got %g, want %g)", inValue, inExpected);
    Check(Near(inValue, inExpected), inWhat + theText);
}

// the shot of a group: its type number is the damage it deals to any target
static int TypeShot(const Group& inAttacker, const Group&)
{
    return (int)inAttacker.mType;
}

static Group Kind(long inShot, long inOwner, float inRange, float inSpeed, float inReload)
{
    Group theKind = {};
    theKind.mType = inShot;
    theKind.mOwner = inOwner;
    theKind.mRange = inRange;
    theKind.mSpeed = inSpeed;
    theKind.mReload = inReload;
    return theKind;
}

static Side sA, sB;

static void TestShotDamage()
{
    Check(ShotDamage(10, 1, 3) == 7, "10 x 1 - 3 armour = 7");
    Check(ShotDamage(10, 0.5f, 6) == 1, "10 x 0.5 - 6 = -1: at least 1");
    Check(ShotDamage(10, 0.35f, 0) == 4, "3.5 rounds to 4");
    Check(ShotDamage(10, 0.34f, 0) == 3, "3.4 rounds to 3");
    Check(ShotDamage(10, 0, 0) == 0, "a factor of 0: cannot hit");
    Check(ShotDamage(0, 1, 0) == 0, "no attack: no damage");
}

static void TestSquareLaw()
{
    // two units against one of the same kind: 10 a shot, reload 2 s, so 10 / (2 + 2) = 2.5 per second each
    Clear(sA);
    Clear(sB);
    Check(Put(sA, Kind(10, 1, 1, 1, 2), 100, 0, 0) && Put(sA, Kind(10, 1, 1, 1, 2), 100, 1, 0), "two units in one group");
    Check(sA.mGroupCount == 1 && sA.mCount == 2 && Near(sA.mHP, 200), "one group of 2 units, 200 hit points");
    Put(sB, Kind(10, 2, 1, 1, 2), 100, 5, 0);
    Result r;
    Estimate(sA, sB, TypeShot, r);
    CheckNear(r.mContactDps[0], 5.0f, "two units: 5 per second");
    CheckNear(r.mContactDps[1], 2.5f, "one unit: 2.5 per second");
    CheckNear(r.mClose[0], 0.0f, "equal ranges: no closing");
    CheckNear(r.mTime[0], 20.0f, "100 hit points at 5 per second");
    CheckNear(r.mTime[1], 80.0f, "200 hit points at 2.5 per second");
    CheckNear(Ratio(sA, sB, TypeShot), 2.0f, "twice the units: the root of 80 / 20 = 2");
    CheckNear(Ratio(sB, sA, TypeShot), 0.5f, "the other way: 0.5");
}

static void TestRange()
{
    // a tower (cannot move, range 5) against a unit of range 7: the tower never hits it
    Clear(sA);
    Clear(sB);
    Put(sA, Kind(10, 1, 5, 0, 2), 300, 0, 0);
    Put(sB, Kind(10, 2, 7, 1, 2), 100, 7, 0);
    Result r;
    Estimate(sA, sB, TypeShot, r);
    CheckNear(r.mFreeDps[0], 0.0f, "the outranged tower deals nothing");
    Check(r.mTime[0] >= 1e9f, "the tower never destroys the unit");
    CheckNear(r.mTime[1], 120.0f, "the unit destroys the tower: 300 / 2.5");
    CheckNear(Ratio(sA, sB, TypeShot), 0.0f, "the tower loses");
    Check(Ratio(sB, sA, TypeShot) >= 1e6f, "the unit cannot lose");
}

static void TestClosing()
{
    // a fighter (range 1, speed 2, 9 a shot, reload 1: 3 per second) against an archer (range 5, speed 1, 6 a shot, reload 1:
    // 2 per second). The fighter walks 5 - 1 = 4 tiles at 2 tiles per second under fire: 2 s.
    Clear(sA);
    Clear(sB);
    Put(sA, Kind(9, 1, 1, 2, 1), 100, 0, 0);
    Put(sB, Kind(6, 2, 5, 1, 1), 100, 5, 0);
    Result r;
    Estimate(sA, sB, TypeShot, r);
    CheckNear(r.mClose[0], 2.0f, "the fighter closes in 2 s");
    CheckNear(r.mClose[1], 0.0f, "the archer has nothing to close");
    // the fighter: 2 s walking, then 100 / 3; the archer: 2 s free fire (4), then 96 / 2 after the fighter's 2 s
    CheckNear(r.mTime[0], 2.0f + 100.0f / 3.0f, "the fighter needs 35.33 s");
    CheckNear(r.mTime[1], 2.0f + 96.0f / 2.0f, "the archer needs 50 s");
    CheckNear(Ratio(sA, sB, TypeShot), sqrtf(50.0f / (2.0f + 100.0f / 3.0f)), "the fighter is 1.19 times stronger");
    CheckNear(KillTime(3, 2, 2, 2, 5), 2.0f + 1.5f, "a target destroyed during the free fire: own closing + 3 / 2");
}

static void TestArea()
{
    // three targets at 0,0, 1,0 and 5,5: within 1.5 tiles each of the first two has 2 units, the third 1; 5 / 3 - 1 = 0.667
    Clear(sA);
    Clear(sB);
    Group theGun = Kind(12, 1, 10, 1, 2);
    theGun.mArea = 1.5f;
    Put(sA, theGun, 100, 20, 20);
    Put(sB, Kind(1, 2, 1, 1, 2), 50, 0, 0);
    Put(sB, Kind(1, 2, 1, 1, 2), 50, 1, 0);
    Put(sB, Kind(1, 2, 1, 1, 2), 50, 5, 5);
    CheckNear(Neighbours(sB, 0, 1.5f), 5.0f / 3.0f - 1.0f, "two of three units have a neighbour");
    // 12 / (2 + 2) = 3 per second, x (1 + 0.667)
    CheckNear(SideDps(sA, sB, false, TypeShot), 3.0f * (1.0f + (5.0f / 3.0f - 1.0f)), "area damage counts the neighbours");
    // five targets on one tile: 4 neighbours each, counted as 2
    Clear(sB);
    for (int i = 0; i < 5; i++)
        Put(sB, Kind(1, 2, 1, 1, 2), 50, 3, 3);
    CheckNear(SideDps(sA, sB, false, TypeShot), 3.0f * 3.0f, "at most 2 neighbours count");
}

static void TestMinimumRange()
{
    // artillery with a minimum range of 3 against a mobile unit of range 1: free fire hits, contact fire does not
    Clear(sA);
    Clear(sB);
    Group theArtillery = Kind(12, 1, 10, 0.5f, 2);
    theArtillery.mMinRange = 3;
    Put(sA, theArtillery, 100, 0, 0);
    Put(sB, Kind(5, 2, 1, 1, 2), 100, 10, 0);
    CheckNear(SideDps(sA, sB, false, TypeShot), 3.0f, "before contact: 12 / 4");
    CheckNear(SideDps(sA, sB, true, TypeShot), 0.0f, "in contact: inside the minimum range");
}

static void TestGroups()
{
    Clear(sA);
    Check(Put(sA, Kind(5, 1, 1, 1, 2), 10, 0, 0) && Put(sA, Kind(5, 2, 1, 1, 2), 10, 0, 0), "one type, two owners");
    Check(sA.mGroupCount == 2, "two groups");
    Clear(sA);
    bool theAll = true;
    for (long t = 0; t < kGroups; t++)
        theAll = theAll && Put(sA, Kind(t, 1, 1, 1, 2), 10, 0, 0);
    Check(theAll, "64 groups fit");
    Check(!Put(sA, Kind(kGroups, 1, 1, 1, 2), 10, 0, 0), "a 65th group does not");
    Check(Put(sA, Kind(0, 1, 1, 1, 2), 10, 0, 0), "a unit of a known group still does");
    Clear(sA);
    Clear(sB);
    Put(sB, Kind(5, 2, 1, 1, 2), 10, 0, 0);
    Check(Ratio(sA, sB, TypeShot) == 0.0f, "nobody to fight with: 0");
    Check(Ratio(sB, sA, TypeShot) >= 1e6f, "nothing to fight: 1e6");
}

int main()
{
    TestShotDamage();
    TestSquareLaw();
    TestRange();
    TestClosing();
    TestArea();
    TestMinimumRange();
    TestGroups();
    printf("ageofai stats: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
