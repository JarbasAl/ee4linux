// Lobbing shot by shot ([general] lob_each_shot). A unit whose stats may lob (the bows but the crossbow, the slinger, the
// pilum, the trebuchet) shoots flat until one of its shots hits a wall on the way; the game then sets the unit to fire high
// and it lobs every later shot, at 1.5 times its reload, until it gets a new goal, the wall gone or not. Now it looks along
// the line to its target before each shot: a wall in the way gets the high arc, else it shoots flat. After the game's own
// flat shot hit a wall, it lobs kHold more shots before it looks again (else it would aim flat into that wall every other
// shot).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/engine/lob.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#ifndef AGEOFAI_LOB_H
#define AGEOFAI_LOB_H

namespace AgeOfAI {
namespace Lob {

const int kHold = 4;

// What a unit remembers of its lobbing: mOurs, its high arc was chosen by a look; mHold, the shots it still lobs after
// the game's own wall hit
struct State {
    State() : mOurs(false), mHold(0) {}
    bool mOurs;
    int mHold;
};

// Whether the next shot goes high. inGameHigh: the unit's fire-high flag as it stands (the game sets it when a flat shot hit
// a wall); inBlocked: a wall (not the target) stands in the line to the target.
bool Decide(State& ioState, bool inGameHigh, bool inBlocked);

}
}

#endif
