// The reduced units_first rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIUnitsFirstRules.h): a unit that picked a
// building for itself turns on the nearest enemy unit close enough to shoot. The expected values follow
// unitsfirst.c's uf_building, uf_unit, uf_pick and units_first_tick, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIUnitsFirstRules.h"

using namespace AgeOfAI::UnitsFirstRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestCategories()
{
    const unsigned long theBuildings[] = { kBuilding, kNavalBuilding, kGate, kFortress, kBuildingPriest, kSpaceBuilding };
    for (unsigned i = 0; i < sizeof theBuildings / sizeof theBuildings[0]; i++)
        Check(IsBuilding(theBuildings[i]), "the building families count as buildings");
    Check(!IsBuilding(kLand), "a land unit is not a building");
    Check(!IsBuilding(kAirplane), "an aircraft is not a building");
    Check(!IsBuilding(kResource), "a resource is not a building");

    const unsigned long theUnits[] = {
        kNaval, kLand, kLandCitizen, kHelicopter, kSubmergedNaval, kLandPriest,
        kLandTransport, kHelicopterTransport, kNavalTransport, kNavalCitizen, kLandMilitaryHero, kHelicopterPriest
    };
    for (unsigned i = 0; i < sizeof theUnits / sizeof theUnits[0]; i++)
        Check(IsSwitchTarget(theUnits[i]), "walkers, sailors and hoverers are worth turning on");
    Check(!IsSwitchTarget(kAirplane), "an aircraft passing over is not worth turning on");
    Check(!IsSwitchTarget(kProjectile), "a shot is not worth turning on");
    Check(!IsSwitchTarget(kSeaAnimal), "an animal is not worth turning on");
    Check(!IsSwitchTarget(kBuilding), "a building is not worth turning on");
}

static void TestShouldSwitch()
{
    Check(ShouldSwitch(kFace, false, 100, kBuilding), "turning to a self-picked building gets a look");
    Check(ShouldSwitch(kWaitReload, false, 100, kGate), "waiting to reload on a self-picked gate gets a look");
    Check(!ShouldSwitch(2001, false, 100, kBuilding), "firing is not one of the two states");
    Check(!ShouldSwitch(kFace, true, 100, kBuilding), "an ordered attack keeps its target");
    Check(!ShouldSwitch(kFace, false, 0, kBuilding), "a dead unit keeps its target");
    Check(!ShouldSwitch(kFace, false, -3, kBuilding), "a negative hit point count keeps its target");
    Check(!ShouldSwitch(kFace, false, 100, kLand), "a unit already attacking a unit keeps its target");
}

static void TestReachAndPick()
{
    Check(Reach(6.0f) == 10.0f, "the reach adds four tiles to the weapon range");
    Check(Reach(0.0f) == 4.0f, "a weapon without range still looks four tiles out");
    const float theReachSq = Reach(6.0f) * Reach(6.0f);   // 100
    Check(Better(36, -1, theReachSq, true, true), "the first seen, hurt-able unit in reach is picked");
    Check(!Better(121, -1, theReachSq, true, true), "a unit beyond the reach is not picked");
    Check(Better(100, -1, theReachSq, true, true), "a unit exactly at the reach bound is picked");
    Check(!Better(36, -1, theReachSq, false, true), "an unseen unit is not picked");
    Check(!Better(36, -1, theReachSq, true, false), "a unit the weapon cannot hurt is not picked");
    Check(Better(25, 36, theReachSq, true, true), "a nearer unit replaces the current best");
    Check(!Better(49, 36, theReachSq, true, true), "a farther unit does not replace the current best");
    Check(!Better(36, 36, theReachSq, true, true), "an equal distance keeps the current best");
}

int main()
{
    TestCategories();
    TestShouldSwitch();
    TestReachAndPick();
    printf("ageofai units first: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
