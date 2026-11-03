// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\Unit09.cpp (file name inferred: point-set
// placement and relaxation inside a ring; called by the CMU unit's position generators)
// Unit flags (calibrated, docs/CALIBRATION.md): -O1 -G6 -MD -GR
#include <windows.h>
#include <float.h>
#include <vector>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "ee/EERandomMap.h"
#include "Unit09.h"

// Compiler-generated for this unit (names in analysis/default/names.txt):
// SYNTHETIC: DEFAULT 0x1000bc8a
// SYNTHETIC: DEFAULT 0x1000bc94
// SYNTHETIC: DEFAULT 0x1000bca5
// SYNTHETIC: DEFAULT 0x1000bcb1
// SYNTHETIC: DEFAULT 0x1000bcbc
// SYNTHETIC: DEFAULT 0x1000bcc6
// SYNTHETIC: DEFAULT 0x1000bcd7
// SYNTHETIC: DEFAULT 0x1000bce3
// SYNTHETIC: DEFAULT 0x1000bcee
// SYNTHETIC: DEFAULT 0x1000bcf8
// SYNTHETIC: DEFAULT 0x1000bd09
// SYNTHETIC: DEFAULT 0x1000bd15
// SYNTHETIC: DEFAULT 0x1000bd20
// SYNTHETIC: DEFAULT 0x1000bd2a
// SYNTHETIC: DEFAULT 0x1000bd3b
// SYNTHETIC: DEFAULT 0x1000bd47
// SYNTHETIC: DEFAULT 0x1000bd52
// SYNTHETIC: DEFAULT 0x1000bd5c
// SYNTHETIC: DEFAULT 0x1000bd6d
// SYNTHETIC: DEFAULT 0x1000bd79
// SYNTHETIC: DEFAULT 0x1000bd84
// SYNTHETIC: DEFAULT 0x1000bd8e
// SYNTHETIC: DEFAULT 0x1000bd9f
// SYNTHETIC: DEFAULT 0x1000bdab
// SYNTHETIC: DEFAULT 0x1000bdb6
// SYNTHETIC: DEFAULT 0x1000bdc0
// SYNTHETIC: DEFAULT 0x1000bdd1
// SYNTHETIC: DEFAULT 0x1000bddd
// SYNTHETIC: DEFAULT 0x1000c721
// SYNTHETIC: DEFAULT 0x1000c72b

// FUNCTION: DEFAULT 0x1000bde8
// Pushes the closest pair of points apart (inside the ring) until their squared distance exceeds a
// minimum or the iteration limit; returns the final closest squared distance
float RelaxPoints(std::vector<GE2DPoint>& ioPoints, const EERMRing& inRing, float inStep, float inMinimumDistance,
                  unsigned long inMaximumIterations)
{
    std::vector<GE2DPoint>::iterator theA;
    std::vector<GE2DPoint>::iterator theB;
    float theDistance = 0.0f;
    for (unsigned long i = 0; theDistance <= inMinimumDistance && i < inMaximumIterations; i++) {
        SeparateClosestPair(ioPoints, inRing, inStep);
        theDistance = FindClosestPair(ioPoints, theA, theB);
    }
    return theDistance;
}

// FUNCTION: DEFAULT 0x1000be31
// Pushes the closest pair apart as above, also keeping clear of fixed points that never move
float RelaxPoints(std::vector<GE2DPoint>& ioPoints, std::vector<GE2DPoint>& inFixed, const EERMRing& inRing,
                  float inStep, float inMinimumDistance, unsigned long inMaximumIterations)
{
    std::vector<GE2DPoint>::iterator theA;
    std::vector<GE2DPoint>::iterator theB;
    float theDistance = 0.0f;
    for (unsigned long i = 0; theDistance <= inMinimumDistance && i < inMaximumIterations; i++) {
        SeparateClosestPair(ioPoints, inFixed, inRing, inStep);
        theDistance = FindClosestPair(ioPoints, inFixed, theA, theB);
    }
    return theDistance;
}

// FUNCTION: DEFAULT 0x1000be80
// Finds the point nearest to a position; returns its squared distance and sets the iterator to it
float FindClosestPoint(std::vector<GE2DPoint>& inPoints, const GE2DPoint& inPoint,
                       std::vector<GE2DPoint>::iterator& outClosest)
{
    float theBest = FLT_MAX;
    for (std::vector<GE2DPoint>::iterator i = inPoints.begin(); i != inPoints.end(); i++) {
        float theDistance = inPoint.DistanceToSquared(*i);
        if (theDistance < theBest) {
            theBest = theDistance;
            outClosest = i;
        }
    }
    return theBest;
}

