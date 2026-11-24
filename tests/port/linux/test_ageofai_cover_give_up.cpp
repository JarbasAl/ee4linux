// The reduced fall_back.give_up rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICoverGiveUpRules.h): a soldier that
// breaks off from one attack group fall_back_breaks times stays at cover while that group lasts. Expected values are
// worked out from cover.c's cv_tally and kind-3 hold, without reading back from the rules.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAICoverGiveUpRules.h"

using namespace AgeOfAI::CoverGiveUpRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestTallyReset()
{
    Check(!ResetTally(600000, 0), "the first break-off at ten minutes still counts");
    Check(!ResetTally(900001, 300001), "a record ten minutes old still counts");
    Check(ResetTally(900002, 300001), "a record ten minutes and one tick old starts again");
    Check(ResetTally(300001, 900001), "a new game starts again");
    Check(!ResetTally(0, 0), "a break-off at time zero counts");
}

static void TestGiveUp()
{
    Check(!GaveUp(1, 2), "one break-off is not a give-up at two");
    Check(GaveUp(2, 2), "two break-offs give up at two");
    Check(GaveUp(3, 2), "three break-offs give up at two");
    Check(GaveUp(1, 1), "one break-off is a give-up at one");
    Check(!GaveUp(2, 4), "two break-offs are not a give-up at four");
    Check(!GaveUp(2, 0), "a zero break count disables the give-up");
    Check(!GaveUp(2, -1), "a negative break count disables the give-up");
}

static void TestHold()
{
    Check(!GiveUpDone(true, 300000, 0), "a gave-up soldier stays for five minutes while its group lasts");
    Check(GiveUpDone(true, 300002, 1), "a gave-up soldier is released after five minutes");
    Check(GiveUpDone(false, 20000, 0), "a gave-up soldier is released when its group is gone");
    Check(GiveUpDone(false, 299999, 0), "a gone group releases before the time cap");
    Check(GiveUpDone(true, 0, 1), "a new game releases a gave-up soldier");
}

int main()
{
    TestTallyReset();
    TestGiveUp();
    TestHold();
    printf("ageofai cover give-up: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
