// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EELandClumps.h (name PROVISIONAL)
// RTTI .?AVEELandClumps@@ (vtable 0x1921c); base of EEIslandClumps. Requires EEClumps.h.
#ifndef EELANDCLUMPS_H
#define EELANDCLUMPS_H

class EELandClumps : public EEClumps {          // proven sizeof 0xb0 (Default.cpp 0x78c5: operator new(0xb0))
public:
    EELandClumps(EERandomMap* inMap, EECellGrid* inClaimed, std::vector<unsigned long>* inDistances,
                 unsigned long inClumpCount, float inSpread, unsigned long inTargetSize, long inPlayer,
                 bool inFlag);
    virtual ~EELandClumps();                                // slot 0 (0x98d4; deleting 0x98b8)
    virtual bool IsValidLocation(long inX, long inY);       // slot 2 (0x9af6)
    virtual bool AddLocation(const GE2DPoint& inLocation);  // slot 4 (0x98df)
    virtual bool IsClaimed(long inX, long inY);             // slot 5 (0x9c28)
    virtual bool IsFree(long inX, long inY);                // slot 6 (0x9c69)
    virtual void Claim(long inX, long inY, bool inMark);    // slot 8 (0x9caa)
    virtual void AddClump();                                // slot 9 (0x9e27)
    virtual bool IsNearOtherClumps(const GE2DPoint& inPoint, float inDistance);   // slot 10 (0x9a4a)

    // const inline accessor (name PROVISIONAL): IsClaimed 0x9c28 / IsFree 0x9c69 index through const operator[]
    // (the column address is materialised and a dead lea of the cell follows the test)
    const std::vector<std::vector<unsigned long> >& GetTiles() const { return *this->mTiles; }
    std::vector<std::vector<unsigned long> >* mTiles;   // +0xa0 (map +0xc0)
    unsigned long mTileIndex;                   // +0xa4 (player +0x2c)
    long mTeam;                                 // +0xa8 (player +0x30)
    bool mFlag;                                 // +0xac
};

#endif
