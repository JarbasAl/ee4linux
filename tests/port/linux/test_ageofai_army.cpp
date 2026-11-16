// The rules of the outdated units' attack (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIArmy.h): which unit is outdated, and
// which enemy building the units attack. The expected values follow upstream's ou_outdated and target loop in army.c,
// worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIArmy.h"

using namespace AgeOfAI::ArmyRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestOutdated()
{
    Check(IsOutdated(5, false, 8, 9), "a land unit of last epoch 8 in epoch 9: outdated");
    Check(!IsOutdated(5, false, 9, 9), "last epoch 9 in epoch 9: the game still offers it");
    Check(!IsOutdated(5, false, 14, 3), "last epoch 14 (the default) in epoch 3: not outdated");
    Check(!IsOutdated(5, true, 2, 9), "a citizen of class Land: never sent");
    Check(!IsOutdated(6, false, 2, 9), "class 6 (a land citizen): not class Land");
    Check(!IsOutdated(2, false, 2, 9), "a building (class 2): not class Land");
    Check(!IsOutdated(4, false, 2, 9), "a ship (class 4): not class Land");
}

static void TestTarget()
{
    int a = 0, b = 0, c = 0, d = 0;
    {
        TargetChoice<int> theChoice;
        Check(theChoice.Chosen() == NULL, "no candidates: no target");
    }
    {
        TargetChoice<int> theChoice;
        theChoice.Offer(&a, 100, false);
        theChoice.Offer(&b, 25, true);
        theChoice.Offer(&c, 49, false);
        Check(theChoice.Chosen() == &c, "the nearest building that is no defence (49), though a tower is nearer (25)");
    }
    {
        TargetChoice<int> theChoice;
        theChoice.Offer(&a, 400, true);
        theChoice.Offer(&b, 81, true);
        theChoice.Offer(&c, 144, true);
        Check(theChoice.Chosen() == &b, "only defences left: the nearest of them (81)");
    }
    {
        TargetChoice<int> theChoice;
        theChoice.Offer(&a, 36, false);
        theChoice.Offer(&b, 36, false);
        theChoice.Offer(&c, 0, true);
        theChoice.Offer(&d, 37, false);
        Check(theChoice.Chosen() == &a, "two at 36: the first offered stays");
    }
    {
        TargetChoice<int> theChoice;
        theChoice.Offer(&a, 0, false);
        theChoice.Offer(&b, 0, true);
        Check(theChoice.Chosen() == &a, "a building on the middle itself (0) is taken");
    }
}

int main()
{
    TestOutdated();
    TestTarget();
    printf("ageofai army: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
