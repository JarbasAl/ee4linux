// The switches of the headless simulation (ee-aoc --sim, src/Port/EESim.cpp). The simulation runs the game's own code
// with its front end idle: no splash, no movies, no rendering, the menus driven in-process, and the physics thread
// producing its own communications ticks back to back instead of waiting for the wall clock. Everything here is off
// (zero) in a normal game; the game's units test the flags at the few places the simulation differs.
//
// Ported from Protryon/empire_earth_next, include/platform/PlatSim.h at ddb437ab (no license granted by its authors).
#ifndef PLATFORM_PLATSIM_H
#define PLATFORM_PLATSIM_H

#include <stdint.h>

#include "lle/LowLevelEngineCore.h"   // LLE_API (needs <windows.h> first)

extern "C" {

// nonzero in the simulation (set before the game starts):
// - the graphics thread renders nothing (RTSGraphics::Main);
// - once the local server's game runs, its send thread sends no communications ticks of its own; the physics thread
//   sends the next one (NESynchronousServer::SimCommunicationsTick) as soon as it has run the last, without waiting
//   for its timer (RTSPhysics::Main), while gPlatSimRun is set.
LLE_API extern int gPlatSimMode;
// the physics thread runs ticks only while this is set (the driver sets it once the game is ready; clearing it holds
// the game between communications ticks)
LLE_API extern volatile int gPlatSimRun;
// called on the physics thread after every communications tick's physics ticks, under the physics lock
LLE_API extern void (*gPlatSimAfterTick)(void);
// the start seed: EEMStartLoading::Execute takes it instead of the lobby's rand() when gPlatSimSeedSet
LLE_API extern int gPlatSimSeedSet;
LLE_API extern uint32_t gPlatSimSeed;
// the idle local human is never taken out of the game nor defeated: EEWorld::RemovePlayerFromGame(false, local
// player) and EEPlayer::SetGameResult(2) on the local player do nothing, and count a hit
LLE_API extern int gPlatSimNoHumanDefeat;
LLE_API extern volatile long gPlatSimNoHumanDefeatHits;
// added to the TCP protocol's fixed ports 33334/33335 (0 in the game), so simulations on one machine do not share them
LLE_API extern int gPlatSimPortOffset;

}

#endif
