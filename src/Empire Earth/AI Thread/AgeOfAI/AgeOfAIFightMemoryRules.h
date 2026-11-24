// The deterministic part of Grille's cover.c cv_foe_mem and cv_mark_mem at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07: which remembered enemies still count in a fight judgement. Kept free of
// game types so the arithmetic is testable. AgeOfAIDanger.cpp reads the records and adds them to the estimate.
#ifndef AGEOFAI_FIGHTMEMORY_RULES_H
#define AGEOFAI_FIGHTMEMORY_RULES_H

namespace AgeOfAI {
namespace FightMemoryRules {

const unsigned kMemoryMs = 180000;   // CV_MEMORY: a remembered soldier counts for three game minutes
const float kLongRange = 30.0f;      // a range this long or longer does not widen a zone and is not counted as reach

// A record counts when it is not the one seen at the last scan (moved to "somewhere"), was seen before now, and is no
// older than the memory window (cv_foe_mem's t / last / now tests).
inline bool Fresh(unsigned long inNow, unsigned long inSeenAt, unsigned long inLast)
{
    return inSeenAt != inLast && inNow >= inSeenAt && inNow - inSeenAt <= kMemoryMs;
}

// The reach a remembered fighter adds to a zone: weapons that reach 30 tiles or more are treated as having no reach
// here, as in cv_foe_mem.
inline float Reach(float inRange)
{
    return inRange >= kLongRange ? 0.0f : inRange;
}

}
}

#endif
