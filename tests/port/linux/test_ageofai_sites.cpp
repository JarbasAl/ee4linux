// The rules of the mine search (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISites.h): which mine is another town's, and
// which one is the citizen's own. The expected values follow upstream's LOCAL_MINE_TILES and LOCAL_MINE_FAR in
// sites.c, worked out by hand.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAISites.h"

using namespace AgeOfAI::SitesRules;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static void TestOwnTown()
{
    Check(MineLocal(40, 0, 0, 0) == 1, "a drop site 40 tiles off is the citizen's own town's");
    Check(MineLocal(0, 40, 0, 0) == 1, "40 tiles down as well");
    Check(MineLocal(28, 28, 0, 0) == 1, "39.6 tiles on the diagonal: own town still");
    Check(MineLocal(41, 0, 0, 0) == 0, "41 tiles off: in reach, but not its own town's first");
    Check(MineLocal(-41, -41, 0, 0) == 0, "58 off on the other diagonal: in reach");
}

static void TestAnotherTown()
{
    Check(MineLocal(60, 0, 0, 0) == 0, "exactly 60 tiles: still considered");
    Check(MineLocal(61, 0, 0, 0) == -1, "61 tiles: another town's mine, refused");
    Check(MineLocal(43, 43, 0, 0) == -1, "60.8 tiles on the diagonal: refused");
    Check(MineLocal(100, 20, 5, 5) == -1, "a mine across the map: refused");
}

static void TestTheCitizensTile()
{
    // the distance is measured from where the citizen stands, not from the bottom left of the map
    Check(MineLocal(150, 100, 130, 100) == 1, "20 tiles from a citizen at 130,100: its own town's");
    Check(MineLocal(200, 100, 130, 100) == -1, "70 tiles from that citizen: another town's");
}

int main()
{
    TestOwnTown();
    TestAnotherTown();
    TestTheCitizensTile();
    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
