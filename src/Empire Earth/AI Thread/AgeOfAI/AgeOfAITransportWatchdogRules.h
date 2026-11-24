// The stuck-transport watchdog of Grille's Age of AI (dll/ai/transports.c tw_dog at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07). A transport that stands in one state on one tile with a goal or cargo
// for long enough is freed: pending passengers that are not coming are dropped from its load goal, and a loaded
// transport idle with its unload goal is set to PrepareToMove. The arithmetic and the category tests are free of
// game types so they can be tested on their own.
#ifndef AGEOFAI_AGEOAITRANSPORTWATCHDOG_RULES_H
#define AGEOFAI_AGEOAITRANSPORTWATCHDOG_RULES_H

namespace AgeOfAI {
namespace TransportWatchdogRules {

// The script states (EEUnitAction.h) and the cadence, in game milliseconds (upstream's TW_* and tw_tick).
enum { kIdle = 5000, kPrepareToMove = 8000, kWaitForPassengers = 12010 };
const long kScanEveryMs = 1000;
const long kDropAfterMs = 30000;
const long kKickAfterMs = 20000;
const int kMaxKicks = 3;

// EEUnitCategory values of the units that carry cargo (tw_kind): land transports, helicopters, naval transports and
// fortresses. The space transport uses a different goal and is not watched.
enum { kLandTransport = 0x10, kHelicopterTransport = 0x11, kNavalTransport = 0x12, kFortress = 0x1d };

inline bool IsTransport(unsigned long inCategory)
{
    switch (inCategory) {
    case kLandTransport:
    case kHelicopterTransport:
    case kNavalTransport:
    case kFortress:
        return true;
    default:
        return false;
    }
}

// A pending passenger of a load goal (tw_passenger). Only one still walking to the transport (TWP_BOARD) is kept.
enum { kBoard = 0, kIdleWithBoard = 1, kNoGoal = 2, kOtherOrder = 3, kGone = 4 };

inline bool IsGone(long inHitPoints, long inTileX, bool inHasClass)
{
    return inHitPoints <= 0 || !inHasClass || inTileX < 0;
}

// Whether a passenger holds this transport's board goal and is still coming (tw_passenger + tw_dog).
inline int PassengerState(long inHitPoints, long inTileX, bool inHasClass, bool inHasGoal,
                          bool inBoardGoalForThisTransport, unsigned long inState)
{
    if (IsGone(inHitPoints, inTileX, inHasClass))
        return kGone;
    if (!inHasGoal)
        return kNoGoal;
    if (!inBoardGoalForThisTransport)
        return kOtherOrder;
    return inState == kIdle ? kIdleWithBoard : kBoard;
}

inline bool ShouldDrop(int inPassengerState)
{
    return inPassengerState != kBoard;
}

// A waiting load goal lets go of its not-coming passengers after 30 seconds in WaitForPassengers.
inline bool ShouldDropPassengers(unsigned long inState, bool inHasLoadGoal, unsigned long inElapsed)
{
    return inState == kWaitForPassengers && inHasLoadGoal && inElapsed >= kDropAfterMs;
}

// A loaded transport idle with an unload goal is nudged to PrepareToMove after 20 seconds, at most three times.
inline bool ShouldKick(unsigned long inState, bool inHasUnloadGoal, unsigned long inCargo,
                       unsigned long inElapsed, int inKicks)
{
    return inState == kIdle && inHasUnloadGoal && inCargo > 0 && inElapsed >= kKickAfterMs && inKicks < kMaxKicks;
}

// The stall test in tw_unit: the same state on the same tile, with a goal or cargo aboard. An idle, empty transport
// is not stalled. Upstream restarts the clock on any change of state or tile.
inline bool IsStall(unsigned long inState, long inX, long inY, unsigned long inLastState, long inLastX, long inLastY,
                    bool inHasGoal, unsigned long inCargo)
{
    return inState == inLastState && inX == inLastX && inY == inLastY && (inHasGoal || inCargo > 0);
}

}  // namespace TransportWatchdogRules
}  // namespace AgeOfAI

#endif
