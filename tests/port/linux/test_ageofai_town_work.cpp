// The reduced town_work rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITownWorkRules.h): a site whose nearest drop site
// is far from the citizen's home is swapped for work at home. Expected values are worked out from sites.c's
// TOWN_FAR and town_work_for, without reading back from the rules.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITownWorkRules.h"

using namespace AgeOfAI::TownWorkRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestDistance()
{
    Check(!IsFar(0, 0, 0, 0), "a drop site on the home tile is local");
    Check(!IsFar(60, 0, 0, 0), "a drop site at TOWN_FAR is local");
    Check(!IsFar(0, -60, 0, 0), "a drop site at TOWN_FAR north is local");
    Check(!IsFar(42, 42, 0, 0), "a diagonal inside TOWN_FAR is local");
    Check(IsFar(61, 0, 0, 0), "a drop site one tile past TOWN_FAR is far");
    Check(IsFar(-61, 0, 0, 0), "a drop site past TOWN_FAR west is far");
    Check(IsFar(0, 61, 0, 0), "a drop site past TOWN_FAR north is far");
    Check(IsFar(43, 43, 0, 0), "a diagonal past TOWN_FAR is far");
    Check(!IsFar(50, 50, 60, 60), "distance is measured from the home, not the origin");
}

static void TestShortfall()
{
    const long theShort[5] = {3, 7, 0, -2, 7};
    Check(BestShortfall(theShort, 0, 5) == 1, "the first largest shortfall wins");
    Check(BestShortfall(theShort, 1UL << 1, 5) == 4, "a tried shortfall cannot be chosen again");
    Check(BestShortfall(theShort, (1UL << 1) | (1UL << 4), 5) == 0, "the next largest shortfall is chosen");
    Check(BestShortfall(theShort, 0x1F, 5) == -1, "nothing is left when all are tried");
    Check(BestShortfall(theShort, 0, 0) == -1, "no resources means no choice");
    const long theNone[3] = {0, -1, -5};
    Check(BestShortfall(theNone, 0, 3) == -1, "no positive shortfall means no choice");
}

int main()
{
    TestDistance();
    TestShortfall();
    printf("ageofai town work: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