// FUNCTION: DEFAULT 0x1000bec2
// Repeats rounds of pair separation until the average closest squared distance settles within a
// tolerance; returns that average
float SpreadPoints(std::vector<GE2DPoint>& ioPoints, const EERMRing& inRing, float inStep, float inPasses,
                   float inTolerance, unsigned long inMaximumRounds)
{
    std::vector<GE2DPoint>::iterator theA;
    std::vector<GE2DPoint>::iterator theB;
    float theAverage = 0.0f;
    for (unsigned long j = 0; j < inMaximumRounds; j++) {
        float thePrevious = theAverage;
        float theTotal = 0.0f;
        for (unsigned long i = 0; i < inPasses; i++) {
            SeparateClosestPair(ioPoints, inRing, inStep);
            theTotal += FindClosestPair(ioPoints, theA, theB);
        }
        theAverage = theTotal / inPasses;
        float theChange = theAverage - thePrevious;
        if ((theChange < 0.0f ? -theChange : theChange) < inTolerance)
            break;
    }
    return theAverage;
}

// FUNCTION: DEFAULT 0x1000bf49
// Spreads points as above while also keeping clear of fixed points
float SpreadPoints(std::vector<GE2DPoint>& ioPoints, std::vector<GE2DPoint>& inFixed, const EERMRing& inRing,
                   float inStep, float inPasses, float inTolerance, unsigned long inMaximumRounds)
{
    std::vector<GE2DPoint>::iterator theA;
    std::vector<GE2DPoint>::iterator theB;
    float theAverage = 0.0f;
    for (unsigned long j = 0; j < inMaximumRounds; j++) {
        float thePrevious = theAverage;
        float theTotal = 0.0f;
        for (unsigned long i = 0; i < inPasses; i++) {
            SeparateClosestPair(ioPoints, inFixed, inRing, inStep);
            theTotal += FindClosestPair(ioPoints, inFixed, theA, theB);
        }
        theAverage = theTotal / inPasses;
        float theChange = theAverage - thePrevious;
        if ((theChange < 0.0f ? -theChange : theChange) < inTolerance)
            break;
    }
    return theAverage;
}

// FUNCTION: DEFAULT 0x1000bfd6
// Finds the two closest points; returns their squared distance
float FindClosestPair(std::vector<GE2DPoint>& inPoints, std::vector<GE2DPoint>::iterator& outA,
                      std::vector<GE2DPoint>::iterator& outB)
{
    float theBest = FLT_MAX;
    for (std::vector<GE2DPoint>::iterator i = inPoints.begin(); i != inPoints.end(); i++)
        for (std::vector<GE2DPoint>::iterator j = i + 1; j != inPoints.end(); j++) {
            float theDistance = i->DistanceToSquared(*j);
            if (!(theDistance < theBest))
                continue;
            theBest = theDistance;
            outA = i;
            outB = j;
        }
    return theBest;
}

// FUNCTION: DEFAULT 0x1000c03e
// Finds the closest pair among the points and between a point and a fixed point; returns the distance
float FindClosestPair(std::vector<GE2DPoint>& inPoints, std::vector<GE2DPoint>& inFixed,
                      std::vector<GE2DPoint>::iterator& outA, std::vector<GE2DPoint>::iterator& outB)
{
    // i is initialised before theBest, so theBest (first assigned after inPoints is read) takes inPoints' slot.
    std::vector<GE2DPoint>::iterator i = inPoints.begin();
    float theBest = FLT_MAX;
    for (; i != inPoints.end(); i++) {
        std::vector<GE2DPoint>::iterator j;
        for (j = i + 1; j != inPoints.end(); j++) {
            float theDistance = i->DistanceToSquared(*j);
            if (!(theDistance < theBest))
                continue;
            theBest = theDistance;
            outA = i;
            outB = j;
        }
        for (j = inFixed.begin(); j != inFixed.end(); j++) {
            float theDistance = i->DistanceToSquared(*j);
            if (!(theDistance < theBest))
                continue;
            theBest = theDistance;
            outA = i;
            outB = j;
        }
    }
    return theBest;
}

const float kRingAngleStep = 0.0175f;     // radians per placed point (loaded before ioAngle)

// FUNCTION: DEFAULT 0x1000c0db
// Places a point at a random radius within the ring at the given angle, then advances the angle
void PlacePointOnRing(URandom& inRandom, const EERMRing& inRing, GE2DPoint& outPoint, float& ioAngle)
{
    GEVector theRadius;
    GEVector theOffset;
    theRadius.mY = inRandom.Generate() * (inRing.mMaxRadius - inRing.mMinRadius) + inRing.mMinRadius;
    GETransformation theRotation;
    theRotation.SetOrientationYPR(ioAngle, 0.0f, 0.0f);
    theRotation.Apply(theRadius, theOffset);
    outPoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
    outPoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    ioAngle += kRingAngleStep;
}

