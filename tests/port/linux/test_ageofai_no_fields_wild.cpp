// The reduced food_plan=no_fields_wild rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAINoFieldsWildRules.h): field
// foundations keep their builders while wild food is free. Expected values come from food.c food_fields_wait, not from
// the rules themselves.
#include <cstdio>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAINoFieldsWildRules.h"

using namespace AgeOfAI::NoFieldsWildRules;

static int sChecks, sFailures;

static void Check(bool inOk, const char* inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat);
    }
}

static void TestReach()
{
    Check(BushReach2(10) == 3600, "the bush reach is six times the drop-off distance");
    Check(BushReach2(0) == 0, "a player with no drop-off distance has no reach");
    Check(BushReach2(13) == 6084, "the reach is squared like upstream's r6 * r6 * 36");
}

static void TestDefer()
{
    Check(kFarmEpoch == 2, "fields come with the Copper Age");
    Check(!DeferFields(false, 5, true, true), "the bit off never defers");
    Check(!DeferFields(true, 1, true, true), "before the Copper Age wild food does not defer");
    Check(!DeferFields(true, 2, false, false), "no wild food does not defer");
    Check(DeferFields(true, 2, true, false), "a free bush defers");
    Check(DeferFields(true, 2, false, true), "a free animal defers");
    Check(DeferFields(true, 4, true, true), "later epochs still defer");
}

int main()
{
    TestReach();
    TestDefer();
    printf("ageofai no fields wild: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
