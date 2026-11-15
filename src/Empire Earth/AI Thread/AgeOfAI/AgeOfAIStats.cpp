// The fight estimate (AgeOfAIStats.h): the arithmetic, with no game types.
//
// Ported from Grille's Empire Earth Age of AI (https://github.com/Grille/Empire-Earth-Age-of-AI), dll/ai/stats.c at
// 39d8ee4256a4e6b50c1b4d8e3af0c3f00b30fc07, GPL-3.0.
#include "AgeOfAIStats.h"

#include <cmath>

namespace AgeOfAI {
namespace Stats {

void Clear(Side& outSide)
{
    outSide.mGroupCount = 0;
    outSide.mCount = 0;
    outSide.mHP = 0;
    outSide.mTiles.clear();
    outSide.mRadiusCount = 0;
}

bool Put(Side& ioSide, const Group& inKind, int inHP, int inX, int inY)
{
    int k = 0;
    while (k < ioSide.mGroupCount && (ioSide.mGroups[k].mType != inKind.mType || ioSide.mGroups[k].mOwner != inKind.mOwner))
        k++;
    if (k == ioSide.mGroupCount) {
        if (k == kGroups || ioSide.mCount == kUnits)
            return false;
        Group& theGroup = ioSide.mGroups[k];
        theGroup = inKind;
        theGroup.mCount = 0;
        theGroup.mHP = 0;
        theGroup.mPart = 0;
        for (int r = 0; r < kRadii; r++)
            theGroup.mNeighbours[r] = 0;
        ioSide.mGroupCount++;
    }
    if (ioSide.mCount == kUnits)
        return false;
    Group& theGroup = ioSide.mGroups[k];
    if (theGroup.mRep == NULL && inKind.mRep != NULL) {   // a live unit: its own reload
        theGroup.mRep = inKind.mRep;
        theGroup.mReload = inKind.mReload;
    }
    theGroup.mCount++;
    theGroup.mHP += (float)inHP;
    ioSide.mHP += (float)inHP;
    ioSide.mX[ioSide.mCount] = (short)inX;
    ioSide.mY[ioSide.mCount] = (short)inY;
    ioSide.mGroupOf[ioSide.mCount] = (unsigned char)k;
    ioSide.mCount++;
    ioSide.mRadiusCount = 0;   // the neighbour counts are asked again; the tile counts stay as first counted
    return true;
}

int ShotDamage(float inAttack, float inFactor, float inArmor)
{
    if (inAttack <= 0 || inFactor <= 0)
        return 0;
    float theDamage = inFactor * inAttack - inArmor;
    int theRounded = (int)(theDamage + 0.5f);
    return theRounded < 1 ? 1 : theRounded;
}

float Neighbours(Side& ioSide, int inGroup, float inRadius)
{
    int ri = 0;
    while (ri < ioSide.mRadiusCount && ioSide.mRadius[ri] != inRadius)
        ri++;
    if (ri < ioSide.mRadiusCount)
        return ioSide.mGroups[inGroup].mNeighbours[ri];
    if (ioSide.mTiles.empty()) {   // the tile counts, once
        int theX1 = -100000, theY1 = -100000;
        ioSide.mX0 = ioSide.mY0 = 100000;
        for (int i = 0; i < ioSide.mCount; i++) {
            if (ioSide.mX[i] < ioSide.mX0)
                ioSide.mX0 = ioSide.mX[i];
            if (ioSide.mY[i] < ioSide.mY0)
                ioSide.mY0 = ioSide.mY[i];
            if (ioSide.mX[i] > theX1)
                theX1 = ioSide.mX[i];
            if (ioSide.mY[i] > theY1)
                theY1 = ioSide.mY[i];
        }
        ioSide.mWidth = theX1 - ioSide.mX0 + 1;
        ioSide.mHeight = theY1 - ioSide.mY0 + 1;
        if (ioSide.mCount <= 0 || ioSide.mWidth <= 0 || ioSide.mHeight <= 0 || ioSide.mWidth > 1024 || ioSide.mHeight > 1024)
            return 0;
        ioSide.mTiles.assign((size_t)ioSide.mWidth * ioSide.mHeight, 0);
        for (int i = 0; i < ioSide.mCount; i++)
            ioSide.mTiles[(ioSide.mY[i] - ioSide.mY0) * ioSide.mWidth + ioSide.mX[i] - ioSide.mX0]++;
    }
    if (ioSide.mRadiusCount == kRadii)
        ioSide.mRadiusCount = 0;
    ri = ioSide.mRadiusCount++;
    ioSide.mRadius[ri] = inRadius;
    int theReach = (int)inRadius;
    int theSum[kGroups] = {0};
    for (int i = 0; i < ioSide.mCount; i++)
        for (int dy = -theReach; dy <= theReach; dy++)
            for (int dx = -theReach; dx <= theReach; dx++) {
                int x = ioSide.mX[i] + dx - ioSide.mX0, y = ioSide.mY[i] + dy - ioSide.mY0;
                if ((float)(dx * dx + dy * dy) <= inRadius * inRadius && x >= 0 && y >= 0 && x < ioSide.mWidth
                    && y < ioSide.mHeight)
                    theSum[ioSide.mGroupOf[i]] += ioSide.mTiles[y * ioSide.mWidth + x];
            }
    for (int g = 0; g < ioSide.mGroupCount; g++)
        ioSide.mGroups[g].mNeighbours[ri] = ioSide.mGroups[g].mCount ? (float)theSum[g] / ioSide.mGroups[g].mCount - 1.0f : 0;
    return ioSide.mGroups[inGroup].mNeighbours[ri];
}

namespace {

float Dps(const Group& inAttacker, const Group& inTarget, ShotFunction inShot)
{
    return inShot(inAttacker, inTarget) / (inAttacker.mReload + kShotOverhead);
}

// a unit that cannot move never hits one that outranges it
bool Reaches(const Group& inAttacker, const Group& inTarget)
{
    return inAttacker.mSpeed > 0 || inAttacker.mRange + 0.5f >= inTarget.mRange;
}

// once a mobile target has closed to its own range, an attacker whose minimum range is longer cannot hit it (artillery)
bool InsideMinimum(const Group& inAttacker, const Group& inTarget)
{
    return inTarget.mSpeed > 0 && inAttacker.mMinRange > 0 && inAttacker.mMinRange >= inTarget.mRange;
}

}

float SideDps(Side& ioA, Side& ioB, bool inContact, ShotFunction inShot)
{
    float theTotal = 0;
    for (int i = 0; i < ioA.mGroupCount; i++) {
        Group& theUnit = ioA.mGroups[i];
        float theNumerator = 0, theDenominator = 0;
        for (int j = 0; j < ioB.mGroupCount; j++) {
            Group& theTarget = ioB.mGroups[j];
            float d = Reaches(theUnit, theTarget) && !(inContact && InsideMinimum(theUnit, theTarget)) ? Dps(theUnit, theTarget, inShot) : 0;
            if (d <= 0)
                continue;
            if (theUnit.mArea > 0) {
                float theNear = Neighbours(ioB, j, theUnit.mArea);
                d *= 1.0f + (theNear < kAreaMax ? theNear : kAreaMax);
            }
            theNumerator += theTarget.mCount * d;
            theDenominator += theTarget.mCount;
        }
        theUnit.mPart = theDenominator > 0 ? theUnit.mCount * theNumerator / theDenominator : 0;
        theTotal += theUnit.mPart;
    }
    return theTotal;
}

float Close(const Side& inA, const Side& inB, ShotFunction inShot)
{
    float theLongest = 0, theTime = 0, theWeight = 0;
    for (int j = 0; j < inB.mGroupCount; j++)
        if (inB.mGroups[j].mRange > theLongest)
            theLongest = inB.mGroups[j].mRange;
    for (int i = 0; i < inA.mGroupCount; i++) {
        const Group& theUnit = inA.mGroups[i];
        float d = 0, theSpeed = theUnit.mSpeed, theGap = theLongest - theUnit.mRange;
        for (int j = 0; j < inB.mGroupCount && d <= 0; j++)
            d = Dps(theUnit, inB.mGroups[j], inShot);
        if (d <= 0 || theSpeed <= 0)
            continue;
        theTime += theUnit.mCount * d * (theGap > 0 ? theGap / theSpeed : 0);
        theWeight += theUnit.mCount * d;
    }
    return theWeight > 0 ? theTime / theWeight : 0;
}

float KillTime(float inHP, float inOwnClose, float inTheirClose, float inFreeDps, float inContactDps)
{
    if (inFreeDps > 0 && inHP <= inFreeDps * inTheirClose)
        return inOwnClose + inHP / inFreeDps;
    inHP -= inFreeDps * inTheirClose;
    return inContactDps > 0 ? inOwnClose + inTheirClose + inHP / inContactDps : 1e9f;
}

void Estimate(Side& ioOwn, Side& ioFoe, ShotFunction inShot, Result& outResult)
{
    outResult.mContactDps[0] = SideDps(ioOwn, ioFoe, true, inShot);
    outResult.mContactDps[1] = SideDps(ioFoe, ioOwn, true, inShot);
    outResult.mFreeDps[0] = SideDps(ioOwn, ioFoe, false, inShot);
    outResult.mFreeDps[1] = SideDps(ioFoe, ioOwn, false, inShot);
    outResult.mClose[0] = Close(ioOwn, ioFoe, inShot);
    outResult.mClose[1] = Close(ioFoe, ioOwn, inShot);
    outResult.mTime[0] = KillTime(ioFoe.mHP, outResult.mClose[0], outResult.mClose[1], outResult.mFreeDps[0], outResult.mContactDps[0]);
    outResult.mTime[1] = KillTime(ioOwn.mHP, outResult.mClose[1], outResult.mClose[0], outResult.mFreeDps[1], outResult.mContactDps[1]);
}

float Ratio(Side& ioOwn, Side& ioFoe, ShotFunction inShot)
{
    if (!ioFoe.mCount || ioFoe.mHP <= 0)
        return 1e6f;
    if (!ioOwn.mCount || ioOwn.mHP <= 0)
        return 0.0f;
    Result theResult;
    Estimate(ioOwn, ioFoe, inShot, theResult);
    if (theResult.mTime[0] >= 1e9f)
        return theResult.mTime[1] >= 1e9f ? 1.0f : 0.0f;
    return theResult.mTime[1] >= 1e9f ? 1e6f : sqrtf(theResult.mTime[1] / (theResult.mTime[0] > 0.1f ? theResult.mTime[0] : 0.1f));
}

}
}
