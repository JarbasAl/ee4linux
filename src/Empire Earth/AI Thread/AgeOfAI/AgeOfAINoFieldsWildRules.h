// NO FIELDS WHILE WILD FOOD IS FREE, the no_fields_wild bit of Grille's Age of AI food_plan (dll/ai/food.c
// food_fields_wait, dll/ai/econ.c econ_fields, at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). From the Copper Age,
// field foundations keep their builders while a free bush or animal is in reach. The arithmetic needs no game types.
// The fish branch of upstream's rule is not ported.
#ifndef AGEOFAI_AGEOFAI_NO_FIELDS_WILD_RULES_H
#define AGEOFAI_AGEOFAI_NO_FIELDS_WILD_RULES_H

#include "AgeOfAISites.h"

namespace AgeOfAI {
namespace NoFieldsWildRules {

// FARM_EPOCH (dll/ai/econ.c) is the Copper Age: fields and granaries come with it.
const long kFarmEpoch = 2;

// food_fields_wait searches bushes within six times the drop-off distance of a drop site (r6 * r6 * 36).
inline long BushReach2(long inDropOff)
{
    return SitesRules::ForageReach2(inDropOff, false);
}

// Upstream's WILD test, reduced to the free bush and animal branch. The fish branch is left out.
inline bool DeferFields(bool inEnabled, long inEpoch, bool inBush, bool inAnimal)
{
    return inEnabled && inEpoch >= kFarmEpoch && (inBush || inAnimal);
}

}  // namespace NoFieldsWildRules
}  // namespace AgeOfAI

#endif
