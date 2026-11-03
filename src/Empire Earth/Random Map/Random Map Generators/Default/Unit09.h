// Unit 9 (0xbc8a..0xc737): point placement and relaxation inside a ring. Names PROVISIONAL.
// Requires <vector>, lle/LowLevelEngine.h, ee/EERandomMap.h.
#ifndef UNIT09_H
#define UNIT09_H

float RelaxPoints(std::vector<GE2DPoint>& ioPoints, const EERMRing& inRing, float inStep, float inMinimumDistance,
                  unsigned long inMaximumIterations);
float RelaxPoints(std::vector<GE2DPoint>& ioPoints, std::vector<GE2DPoint>& inFixed, const EERMRing& inRing,
                  float inStep, float inMinimumDistance, unsigned long inMaximumIterations);
float FindClosestPoint(std::vector<GE2DPoint>& inPoints, const GE2DPoint& inPoint,
                       std::vector<GE2DPoint>::iterator& outClosest);
float SpreadPoints(std::vector<GE2DPoint>& ioPoints, const EERMRing& inRing, float inStep, float inPasses,
                   float inTolerance, unsigned long inMaximumRounds);
float SpreadPoints(std::vector<GE2DPoint>& ioPoints, std::vector<GE2DPoint>& inFixed, const EERMRing& inRing,
                   float inStep, float inPasses, float inTolerance, unsigned long inMaximumRounds);
float FindClosestPair(std::vector<GE2DPoint>& inPoints, std::vector<GE2DPoint>::iterator& outA,
                      std::vector<GE2DPoint>::iterator& outB);
float FindClosestPair(std::vector<GE2DPoint>& inPoints, std::vector<GE2DPoint>& inFixed,
                      std::vector<GE2DPoint>::iterator& outA, std::vector<GE2DPoint>::iterator& outB);
void PlacePointOnRing(URandom& inRandom, const EERMRing& inRing, GE2DPoint& outPoint, float& ioAngle);
void PlacePointsOnRing(URandom& inRandom, const EERMRing& inRing, std::vector<GE2DPoint>& ioPoints);
void SeparateClosestPair(std::vector<GE2DPoint>& ioPoints, const EERMRing& inRing, float inStep);
void SeparateClosestPair(std::vector<GE2DPoint>& ioPoints, std::vector<GE2DPoint>& inFixed, const EERMRing& inRing,
                         float inStep);

#endif
