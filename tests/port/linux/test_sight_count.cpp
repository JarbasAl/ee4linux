// A tile's sight count (include/ee/RTSSight.h): kept whole, with the entry's low byte reading 0, 1 or 2..255 as the
// game's readers expect. The expected entries are worked out by hand from the layout: low byte 2..255, high byte the
// number of 254-unit blocks above it.
#include <cstdio>
#include <string>

#include "../../../include/ee/RTSSight.h"

static int sChecks, sFailures;

static void Check(bool inOk, const std::string& inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat.c_str());
    }
}

// One unit more or less, on an entry, as the tile does it
static unsigned long Add(unsigned long inEntry) { return RTSSightEntry(RTSSightAdded(RTSSightCount(inEntry))); }
static unsigned long Remove(unsigned long inEntry) { return RTSSightEntry(RTSSightRemoved(RTSSightCount(inEntry))); }

static void TestEntries()
{
    Check(RTSSightEntry(0) == 0 && RTSSightEntry(1) == 1 && RTSSightEntry(2) == 2, "0, 1 and 2 are stored as they are");
    Check(RTSSightEntry(255) == 255, "255 units: low byte 255 (a count below 256 is stored as the game stores it)");
    Check(RTSSightEntry(256) == 0x0102, "256 units: one block, low byte 2");
    Check(RTSSightEntry(257) == 0x0103, "257 units: low byte 3");
    Check(RTSSightEntry(509) == 0x01FF, "509 units: the last of the first block, low byte 255");
    Check(RTSSightEntry(510) == 0x0202, "510 units: two blocks, low byte 2");
    Check(RTSSightEntry(kRTSSightMax) == 0xFFFF, "the top, 65025, is the full entry");
    Check(RTSSightCount(0x0102) == 256 && RTSSightCount(0xFFFF) == 65025 && RTSSightCount(0x0001) == 1, "counts read back");
    for (unsigned long n = 0; n <= kRTSSightMax; n++) {
        unsigned long theEntry = RTSSightEntry(n);
        unsigned long theLow = theEntry & 0xff;
        bool theLowOK = n < 2 ? theLow == n : theLow >= 2;
        if (!theLowOK || RTSSightCount(theEntry) != n || theEntry > 0xFFFF) {
            Check(false, "count " + std::to_string(n) + " round trip or low byte");
            break;
        }
    }
    Check(true, "every count 0..65025 round trips and keeps its low byte as 0, 1 or 2..255");
}

// A tile seen by 300 units, then by none: the original's low byte reached 0 at the 255th add
static void TestCrowd()
{
    unsigned long theEntry = 1;                              // explored
    for (int k = 1; k <= 300; k++) {
        theEntry = Add(theEntry);
        if ((theEntry & 0xff) < 2) { Check(false, "low byte " + std::to_string(theEntry & 0xff) + " after " + std::to_string(k) + " units"); return; }
        if (RTSSightCount(theEntry) != (unsigned long)(k + 1)) { Check(false, "count after " + std::to_string(k) + " units"); return; }
    }
    Check(RTSSightCount(theEntry) == 301, "300 units on an explored tile: count 301 (2 + 299)");
    for (int k = 1; k <= 299; k++)
        theEntry = Remove(theEntry);
    Check(RTSSightCount(theEntry) == 2, "299 leave: one unit still sees it, count 2");
    theEntry = Remove(theEntry);
    Check(theEntry == 1, "the last leaves the tile explored (1)");
    theEntry = Remove(theEntry);
    Check(theEntry == 1, "a remove on an explored tile changes nothing");
    Check(Add(0) == 2, "an unexplored tile seen by one unit: 2");
    Check(Add(1) == 2, "an explored tile seen by one unit: 2");
    // the original on the same walk, as the reference of what was wrong: a byte restarting at 2
    unsigned long theOriginal = 2;
    for (int k = 1; k <= 254; k++)
        theOriginal = (theOriginal & 0xff) != 0 ? theOriginal + 1 : 2;
    Check((theOriginal & 0xff) == 0, "the original's entry after 254 more adds reads 0 (unexplored)");
    // the top holds
    unsigned long theTop = RTSSightEntry(kRTSSightMax);
    Check(Add(theTop) == theTop, "a full count stays full");
}

int main()
{
    TestEntries();
    TestCrowd();
    printf("test_sight_count.cpp: %d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
