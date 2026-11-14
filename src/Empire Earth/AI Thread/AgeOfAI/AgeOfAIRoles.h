// What a unit class is, as Age of AI's code asks it: a tower, a wall, a gate, a siege weapon, a ram, a citizen, a
// transport ship, a fishing boat, a building, a soldier. The answer comes from the class's category, family and range
// only, so the data of a mod that adds a tower or a ram needs no name here.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/game/roles.c and
// the role table of dll/game/unitnames.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_ROLES_H
#define AGEOFAI_ROLES_H

namespace AgeOfAI {
namespace Roles {

enum Role {
    kTower = 1,         // family towers
    kWall = 2,          // family walls
    kGate = 4,          // category gate
    kSiege = 8,         // a land unit of family siege or machines that shoots from 3 tiles on
    kRam = 16,          // a land unit of family ram
    kCitizen = 32,      // category land citizen, family citizen
    kTransport = 64,    // category naval transport
    kFishingBoat = 128  // category naval citizen
};

// The roles of a class, as a set of Role bits
unsigned RolesOf(unsigned long inCategory, unsigned long inFamily, float inRange);

// Any of the building categories: buildings, naval, mining, priest and space buildings, gates, fortresses
bool IsBuildingCategory(unsigned long inCategory);

// A soldier: a unit of the military classes (land, naval, air, heroes, priests, transports, spaceships; siege weapons
// too). Not a building, a citizen, a ram, an animal, a shot or a resource.
bool IsSoldier(unsigned long inCategory, unsigned long inFamily);

}
}

#endif
