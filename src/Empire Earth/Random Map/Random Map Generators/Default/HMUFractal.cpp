// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\HMUFractal.cpp (file name inferred; the unit
// holds HMUFractal (RTTI) and the height-map entry points CMUCreateHeightMap / CMUFilterHeightMap)
// Unit flags (calibrated, docs/CALIBRATION.md): -O1 -G6 -MD -GR
#include <windows.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "lle/U2DFractal.h"
#include "ee/EERandomMap.h"
#include "HMUFractal.h"
#include "Unit11.h"

static bool IsTileClear(const std::vector<std::vector<unsigned long> >& inTiles, unsigned long inX,
                        unsigned long inY);

// Compiler-generated for this unit (names in analysis/default/names.txt):
// SYNTHETIC: DEFAULT 0x1000afcd
// SYNTHETIC: DEFAULT 0x1000abc5
// SYNTHETIC: DEFAULT 0x1000abcf
// SYNTHETIC: DEFAULT 0x1000abe0
// SYNTHETIC: DEFAULT 0x1000abec
// SYNTHETIC: DEFAULT 0x1000abf7
// SYNTHETIC: DEFAULT 0x1000ac01
// SYNTHETIC: DEFAULT 0x1000ac12
// SYNTHETIC: DEFAULT 0x1000ac1e
// SYNTHETIC: DEFAULT 0x1000ac29
// SYNTHETIC: DEFAULT 0x1000ac33
// SYNTHETIC: DEFAULT 0x1000ac44
// SYNTHETIC: DEFAULT 0x1000ac50
// SYNTHETIC: DEFAULT 0x1000ac5b
// SYNTHETIC: DEFAULT 0x1000ac65
// SYNTHETIC: DEFAULT 0x1000ac76
// SYNTHETIC: DEFAULT 0x1000ac82
// SYNTHETIC: DEFAULT 0x1000ac8d
// SYNTHETIC: DEFAULT 0x1000ac97
// SYNTHETIC: DEFAULT 0x1000aca8
// SYNTHETIC: DEFAULT 0x1000acb4
// SYNTHETIC: DEFAULT 0x1000acbf
// SYNTHETIC: DEFAULT 0x1000acc9
// SYNTHETIC: DEFAULT 0x1000acda
// SYNTHETIC: DEFAULT 0x1000ace6
// SYNTHETIC: DEFAULT 0x1000acf1
// SYNTHETIC: DEFAULT 0x1000acfb
// SYNTHETIC: DEFAULT 0x1000ad0c
// SYNTHETIC: DEFAULT 0x1000ad18
// SYNTHETIC: DEFAULT 0x1000bc74
// SYNTHETIC: DEFAULT 0x1000bc7e

// FUNCTION: DEFAULT 0x1000ad23
// Computes and stores one tile's height: flat tiles get the flat elevation, land rises randomly above
// the land threshold (capped at 1), water falls below it; empty space-map tiles sit just under it
float HMUFractal::GenerateFractalValue(URandom& inRandom, float inRange, float inScale, float inAverage,
                                       unsigned long inX, unsigned long inY)
{
    float theLow = this->mMap->mLandElevationThreshold - this->mMap->mElevationUnit * 0.5f;
    float theHigh = this->mMap->mLandElevationThreshold - this->mMap->mElevationUnit;
    if (this->mMap->mIsSpaceMap && IsTileClear(this->mMap->mTiles, inX, inY))
        return this->mMap->mHeights[inX][inY] = theLow;
    bool theLand = !IsTileClear(this->mMap->mTiles, inX, inY);
    unsigned char theFlag = this->mMap->mFlatTiles[inX][inY];
    bool theShore = Unit11_1581c(this->mMap->mTiles, inX, inY);
    float theValue;
    if (theFlag)
        theValue = this->mMap->mFlatElevation;
    else if (theShore)
        theValue = theLow;
    else if (theLand) {
        if (inAverage < this->mMap->mLandElevationThreshold)
            inAverage = this->mMap->mLandElevationThreshold - inAverage + this->mMap->mLandElevationThreshold;
        theValue = inAverage - (inRange - inRandom.Generate() * (inRange + inRange)) * this->mMap->mElevationScale * inScale;
        theValue = min(max(theValue, this->mMap->mLandElevationThreshold), 1.0f);
    } else {
        if (this->mMap->mCoastalToDeeperWaterPercentage > 0.0f && this->mMap->mRandom.Generate() <= this->mMap->mCoastalToDeeperWaterPercentage)
            theHigh -= this->mMap->mElevationUnit;
        if (inAverage > theHigh)
            inAverage = theHigh;
        theValue = inAverage + (inRange - inRandom.Generate() * (inRange + inRange)) * this->mMap->mElevationScale * inScale;
        theValue = min(max(theValue, 0.0f), theHigh);
    }
    return this->mMap->mHeights[inX][inY] = theValue;
}

