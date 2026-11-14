// The memory of an Age of AI player (src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIIntel.cpp): what counts as seen, what
// stays known, what moves on, what is forgotten. Each expected value is worked out by hand from the rules in
// AgeOfAIIntel.h, not read back from the memory.
#include <cstdio>
#include <string>

#include "../../../src/Empire Earth/AI Thread/AgeOfAI/AgeOfAIIntel.cpp"

using namespace AgeOfAI::Intel;

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

static Sighting Unit(unsigned long inID, int inOwner, Kind inKind, int inX, int inY, unsigned inCount, int inHP = 100,
                     float inAttack = 2.0f)
{
    Sighting s = {};
    s.mUnit = inID; s.mOwner = inOwner; s.mType = 7; s.mKind = inKind; s.mX = inX; s.mY = inY;
    s.mHitPoints = inHP; s.mAttack = inAttack; s.mCount = inCount;
    return s;
}

// A scan of the units given, all of them live; eyes at the places given
static Scan MakeScan(unsigned inNow, std::vector<Sighting> inEnemies, std::vector<Eye> inEyes = {})
{
    Scan s;
    s.mNow = inNow;
    s.mEnemies = inEnemies;
    s.mEyes = inEyes;
    for (const Sighting& e : inEnemies)
        s.mLive.push_back(e.mUnit);
    std::sort(s.mLive.begin(), s.mLive.end());
    return s;
}

static const Record* Find(const Memory& inMemory, unsigned long inUnit)
{
    for (const Record& r : inMemory.Records())
        if (r.mUnit == inUnit)
            return &r;
    return NULL;
}

// The floor is the least count among an owner's units: a count above it is seen, a count at it is not
static void TestFloor()
{
    Memory m;
    // owner 2: counts 1, 1, 3; owner 3: counts 2, 2 (each owner has its own floor)
    Scan s = MakeScan(3000, {Unit(10, 2, kSoldier, 20, 20, 1), Unit(11, 2, kSoldier, 21, 20, 1), Unit(12, 2, kSoldier, 22, 20, 3),
                             Unit(20, 3, kSoldier, 40, 40, 2), Unit(21, 3, kSoldier, 41, 40, 2)});
    m.Update(s);
    Check(m.Floor(2) == 1, "owner 2's floor is its least count, 1");
    Check(m.Floor(3) == 2, "owner 3's floor is 2, not owner 2's");
    Check(m.SeenNow() == 1, "of five units only the one above its floor is seen");
    Check(Find(m, 12) != NULL && Find(m, 10) == NULL && Find(m, 20) == NULL, "the seen unit is remembered, the floor ones not");
    Check(!m.IsSeen(s.mEnemies[0]) && m.IsSeen(s.mEnemies[2]), "IsSeen: count 1 of floor 1 no, count 3 yes");

    // a unit hitting one of the viewer's units is seen at the floor
    Memory r;
    Scan t = MakeScan(3000, {Unit(10, 2, kSoldier, 20, 20, 1), Unit(11, 2, kSoldier, 21, 20, 1)});
    t.mEnemies[1].mRevealed = true;
    r.Update(t);
    Check(r.SeenNow() == 1 && Find(r, 11) != NULL && Find(r, 10) == NULL, "a revealed unit is seen at the floor, its neighbour is not");
}

