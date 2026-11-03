// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EELandClumps.cpp (file name inferred; RTTI
// .?AVEELandClumps@@ proves the class)
// Unit flags (hypothesis, docs/CALIBRATION.md): -O1 -G6 -MD -GR
#include <windows.h>
#include <math.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "ee/EERandomMap.h"
#include "EEClumps.h"
#include "EELandClumps.h"
#include "CMU Map Generator.h"
#include "Unit11.h"
#include "ee/EERandomMapIDs.h"


// Compiler-generated for this unit (names in analysis/default/names.txt):
// SYNTHETIC: DEFAULT 0x100098b8
// SYNTHETIC: DEFAULT 0x10009fba
// SYNTHETIC: DEFAULT 0x1000a2f8
// SYNTHETIC: DEFAULT 0x1000a3ad
// TEMPLATE: DEFAULT 0x10009e80
// TEMPLATE: DEFAULT 0x10009eac
// TEMPLATE: DEFAULT 0x10009f60
// TEMPLATE: DEFAULT 0x1000a279
// TEMPLATE: DEFAULT 0x10009e61
// TEMPLATE: DEFAULT 0x1000a2c3
// TEMPLATE: DEFAULT 0x1000a329
// TEMPLATE: DEFAULT 0x1000a351
// TEMPLATE: DEFAULT 0x1000a382
// SYNTHETIC: DEFAULT 0x100096d5
// SYNTHETIC: DEFAULT 0x100096df
// SYNTHETIC: DEFAULT 0x100096f0
// SYNTHETIC: DEFAULT 0x100096fc
// SYNTHETIC: DEFAULT 0x10009707
// SYNTHETIC: DEFAULT 0x10009711
// SYNTHETIC: DEFAULT 0x10009722
// SYNTHETIC: DEFAULT 0x1000972e
// SYNTHETIC: DEFAULT 0x10009739
// SYNTHETIC: DEFAULT 0x10009743
// SYNTHETIC: DEFAULT 0x10009754
// SYNTHETIC: DEFAULT 0x10009760
// SYNTHETIC: DEFAULT 0x1000976b
// SYNTHETIC: DEFAULT 0x10009775
// SYNTHETIC: DEFAULT 0x10009786
// SYNTHETIC: DEFAULT 0x10009792
// SYNTHETIC: DEFAULT 0x1000979d
// SYNTHETIC: DEFAULT 0x100097a7
// SYNTHETIC: DEFAULT 0x100097b8
// SYNTHETIC: DEFAULT 0x100097c4
// SYNTHETIC: DEFAULT 0x100097cf
// SYNTHETIC: DEFAULT 0x100097d9
// SYNTHETIC: DEFAULT 0x100097ea
// SYNTHETIC: DEFAULT 0x100097f6
// SYNTHETIC: DEFAULT 0x10009801
// SYNTHETIC: DEFAULT 0x1000980b
// SYNTHETIC: DEFAULT 0x1000981c
// SYNTHETIC: DEFAULT 0x10009828
// SYNTHETIC: DEFAULT 0x1000a3ca
// SYNTHETIC: DEFAULT 0x1000a3d4

// FUNCTION: DEFAULT 0x10009833
// Creates a land grower for one player that writes its tiles into the map and tracks clump circles
EELandClumps::EELandClumps(EERandomMap* inMap, EECellGrid* inClaimed, std::vector<unsigned long>* inDistances,
                           unsigned long inClumpCount, float inSpread, unsigned long inTargetSize, long inPlayer,
                           bool inFlag)
    : EEClumps(inMap, inClaimed, inSpread, inClumpCount, inTargetSize, inPlayer, inDistances),
      mTiles(&inMap->mTiles)
{
    this->mTileIndex = inMap->mPlayers[inPlayer].mIndex;
    this->mFlag = inFlag;
    inMap->mClumpCircles[inPlayer].resize(inClumpCount);
    this->mTeam = inMap->mPlayers[inPlayer].mTeam;
}

// FUNCTION: DEFAULT 0x100098d4
// Destroys the land grower (members only)
EELandClumps::~EELandClumps()
{
}

// FUNCTION: DEFAULT 0x100098df
// Starts a clump (a player's first one also claims the flat area around it) and resets the clump's
// circle to the location, even when the start failed; returns whether it started
bool EELandClumps::AddLocation(const GE2DPoint& inLocation)
{
    EERandomMap* theMap = this->mMap;
    bool theResult = EEClumps::AddLocation(inLocation);
    if (theResult && this->mIndex == 0 && !this->mFlag) {
        long theLeft = UMFloor(theMap->mFlatArea.GetLeft() + inLocation.GetX());
        long theBottom = UMFloor(theMap->mFlatArea.GetBottom() + inLocation.GetY());
        long theRight = UMFloor(theMap->mFlatArea.GetRight() + inLocation.GetX()) + 1;
        long theTop = UMFloor(theMap->mFlatArea.GetTop() + inLocation.GetY()) + 1;
        for (long x = theLeft; x <= theRight; x++)
            for (long y = theBottom; y <= theTop; y++)
                if (CMUIsCellSet(theMap->mValidTiles, x, y) && this->IsFree(x, y))
                    this->Claim(x, y, true);
    }
    theMap->mClumpCircles[this->mType][this->mIndex].mCenter = inLocation;
    theMap->mClumpCircles[this->mType][this->mIndex].mRadius = 0.0f;
    return theResult;
}

