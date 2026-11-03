// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EEClumps.h (name PROVISIONAL)
// Clump growers: grow regions of map cells from start points. RTTI proves the class names and the hierarchy
// (vtables 0x19190 EEClumps, 0x191c0 EEFlatClumps, 0x191ec EEIslandClumps, 0x1921c EELandClumps,
// 0x19258 EETreeClumps). Member and method names PROVISIONAL; offsets proven by EEClumps.cpp codegen.
// Requires <vector>, lle/LowLevelEngine.h, lle/U2DPoint.h, ee/EERandomMap.h.
#ifndef EECLUMPS_H
#define EECLUMPS_H

typedef std::vector<std::vector<unsigned char> > EECellGrid;

class EEClumps {                                // proven sizeof 0xa0 (derived classes start at +0xa0)
public:
    EEClumps(EERandomMap* inMap, EECellGrid* inClaimed, float inSpread, unsigned long inClumpCount,
             unsigned long inTargetSize, long inType, std::vector<unsigned long>* inDistances);
    virtual ~EEClumps();                                    // slot 0 (0x7c61; deleting 0x7c45)
    virtual void Begin(const GE2DPoint& inStart);           // slot 1 (0x83b6)
    virtual bool IsValidLocation(long inX, long inY);       // slot 2 (0x849f)
    virtual void ClearFrontier();                           // slot 3 (0x7fe9)
    virtual bool AddLocation(const GE2DPoint& inLocation);  // slot 4 (0x7ff1)
    virtual bool IsClaimed(long inX, long inY);             // slot 5 (0x84af)
    virtual bool IsFree(long inX, long inY);                // slot 6 (0x84e7)
    virtual bool PickDirection();                           // slot 7 (0x8586)
    virtual void Claim(long inX, long inY, bool inMark);    // slot 8 (0x8ae0)
    virtual void AddClump();                                // slot 9 (0x8c19)

    long Grow();                                            // 0x7c94: 0 grew, 1 frontier cell spent, 2 none
    void Claim(const GE2DPoint& inLocation, bool inMark);   // 0x8074
    void RemoveFrontier(long inX, long inY);                // 0x80c1
    void ResetTargets();                                    // 0x8103
    unsigned long GetTotalSize() const;                        // 0x8129
    void Generate();                                        // 0x8145
    bool Step();                                            // 0x8239
    void Reset();                                           // 0x83e8
    bool HasFreeNeighbor(long inX, long inY);               // 0x844c
    bool IsInMap(long inX, long inY);                       // 0x851f
    bool IsFrontierLocation(long inX, long inY);            // 0x8554
    bool PickStart();                                       // 0x889b
    void UpdateFrontier(long inX, long inY);                // 0x8b60
    // inline accessors (names PROVISIONAL): EEFlatClumps::IsValidLocation 0x91f9 and EETreeClumps::IsValidLocation
    // 0xa6e2 load the member into a register ahead of the other call arguments, and EETreeClumps::Claim 0xa798 needs
    // the budget of three GetMap() calls (each exact only through them)
    EECellGrid* GetAllowed() { return this->mAllowed; }
    EERandomMap* GetMap() { return this->mMap; }

    long mType;                                 // +0x04
    GEVector mDirection;                        // +0x08
    std::vector<GE2DPoint> mCenters;            // +0x14
    std::vector<GE2DPoint> mPlaced;             // +0x24
    std::vector<unsigned long> mTargets;        // +0x34
    std::vector<unsigned long> mSizes;          // +0x44
    std::vector<U2DPoint<long> > mFrontier;     // +0x54
    EECellGrid* mClaimed;                       // +0x64
    EECellGrid* mAllowed;                       // +0x68 (map +0xf0)
    std::vector<unsigned long>* mDistances;     // +0x6c
    URandom* mRandom;                           // +0x70 (the map)
    URandomIntegers* mRandomIntegers;           // +0x74 (map +0x10)
    EERandomMap* mMap;                          // +0x78
    float mSpread;                              // +0x7c
    long mMaximumY;                             // +0x80 (map +0x390)
    long mMaximumX;                             // +0x84 (map +0x398)
    unsigned long mMapSize;                     // +0x88 (map +0x394)
    unsigned long mIndex;                       // +0x8c
    unsigned long mTargetSize;                  // +0x90
    unsigned long mClumpCount;                  // +0x94
    unsigned long mGrowCount;                   // +0x98
    bool mStarted;                              // +0x9c
};

#endif
