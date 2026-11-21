// The reduced hospital rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIHospitalRules.h): who walks to a hospital, where
// it stops, and when it goes back. The expected values follow cover.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIHospitalRules.h"

using namespace AgeOfAI::HospitalRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestNeeds()
{
    Check(NeedsHospital(39, 100, 0.40f), "39 of 100 hit points is below 40 percent");
    Check(!NeedsHospital(40, 100, 0.40f), "40 of 100 is the cutoff, not below it");
    Check(!NeedsHospital(41, 100, 0.40f), "41 of 100 stays in the fight");
    Check(NeedsHospital(79, 200, 0.40f), "79 of 200 is below 40 percent");
    Check(!NeedsHospital(0, 100, 0.40f), "a dead unit is not sent");
    Check(!NeedsHospital(10, 0, 0.40f), "no maximum, no rule");
}

static void TestTarget()
{
    long theX = 0, theY = 0;
    HospitalTarget(10, 0, 0, 0, theX, theY);
    Check(theX == 2 && theY == 0, "two tiles east of the hospital");
    HospitalTarget(10, 10, 0, 0, theX, theY);
    Check(theX == 1 && theY == 1, "diagonal: one tile each way");
    HospitalTarget(0, 0, 0, 0, theX, theY);
    Check(theX == 2 && theY == 0, "on the hospital: the default east stop");
    HospitalTarget(-10, 0, 0, 0, theX, theY);
    Check(theX == -2 && theY == 0, "two tiles west of the hospital");
}

static void TestRelease()
{
    Check(HospitalRelease(90, 100, true, 1000, 0), "90 percent heals and goes back");
    Check(!HospitalRelease(89, 100, true, 1000, 0), "89 percent stays");
    Check(HospitalRelease(50, 100, false, 1000, 0), "no hospital near: let go");
    Check(HospitalRelease(50, 100, true, 240001, 0), "four minutes and one tick: let go");
    Check(!HospitalRelease(50, 100, true, 240000, 0), "four minutes exactly: still held");
    Check(!HospitalRelease(50, 0, true, 1000, 0), "no maximum: the heal check cannot release it");
    Check(HospitalRelease(50, 0, true, 240001, 0), "no maximum: the timeout still releases it");
}

int main()
{
    TestNeeds();
    TestTarget();
    TestRelease();
    printf("ageofai hospital: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