// FUNCTION: DEFAULT 0x10009a4a
// Returns whether a point lies within a distance of any clump circle of another player
bool EELandClumps::IsNearOtherClumps(const GE2DPoint& inPoint, float inDistance)
{
    for (unsigned long i = 0; i < this->mMap->mPlayers.size(); i++)
        if (i != this->mType)
            for (unsigned long j = 0; j < this->mMap->mClumpCircles[i].size(); j++) {
                float theRange = inDistance + this->mMap->mClumpCircles[i][j].mRadius;
                if (inPoint.DistanceToSquared(this->mMap->mClumpCircles[i][j].mCenter) <= theRange * theRange)
                    return true;
            }
    return false;
}

// FUNCTION: DEFAULT 0x10009af6
// Returns whether a free tile may become land: moons stay within 0.8 moon size of the clump centre,
// other land keeps a (randomised) water border from the map edge
bool EELandClumps::IsValidLocation(long inX, long inY)
{
    EERandomMap* theMap = this->mMap;
    if (!EEClumps::IsValidLocation(inX, inY))
        return false;
    if (theMap->mPlayers[this->mType].mIsMoon) {
        GE2DPoint thePoint((float)inX, (float)inY);
        if (this->mCenters[this->mIndex].DistanceTo(thePoint) > (float)theMap->mMoonSize * 0.8f)
            return false;
    } else if (theMap->mWaterBorder > 0) {
        if (theMap->mIsSpaceMap && (theMap->mMapCharacteristics & kRMMapEarthOrMarsMap)) {
            GE2DPoint thePoint((float)inX, (float)inY);
            if (theMap->mRing.mCenter.DistanceTo(thePoint) > (theMap->mHalfMapSize - theMap->mWaterBorder) * 0.75f)
                return false;
        } else {
            long theRadius = UMFloatToLong((theMap->mRandom.Generate() * 0.25f + 0.75f) * theMap->mWaterBorder);
            if (IsSquareOpen(*this->mAllowed, inX, inY, theRadius))
                return false;
        }
    }
    return true;
}

// FUNCTION: DEFAULT 0x10009c28
// Returns whether a tile in the allowed area already has a land owner
bool EELandClumps::IsClaimed(long inX, long inY)
{
    return this->IsInMap(inX, inY) && (this->GetTiles()[inX][inY] & 0x7fff) != 0;
}

// FUNCTION: DEFAULT 0x10009c69
// Returns whether a tile in the allowed area has no land owner yet
bool EELandClumps::IsFree(long inX, long inY)
{
    return this->IsInMap(inX, inY) && (this->GetTiles()[inX][inY] & 0x7fff) == 0;
}

// FUNCTION: DEFAULT 0x10009caa
// Claims a tile as this player's land (neutral land gets owner + 0x400, moons also 0x8000) and grows
// the clump's circle to cover it
void EELandClumps::Claim(long inX, long inY, bool inMark)
{
    GE2DPoint thePoint((float)inX, (float)inY);
    EEClumps::Claim(inX, inY, false);
    if (this->mFlag) {
        (*this->mTiles)[inX][inY] = this->mTileIndex + 0x400;
        if (this->mMap->mPlayers[this->mType].mIsMoon)
            (*this->mTiles)[inX][inY] |= 0x8000;
    } else
        (*this->mTiles)[inX][inY] = this->mTileIndex + 1;
    this->mMap->mClumpCircles[this->mType][this->mIndex].Extend(thePoint);
}

// FUNCTION: DEFAULT 0x10009df0
// Grows the circle's radius to reach a point
void EERMCircle::Extend(const GE2DPoint& inPoint)
{
    float theDistance = inPoint.DistanceToSquared(this->mCenter);
    if (theDistance > this->mRadius * this->mRadius)
        this->mRadius = (float)sqrt(theDistance);
}

// FUNCTION: DEFAULT 0x10009e27
// Splits the current clump like the base and adds an empty clump circle
void EELandClumps::AddClump()
{
    EERMCircle theCircle;
    EEClumps::AddClump();
    this->mMap->mClumpCircles[this->mType].push_back(theCircle);
}
