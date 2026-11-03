// C:\EEX\Empire Earth\Graphics Thread\EEE3DStorm.h (RECONSTRUCTION; file name inferred).
// The abstract weather effect over a list of tiles, the base of EEE3DRain and EEE3DSnow (vtable 0x440e44; RTTI
// EEE3DStorm > EEEffect3D; EEE3DStorm.cpp 0xfb088..0xfb9f6: a map's init chain, the ctors .. Save, the map's tree
// COMDATs; no key chain). Names are provisional. Requires ee/EEEffect3D.h, lle/U2DRectangle.h,
// ee/EETileCoordinate.h, lle/U2DPoint.h, <vector>.
#ifndef EE_EEE3DSTORM_H
#define EE_EEE3DSTORM_H

#include <vector>

class GEModel;
template <class T> class U2DRectangle;

// Free helpers in another unit (0x241c2d.. range; provisional names): the 8-way direction from (inX1, inY1) towards
// (inX2, inY2) (UMArcTan2) and the tile inDistance steps from inTile in that direction.
long EEGetTileDirection(long inX1, long inY1, long inX2, long inY2);  // 0x2451df
void EEGetNextTile(const EETileCoordinate& inTile, EETileCoordinate& outTile, long inDirection, long inDistance);  // 0x248ecc

class EEE3DStorm : public EEEffect3D {
public:
    // 0xfb0c8 (no unit: the effect covers inTiles)
    EEE3DStorm(long inKind, const std::vector<EETileCoordinate>& inTiles, EEGraphicEffect* inGraphicEffect);
    EEE3DStorm(long inKind);                // 0xfb191
    virtual ~EEE3DStorm();                  // +0x00 0xfb217 (??_G 0xfb175)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xfb756
    virtual long RepairPointers();          // +0x3c (EEEffect::RepairPointers: ICF-folded into EEE3DBlastWave's 0xe93e0)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xfb83f
    // +0x58 0xfb252: move the tiles towards a new centre (each one tile step towards its new place when that tile
    // exists), then the new mean centre, the transformation there, the bounds and the radius (provisional name)
    // outBounds: the storm's mArea (EEEMStorm +0x28 passed straight at 0x12ebfe)
    virtual void Move(const U2DPoint<long>& inCenter, U2DRectangle<long>& outBounds);
    virtual U2DPoint<long>& GetCenter();    // +0x5c 0xfb500: mCenter = the tiles' mean; the transformation there
    virtual void GetBounds(U2DRectangle<long>& outBounds);   // +0x60 0xfb6b4: the tiles' bounding box (EEEMStorm mArea, 0x12e364)
    virtual void Virtual64();               // +0x64 0xfb72e: the graphic effect's parameters

    long mL74;                              // +0x74
    std::vector<EETileCoordinate> mTiles;   // +0x78 (saved)
    U2DPoint<long> mCenter;                 // +0x88 (saved)
    GEModel* mModel;                        // +0x90 (dereferenced by the dtor)
    float mFallSpeed;                       // +0x94 (the effect's mF28)
    float mSpawnHeight;                     // +0x98 (the effect's mF2C)
    float mDensity;                         // +0x9c (the effect's mF30)
    float mIntensity;                       // +0xa0 (1; saved)
    unsigned long mDuration;                // +0xa4 (5000; the effect's duration)
    float mElapsedTime;                     // +0xa8 (saved)
    bool mIsRestored;                       // +0xac
    bool mIsSustained;                      // +0xad (saved)
};

#endif