// FUNCTION: DEFAULT 0x1000af40
// Returns whether a tile has no land owner (low 15 bits zero)
static bool IsTileClear(const std::vector<std::vector<unsigned long> >& inTiles, unsigned long inX,
                        unsigned long inY)
{
    return (inTiles[inX][inY] & 0x7fff) == 0;
}

// FUNCTION: DEFAULT 0x1000af67
// Builds the height map by running the fractal from the script's initial elevation, chaos and seed
long CMUCreateHeightMap(EERandomMap* ioMap)
{
    HMUFractal theFractal(ioMap);
    unsigned long theStart = TMGetTime();
    theFractal.ComputeFractal(ioMap->mInitialElevation, ioMap->mHeightMapChaos, ioMap->mSeed);
    unsigned long theEnd = TMGetTime();
    return 0;
}


// FUNCTION: DEFAULT 0x1000aff0
// Sweeps the height map diagonally in both directions, limiting the slope between neighbours to one
// elevation step and flattening tiles that touch both land and water to sea level
long CMUFilterHeightMap(EERandomMap* ioMap)
{
    float theRange = (float)ioMap->mMaxIntElevation - ioMap->mMinIntElevation;
    float theRiseStep;
    float theFallStep;
    float theSeaLevel;
    long theSize = ioMap->mGridSize;
    std::vector<std::vector<float> >& theHeights = ioMap->mHeights;
    long i;
    long j;
    long x;
    long theHigher;
    long theLower;
    bool theA;
    bool theB;
    bool theC;
    float theValue;
    float theNeighbor;
    unsigned long theStart;
    theRiseStep = theFallStep = 1.0f / theRange;
    theSeaLevel = -(ioMap->mMinIntElevation / theRange);
    theStart = TMGetTime();
    ESDebugOut("CMUFilterHeightMap start of function\n");

    for (i = theSize; i > 0; i--)
        for (j = 0; (x = i - 1 + j) < theSize; j++) {
            theValue = theHeights[j][x];
            theHigher = 0;
            theLower = 0;
            theC = false;
            theA = false;
            theB = false;
            if (j > 0) {
                theNeighbor = theHeights[j - 1][x];
                if (theNeighbor > theSeaLevel) {
                    theA = true;
                    theHigher = 1;
                } else if (theNeighbor < theSeaLevel)
                    theLower = 1;
                if (theValue - theNeighbor > theRiseStep)
                    theValue = theNeighbor + theRiseStep;
                if (x < theSize - 1) {
                    theNeighbor = theHeights[j - 1][x + 1];
                    if (theNeighbor > theSeaLevel) {
                        theHigher++;
                        theB = true;
                    } else if (theNeighbor < theSeaLevel)
                        theLower++;
                    if (theValue - theNeighbor > theRiseStep)
                        theValue = theNeighbor + theRiseStep;
                }
            }
            if (x < theSize - 1) {
                theNeighbor = theHeights[j][x + 1];
                if (theNeighbor > theSeaLevel) {
                    theHigher++;
                    theC = true;
                } else if (theNeighbor < theSeaLevel)
                    theLower++;
                if (theValue - theNeighbor > theRiseStep)
                    theValue = theNeighbor + theRiseStep;
            }
            if (theHigher > 0) {
                if (theLower > 0) {
                    theValue = theSeaLevel;
                    if (theC)
                        theHeights[j][x + 1] = theSeaLevel;
                    if (theB)
                        theHeights[j - 1][x + 1] = theSeaLevel;
                    if (theA)
                        theHeights[j - 1][x] = theSeaLevel;
                } else if (theValue < theSeaLevel)
                    theValue = theSeaLevel;
            } else if (theLower > 0 && theValue > theSeaLevel)
                theValue = theSeaLevel;
            theHeights[j][x] = theValue;

            theValue = theHeights[x][j];
            theHigher = 0;
            theLower = 0;
            theC = false;
            theB = false;
            theA = false;
            if (x < theSize - 1) {
                theNeighbor = theHeights[x + 1][j];
                if (theNeighbor > theSeaLevel) {
                    theB = true;
                    theHigher = 1;
                } else if (theNeighbor < theSeaLevel)
                    theLower = 1;
                if (theValue - theNeighbor > theFallStep)
                    theValue = theNeighbor + theFallStep;
                if (j > 0) {
                    theNeighbor = theHeights[x + 1][j - 1];
                    if (theNeighbor > theSeaLevel) {
                        theHigher++;
                        theA = true;
                    } else if (theNeighbor < theSeaLevel)
                        theLower++;
                    if (theValue - theNeighbor > theFallStep)
                        theValue = theNeighbor + theFallStep;
                }
            }
            if (j > 0) {
                theNeighbor = theHeights[x][j - 1];
                if (theNeighbor > theSeaLevel) {
                    theHigher++;
                    theC = true;
                } else if (theNeighbor < theSeaLevel)
                    theLower++;
                if (theValue - theNeighbor > theFallStep)
                    theValue = theNeighbor + theFallStep;
            }
            if (theHigher > 0) {
                if (theLower > 0) {
                    theValue = theSeaLevel;
                    if (theC)
                        theHeights[x][j - 1] = theSeaLevel;
                    if (theA)
                        theHeights[x + 1][j - 1] = theSeaLevel;
                    if (theB)
                        theHeights[x + 1][j] = theSeaLevel;
                } else if (theValue < theSeaLevel)
                    theValue = theSeaLevel;
            } else if (theLower > 0 && theValue > theSeaLevel)
                theValue = theSeaLevel;
            theHeights[x][j] = theValue;
        }

    for (i = 0; i < theSize; i++)
        for (j = 0; (x = i + j) < theSize; j++) {
            theValue = theHeights[x][j];
            theHigher = 0;
            theLower = 0;
            theC = false;
            theA = false;
            theB = false;
            if (x > 0) {
                theNeighbor = theHeights[x - 1][j];
                if (theNeighbor > theSeaLevel) {
                    theA = true;
                    theHigher = 1;
                } else if (theNeighbor < theSeaLevel)
                    theLower = 1;
                if (theValue - theNeighbor > theRiseStep)
                    theValue = theNeighbor + theRiseStep;
                if (j < theSize - 1) {
                    theNeighbor = theHeights[x - 1][j + 1];
                    if (theNeighbor > theSeaLevel) {
                        theHigher++;
                        theB = true;
                    } else if (theNeighbor < theSeaLevel)
                        theLower++;
                    if (theValue - theNeighbor > theRiseStep)
                        theValue = theNeighbor + theRiseStep;
                }
            }
            if (j < theSize - 1) {
                theNeighbor = theHeights[x][j + 1];
                if (theNeighbor > theSeaLevel) {
                    theHigher++;
                    theC = true;
                } else if (theNeighbor < theSeaLevel)
                    theLower++;
                if (theValue - theNeighbor > theRiseStep)
                    theValue = theNeighbor + theRiseStep;
            }
            if (theHigher > 0) {
                if (theLower > 0) {
                    theValue = theSeaLevel;
                    if (theC)
                        theHeights[x][j + 1] = theSeaLevel;
                    if (theB)
                        theHeights[x - 1][j + 1] = theSeaLevel;
                    if (theA)
                        theHeights[x - 1][j] = theSeaLevel;
                } else if (theValue < theSeaLevel)
                    theValue = theSeaLevel;
            } else if (theLower > 0 && theValue > theSeaLevel)
                theValue = theSeaLevel;
            theHeights[x][j] = theValue;

            theValue = theHeights[j][x];
            theHigher = 0;
            theLower = 0;
            theC = false;
            theB = false;
            theA = false;
            if (j < theSize - 1) {
                theNeighbor = theHeights[j + 1][x];
                if (theNeighbor > theSeaLevel) {
                    theB = true;
                    theHigher = 1;
                } else if (theNeighbor < theSeaLevel)
                    theLower = 1;
                if (theValue - theNeighbor > theFallStep)
                    theValue = theNeighbor + theFallStep;
                if (x > 0) {
                    theNeighbor = theHeights[j + 1][x - 1];
                    if (theNeighbor > theSeaLevel) {
                        theHigher++;
                        theA = true;
                    } else if (theNeighbor < theSeaLevel)
                        theLower++;
                    if (theValue - theNeighbor > theFallStep)
                        theValue = theNeighbor + theFallStep;
                }
            }
            if (x > 0) {
                theNeighbor = theHeights[j][x - 1];
                if (theNeighbor > theSeaLevel) {
                    theHigher++;
                    theC = true;
                } else if (theNeighbor < theSeaLevel)
                    theLower++;
                if (theValue - theNeighbor > theFallStep)
                    theValue = theNeighbor + theFallStep;
            }
            if (theHigher > 0) {
                if (theLower > 0) {
                    theValue = theSeaLevel;
                    if (theC)
                        theHeights[j][x - 1] = theSeaLevel;
                    if (theA)
                        theHeights[j + 1][x - 1] = theSeaLevel;
                    if (theB)
                        theHeights[j + 1][x] = theSeaLevel;
                } else if (theValue < theSeaLevel)
                    theValue = theSeaLevel;
            } else if (theLower > 0 && theValue > theSeaLevel)
                theValue = theSeaLevel;
            theHeights[j][x] = theValue;
        }

    unsigned long theEnd = TMGetTime();
    ESDebugOut("CMUFilterHeightMap: time:     %6.2f s\n", (theEnd - theStart) * 0.001f);
    return 0;
}