// FUNCTION: DEFAULT 0x1000c192
// Scatters every point at a random angle and radius within the ring
void PlacePointsOnRing(URandom& inRandom, const EERMRing& inRing, std::vector<GE2DPoint>& ioPoints)
{
    GEVector theRadius;
    GEVector theOffset;
    GETransformation theRotation;
    for (std::vector<GE2DPoint>::iterator i = ioPoints.begin(); i != ioPoints.end(); i++) {
        float theDistance = inRandom.GenerateUnit() * (inRing.mMaxRadius - inRing.mMinRadius) + inRing.mMinRadius;
        theRadius.mY = theDistance;
        theRotation.SetOrientationYPR(inRandom.GenerateUnit() * 6.2831855f, 0.0f, 0.0f);
        theRotation.Apply(theRadius, theOffset);
        i->mX = inRing.mCenter.GetX() + theOffset.GetX();
        i->mY = inRing.mCenter.GetY() + theOffset.GetY();
    }
}

// FUNCTION: DEFAULT 0x1000c29f
// Moves the closest two points a step apart, clamping each back between the ring's radii
void SeparateClosestPair(std::vector<GE2DPoint>& ioPoints, const EERMRing& inRing, float inStep)
{
    GE2DPoint thePoint;
    GEVector theDirection;
    GEVector theOffset;
    unsigned long theMinimum = UMFloatToLong(UMSquare(inRing.mMinRadius));
    unsigned long theMaximum = UMFloatToLong(UMSquare(inRing.mMaxRadius));
    std::vector<GE2DPoint>::iterator theA;
    std::vector<GE2DPoint>::iterator theB;
    FindClosestPair(ioPoints, theA, theB);
    theDirection.mX = theA->GetX() - theB->GetX();
    theDirection.mY = theA->GetY() - theB->GetY();
    theDirection.Normalize();

    thePoint.mX = theA->GetX() + theDirection.GetX() * inStep;
    thePoint.mY = theA->GetY() + theDirection.GetY() * inStep;
    if (thePoint.DistanceToSquared(inRing.mCenter) > theMaximum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMaxRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    } else if (thePoint.DistanceToSquared(inRing.mCenter) < theMinimum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMinRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    }
    *theA = thePoint;

    thePoint.mX = theB->mX - theDirection.mX * inStep;
    thePoint.mY = theB->mY - theDirection.mY * inStep;
    if (thePoint.DistanceToSquared(inRing.mCenter) > theMaximum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMaxRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    } else if (thePoint.DistanceToSquared(inRing.mCenter) < theMinimum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMinRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    }
    *theB = thePoint;
}

// FUNCTION: DEFAULT 0x1000c4c8
// Moves the closest pair apart as above, but never moves a point that belongs to the fixed set
void SeparateClosestPair(std::vector<GE2DPoint>& ioPoints, std::vector<GE2DPoint>& inFixed, const EERMRing& inRing,
                         float inStep)
{
    GE2DPoint thePoint;
    GEVector theDirection;
    GEVector theOffset;
    unsigned long theMinimum = UMFloatToLong(UMSquare(inRing.mMinRadius));
    unsigned long theMaximum = UMFloatToLong(UMSquare(inRing.mMaxRadius));
    bool theFixed = false;
    std::vector<GE2DPoint>::iterator theA;
    std::vector<GE2DPoint>::iterator theB;
    FindClosestPair(ioPoints, inFixed, theA, theB);
    theDirection.mX = theA->GetX() - theB->GetX();
    theDirection.mY = theA->GetY() - theB->GetY();
    theDirection.Normalize();

    thePoint.mX = theA->GetX() + theDirection.GetX() * inStep;
    thePoint.mY = theA->GetY() + theDirection.GetY() * inStep;
    if (thePoint.DistanceToSquared(inRing.mCenter) > theMaximum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMaxRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    } else if (thePoint.DistanceToSquared(inRing.mCenter) < theMinimum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMinRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    }
    *theA = thePoint;

    for (std::vector<GE2DPoint>::iterator i = inFixed.begin(); !theFixed && i != inFixed.end(); i++)
        theFixed = (i == theB);
    if (theFixed)
        return;
    thePoint.mX = theB->mX - theDirection.mX * inStep;
    thePoint.mY = theB->mY - theDirection.mY * inStep;
    if (thePoint.DistanceToSquared(inRing.mCenter) > theMaximum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMaxRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    } else if (thePoint.DistanceToSquared(inRing.mCenter) < theMinimum) {
        theOffset.mX = thePoint.GetX() - inRing.mCenter.GetX();
        theOffset.mY = thePoint.GetY() - inRing.mCenter.GetY();
        theOffset.Normalize();
        theOffset.Scale(inRing.mMinRadius, theOffset);
        thePoint.mX = inRing.mCenter.GetX() + theOffset.GetX();
        thePoint.mY = inRing.mCenter.GetY() + theOffset.GetY();
    }
    *theB = thePoint;
}
