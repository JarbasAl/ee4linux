// Lobbing shot by shot (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAILob.cpp): sequences of shots with what the game and the
// line of fire say, and the arc each shot takes, written by hand from the rules in AgeOfAILob.h.
#include <cstdio>
#include <string>
#include <vector>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAILob.cpp"

using namespace AgeOfAI::Lob;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

// One shot: the unit's fire-high flag now, the wall in the line; the flag after the shot is the arc it took
struct Shooter {
    State mState;
    bool mHigh;
    Shooter() : mHigh(false) {}
    bool Shoot(bool inBlocked)
    {
        mHigh = Decide(mState, mHigh, inBlocked);
        return mHigh;
    }
};

static void TestLook()
{
    Shooter s;
    Check(!s.Shoot(false), "no wall in the line: flat");
    Check(s.Shoot(true), "a wall appears in the line: the high arc");
    Check(s.Shoot(true), "the wall stands: high again");
    Check(!s.Shoot(false), "the wall is gone: flat again (the game's lobbed until a new goal)");
    Check(!s.Shoot(false), "and stays flat");
}

static void TestGameHit()
{
    // a flat shot hit a wall the look missed: the game sets the flag (mHigh true, not ours)
    Shooter s;
    s.mHigh = true;
    std::vector<bool> arcs;
    for (int i = 0; i < 7; i++)
        arcs.push_back(s.Shoot(false));
    // the game's hit shot and kHold more are high (5 shots), then it looks: no wall, flat
    bool expected[7] = {true, true, true, true, true, false, false};
    bool same = true;
    for (int i = 0; i < 7; i++)
        same = same && arcs[i] == expected[i];
    Check(same, "after the game's wall hit: 5 high shots (1 + the hold of 4), then flat when the line is clear");
    // a wall still in the line when it looks again: it stays high
    Shooter t;
    t.mHigh = true;
    for (int i = 0; i < 5; i++)
        t.Shoot(true);
    Check(t.Shoot(true), "the hold ends with the wall still there: the look says high");
    Check(t.mState.mOurs, "and the high arc is now its own (a look chose it)");
    // a wall hit by a flat shot again after it went flat: the game sets the flag again
    Shooter u;
    u.Shoot(false);
    u.mHigh = true;   // the game's, on this shot
    Check(u.Shoot(false), "a new game hit after flat shots: high and a new hold");
    Check(u.mState.mHold == kHold, "with the hold at 4 shots");
}

int main()
{
    TestLook();
    TestGameHit();
    printf("test_ageofai_lob.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
