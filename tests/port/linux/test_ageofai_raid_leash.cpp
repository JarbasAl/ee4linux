// The reduced raid_leash rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIRaidLeashRules.h): a member of a force that
// chases past the leash around the force's destination goes home. Expected values are worked out from armies.c's
// a->out == 2 rule and AR_DONE, without reading back from the rules.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIRaidLeashRules.h"

using namespace AgeOfAI::RaidLeashRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestTasks()
{
    Check(IsRaidTask(kTaskLocation), "a location task is a raid");
    Check(IsRaidTask(kTaskAttackLocation), "an attack-location task is a raid");
    Check(!IsRaidTask(kTaskNone), "a group with no task is not a raid");
    Check(!IsRaidTask(kTaskTerritory), "a territory task is not a raid");
    Check(!IsRaidTask(kTaskUnit), "a hunt for one enemy unit is not a raid");
    Check(!IsRaidTask(kTaskReturn), "a force returning home is not a raid");
    Check(!IsRaidTask(kTaskDefendWonder), "defending a wonder is not a raid");
    Check(!IsRaidTask(kTaskAttackWonder), "attacking a wonder is not a raid");
}

static void TestDistance()
{
    Check(!BeyondLeash(0, 0, 12), "a member on the destination stays with the force");
    Check(!BeyondLeash(8, 0, 12), "a member at AR_DONE stays with the force");
    Check(!BeyondLeash(0, -8, 12), "a member eight tiles north stays with the force");
    Check(!BeyondLeash(14, 0, 12), "a member inside the leash stays with the force");
    Check(!BeyondLeash(10, 10, 12), "a diagonal inside the leash stays with the force");
    Check(!BeyondLeash(20, 0, 12), "a member exactly at the leash stays with the force");
    Check(BeyondLeash(21, 0, 12), "a member one tile past the leash goes home");
    Check(BeyondLeash(-21, 0, 12), "a member past the leash in the other direction goes home");
    Check(BeyondLeash(0, 21, 12), "a member past the leash to the north goes home");
    Check(BeyondLeash(15, 15, 12), "a diagonal past the leash goes home");
    Check(BeyondLeash(9, 0, 0), "with no leash the arrival radius alone bounds the force");
    Check(!BeyondLeash(8, 0, 0), "with no leash AR_DONE still holds a member");
}

static void TestCall()
{
    Check(ShouldCall(true, true, true, 21, 0, 12), "a chasing member of a raid is called home");
    Check(!ShouldCall(false, true, true, 21, 0, 12), "a member of a non-raid is left alone");
    Check(!ShouldCall(true, false, true, 21, 0, 12), "a dead member is left alone");
    Check(!ShouldCall(true, true, false, 21, 0, 12), "a member already out of the force is left alone");
    Check(!ShouldCall(true, true, true, 14, 0, 12), "a member inside the leash is left alone");
}

int main()
{
    TestTasks();
    TestDistance();
    TestCall();
    printf("ageofai raid leash: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
