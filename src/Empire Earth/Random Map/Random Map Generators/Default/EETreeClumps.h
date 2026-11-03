// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EETreeClumps.h (name PROVISIONAL)
// RTTI .?AVEETreeClumps@@ (vtable 0x19258). Requires EEClumps.h.
#ifndef EETREECLUMPS_H
#define EETREECLUMPS_H

class EETreeClumps : public EEClumps {          // proven sizeof >= 0xd5 (+0xd4 byte)
public:
    EETreeClumps(EERandomMap* inMap, EECellGrid* inClaimed, float inSpread, unsigned long inClumpCount,
                 unsigned long inTargetSize, std::vector<unsigned long>* inDistances, long inType, bool inFlag);
    virtual ~EETreeClumps();                                // slot 0 (0xa6c5; deleting 0xa6a9)
    virtual bool IsValidLocation(long inX, long inY);       // slot 2 (0xa6e2)
    virtual void Claim(long inX, long inY, bool inMark);    // slot 8 (0xa798)

    bool IsFarFromCenters(const GE2DPoint& inPoint);        // 0xa73e

    std::vector<GE2DPoint> mCentersToAvoid;     // +0xa0 (the map's unflagged players' positions)
    std::vector<std::vector<unsigned long> >* mTiles;   // +0xb0 (map +0xc0)
    EECellGrid* mFlatTiles;                     // +0xb4 (map +0xa0)
    std::vector<EERMObject>* mObjects;          // +0xb8 (map +0x120)
    std::vector<std::vector<float> >* mHeights; // +0xbc (map +0xb0)
    float mLandElevationThreshold;              // +0xc0 (map +0x2a4)
    float mMinimumDistanceSquared;              // +0xc4 (map +0x288 squared)
    unsigned long mResourceToWaterDistance;     // +0xc8 (map +0x3d4)
    unsigned long mResourceToEdgeDistance;      // +0xcc (map +0x3d0)
    long mTreeType;                             // +0xd0 (-1: per cell, from the tile value)
    bool mFlag;                                 // +0xd4
};

#endif