// A building stays known until its place is looked at and it is gone
static void TestBuildings()
{
    // seen, then not seen with no eye near and the unit alive: still remembered
    Memory m;
    m.Update(MakeScan(3000, {Unit(50, 2, kBuilding, 100, 100, 3), Unit(51, 2, kSoldier, 5, 5, 1)}));
    Check(m.Remembers(50), "a building seen is remembered");
    m.Update(MakeScan(6000, {Unit(50, 2, kBuilding, 100, 100, 1), Unit(51, 2, kSoldier, 5, 5, 1)}));
    Check(m.Remembers(50) && Find(m, 50)->mTime == 3000, "unseen now and nobody near: still remembered, last seen at 3000");
    // looked at, building still there but at the floor (cannot happen in the game; a unit at the floor is not seen): an
    // eye within 6 tiles and the unit gone from the list: the memory drops it
    Scan gone = MakeScan(9000, {Unit(51, 2, kSoldier, 5, 5, 1)}, {Eye{103, 100}});
    m.Update(gone);
    Check(!m.Remembers(50) && Find(m, 50) == NULL, "an eye 3 tiles from where it stood and it is gone: dropped");

    // an eye 7 tiles away does not look at the spot (6 tiles)
    Memory far;
    far.Update(MakeScan(3000, {Unit(50, 2, kBuilding, 100, 100, 3), Unit(51, 2, kSoldier, 5, 5, 1)}));
    far.Update(MakeScan(9001, {Unit(51, 2, kSoldier, 5, 5, 1)}, {Eye{107, 100}}));
    Check(far.Remembers(50), "an eye 7 tiles away does not look at the spot: it stays remembered (the unit is gone 6.001 s)");
    Memory near;
    near.Update(MakeScan(3000, {Unit(50, 2, kBuilding, 100, 100, 3), Unit(51, 2, kSoldier, 5, 5, 1)}));
    near.Update(MakeScan(9001, {Unit(51, 2, kSoldier, 5, 5, 1)}, {Eye{106, 100}}));
    Check(!near.Remembers(50), "an eye exactly 6 tiles away looks at the spot: dropped");

    // gone from the list within 6 seconds of the last sighting: it died in sight
    Memory died;
    died.Update(MakeScan(3000, {Unit(50, 2, kBuilding, 100, 100, 3), Unit(51, 2, kSoldier, 5, 5, 1)}));
    died.Update(MakeScan(6000, {Unit(51, 2, kSoldier, 5, 5, 1)}));
    Check(!died.Remembers(50), "gone 3 seconds after the last sighting: dropped, it died in sight");
    // gone for longer than 6 seconds: destroyed out of sight, still remembered
    Memory unseen;
    unseen.Update(MakeScan(3000, {Unit(50, 2, kBuilding, 100, 100, 3), Unit(51, 2, kSoldier, 5, 5, 1)}));
    unseen.Update(MakeScan(9001, {Unit(51, 2, kSoldier, 5, 5, 1)}));
    Check(unseen.Remembers(50), "gone 6.001 seconds after the last sighting with nobody near: still remembered");
    Memory edge;
    edge.Update(MakeScan(3000, {Unit(50, 2, kBuilding, 100, 100, 3), Unit(51, 2, kSoldier, 5, 5, 1)}));
    edge.Update(MakeScan(9000, {Unit(51, 2, kSoldier, 5, 5, 1)}));
    Check(!edge.Remembers(50), "gone exactly 6 seconds after the last sighting: dropped");
}

