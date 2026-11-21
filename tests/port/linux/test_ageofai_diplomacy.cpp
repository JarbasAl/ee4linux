// The reduced answer-war rules (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIDiplomacy.h): the 3-6 second delay and the
// two stance checks. The expected values follow diplo.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIDiplomacy.h"

using namespace AgeOfAI::DiplomacyRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestDelay()
{
    Check(AnswerWarDelay(1, 2) == 4000, "players 1 and 2 answer after 4 seconds");
    Check(AnswerWarDelay(2, 3) == 6000, "players 2 and 3 answer after 6 seconds");
    Check(AnswerWarDelay(1, 1) == 5000, "player 1 against itself computes 5 seconds");
    for (unsigned long p = 1; p < 8; p++)
        for (unsigned long j = 1; j < 8; j++) {
            unsigned long theDelay = AnswerWarDelay(p, j);
            Check(theDelay >= 3000 && theDelay <= 6000 && theDelay % 1000 == 0,
                  "delay stays in the 3 to 6 second grid");
        }
}

static void TestStances()
{
    Check(AnswerWarTriggered(1, 0, 0), "a new enemy that was an ally is a trigger");
    Check(!AnswerWarTriggered(0, 0, 0), "still an ally: no trigger");
    Check(!AnswerWarTriggered(1, 1, 0), "already an enemy on the last look: no trigger");
    Check(!AnswerWarTriggered(1, 0, 1), "already at war ourselves: no trigger");

    Check(AnswerWarDue(1, 0, 0), "still at war and still allied ourselves: answer");
    Check(!AnswerWarDue(0, 0, 0), "peace again: cancel");
    Check(!AnswerWarDue(1, 1, 0), "an answer already given: cancel");
    Check(!AnswerWarDue(1, 0, 2), "a defeated player: cancel");
}

int main()
{
    TestDelay();
    TestStances();
    printf("ageofai diplomacy: %d checks, %d failures\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
