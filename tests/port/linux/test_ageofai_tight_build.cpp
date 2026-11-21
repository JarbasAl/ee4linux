// The reduced packed-building rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITightBuildRules.h): which buildings a
// packed spot places and touches, how a spot is scored, and when the tiles round a footprint leave one walkable arc.
// The expected values follow packing.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITightBuildRules.h"

using namespace AgeOfAI::TightBuildRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestTypes()
{
    Check(IsPackedType(kUnitTypeBarracks), "a barracks is placed packed");
    Check(IsPackedType(kUnitTypeArcheryRange), "an archery range is placed packed");
    Check(IsPackedType(kUnitTypeStable), "a stable is placed packed");
    Check(IsPackedType(kUnitTypeSiegeFactory), "a siege factory is placed packed");
    Check(IsPackedType(kUnitTypeTankFactory), "a tank factory is placed packed");
    Check(IsPackedType(kUnitTypeCyberFactory), "a cyber factory is placed packed");
    Check(!IsPackedType(kUnitTypeHouse), "a house keeps the game's own search");
    Check(!IsPackedType(kUnitTypeAirport), "an airport keeps the game's own search");
    Check(!IsPackedType(kUnitTypeTownCenter), "a town centre keeps the game's own search");
    Check(IsKinType(kUnitTypeBarracks), "a production building can be touched");
    Check(IsKinType(kUnitTypeHouse), "a house can be touched");
    Check(IsKinType(kUnitTypeHospital), "a hospital can be touched");
    Check(IsKinType(kUnitTypeTemple), "a temple can be touched");
    Check(IsKinType(kUnitTypeUniversity), "a university can be touched");
    Check(!IsKinType(kUnitTypeGranary), "a granary is not a kindred building");
}

static void TestScore()
{
    Check(SpotScore(0, 4) == -4, "no touching tile, four rings out");
    Check(SpotScore(1, 4) == 4, "one touching tile is worth eight");
    Check(SpotScore(2, 10) == 6, "two touching tiles beat a closer lone one");
    Check(SpotScore(1, 4) > SpotScore(0, 2), "touching wins over two rings of distance");
    Check(SpotScore(1, 12) < SpotScore(0, 2), "far enough out, distance wins");
}

static void TestRing()
{
    const bool kFull[8] = { true, true, true, true, true, true, true, true };
    const bool kArc[8] = { true, true, true, true, true, false, false, false };
    const bool kWrap[8] = { true, true, false, false, false, false, true, true };
    const bool kTwo[8] = { true, true, false, true, true, false, false, false };
    const bool kNone[8] = { false, false, false, false, false, false, false, false };
    long theWalkable = 0;

    Check(RingWalkable(kFull, 8, 3, theWalkable) && theWalkable == 8, "a wholly walkable ring is one arc");
    Check(RingWalkable(kArc, 8, 3, theWalkable) && theWalkable == 5, "five tiles in a row are one arc");
    Check(RingWalkable(kWrap, 8, 3, theWalkable) && theWalkable == 4, "an arc may wrap round the ring end");
    Check(!RingWalkable(kTwo, 8, 3, theWalkable), "two arcs leave the footprint cutting the way");
    Check(!RingWalkable(kNone, 8, 3, theWalkable) && theWalkable == 0, "no walkable tile seals the footprint in");
    Check(!RingWalkable(kArc, 8, 6, theWalkable), "five walkable tiles fail a six tile minimum");
    Check(RingWalkable(kArc, 8, 5, theWalkable), "five walkable tiles pass a five tile minimum");
}

int main()
{
    TestTypes();
    TestScore();
    TestRing();
    printf("ageofai tight build: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
