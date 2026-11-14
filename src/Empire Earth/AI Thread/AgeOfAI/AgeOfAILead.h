// Aiming ahead of a moving target ([general] lead_targets). A projectile flies a fixed arc to the point it was aimed at,
// and hits only when it lands on the target's footprint, so a unit walking across the line of fire is missed by every
// slow projectile (arrows, catapult stones, cannon balls). The aim point moves to where the target will be when the
// projectile comes down: its velocity times the flight time, the flight time from the game's own arc solver, iterated
// because the new point changes the arc.
//
// This part has no game types: the caller gives the flight time as a function of the aim point.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/aim.c
// (lead_aim) at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_LEAD_H
#define AGEOFAI_LEAD_H

#include <functional>

namespace AgeOfAI {
namespace Lead {

const float kMaxFlight = 6.0f;      // seconds: a longer flight is not led further
const int kIterations = 3;

// inFlight(x, y) is the seconds a projectile flies to (x, y), or < 0 when unknown. inCentred: the aim is the centre of
// the target's tile, so the led aim is the centre of the tile the target will be on; else the aim moves by the same
// distance as the target. inTargetX, inTargetY: the target's position; inAimX, inAimY: the aim; inVX, inVY: the target's
// velocity in tiles per second; inFlightTime: the flight time to the aim. Returns the flight time to the led aim, or
// inFlightTime when the aim does not move; outX, outY hold the led aim (the aim itself when unknown).
float LeadAim(float inTargetX, float inTargetY, float inAimX, float inAimY, bool inCentred, float inVX, float inVY,
              float inFlightTime, const std::function<float(float, float)>& inFlight, float& outX, float& outY);

}
}

#endif
