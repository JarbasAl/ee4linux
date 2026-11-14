// The worker plan of an Age of AI player before farms (the Prehistoric and Stone Ages): how many gatherers each resource
// gets. The game wants gatherers by its demand, and its food demand is a phantom: food and wood run short while the epoch
// is paid. The plan asks what each resource still needs, from the demand, the stock, what the next epoch costs
// (the reserve) and what the citizens and the first towers need, and gives the workers in proportion to the time each
// need takes to gather (need / income per worker). No game types: the resources are the game's indices 2 food, 3 wood, 4
// stone, 5 gold, 6 iron (0 and 1 are not used).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/econ.c
// (econ_targets, before farms) and dll/ai/food.c (food_plan_needs) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_WORKERS_H
#define AGEOFAI_WORKERS_H

namespace AgeOfAI {
namespace Workers {

const int kFirst = 2, kLast = 6;           // the resources
const long kFoodPerCitizen = 50;           // what a citizen to come costs: counted in the food need
const long kFoodPlanCitizens = 10;         // at most this many citizens to come are counted at once

// What each resource still needs (need[2..6], below 0 allowed: the caller treats it as 0). inDemand: the game's demand;
// inStock; inReserve: what the next epoch costs. inMineralMargin: the military's demand for stone, gold and iron counts
// only up to the reserve plus this (-1: all of it). inTowerStone: the stone of the town's first towers. inCitizensMore:
// the citizens still wanted (the food of 50 each, at most 10), 0 when the plan does not count them. inWoodCeiling: the
// wood the buildings really need (the next three queued ones, the fields, 100 more), or -1 for no ceiling: the game's
// demand of refused requests ran to 4500 wood in the Prehistoric and took half the workers.
void Needs(const long inDemand[7], const long inStock[7], const long inReserve[7], long inMineralMargin, long inTowerStone,
           long inCitizensMore, long inWoodCeiling, long outNeed[7]);

// The gatherers of each resource out of inTotal: a few stay on food and wood (the floors 2 and 2), the rest goes by the
// time each need takes (need / income per worker, at least 1; inReservePlan 0: by the need itself), the rounding rest to the
// largest needs in turn. inCopperPlan 2: half the workers on food, up to inFoodFloor, taken from the largest other targets.
// Returns false (and leaves ioTarget alone) when nothing is needed or there are fewer than 6 workers.
bool Split(const long inNeed[7], const float inPerWorker[7], int inTotal, bool inReservePlan, int inCopperPlan,
           int inFoodFloor, unsigned long ioTarget[7]);

// What a training order may spend (outVisible[2..6]) of the stock, the reserve held back for the next epoch. A unit that is no
// citizen sees the stock above the reserve. A citizen before farms (inCopperPlan 1 or 2) sees the whole stock of wood,
// stone, gold and iron (the plan counts it), and of food while the workforce is under 10 or under inMinWorkforce (the
// economy grows first); above that the reserve applies to it too.
void Visible(const long inStock[7], const long inReserve[7], bool inCitizen, int inCopperPlan, long inWorkforce,
             long inMinWorkforce, long outVisible[7]);

}
}

#endif
