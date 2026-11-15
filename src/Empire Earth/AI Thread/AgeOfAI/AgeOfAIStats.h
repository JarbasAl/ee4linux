// The fight estimate of an Age of AI player: how long each of two sides needs to destroy the other, from the game's own
// unit data, so the AI's judgements follow the game and the units modders add. A side holds its units by type and owner
// (a group), because the units of one type and player share their stats: the work grows with types x types, not units x
// units. Range counts (a unit that cannot move never hits one that outranges it; an outranged unit walks the difference
// under fire first), and so do minimum range and area damage.
//
// The arithmetic here has no game types. The game's part (AgeOfAIStatsUnits.cpp) fills each group's range, speed, minimum
// range, reload and area from the unit's type and its owner's stats, and gives the damage of one shot between two groups.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/stats.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0. Not ported: the control pipe's `stats` command (cmd_stats).
#ifndef AGEOFAI_STATS_H
#define AGEOFAI_STATS_H

#include <vector>

class EEPlayer;
class EEUnit;
class EEUnitClass;
class EEUnitClassUpgrade;

namespace AgeOfAI {
namespace Stats {

const int kGroups = 64;
const int kUnits = 4096;
const int kRadii = 4;
// In a fight a shot takes longer than the reload: picking the next target, turning, stepping into range. In duels
// machine gunners (reload 1 s) fired every 3.3-5.1 s and Mk V tanks (4 s) every 4.6-7.1 s: about 2 s on every shot.
const float kShotOverhead = 2.0f;
// Area damage: at most this many units beside the target count. Shells are slow, targets move and a crowd spreads.
const float kAreaMax = 2.0f;

// The units of one type and owner. mRep: a live unit of it (NULL for remembered units only); mClass and mStats: the type
// and its owner's stats of it (mStats NULL when unknown). mRange, mSpeed (tiles per second, 0: cannot move), mMinRange,
// mReload (seconds between shots) and mArea come from them. mPart: the group's share of the last SideDps.
struct Group {
    EEUnit* mRep;
    EEUnitClass* mClass;
    EEUnitClassUpgrade* mStats;
    long mType;
    long mOwner;
    int mCount;
    float mHP;
    float mRange;
    float mSpeed;
    float mMinRange;
    float mReload;
    float mArea;
    float mNeighbours[kRadii];
    float mPart;
};

struct Side {
    int mGroupCount;
    int mCount;
    float mHP;
    Group mGroups[kGroups];
    short mX[kUnits];
    short mY[kUnits];
    unsigned char mGroupOf[kUnits];
    int mX0, mY0, mWidth, mHeight;          // the tile counts' bounding box
    std::vector<unsigned short> mTiles;     // units per tile, counted once
    int mRadiusCount;
    float mRadius[kRadii];                  // radii whose neighbour counts are known
};

struct Result {
    float mFreeDps[2];                      // [0]: own at foe, [1]: foe at own; before contact
    float mContactDps[2];                   // after contact (nothing inside the minimum range counts)
    float mClose[2];                        // seconds each side walks under fire before it shoots
    float mTime[2];                         // [0]: how long own needs to destroy foe, [1]: foe to destroy own; 1e9: never
};

// The damage of one shot of one group at another (0: cannot hurt it)
typedef int (*ShotFunction)(const Group& inAttacker, const Group& inTarget);

void Clear(Side& outSide);

// One unit of inKind's type and owner with inHP at inX, inY. inKind carries the group's data; its mRep is the unit when
// it is live and seen (else NULL), and its mReload that unit's own when it has one. Returns false when the side is full.
bool Put(Side& ioSide, const Group& inKind, int inHP, int inX, int inY);

// The damage of one shot: inFactor (the game's hit factor times its weapon table) x inAttack less the target's armour,
// rounded, at least 1; 0 when the attack or the factor is not above 0.
int ShotDamage(float inAttack, float inFactor, float inArmor);

// The average number of the side's units within inRadius of a unit of group inGroup (itself not counted)
float Neighbours(Side& ioSide, int inGroup, float inRadius);

// Side inA's damage per second at side inB: each unit spreads its fire over the units of inB it can hurt, by their count.
// inContact: inB has closed in. Area damage: a target counts with its neighbours.
float SideDps(Side& ioA, Side& ioB, bool inContact, ShotFunction inShot);

// Seconds side inA walks under fire before it shoots: each mobile group's range short of inB's longest, over its speed,
// weighted by what it deals
float Close(const Side& inA, const Side& inB, ShotFunction inShot);

// Time to destroy inHP: first the own closing (no fire), then free fire while the enemy closes, then contact fire
float KillTime(float inHP, float inOwnClose, float inTheirClose, float inFreeDps, float inContactDps);

void Estimate(Side& ioOwn, Side& ioFoe, ShotFunction inShot, Result& outResult);

// How much stronger own is than foe, on the scale of summed hit points x attack: the square root of the time foe needs to
// destroy own over the time own needs to destroy foe. Twice the units of one type make the time ratio 4 (the square law)
// and the root 2, so thresholds tuned on hit points x attack keep their meaning. Nothing to fight: 1e6; nothing to fight
// with: 0.
float Ratio(Side& ioOwn, Side& ioFoe, ShotFunction inShot);

// The game's part (AgeOfAIStatsUnits.cpp)
EEUnitClassUpgrade* Record(EEPlayer* inPlayer, long inType);          // the player's stats of a type (NULL: none)
int Shot(const Group& inAttacker, const Group& inTarget);              // the game's damage of one shot
int UnitShot(EEUnit* inAttacker, EEUnit* inTarget);
float Range(EEUnit* inUnit);
float Speed(EEUnit* inUnit);                                           // tiles per second, 0: cannot move
bool Add(Side& ioSide, EEUnit* inUnit);
bool AddRecord(Side& ioSide, long inType, long inOwner, int inHP, int inX, int inY);   // a remembered unit
void Estimate(Side& ioOwn, Side& ioFoe, Result& outResult);
float Ratio(Side& ioOwn, Side& ioFoe);

}
}

#endif
