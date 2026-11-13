// The headless simulation's switches (include/platform/PlatSim.h), in the engine library so that the game and its
// plug-ins share them. All zero in a normal game.
#include <windows.h>

#include "platform/PlatSim.h"

extern "C" {

int gPlatSimMode;
volatile int gPlatSimRun;
void (*gPlatSimAfterTick)(void);
int gPlatSimSeedSet;
uint32_t gPlatSimSeed;
int gPlatSimNoHumanDefeat;
volatile long gPlatSimNoHumanDefeatHits;
int gPlatSimPortOffset;

}
