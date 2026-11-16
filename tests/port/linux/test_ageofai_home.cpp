// The rules of the idle citizens' way home (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIHome.h): which citizen goes, and
// where it stops. The expected values follow upstream's citizens_home in econ.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIHome.h"

using namespace AgeOfAI::HomeRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static bool Near(float inA, float inB)
{
    return inA - inB < 1e-4f && inB - inA < 1e-4f;
}

static void TestWhoGoes()
{
    Check(!IdleFar(15 * 15), "idle 15 tiles from the nearest town: stays");
    Check(IdleFar(15 * 15 + 1), "idle just past 15 tiles: goes home");
    Check(HeadingToBase(2, 2), "a walk ending 2 and 2 tiles off the base (8 squared): to the base");
    Check(!HeadingToBase(3, 1), "3 and 1 tiles off (10 squared): elsewhere");
    Check(HeadingToBase(3, 0), "3 tiles off: to the base");
    Check(TownCloser(5.0f, 15.0f), "a town at 5 tiles, the base at 15: 10 nearer, the citizen turns");
    Check(!TownCloser(5.5f, 15.0f), "a town at 5.5 tiles, the base at 15: keeps walking to the base");
}

static void TestSpot()
{
    float x, y;
    // the citizen at (13, 14), the town at (10, 10): 3 and 4 tiles off, length 5; 4 tiles out: 2.4 and 3.2
    Spot(10, 10, 13, 14, x, y);
    Check(Near(x, 12.9f) && Near(y, 13.7f), "4 tiles from the town's middle towards the citizen: (12.9, 13.7)");
    Spot(10, 10, 10, 10, x, y);
    Check(Near(x, 14.5f) && Near(y, 10.5f), "a citizen on the town's tile stops 4 tiles east: (14.5, 10.5)");
    Spot(10, 10, 4, 10, x, y);
    Check(Near(x, 6.5f) && Near(y, 10.5f), "a citizen due west: 4 tiles west of the middle, (6.5, 10.5)");
}

int main()
{
    TestWhoGoes();
    TestSpot();
    printf("ageofai home: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
