// C:\EEX\Empire Earth\...\EEAIRectangleArea.h (RECONSTRUCTION; file path inferred). An AI map area
// (RTTI EEAIRectangleArea > EEAIArea @0; vtable 0x43a540, 12 slots). Only the base is declared: EECPILocation::Update 0x80a57 reads the
// EEAIArea serial of what EEAIContinentManager's lookup returns. Requires ee/EEAIArea.h, lle/U2DRectangle.h.
#ifndef EE_EEAIRECTANGLEAREA_H
#define EE_EEAIRECTANGLEAREA_H

class EEAIRectangleArea : public EEAIArea {
public:
    // (EEAIRectangleArea.cpp 0x2c79f..0x2cc87): ctor 0x2c79f (init list, then the vptr), ??_G 0x2cc65
    // (own vptr, then ~EEAIArea: a user-declared empty dtor)
    EEAIRectangleArea();
    virtual ~EEAIRectangleArea() {}
    // +0x20 0x2c939: EEAIArea's random point; when its tile is off the map, a random one of the tiles on the map
    virtual U2DPoint<long> GetRandomPoint();
    // +0x2c 0x2c929 (ret 0xc: two ignored dword arguments): EEAIArea::GetRandomPoint() (provisional name)
    virtual U2DPoint<long> GetBaseRandomPoint(long inArg1, long inArg2);
    // 0x2c7df: counts the rectangle's tiles per continent kind (+0x7c/+0x80/+0x84); mW88 = the highest continent id
    // seen; returns it (provisional name)
    unsigned short UpdateContinents();

    // EECPCombatManager 0x47306c walks mRect's tiles (x mLeft..mRight, y mTop..mBottom) when the count for
    // its argument (+0x7c / +0x80 / +0x84) is positive, unless mW88 is the continent asked for. (provisional names)
    U2DRectangle<long> mRect;               // +0x68
    long mScenarioAreaID;                   // +0x78 (not read there)
    long mNumLandTiles;                     // +0x7c
    long mNumWaterTiles;                    // +0x80
    long mNumSpaceTiles;                    // +0x84
    unsigned short mContinentID;            // +0x88 a continent id (EEAIContinent::mID)
    unsigned short mID;                     // +0x8a EEAIContinentManager::mRectangleAreas index (0x1f3a0)
};

#endif
