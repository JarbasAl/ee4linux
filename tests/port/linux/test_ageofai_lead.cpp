// Aiming ahead (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAILead.cpp). A projectile that flies at speed s from a shooter
// meets a target that walks at v where the closed form says it does; the expected aims are worked out by hand from that,
// not read back from the iteration.
#include <cmath>
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAILead.cpp"

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static bool Near(float a, float b, float inTolerance) { return fabsf(a - b) <= inTolerance; }

// a projectile at speed 20 tiles/s from (0, 5.5) in a straight line (no arc): flight time = distance / 20
static float Flight(float x, float y) { return sqrtf(x * x + (y - 5.5f) * (y - 5.5f)) / 20.0f; }

static void TestFree()
{
    // target at (30, 5.5) walking along x at 2 tiles/s; the aim is its position. Meeting point: x = 30 + 2T, and 20T = x,
    // so 18T = 30, T = 1.6667, x = 33.3333. Three iterations from the flight time to its position (1.5 s):
    // T 1.5 -> x 33.0 (T 1.65) -> x 33.3 (T 1.665) -> x 33.33 (T 1.6665).
    float x, y;
    float T = AgeOfAI::Lead::LeadAim(30.0f, 5.5f, 30.0f, 5.5f, false, 2.0f, 0.0f, Flight(30.0f, 5.5f), Flight, x, y);
    Check(Near(x, 33.3333f, 0.01f) && Near(y, 5.5f, 0.0001f), "the led aim is 33.33 (closed form: 30 / 0.9)");
    Check(Near(T, 1.6667f, 0.001f), "its flight time is 1.667 s");
    // across the line of fire: target (30, 5.5) walking along y at 3 tiles/s. Meeting point: (30, 5.5 + 3T), flight
    // sqrt(900 + 9T^2) / 20 = T -> 900 + 9T^2 = 400 T^2 -> T = 30 / sqrt(391) = 1.5173, y = 10.052
    T = AgeOfAI::Lead::LeadAim(30.0f, 5.5f, 30.0f, 5.5f, false, 0.0f, 3.0f, Flight(30.0f, 5.5f), Flight, x, y);
    Check(Near(x, 30.0f, 0.0001f) && Near(y, 10.052f, 0.01f), "across the line of fire: (30, 10.05)");
    // a target that stands: no change
    T = AgeOfAI::Lead::LeadAim(30.0f, 5.5f, 30.0f, 5.5f, false, 0.0f, 0.0f, 1.5f, Flight, x, y);
    Check(x == 30.0f && y == 5.5f && T == 1.5f, "a target that stands keeps its aim and its time");
}

static void TestCentred()
{
    // target on the tile whose centre is (10.5, 5.5), walking at 2 tiles/s; shooter at (0, 5.5): 18T = 10.5, T = 0.5833,
    // x = 11.667: the tile 11, its centre 11.5
    float x, y;
    float T = AgeOfAI::Lead::LeadAim(10.5f, 5.5f, 10.5f, 5.5f, true, 2.0f, 0.0f, Flight(10.5f, 5.5f), Flight, x, y);
    Check(x == 11.5f && y == 5.5f, "tile-centre aims stay tile centres: (11.5, 5.5)");
    Check(Near(T, 11.5f / 20.0f, 0.0001f), "the flight time is that of the centre, 0.575 s");
    // a target slow enough to stay on its tile: the aim stays
    T = AgeOfAI::Lead::LeadAim(10.5f, 5.5f, 10.5f, 5.5f, true, 0.1f, 0.0f, Flight(10.5f, 5.5f), Flight, x, y);
    Check(x == 10.5f && y == 5.5f, "a slow target stays on its tile: the aim stays");
    // the target stands at the right edge of its tile (10.95, 5.5) while the aim is the tile's centre: the led tile is that of
    // the target's position moved, not of the centre: T 0.525 -> 10.95 + 1.05 = 12.0 -> tile 12 (12.5), T 0.625 -> 12.2 -> 12.5
    T = AgeOfAI::Lead::LeadAim(10.95f, 5.5f, 10.5f, 5.5f, true, 2.0f, 0.0f, Flight(10.5f, 5.5f), Flight, x, y);
    Check(x == 12.5f && y == 5.5f, "a centred aim leads from the target's position, not from the tile centre: 12.5");
    // the target position and the aim differ for a hit offset: not centred, the aim moves by the target's distance
    T = AgeOfAI::Lead::LeadAim(30.0f, 5.5f, 30.0f, 7.0f, false, 2.0f, 0.0f, 1.5f, Flight, x, y);
    Check(x > 32.9f && x < 33.6f && y == 7.0f, "an offset aim moves along x only, y stays 7");
}

static void TestLimits()
{
    float x, y;
    // the flight time is limited to 6 s: a velocity of 10 tiles/s moves the first step 60 tiles at most
    int calls = 0;
    float firstX = 0;
    auto Slow = [&](float px, float py) -> float {
        if (calls++ == 0)
            firstX = px;
        return 100.0f;      // always far: the time stays over 6 s
    };
    AgeOfAI::Lead::LeadAim(30.0f, 5.5f, 30.0f, 5.5f, false, 10.0f, 0.0f, 100.0f, Slow, x, y);
    Check(Near(firstX, 90.0f, 0.001f), "a flight over 6 s leads 6 s only: 30 + 10 x 6 = 90");
    Check(calls == AgeOfAI::Lead::kIterations, "three iterations");
    // an unknown flight time (< 0) stops it and keeps the last good aim
    int n = 0;
    auto Unknown = [&](float px, float py) -> float { return n++ == 0 ? Flight(px, py) : -1.0f; };
    float T = AgeOfAI::Lead::LeadAim(30.0f, 5.5f, 30.0f, 5.5f, false, 2.0f, 0.0f, 1.5f, Unknown, x, y);
    Check(Near(x, 33.0f, 0.0001f) && Near(T, Flight(33.0f, 5.5f), 0.0001f), "unknown on the second try: the first led aim stays (33.0)");
    n = 1;
    T = AgeOfAI::Lead::LeadAim(30.0f, 5.5f, 30.0f, 5.5f, false, 2.0f, 0.0f, 1.5f, Unknown, x, y);
    Check(x == 30.0f && y == 5.5f && T == 1.5f, "unknown at once: the aim and the time stay");
}

int main()
{
    TestFree();
    TestCentred();
    TestLimits();
    printf("test_ageofai_lead.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
