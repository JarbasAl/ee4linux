// The reduced answer_fire rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIAnswerFireRules.h): an idle fighter shot
// from beyond its reply range goes for the aiming enemy. The expected values follow unitsfirst.c's af_fighter,
// af_reachable, answer_fire_tick and uf_seen, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIAnswerFireRules.h"

using namespace AgeOfAI::AnswerFireRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestMode()
{
    Check(!AppliesTo(kGame, false), "the shipped game value does nothing");
    Check(!AppliesTo(kGame, true), "the shipped game value does nothing for a human either");
    Check(AppliesTo(kAll, false), "all covers a computer player");
    Check(AppliesTo(kAll, true), "all covers a human player");
    Check(AppliesTo(kComputers, false), "computers covers a computer player");
    Check(!AppliesTo(kComputers, true), "computers skips a human player");
}

static void TestFighters()
{
    const unsigned long theFighters[] = { kLand, kLandMilitaryHero, kNaval, kSubmergedNaval, kHelicopter };
    for (unsigned i = 0; i < sizeof theFighters / sizeof theFighters[0]; i++) {
        Check(IsFighter(theFighters[i], 12.0f), "a land, sea or hovering fighter with an attack answers fire");
        Check(!IsFighter(theFighters[i], 0.0f), "a fighter without an attack does not answer fire");
        Check(!IsFighter(theFighters[i], -1.0f), "a negative attack does not answer fire");
    }
    const unsigned long theNonFighters[] = { 6, 0xd, 0x10, 0x12, 0x16, 0x11, 0x1a, 2, 3, 9 };
    for (unsigned i = 0; i < sizeof theNonFighters / sizeof theNonFighters[0]; i++)
        Check(!IsFighter(theNonFighters[i], 12.0f), "a citizen, priest, transport, building, aircraft or shot does not answer fire");
}

static void TestAreas()
{
    Check(AreaKind(kLand) == kLandArea, "a land unit uses the land area");
    Check(AreaKind(kLandMilitaryHero) == kLandArea, "a land hero uses the land area");
    Check(AreaKind(kNaval) == kWaterArea, "a ship uses the water area");
    Check(AreaKind(kSubmergedNaval) == kWaterArea, "a submarine uses the water area");
    Check(AreaKind(kHelicopter) == kFlyingArea, "a helicopter needs no area");

    Check(Reachable(kLand, 7, 7), "land units on the same land reach each other");
    Check(!Reachable(kLand, 7, 8), "land units on different lands do not reach each other");
    Check(!Reachable(kLand, 0, 0), "no land means no reach");
    Check(Reachable(kNaval, 3, 3), "ships on the same water reach each other");
    Check(!Reachable(kNaval, 3, 4), "ships on different water do not reach each other");
    Check(Reachable(kHelicopter, 0, 0), "a helicopter flies over the missing area");
    Check(Reachable(kHelicopter, 7, 8), "a helicopter flies between two lands");
}

static void TestShouldAnswer()
{
    Check(ShouldAnswer(kIdle, false, 100, kLand, 12.0f), "an idle armed fighter answers fire");
    Check(!ShouldAnswer(2001, false, 100, kLand, 12.0f), "a unit already firing does not answer fire");
    Check(!ShouldAnswer(kIdle, true, 100, kLand, 12.0f), "a unit with a goal does not answer fire");
    Check(!ShouldAnswer(kIdle, false, 0, kLand, 12.0f), "a dead unit does not answer fire");
    Check(!ShouldAnswer(kIdle, false, -5, kLand, 12.0f), "a negative hit point count does not answer fire");
    Check(!ShouldAnswer(kIdle, false, 100, 6, 12.0f), "a citizen does not answer fire");
}

static void TestCanAnswer()
{
    Check(InReach(24, 0), "twenty-four tiles along an axis is in reach");
    Check(InReach(0, -24), "twenty-four tiles the other way is in reach");
    Check(InReach(16, 16), "a diagonal inside the reach is in reach");
    Check(!InReach(17, 17), "a diagonal just outside the reach is out");
    Check(!InReach(24, 1), "one tile beyond the reach is out");
    Check(!InReach(25, 0), "twenty-five tiles is out");

    Check(CanAnswer(10, 0, 50, 8, true, true, true), "a seen, hurt-able enemy on the same area is answered");
    Check(!CanAnswer(10, 0, 0, 8, true, true, true), "a dead enemy is not answered");
    Check(!CanAnswer(10, 0, 50, -1, true, true, true), "an enemy off the map is not answered");
    Check(!CanAnswer(10, 0, 50, 8, false, true, true), "an unseen enemy is not answered");
    Check(!CanAnswer(10, 0, 50, 8, true, false, true), "an enemy the weapon cannot hurt is not answered");
    Check(!CanAnswer(10, 0, 50, 8, true, true, false), "an enemy on another area is not answered");
    Check(!CanAnswer(25, 0, 50, 8, true, true, true), "an enemy beyond the reach is not answered");
}

int main()
{
    TestMode();
    TestFighters();
    TestAreas();
    TestShouldAnswer();
    TestCanAnswer();
    printf("ageofai answer fire: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
