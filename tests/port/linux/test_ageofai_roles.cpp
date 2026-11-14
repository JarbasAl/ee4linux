// The roles of a unit class (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIRoles.cpp). Each class is given by its category
// and family numbers as the game's tables hold them, and each expected set is written by hand from the rules in
// AgeOfAIRoles.h.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIRoles.cpp"

using namespace AgeOfAI::Roles;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

// category numbers: 2 building, 3 airplane, 4 naval, 5 land, 6 land citizen, 9 projectile, 0xa submerged, 0x11 helicopter
// transport, 0x12 naval transport, 0x13 (a building class), 0x15 naval building, 0x16 naval citizen, 0x17 hero, 0x18 mining
// building, 0x19 gate, 0x1b sea animal, 0x1d fortress; family numbers: 0x25 towers, 0x2c walls, 0x1e ram, 0x1f siege,
// 0xb machines, 0x24 citizen, 0x11 animal, 2 tank, 9 ship, 5 aircraft, 8 hero
static void TestRoles()
{
    Check(RolesOf(2, 0x25, 8.0f) == kTower, "a tower: category building, family towers");
    Check(RolesOf(2, 0x2c, 0) == kWall, "a wall: family walls");
    Check(RolesOf(0x19, 0x2c, 0) == (kGate | kWall), "a gate of the wall family is a gate and a wall");
    Check(RolesOf(5, 0x1f, 3.0f) == kSiege, "siege: land, family siege, range 3");
    Check(RolesOf(5, 0x1f, 2.9f) == 0, "range 2.9 is no siege weapon (the Sampson is melee)");
    Check(RolesOf(5, 0xb, 12.0f) == kSiege, "land family machines with range 12: siege");
    Check(RolesOf(4, 0x1f, 12.0f) == 0, "a naval unit of family siege: no siege (category naval)");
    Check(RolesOf(5, 0x1e, 1.0f) == kRam, "a ram: land, family ram");
    Check(RolesOf(0x10, 0x1e, 1.0f) == 0, "category 16 of family ram (a siege tower) is no ram: it carries");
    Check(RolesOf(6, 0x24, 1.0f) == kCitizen, "a citizen: land citizen, family citizen");
    Check(RolesOf(6, 0x17, 1.0f) == 0, "a citizen-class human (family human) is no citizen: the Engineer");
    Check(RolesOf(0x12, 9, 0) == kTransport, "a transport ship: category naval transport");
    Check(RolesOf(0x16, 0x24, 0) == kFishingBoat, "a fishing boat: category naval citizen");
    Check(RolesOf(5, 2, 4.0f) == 0, "a tank has no role");
}

static void TestBuildings()
{
    const unsigned long buildings[] = {2, 0x13, 0x15, 0x18, 0x19, 0x1d, 0x1e, 0x20};
    for (unsigned long c : buildings)
        Check(IsBuildingCategory(c), "category " + std::to_string(c) + " is a building");
    const unsigned long others[] = {0, 1, 3, 4, 5, 6, 9, 0x10, 0x12, 0x16, 0x17};
    for (unsigned long c : others)
        Check(!IsBuildingCategory(c), "category " + std::to_string(c) + " is no building");
}

static void TestSoldiers()
{
    Check(IsSoldier(5, 2), "a land tank");
    Check(IsSoldier(5, 0x1f), "a siege weapon counts as a soldier");
    Check(IsSoldier(4, 9), "a ship");
    Check(IsSoldier(3, 5), "an airplane");
    Check(IsSoldier(7, 0x10), "a helicopter");
    Check(IsSoldier(0x17, 8), "a hero");
    Check(IsSoldier(0xd, 6), "a priest");
    Check(IsSoldier(0x10, 0x1f), "a land transport");
    Check(IsSoldier(0x12, 9), "a naval transport");
    Check(IsSoldier(0xa, 7), "a submarine");
    Check(!IsSoldier(5, 0x1e), "a ram is no soldier");
    Check(!IsSoldier(6, 0x24), "a citizen is no soldier");
    Check(!IsSoldier(0x16, 0x24), "a fishing boat is no soldier");
    Check(!IsSoldier(2, 3), "a building is no soldier");
    Check(!IsSoldier(0x15, 3), "a naval building is no soldier (upstream's test counts it)");
    Check(!IsSoldier(0x19, 0x2c), "a gate is no soldier");
    Check(!IsSoldier(0, 0x23), "an ambient unit is no soldier");
    Check(!IsSoldier(1, 0), "a resource is no soldier");
    Check(!IsSoldier(9, 0), "a shot is no soldier");
    Check(!IsSoldier(0x1b, 0x12), "a sea animal is no soldier");
    Check(!IsSoldier(5, 0x11), "a land animal (family animal) is no soldier");
}

int main()
{
    TestRoles();
    TestBuildings();
    TestSoldiers();
    printf("test_ageofai_roles.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
