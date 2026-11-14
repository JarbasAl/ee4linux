// What a unit class is (AgeOfAIRoles.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/game/roles.c and
// dll/game/unitnames.c at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include "ee/EEUnitClassIDs.h"
#include "AgeOfAIRoles.h"

namespace AgeOfAI {
namespace Roles {

unsigned RolesOf(unsigned long inCategory, unsigned long inFamily, float inRange)
{
    unsigned theRoles = 0;
    if (inFamily == kUnitFamilyTowers)
        theRoles |= kTower;
    if (inFamily == kUnitFamilyWalls)
        theRoles |= kWall;
    if (inCategory == kUnitCategoryGate)
        theRoles |= kGate;
    if (inCategory == kUnitCategoryLand && (inFamily == kUnitFamilySiege || inFamily == kUnitFamilyMachines) && inRange >= 3.0f)
        theRoles |= kSiege;
    if (inCategory == kUnitCategoryLand && inFamily == kUnitFamilyRam)
        theRoles |= kRam;
    if (inCategory == kUnitCategoryLandCitizen && inFamily == kUnitFamilyCitizen)
        theRoles |= kCitizen;
    if (inCategory == kUnitCategoryNavalTransport)
        theRoles |= kTransport;
    if (inCategory == kUnitCategoryNavalCitizen)
        theRoles |= kFishingBoat;
    return theRoles;
}

bool IsBuildingCategory(unsigned long inCategory)
{
    return inCategory == kUnitCategoryBuilding || inCategory == kUnitCategoryBuilding19 || inCategory == kUnitCategoryNavalBuilding
        || inCategory == kUnitCategoryMiningBuilding || inCategory == kUnitCategoryGate || inCategory == kUnitCategoryFortress
        || inCategory == kUnitCategoryBuildingPriest || inCategory == kUnitCategorySpaceBuilding;
}

bool IsSoldier(unsigned long inCategory, unsigned long inFamily)
{
    if (IsBuildingCategory(inCategory))
        return false;
    switch (inCategory) {
    case kUnitCategoryLandCitizen:
    case kUnitCategoryNavalCitizen:
    case kUnitCategoryAmbient:
    case kUnitCategoryResource:
    case kUnitCategoryProjectile:
    case kUnitCategorySeaAnimal:
    case kUnitCategoryAirAnimal:
        return false;
    }
    return inFamily != kUnitFamilyRam && inFamily != kUnitFamilyAnimal && inFamily != kUnitFamilyCitizen;
}

}
}