// A soldier counts for 10 minutes where it was seen, and moves on when the place is looked at without it
static void TestSoldiers()
{
    Memory m;
    m.Update(MakeScan(3000, {Unit(10, 2, kSoldier, 50, 50, 1, 80, 2.5f), Unit(11, 2, kSoldier, 52, 50, 3, 120, 4.0f),
                             Unit(12, 2, kSoldier, 90, 90, 3, 200, 1.0f)}));
    // 11 and 12 are above the floor (1): seen
    float hp = 0, att = 0, pw = 0; int n = 0;
    m.Army(hp, att, pw, n);
    Check(n == 2 && hp == 320 && att == 5.0f && pw == 120 * 4.0f + 200 * 1.0f, "the army it knows of: units 11 and 12, hp 320, attack 5, power 680");
    // seen now: not in Soldiers (only what is remembered, not seen now)
    hp = att = pw = 0; n = 0;
    m.Soldiers(50, 50, 10, hp, att, pw, n);
    Check(n == 0, "Soldiers leaves out what it sees now");

    // 3 seconds on: 11 still alive and not seen (count at the floor, no eye near): remembered where it was
    m.Update(MakeScan(6000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 60, 60, 1), Unit(12, 2, kSoldier, 90, 90, 1)}));
    hp = att = pw = 0; n = 0;
    m.Soldiers(50, 50, 10, hp, att, pw, n);
    Check(n == 1 && hp == 120 && att == 4.0f && pw == 480, "within 10 tiles of (50,50): 11 at (52,50), 12 at (90,90) is far");
    hp = att = pw = 0; n = 0;
    m.Soldiers(50, 50, 2, hp, att, pw, n);
    Check(n == 1, "radius 2 reaches (52,50): the distance 2 counts");
    hp = att = pw = 0; n = 0;
    m.Soldiers(50, 50, 1, hp, att, pw, n);
    Check(n == 0, "radius 1 does not");

    // an eye looks at 12's spot (90,90) and it is not seen there: it moved on, "somewhere"
    m.Update(MakeScan(9000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 60, 60, 1), Unit(12, 2, kSoldier, 20, 20, 1)}, {Eye{90, 93}}));
    Check(Find(m, 12) != NULL && Find(m, 12)->mX < 0 && m.Moved() == 1, "looked at (90,90) from 3 tiles, not seen: moved on");
    hp = att = pw = 0; n = 0;
    m.Army(hp, att, pw, n);
    Check(n == 2 && hp == 320, "a unit that moved on is still in the army it knows of");
    hp = att = pw = 0; n = 0;
    m.Soldiers(90, 90, 60, hp, att, pw, n);
    Check(n == 1 && hp == 120, "Soldiers within 60 tiles of (90,90): 11 at (52,50) is 55.2 away; the one that moved on is left out");

    // an unseen soldier older than 5 minutes is left out of Soldiers but stays in the army until 10 minutes
    Memory old;
    old.Update(MakeScan(3000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 52, 50, 3, 120, 4.0f)}));
    old.Update(MakeScan(303000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 52, 50, 1)}));
    hp = att = pw = 0; n = 0;
    old.Soldiers(50, 50, 10, hp, att, pw, n);
    Check(n == 1, "seen exactly 5 minutes ago: counts");
    old.Update(MakeScan(303001 + 3000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 52, 50, 1)}));
    hp = att = pw = 0; n = 0;
    old.Soldiers(50, 50, 10, hp, att, pw, n);
    Check(n == 0, "seen over 5 minutes ago: no");
    n = 0; hp = att = pw = 0;
    old.Army(hp, att, pw, n);
    Check(n == 1, "but still in the army until 10 minutes");
    old.Update(MakeScan(603000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 52, 50, 1)}));
    n = 0; hp = att = pw = 0;
    old.Army(hp, att, pw, n);
    Check(n == 1, "seen exactly 10 minutes ago: it counts");
    old.Update(MakeScan(603001 + 3000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 52, 50, 1)}));
    n = 0; hp = att = pw = 0;
    old.Army(hp, att, pw, n);
    Check(n == 0 && Find(old, 11) == NULL, "seen over 10 minutes ago: forgotten, dropped from the records");

    // a soldier gone from the list soon after the last sighting died in sight
    Memory dead;
    dead.Update(MakeScan(3000, {Unit(10, 2, kSoldier, 50, 50, 1), Unit(11, 2, kSoldier, 52, 50, 3)}));
    dead.Update(MakeScan(6000, {Unit(10, 2, kSoldier, 50, 50, 1)}));
    Check(Find(dead, 11) == NULL && dead.Dropped() == 1, "a soldier gone 3 seconds after being seen: dead, dropped");
}

