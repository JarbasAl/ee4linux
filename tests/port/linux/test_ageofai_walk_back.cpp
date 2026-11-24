// The reduced army_mass=walk_back rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIWalkBackRules.h): REGROUP walks a
// chasing soldier back to its subgroup's pack. Expected values come from mass.c's ms_subgroups and ms_tether, not from
// the rules themselves.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIWalkBackRules.h"

using namespace AgeOfAI::WalkBackRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestOutPastForming()
{
    const long theAway = 31L * 31;
    Check(!OutPastForming(0, theAway), "an idle subgroup is not out");
    Check(!OutPastForming(1000, theAway), "a subgroup waiting for units is not out");
    Check(!OutPastForming(1001, theAway), "a gathering subgroup is not out");
    Check(!OutPastForming(1004, theAway), "a subgroup preparing travel is not out");
    Check(!OutPastForming(8000, theAway), "a loading transport is not out");
    Check(!OutPastForming(3000, 30L * 30), "members exactly 30 tiles away are not out");
    Check(!OutPastForming(3000, 25L * 25), "members inside the safe radius are not out");
    Check(OutPastForming(3000, theAway), "a fighting subgroup past 30 tiles is out");
    Check(OutPastForming(2001, theAway), "a marching subgroup with away members is out");
}

static void TestFightingAndPack()
{
    Check(!IsFightingState(2999), "state below 3000 is not fighting");
    Check(IsFightingState(3000), "state 3000 is fighting");
    Check(IsFightingState(3003), "state 3003 is fighting");
    Check(IsFightingState(3004), "a retreat in the combat phase is still fighting");
    Check(IsFightingState(3999), "state 3999 is fighting");
    Check(!IsFightingState(4000), "state 4000 is past the fighting phase");
    Check(!IsFightingState(2001), "a marching state is not fighting");
    Check(!HasPack(0), "no away members make no pack");
    Check(!HasPack(1), "one away member makes no pack");
    Check(HasPack(2), "two away members make a pack");
    Check(HasPack(3), "three away members make a pack");
}

static void TestWalkBack()
{
    const long theBaseFar = 26L * 26;
    const long thePackFar = 17L * 17;
    Check(!ShouldWalkBack(25L * 25, thePackFar, 16, 100, 100), "a soldier safe by the base stays");
    Check(!ShouldWalkBack(theBaseFar, 15L * 15, 16, 100, 100), "a soldier close to the pack stays");
    Check(!ShouldWalkBack(theBaseFar, 16L * 16 - 1, 16, 100, 100), "a soldier just inside the tether stays");
    Check(ShouldWalkBack(theBaseFar, 16L * 16, 16, 100, 100), "a soldier at the tether walks back");
    Check(!ShouldWalkBack(theBaseFar, thePackFar, 16, 0, 0), "a goal on the pack stays");
    Check(!ShouldWalkBack(theBaseFar, thePackFar, 16, 3, -3), "a goal near the pack on both axes stays");
    Check(ShouldWalkBack(theBaseFar, thePackFar, 16, 4, 0), "a goal four tiles off on one axis walks back");
    Check(ShouldWalkBack(theBaseFar, thePackFar, 16, -4, 0), "a goal four tiles west walks back");
    Check(ShouldWalkBack(theBaseFar, thePackFar, 16, 3, 4), "a goal near on one axis but far on the other walks back");
    Check(ShouldWalkBack(theBaseFar, thePackFar, 16, 100, 100), "a chasing soldier with a far goal walks back");
}

int main()
{
    TestOutPastForming();
    TestFightingAndPack();
    TestWalkBack();
    printf("ageofai walk back: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
