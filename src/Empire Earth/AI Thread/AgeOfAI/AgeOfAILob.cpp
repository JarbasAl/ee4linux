// Lobbing shot by shot (AgeOfAILob.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/lob.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include "AgeOfAILob.h"

namespace AgeOfAI {
namespace Lob {

bool Decide(State& ioState, bool inGameHigh, bool inBlocked)
{
    if (ioState.mHold > 0) {                    // lobs held after the game's wall hit
        if (--ioState.mHold == 0)
            ioState.mOurs = true;               // the next shot looks again
        return true;
    }
    if (inGameHigh && !ioState.mOurs) {         // the game's: a flat shot hit a wall
        ioState.mHold = kHold;
        return true;
    }
    ioState.mOurs = inBlocked;
    return inBlocked;
}

}
}
