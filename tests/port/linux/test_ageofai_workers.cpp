// The worker plan before farms (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIWorkers.cpp). Expected values are worked out by
// hand from the rules in AgeOfAIWorkers.h.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIWorkers.cpp"

using namespace AgeOfAI::Workers;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static const float kPerWorker[7] = {0, 0, 10, 10, 5, 8, 6};

static void TestNeeds()
{
    long demand[7] = {0, 0, 0, 100, 300, 900, 0}, stock[7] = {0}, reserve[7] = {0}, need[7];
    Needs(demand, stock, reserve, 300, 0, 0, -1, need);
    // food: the reserve 0 + 300 - 0; wood 100; stone 300 (at the margin); gold 900 cut to 0 + 300; iron 0
    Check(need[2] == 300 && need[3] == 100 && need[4] == 300 && need[5] == 300 && need[6] == 0, "needs: food 300, wood 100, stone 300, gold 300 (900 cut by the margin), iron 0");
    // the reserve beyond the demand: gold reserve 1000, stock 200: 800 even though the demand's capped need is 100
    long reserve2[7] = {0, 0, 0, 0, 0, 1000, 0}, stock2[7] = {0, 0, 50, 0, 0, 200, 0};
    Needs(demand, stock2, reserve2, 300, 0, 0, -1, need);
    // gold: the demand 900 cut to 1000 + 300? no: 900 < 1300, so 900 - 200 = 700; the reserve's 1000 - 200 = 800 is larger
    Check(need[5] == 800 && need[2] == 250, "the reserve's 800 beats the demand's 700 for gold; food 0 + 300 - 50 = 250");
    // no margin (-1): the whole demand
    Needs(demand, stock, reserve, -1, 0, 0, -1, need);
    Check(need[5] == 900, "margin -1: gold's whole demand, 900");
    // tower stone, and 4 citizens to come: +200 food, +100 stone
    Needs(demand, stock, reserve, 300, 100, 4, -1, need);
    Check(need[4] == 400 && need[2] == 500, "100 more stone for towers; 4 citizens x 50 = 200 more food");
    Needs(demand, stock, reserve, 300, 0, 25, -1, need);
    Check(need[2] == 300 + 500, "at most 10 citizens are counted: +500");
    // the wood ceiling: the demand is 4500 wood, 50 in stock, buildings need up to 400: 350
    long demandW[7] = {0, 0, 0, 4500, 0, 0, 0}, stockW[7] = {0, 0, 0, 50, 0, 0, 0};
    Needs(demandW, stockW, reserve, 300, 0, 0, 400, need);
    Check(need[3] == 350, "wood: 4500 demanded, 400 the ceiling, 50 in stock: 350");
    Needs(demandW, stockW, reserve, 300, 0, 0, 20, need);
    Check(need[3] == 0, "a ceiling below the stock: 0 wood");
}

static void TestSplit()
{
    long need[7] = {0, 0, 300, 100, 300, 300, 0};
    unsigned long target[7] = {0};
    // weights need / income: 30, 10, 60, 37.5, 0 = 137.5; total 20: 16 to share; floors 2 and 2:
    // food 2 + int(3.49) = 5, wood 2 + int(1.16) = 3, stone int(6.98) = 6, gold int(4.36) = 4, iron 0: 18; the rest 2 to food, wood
    Check(Split(need, kPerWorker, 20, true, 1, 18, target), "the split happens");
    Check(target[2] == 6 && target[3] == 4 && target[4] == 6 && target[5] == 4 && target[6] == 0, "20 workers: 6, 4, 6, 4, 0");
    // copper plan 2: half the workers on food, up to 18: food to 10 from the largest others
    unsigned long target2[7] = {0};
    Split(need, kPerWorker, 20, true, 2, 18, target2);
    Check(target2[2] == 10 && target2[3] == 3 && target2[4] == 3 && target2[5] == 4 && target2[6] == 0, "copper plan 2: food 10 (taken from stone 6 to 3 and wood 4 to 3)");
    // the floor of 18: 60 workers: half is 30, the floor 18 holds
    long big[7] = {0, 0, 100, 100, 100, 100, 100};
    unsigned long target3[7] = {0};
    Split(big, kPerWorker, 60, true, 2, 18, target3);
    int sum = 0;
    for (int r = 2; r < 7; r++) sum += (int)target3[r];
    Check(sum == 60 && target3[2] >= 18, "60 workers: all given out, food at least 18");
    // by need, not time (reserve plan off): 300 vs 100 vs 300 vs 300 = 1000: 16 * 0.3 = 4.8 ...
    unsigned long target4[7] = {0};
    Split(need, kPerWorker, 20, false, 1, 18, target4);
    // food 2 + int(4.8) = 6, wood 2 + int(1.6) = 3, stone int(4.8) = 4, gold int(4.8) = 4, iron 0 = 17; rest 3: food, wood, stone
    Check(target4[2] == 7 && target4[3] == 4 && target4[4] == 5 && target4[5] == 4, "by need alone: 7, 4, 5, 4");
    // nothing needed, or too few workers: no change
    long none[7] = {0};
    unsigned long keep[7] = {9, 9, 9, 9, 9, 9, 9};
    Check(!Split(none, kPerWorker, 20, true, 1, 18, keep) && keep[3] == 9, "no need: the targets stay");
    Check(!Split(need, kPerWorker, 5, true, 1, 18, keep) && keep[3] == 9, "fewer than 6 workers: the targets stay");
    long negative[7] = {0, 0, -50, -50, 0, 0, 0};
    Check(!Split(negative, kPerWorker, 20, true, 1, 18, keep), "negative needs count as none");
}

static void TestVisible()
{
    long stock[7] = {0, 0, 600, 300, 100, 900, 50}, reserve[7] = {0, 0, 400, 100, 0, 800, 200}, visible[7];
    // a soldier: the stock above the reserve; below it 0 (iron 50 of 200 reserve)
    Visible(stock, reserve, false, 1, 50, 40, visible);
    Check(visible[2] == 200 && visible[3] == 200 && visible[4] == 100 && visible[5] == 100 && visible[6] == 0,
          "a soldier sees the stock above the reserve: 200, 200, 100, 100, 0");
    // a citizen, workforce 5 (under 10): the whole stock
    Visible(stock, reserve, true, 1, 5, 40, visible);
    Check(visible[2] == 600 && visible[5] == 900, "a citizen with a workforce of 5 sees everything");
    // a citizen, workforce 30 (under min_workforce 40): the whole stock too (the economy grows first)
    Visible(stock, reserve, true, 1, 30, 40, visible);
    Check(visible[2] == 600, "a workforce of 30 of 40: the whole food");
    // a citizen, workforce 45: its food is under the reserve, the other resources are all visible (citizens pay food only)
    Visible(stock, reserve, true, 1, 45, 40, visible);
    Check(visible[2] == 200 && visible[3] == 300 && visible[5] == 900, "a workforce of 45: food above the reserve (200), the rest all");
    // copper plan 0 (the game): a citizen sees the stock above the reserve like a soldier
    Visible(stock, reserve, true, 0, 5, 40, visible);
    Check(visible[2] == 200, "no copper plan: no exception for citizens");
}

int main()
{
    TestVisible();
    TestNeeds();
    TestSplit();
    printf("test_ageofai_workers.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
