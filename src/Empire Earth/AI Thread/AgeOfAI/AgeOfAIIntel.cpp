// The memory of an Age of AI player (AgeOfAIIntel.h).
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/intel.c
// at 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include <algorithm>
#include <cstring>

#include "AgeOfAIIntel.h"

namespace AgeOfAI {
namespace Intel {

Memory::Memory() : mLooked(kCells * kCells, 0)
{
    memset(this->mFloor, 0, sizeof this->mFloor);
    this->mNow = this->mLast = 0;
    this->mSeenNow = this->mDropped = this->mMoved = 0;
}

bool Memory::IsSeen(const Sighting& inSighting) const
{
    if (inSighting.mRevealed)
        return true;
    return inSighting.mCount > this->Floor(inSighting.mOwner);
}

bool Memory::Remembers(unsigned long inUnit) const
{
    return std::binary_search(this->mKnown.begin(), this->mKnown.end(), inUnit);
}

unsigned Memory::LookedAgo(int inX, int inY) const
{
    int theX = inX / kCell, theY = inY / kCell;
    if (inX < 0 || inY < 0 || theX >= kCells || theY >= kCells)
        return 0xFFFFFFFFu;
    uint32_t theTime = this->mLooked[theX * kCells + theY];
    return theTime ? this->mNow - theTime : 0xFFFFFFFFu;
}

void Memory::Soldiers(int inX, int inY, int inRadius, float& ioHitPoints, float& ioAttack, float& ioPower, int& ioCount) const
{
    for (const Record& theRecord : this->mRecords) {
        if (theRecord.mKind != kSoldier || theRecord.mX < 0 || theRecord.mTime == this->mLast
            || this->mNow - theRecord.mTime > kSoldiersNearAfter)
            continue;
        long theDX = theRecord.mX - inX, theDY = theRecord.mY - inY;
        if (theDX * theDX + theDY * theDY > (long)inRadius * inRadius)
            continue;
        ioHitPoints += (float)theRecord.mHitPoints;
        ioAttack += theRecord.mAttack;
        ioPower += (float)theRecord.mHitPoints * theRecord.mAttack;
        ioCount++;
    }
}

void Memory::Army(float& ioHitPoints, float& ioAttack, float& ioPower, int& ioCount) const
{
    for (const Record& theRecord : this->mRecords) {
        if (theRecord.mKind != kSoldier || this->mNow - theRecord.mTime > kSoldierForgottenAfter)
            continue;
        ioHitPoints += (float)theRecord.mHitPoints;
        ioAttack += theRecord.mAttack;
        ioPower += (float)theRecord.mHitPoints * theRecord.mAttack;
        ioCount++;
    }
}

int Memory::CellsLooked(unsigned inWithin, bool inEver) const
{
    int theCount = 0;
    for (uint32_t theTime : this->mLooked)
        if (theTime && (inEver || this->mNow - theTime <= inWithin))
            theCount++;
    return theCount;
}

void Memory::Update(const Scan& inScan)
{
    const unsigned theNow = inScan.mNow;
    if (theNow < this->mNow)
        *this = Memory();                                  // a new game

    // the floor of each owner: the least count among its units
    bool theFloorSet[16] = {};
    memset(this->mFloor, 0, sizeof this->mFloor);
    for (const Sighting& theSighting : inScan.mEnemies) {
        if (theSighting.mOwner <= 0 || theSighting.mOwner >= 16)
            continue;
        if (!theFloorSet[theSighting.mOwner] || theSighting.mCount < this->mFloor[theSighting.mOwner]) {
            this->mFloor[theSighting.mOwner] = theSighting.mCount;
            theFloorSet[theSighting.mOwner] = true;
        }
    }
    this->mLast = this->mNow = theNow;

    // looked at now: the cells whose middle an eye sees
    for (const Eye& theEye : inScan.mEyes)
        for (int theX = (theEye.mX - kEyeRange) / kCell; theX <= (theEye.mX + kEyeRange) / kCell; theX++)
            for (int theY = (theEye.mY - kEyeRange) / kCell; theY <= (theEye.mY + kEyeRange) / kCell; theY++) {
                if (theX < 0 || theY < 0 || theX >= kCells || theY >= kCells)
                    continue;
                int theMidX = theX * kCell + kCell / 2, theMidY = theY * kCell + kCell / 2;
                long theDX = theMidX - theEye.mX, theDY = theMidY - theEye.mY;
                if (theDX * theDX + theDY * theDY <= (long)kEyeRange * kEyeRange)
                    this->mLooked[theX * kCells + theY] = theNow;
            }

    // what it sees now goes into the memory
    this->mSeenNow = 0;
    for (const Sighting& theSighting : inScan.mEnemies) {
        if (theSighting.mOwner <= 0 || theSighting.mOwner >= 16 || !this->IsSeen(theSighting))
            continue;
        this->mSeenNow++;
        size_t theIndex = 0;
        while (theIndex < this->mRecords.size() && this->mRecords[theIndex].mUnit != theSighting.mUnit)
            theIndex++;
        if (theIndex == this->mRecords.size()) {
            if ((int)this->mRecords.size() == kMaxRecords) {   // full: the oldest goes
                theIndex = 0;
                for (size_t i = 1; i < this->mRecords.size(); i++)
                    if (this->mRecords[i].mTime < this->mRecords[theIndex].mTime)
                        theIndex = i;
            } else
                this->mRecords.push_back(Record());
        }
        Record& theRecord = this->mRecords[theIndex];
        theRecord.mUnit = theSighting.mUnit;
        theRecord.mOwner = theSighting.mOwner;
        theRecord.mType = theSighting.mType;
        theRecord.mKind = theSighting.mKind;
        theRecord.mX = theSighting.mX;
        theRecord.mY = theSighting.mY;
        theRecord.mHitPoints = theSighting.mHitPoints;
        theRecord.mAttack = theSighting.mAttack;
        theRecord.mTime = theNow;
    }

    // what it looked at and did not find: buildings gone; soldiers moved on, or dead (seen dying when they went
    // out of the list within a scan of the last sighting); old sightings forgotten
    size_t theKept = 0;
    for (size_t i = 0; i < this->mRecords.size(); i++) {
        Record theRecord = this->mRecords[i];
        bool theKeep = true;
        if (theRecord.mTime != theNow) {
            bool theLooked = false;
            bool theLive = std::binary_search(inScan.mLive.begin(), inScan.mLive.end(), theRecord.mUnit);
            for (size_t q = 0; q < inScan.mEyes.size() && theRecord.mX >= 0 && !theLooked; q++) {
                long theDX = inScan.mEyes[q].mX - theRecord.mX, theDY = inScan.mEyes[q].mY - theRecord.mY;
                theLooked = theDX * theDX + theDY * theDY <= (long)kLookedRange * kLookedRange;
            }
            if (theRecord.mKind == kBuilding)
                theKeep = !(theLooked || (!theLive && theNow - theRecord.mTime <= kBuildingGoneAfter));
            else if (!theLive && theNow - theRecord.mTime <= kBuildingGoneAfter)
                theKeep = false;
            else {
                if (theLooked) {
                    theRecord.mX = -1;
                    this->mMoved++;
                }
                if (theNow - theRecord.mTime > kSoldierForgottenAfter)
                    theKeep = false;
            }
        }
        if (theKeep)
            this->mRecords[theKept++] = theRecord;
        else
            this->mDropped++;
    }
    this->mRecords.resize(theKept);
    this->mKnown.clear();
    for (const Record& theRecord : this->mRecords)
        if (theRecord.mKind == kBuilding)
            this->mKnown.push_back(theRecord.mUnit);
    std::sort(this->mKnown.begin(), this->mKnown.end());
}

}
}
