// The places of the early game (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIEarlyGeometry.cpp). Expected values are worked
// out by hand.
#include <cmath>
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIEarlyGeometry.cpp"

using namespace AgeOfAI::Early;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static bool Near(float a, float b) { return fabsf(a - b) < 0.001f; }

static void TestFront()
{
    float dx, dy;
    FrontToCentre(50, 50, 100, dx, dy);
    Check(dx == 1 && dy == 0, "a base at the centre: the front is (1, 0)");
    FrontToCentre(50, 50, 101, dx, dy);   // the centre (50, 50) again, integer division
    Check(dx == 1 && dy == 0, "map 101: the centre is tile 50 as well");
    FrontToCentre(20, 30, 100, dx, dy);   // (30, 20) / 36.056
    Check(Near(dx, 0.83205f) && Near(dy, 0.55470f), "base (20, 30), map 100: towards (30, 20): 0.832, 0.555");
    FrontToCentre(80, 50, 100, dx, dy);
    Check(Near(dx, -1.0f) && Near(dy, 0.0f), "a base east of the centre: the front points west");
}

static void TestSlots()
{
    int x, y;
    // front (0.832, 0.555) x 4 = (3.33, 2.22): (3, 2)
    CoreSlot(20, 30, 0.83205f, 0.55470f, 0, 0, 0, x, y);
    Check(x == 23 && y == 32, "slot 0 from (20, 30): (23, 32)");
    // across: perpendicular (-0.555, 0.832); citizens at (-3, 4): dot 1.66 + 3.33 > 0: kept; x4 = (-2.22, 3.33): (-2, 3)
    CoreSlot(20, 30, 0.83205f, 0.55470f, 1, -3, 4, x, y);
    Check(x == 18 && y == 33, "slot 1, citizens on the left: (18, 33)");
    // citizens at (3, -4): the dot is negative, the perpendicular flips to (0.555, -0.832): (2, -3)
    CoreSlot(20, 30, 0.83205f, 0.55470f, 1, 3, -4, x, y);
    Check(x == 22 && y == 27, "slot 1, citizens on the right: (22, 27)");
    // no citizens (a zero sum): the dot is 0, not negative: the perpendicular stays
    CoreSlot(20, 30, 0.83205f, 0.55470f, 1, 0, 0, x, y);
    Check(x == 18 && y == 33, "slot 1 with no citizens keeps the first perpendicular");
    // (0, 1) front: slot 0 is (bx, by + 4)
    CoreSlot(10, 10, 0, 1, 0, 0, 0, x, y);
    Check(x == 10 && y == 14, "front (0, 1): four tiles on");
}

static void TestBait()
{
    float x, y;
    BaitSpot(10, 10, 10, 20, x, y);   // enemy to the south (y larger): away from it is y smaller
    Check(Near(x, 10.5f) && Near(y, 9.0f), "enemy at (10, 20): 1.5 tiles past the tower's centre on its far side: (10.5, 9.0)");
    BaitSpot(10, 10, 4, 10, x, y);
    Check(Near(x, 12.0f) && Near(y, 10.5f), "enemy at (4, 10): to the east of the tower: (12.0, 10.5)");
    BaitSpot(10, 10, 10, 10, x, y);
    Check(Near(x, 10.5f) && Near(y, 10.5f), "enemy on the tower's tile: its centre");
    BaitSpot(0, 0, 3, 4, x, y);   // away from (3, 4): direction (-0.6, -0.8): (0.5 - 0.9, 0.5 - 1.2)
    Check(Near(x, -0.4f) && Near(y, -0.7f), "enemy at (3, 4) from (0, 0): (-0.4, -0.7)");
}

int main()
{
    TestFront();
    TestSlots();
    TestBait();
    printf("test_ageofai_early.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
