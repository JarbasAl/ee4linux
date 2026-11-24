// The reduced transport_watchdog rule (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITransportWatchdogRules.h): a stalled
// transport is freed by dropping passengers that are not coming or by leaving Idle. The expected values follow
// transports.c's tw_passenger, tw_dog and tw_unit, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAITransportWatchdogRules.h"

using namespace AgeOfAI::TransportWatchdogRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestCarriers()
{
    const unsigned long theCarriers[] = { kLandTransport, kHelicopterTransport, kNavalTransport, kFortress };
    for (unsigned i = 0; i < sizeof theCarriers / sizeof theCarriers[0]; i++)
        Check(IsTransport(theCarriers[i]), "a land, air, sea or fortress carrier is watched");
    const unsigned long theOthers[] = { 0, 2, 4, 5, 6, 7, 0x15, 0x1a, 0x21 };
    for (unsigned i = 0; i < sizeof theOthers / sizeof theOthers[0]; i++)
        Check(!IsTransport(theOthers[i]), "a building, soldier, ship, aircraft, resource or space unit is not watched");
}

static void TestPassengers()
{
    Check(PassengerState(50, 8, true, true, true, 5001) == kBoard, "a passenger moving to the transport is coming");
    Check(PassengerState(50, 8, true, true, true, kIdle) == kIdleWithBoard, "a passenger idle with its board goal is not coming");
    Check(PassengerState(50, 8, true, false, false, 8000) == kNoGoal, "a passenger with no goal is not coming");
    Check(PassengerState(50, 8, true, true, false, 8000) == kOtherOrder, "a passenger with another order is not coming");
    Check(PassengerState(50, 8, true, true, false, kIdle) == kOtherOrder, "an idle passenger with another goal is not coming");
    Check(PassengerState(0, 8, true, true, true, 5001) == kGone, "a dead passenger is gone");
    Check(PassengerState(50, 8, false, true, true, 5001) == kGone, "a passenger without a class is gone");
    Check(PassengerState(50, -1, true, true, true, 5001) == kGone, "a passenger off the map is gone");

    Check(!ShouldDrop(kBoard), "a passenger still coming is kept");
    Check(ShouldDrop(kIdleWithBoard), "an idle passenger is dropped");
    Check(ShouldDrop(kNoGoal), "a passenger with no goal is dropped");
    Check(ShouldDrop(kOtherOrder), "a passenger with another order is dropped");
    Check(ShouldDrop(kGone), "a passenger that is gone is dropped");
}

static void TestDrop()
{
    Check(ShouldDropPassengers(kWaitForPassengers, true, 30000), "30 s waiting with a load goal drops the rest");
    Check(ShouldDropPassengers(kWaitForPassengers, true, 45000), "more than 30 s waiting also drops the rest");
    Check(!ShouldDropPassengers(kWaitForPassengers, true, 29999), "just under 30 s does not drop");
    Check(!ShouldDropPassengers(kWaitForPassengers, false, 45000), "no load goal means no drop");
    Check(!ShouldDropPassengers(kIdle, true, 45000), "an idle transport does not drop passengers");
    Check(!ShouldDropPassengers(kPrepareToMove, true, 45000), "a moving transport does not drop passengers");
}

static void TestKick()
{
    Check(ShouldKick(kIdle, true, 3, 20000, 0), "20 s idle with cargo and an unload goal is nudged");
    Check(ShouldKick(kIdle, true, 3, 25000, 2), "a third nudge is still allowed");
    Check(!ShouldKick(kIdle, true, 3, 19999, 0), "just under 20 s is not nudged");
    Check(!ShouldKick(kIdle, true, 3, 25000, 3), "a fourth nudge is not allowed");
    Check(!ShouldKick(kIdle, true, 0, 25000, 0), "an empty transport is not nudged");
    Check(!ShouldKick(kIdle, false, 3, 25000, 0), "no unload goal means no nudge");
    Check(!ShouldKick(kWaitForPassengers, true, 3, 25000, 0), "a waiting transport is not nudged");
    Check(!ShouldKick(kPrepareToMove, true, 3, 25000, 0), "a moving transport is not nudged");
}

static void TestStall()
{
    Check(IsStall(kIdle, 10, 20, kIdle, 10, 20, true, 0), "a transport with a goal on one tile in one state stalls");
    Check(IsStall(kIdle, 10, 20, kIdle, 10, 20, false, 3), "a loaded transport stalls without a goal");
    Check(!IsStall(kIdle, 10, 20, kIdle, 10, 20, false, 0), "an idle, empty transport does not stall");
    Check(!IsStall(kPrepareToMove, 10, 20, kIdle, 10, 20, true, 3), "a state change restarts the clock");
    Check(!IsStall(kIdle, 11, 20, kIdle, 10, 20, true, 3), "a move along x restarts the clock");
    Check(!IsStall(kIdle, 10, 21, kIdle, 10, 20, true, 3), "a move along y restarts the clock");
}

int main()
{
    TestCarriers();
    TestPassengers();
    TestDrop();
    TestKick();
    TestStall();
    printf("ageofai transport watchdog: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
