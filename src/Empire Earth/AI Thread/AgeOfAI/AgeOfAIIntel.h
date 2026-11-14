// What an Age of AI player knows of its enemies (honest vision): the units it has seen, where and when, and which
// places it has looked at. The model here holds no game types, so a test feeds it scans by hand; AgeOfAIIntelScan.cpp
// reads the game into a Scan.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/intel.c
// at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Units are known by their unit id, not by their address, so the
// memory does not depend on where the heap puts them.
//
// The game gives every unit a count per player: how many of that player's units (and its allies') see it now. The
// classic AI reads it as it stands, and each unit of the human carries a floor of that count in the AIs' slots, so
// the AIs see the human's units everywhere. With [ai_base] honest_vision an AI sees a unit only when its count is
// above the least count among that owner's units (the floor), or when the unit is hitting one of the AI's units
// (reveal_attackers). It remembers what it saw:
//  - a building stays known until its place is looked at and it is gone;
//  - a soldier counts for 10 minutes at the place it was seen, and moves to "somewhere" when that place is looked at
//    and it is not there;
//  - a place is looked at when a unit of the AI or an ally is within 8 tiles; the map is cut into 6x6 cells.
#ifndef AGEOFAI_INTEL_H
#define AGEOFAI_INTEL_H

#include <cstdint>
#include <vector>

namespace AgeOfAI {
namespace Intel {

enum Kind { kBuilding, kSoldier, kOther };

const int kMaxRecords = 1024;
const int kCell = 6;                          // tiles
const int kCells = 600 / kCell + 1;           // the map up to 600 tiles
const unsigned kScanEvery = 3000;             // game ms between two scans
const unsigned kBuildingGoneAfter = 6000;     // a unit that left the list this long after its last sighting died in sight
const unsigned kSoldierForgottenAfter = 600000;
const unsigned kSoldiersNearAfter = 300000;   // intel_soldiers: seen within 5 minutes
const int kEyeRange = 8;                      // tiles
const int kLookedRange = 6;                   // a record is looked at when an eye is this near

// An enemy unit in a scan. mCount is the viewer's visibility count of the unit.
struct Sighting {
    unsigned long mUnit;
    int mOwner;
    int mType;
    Kind mKind;
    int mX, mY;
    int mHitPoints;
    float mAttack;                            // a building's attack when it shoots at land, a unit's hit points x attack / hit points
    unsigned mCount;
    bool mRevealed;                           // it is hitting one of the viewer's units
};

struct Eye {
    int mX, mY;
};

// The world as the viewer's scan sees it
struct Scan {
    unsigned mNow;                            // game ms
    std::vector<Sighting> mEnemies;           // every live unit of the viewer's enemies (not gaia)
    std::vector<unsigned long> mLive;         // the id of every live unit, sorted
    std::vector<Eye> mEyes;                   // the units of the viewer and its allies
};

struct Record {
    unsigned long mUnit;
    int mOwner, mType;
    Kind mKind;
    int mX, mY;                               // mX < 0: somewhere (a soldier looked for where it was and not there)
    int mHitPoints;
    float mAttack;
    unsigned mTime;                           // game ms of the last sighting
};

class Memory {
public:
    Memory();

    // Takes a scan: the time must have moved on by kScanEvery since the last one (Due). Returns false when the scan
    // was not taken. A scan with an earlier time than the last one starts a new game.
    bool Due(unsigned inNow) const { return this->mNow == 0 || inNow < this->mNow || inNow - this->mNow >= kScanEvery; }
    void Update(const Scan& inScan);

    // The unit's count, above the owner's floor or hitting, so seen now
    bool IsSeen(const Sighting& inSighting) const;
    // A building the viewer has seen and not seen gone
    bool Remembers(unsigned long inUnit) const;
    // A unit that hit one of the viewer's units at the last scan
    bool IsRevealed(unsigned long inUnit) const;
    // ms since a unit of the viewer or an ally was near (x, y); ~0 for never
    unsigned LookedAgo(int inX, int inY) const;
    // Soldiers remembered within inRadius tiles of (x, y) and not seen now, seen within 5 minutes: hit points, attack
    // and hit points x attack summed, and how many
    void Soldiers(int inX, int inY, int inRadius, float& ioHitPoints, float& ioAttack, float& ioPower, int& ioCount) const;
    // The army it knows of: every soldier seen within 10 minutes, seen now included
    void Army(float& ioHitPoints, float& ioAttack, float& ioPower, int& ioCount) const;

    const std::vector<Record>& Records() const { return this->mRecords; }
    unsigned Floor(int inOwner) const { return inOwner >= 0 && inOwner < 16 ? this->mFloor[inOwner] : 0; }
    unsigned Now() const { return this->mNow; }
    unsigned SeenNow() const { return this->mSeenNow; }
    unsigned Dropped() const { return this->mDropped; }
    unsigned Moved() const { return this->mMoved; }
    // Cells looked at, ever and within inWithin ms
    int CellsLooked(unsigned inWithin, bool inEver) const;

private:
    std::vector<Record> mRecords;
    std::vector<unsigned long> mKnown;        // the buildings of mRecords, sorted
    std::vector<unsigned long> mRevealed;     // the units that hit the viewer's at the last scan, sorted
    std::vector<uint32_t> mLooked;            // kCells x kCells: game ms of the last look; 0 never
    unsigned mFloor[16];
    unsigned mNow, mLast;
    unsigned mSeenNow, mDropped, mMoved;
};

}
}

#endif
