// The reduced early_requirements rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIEarlyRequirementsRules.h): ask the
// research manager for the epoch's new things before the advance timer. Expected values come from econ.c, not from the
// rules themselves.
#include <cstdio>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIEarlyRequirementsRules.h"

using namespace AgeOfAI::EarlyRequirementsRules;

static int sChecks, sFailures;

static void Check(bool inOk, const char* inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat);
    }
}

static void TestDecoration()
{
    Check(IsDecorationName("x Statue"), "the editor's decoration prefix");
    Check(!IsDecorationName("Tower"), "a normal unit name");
    Check(!IsDecorationName("x"), "one character is not the prefix");
    Check(!IsDecorationName("xStatue"), "the prefix needs a space");
    Check(!IsDecorationName(NULL), "no name");
}

static void TestFirstAsk()
{
    const unsigned long kNow = 10000;
    Check(AskDecision(kNow, 0, 0, 0, 0, true, false) == 0, "no missing thing asks nothing");
    Check(AskDecision(kNow, 0, 0, 2, 0, true, false) == kAsk, "prefarm asks as soon as the epoch is reached");
    Check(AskDecision(kNow, 0, 0, 2, 700000, false, false) == 0, "a distant attempt waits outside prefarm");
    Check(AskDecision(kNow, 0, 0, 2, 610001, false, false) == 0, "ten minutes and one tick still waits");
    Check(AskDecision(kNow, 0, 0, 2, 610000, false, false) == kAsk, "ten minutes out is near enough");
    Check(AskDecision(kNow, 0, 0, 2, 5000, false, false) == kAsk, "an overdue attempt asks at once");
    Check(AskDecision(kNow, 0, 0, 2, 700000, true, false) == kAsk, "prefarm asks regardless of the timer");
    Check(AskDecision(kNow, 0, 0, 0, 0, true, true) == 0, "a filled gap with a request out asks nothing");
}

static void TestRenew()
{
    const unsigned long kNow = 10000;
    Check(AskDecision(kNow, 5000, 2, 2, 0, true, true) == 0, "three minutes after the ask waits");
    Check(AskDecision(184999, 5000, 2, 2, 0, true, true) == 0, "just inside the renewal window waits");
    Check(AskDecision(185000, 5000, 2, 2, 0, true, true) == kRenew, "the ask renews after three minutes");
    Check(AskDecision(200000, 5000, 2, 2, 0, true, true) == kRenew, "long past three minutes renews");
    Check(AskDecision(kNow, 5000, 3, 2, 0, true, true) == 0, "progress inside three minutes waits");
    Check(AskDecision(185000, 5000, 3, 2, 0, true, true) == kProgress, "progress after three minutes restarts the wait");
    Check(AskDecision(185000, 5000, 2, 3, 0, true, true) == kRenew, "a larger gap does not stop the renewal");
    Check(AskDecision(185000, 5000, 2, 0, 0, true, true) == 0, "a filled gap stops the renewal");
    Check(AskDecision(185000, 5000, 2, 2, 900000, false, true) == 0, "a distant attempt holds the renewal outside prefarm");
    Check(AskDecision(185000, 0, 0, 2, 0, true, true) == kRenew, "a request out before any ask still asks");
}

int main()
{
    TestDecoration();
    TestFirstAsk();
    TestRenew();
    printf("ageofai early requirements: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