static void TestLooked()
{
    Memory m;
    m.Update(MakeScan(3000, {}, {Eye{30, 30}}));
    // (30,30) is in cell (5,5), its middle (33,33): 18 from the eye, within 64: looked at
    Check(m.LookedAgo(30, 30) == 0, "an eye at (30,30) looks at its cell now");
    m.Update(MakeScan(9000, {}, {}));
    Check(m.LookedAgo(30, 30) == 6000, "6 seconds later: 6000 ms ago");
    Check(m.LookedAgo(500, 500) == 0xFFFFFFFFu, "a cell no eye looked at: never");
    Check(m.LookedAgo(700, 0) == 0xFFFFFFFFu && m.LookedAgo(-1, 5) == 0xFFFFFFFFu, "beyond the map: never");
    // the cell whose middle is 9 tiles from the eye is not looked at: cell (7,5) has middle (45,33): 15 tiles from (30,30)
    Check(m.LookedAgo(45, 33) == 0xFFFFFFFFu, "a cell whose middle is 15 tiles from the eye is not looked at");
    // cell (6,5): middle (39,33): 9 from (30,30) is 81+9=90 > 64: not looked at; cell (5,6): (33,39): 9+81=90: no
    Check(m.LookedAgo(36, 30) == 0xFFFFFFFFu, "a cell whose middle is about 9.5 tiles away is not looked at");
    Check(m.CellsLooked(0, true) >= 4 && m.CellsLooked(0, true) <= 12, "a handful of cells looked at ever");
    Check(m.CellsLooked(1000, false) == 0, "none within the last second (the eye left)");
}

static void TestFullMemory()
{
    // 512 soldiers seen at 3000, 512 more at 6000 (the first 512 alive, at the floor: not seen again)
    Memory m;
    std::vector<Sighting> older, newer, all;
    for (unsigned long i = 1; i <= 512; i++)
        older.push_back(Unit(i, 2, kSoldier, 10, 10, 3));
    for (unsigned long i = 513; i <= kMaxRecords; i++)
        newer.push_back(Unit(i, 2, kSoldier, 10, 10, 3));
    Sighting theFloor = Unit(5000, 2, kOther, 400, 400, 1);
    std::vector<Sighting> first = older;
    first.push_back(theFloor);
    m.Update(MakeScan(3000, first));
    std::vector<Sighting> second = newer;
    for (Sighting s : older) { s.mCount = 1; second.push_back(s); }
    second.push_back(theFloor);
    m.Update(MakeScan(6000, second));
    Check((int)m.Records().size() == kMaxRecords, "1024 soldiers seen: 1024 records");
    // a 1025th at 9000: the oldest record (seen at 3000; the first of the 512 ties, id 1) goes
    std::vector<Sighting> third = second;
    for (Sighting& s : third)
        s.mCount = s.mKind == kSoldier ? 1 : s.mCount;
    third.push_back(Unit(1025, 2, kSoldier, 11, 11, 3));
    m.Update(MakeScan(9000, third));
    Check((int)m.Records().size() == kMaxRecords, "a full memory takes no more than 1024");
    Check(Find(m, 1025) != NULL, "the new sighting is in");
    Check(Find(m, 1) == NULL && Find(m, 2) != NULL && Find(m, 513) != NULL, "the oldest (id 1, seen at 3000) went; id 513 (seen at 6000) stays");
}

static void TestTime()
{
    Memory m;
    Check(m.Due(0), "a fresh memory is due");
    m.Update(MakeScan(3000, {Unit(10, 2, kSoldier, 50, 50, 3), Unit(11, 2, kSoldier, 51, 50, 1)}));
    Check(!m.Due(5999) && m.Due(6000), "the next scan is due after 3000 ms");
    Check(m.Due(100), "an earlier time starts again");
    m.Update(MakeScan(100, {}));
    Check(m.Records().empty() && m.Now() == 100, "an earlier time is a new game: the memory is empty");
}

int main()
{
    TestFloor();
    TestBuildings();
    TestSoldiers();
    TestLooked();
    TestFullMemory();
    TestTime();
    printf("test_ageofai_intel.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
