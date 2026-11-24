// The reduced coast-tower rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICoastRules.h): how work sites group into
// cells, when a tower covers them, how far the sea is searched, where the tower stands and the placement ring. The
// expected values follow defence.c coast_spot, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICoastRules.h"

using namespace AgeOfAI::CoastRules;

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
    Check(kSea == 9, "the sea is searched nine tiles out");
    Check(kCell == 6, "work sites share six-tile cells");
    Check(kCover == 6, "a tower covers six tiles on each axis");
    Check(kIn == 2, "the tower stands two tiles in from the shore");
}

static void TestCells()
{
    Check(CellKey(0) == 0 && CellKey(5) == 0 && CellKey(6) == 1, "tiles group into six-tile cells");
    Check(CellKey(11) == 1 && CellKey(12) == 2, "a cell ends at the multiple of six");
    Check(CellKey(-1) == 0 && CellKey(-6) == -1, "tiles below zero group the same way");
}

static void TestCoverage()
{
    Check(TowerCovers(20, 20, 20, 20), "a tower on the work site covers it");
    Check(TowerCovers(20, 20, 26, 24), "six tiles on each axis still covers");
    Check(!TowerCovers(20, 20, 27, 20), "seven tiles out does not");
    Check(!TowerCovers(20, 20, 20, -14), "a tower far above the site is out of reach");
    Check(TowerCovers(-3, -3, 3, 3), "coverage works below zero");
}

static void TestSea()
{
    Check(WithinSea(0, 9), "nine tiles straight out is in reach");
    Check(WithinSea(6, 6), "a diagonal under nine is in reach");
    Check(!WithinSea(9, 9), "a nine-by-nine diagonal is out of reach");
    Check(!WithinSea(-10, 0), "ten tiles out is out of reach");
}

static void TestInset()
{
    long theX = 0, theY = 0;
    Inset(20, 20, 29, 20, theX, theY);
    Check(theX == 27 && theY == 20, "the tower stands two tiles in from a sea nine tiles out");
    Inset(20, 20, 20, 14, theX, theY);
    Check(theX == 20 && theY == 16, "a sea six tiles down leaves two tiles of shore");
    Inset(20, 20, 23, 24, theX, theY);
    Check(theX == 22 && theY == 22, "a diagonal sea rounds each tile of the inset");
    Inset(20, 20, 21, 20, theX, theY);
    Check(theX == 20 && theY == 20, "a sea within two tiles uses the work site");
    Inset(20, 20, 20, 20, theX, theY);
    Check(theX == 20 && theY == 20, "the work site itself is unchanged");
}

static void TestRing()
{
    Check(OnRing(0, 0, 0), "the centre is the only radius-zero tile");
    Check(!OnRing(1, 0, 0), "a ring tile is not the centre");
    Check(OnRing(1, 0, 1) && OnRing(0, -1, 1) && OnRing(-1, -1, 1), "radius one is the eight neighbours");
    Check(!OnRing(0, 0, 1), "the centre is not on radius one");
    Check(OnRing(2, 0, 2) && OnRing(2, 2, 2) && OnRing(0, -2, 2), "radius two is the outer ring");
    Check(!OnRing(1, 1, 2), "a tile inside radius two is not on the ring");
}

int main()
{
    TestConstants();
    TestCells();
    TestCoverage();
    TestSea();
    TestInset();
    TestRing();
    printf("coast rules: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
