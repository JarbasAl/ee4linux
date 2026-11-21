// The deterministic lost-fight decision of Grille's cover.c cv_zone / cv_zones and attack.c hook_asgu_score at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07. Kept free of game types so the arithmetic is testable. The port keeps no
// danger-map bitmap: it holds the same zones as a small list of tile cells, and drops a zone when a later refresh does
// not mark it again. The staging and landing checks upstream makes before the target test are not here.
#ifndef AGEOFAI_AVOIDLOST_RULES_H
#define AGEOFAI_AVOIDLOST_RULES_H

namespace AgeOfAI {
namespace AvoidLostRules {

const int kCell = 8;              // a fight is judged in 8-tile cells (CV_CELL)
const int kFoe = 10;              // enemy fighters counted within this of a cell's middle (CV_ZONE_FOE)
const int kReachCap = 15;         // the longest enemy reach added to a zone (CV_ZONE_REACH)
const unsigned long kRefreshMs = 4000;   // the cells are judged again this often
const float kLoseBelow = 1.0f;    // a cell whose estimate is below this is a lost fight
const float kOwnExtra = 4.0f;     // own and allied fighters count out to the zone edge plus this (cv_zone ro)
const float kReachSlack = 2.0f;   // a soldier fights a target in a zone inside its own range plus this

// The zone's radius: the enemy's longest reach inside kFoe, capped, added to kFoe. Reaches of 30 tiles or more are
// not counted (cv_zone's `rg < 30` test), so pass only those.
inline float ZoneRadius(float inReach)
{
    if (inReach < 0.0f)
        inReach = 0.0f;
    if (inReach > (float)kReachCap)
        inReach = (float)kReachCap;
    return (float)kFoe + inReach;
}

// How far out own and allied fighters count for the judgement.
inline float OwnRadius(float inZoneRadius)
{
    return inZoneRadius + kOwnExtra;
}

// Whether the judge calls the cell lost (cv_zone returns st_ratio, and cv_zones marks below 1).
inline bool IsLost(float inRatio)
{
    return inRatio < kLoseBelow;
}

}
}

#endif
