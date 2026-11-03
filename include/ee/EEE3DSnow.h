// C:\EEX\Empire Earth\Graphics Thread\EEE3DSnow.h (RECONSTRUCTION; file name inferred).
// The snow over a storm's tiles: drifting, spinning flakes (a two-triangle model "EEE3DSnowFlakeModel",
// textures\snowflake) spawned over random tiles (vtable 0x440dc8; RTTI EEE3DSnow > EEE3DStorm; EEE3DSnow.cpp
// 0xf9b6d..0xfb088: the ctors .. Virtual6C, EEE3DSnowFlake's ctor and Update, then the vectors' template COMDATs).
// sizeof 0xd8. Names are provisional. Requires ee/EEE3DStorm.h and its requirements.
#ifndef EE_EEE3DSNOW_H
#define EE_EEE3DSNOW_H

#include <vector>

// a flake (0x24 bytes; polymorphic: vtable 0x440e3c holds only Update)
class EEE3DSnowFlake {
public:
    // 0xf9dcf (defined in the .cpp after its first user Virtual68): a random drift
    EEE3DSnowFlake(const GE3DPoint& inPosition, float inGroundZ, float inAngle);
    virtual long Update();                  // +0x00 0xfa99a: sometimes a new drift, then drifts

    GEVector mDrift;                        // +0x04 (x, y in -1..1; GEVector: the out-of-line copy ctor copies it field
                                            // by field and mPosition with movsd)
    GE3DPoint mPosition;                    // +0x10
    float mGroundZ;                         // +0x1c (the flake melts below it)
    float mAngle;                           // +0x20
};

// a flake to draw this frame (0x10 bytes; Virtual24 copies the flakes)
struct EE3DSnowDraw {
    EE3DSnowDraw(const GE3DPoint& inPosition, float inAngle) : mPosition(inPosition), mAngle(inAngle) {}
    GE3DPoint mPosition;                    // +0x00
    float mAngle;                           // +0x0c
};

class EEE3DSnow : public EEE3DStorm {
public:
    EEE3DSnow(const std::vector<EETileCoordinate>& inTiles, EEGraphicEffect* inGraphicEffect);   // 0xf9b6d (kind 0x3d)
    EEE3DSnow();                            // 0xf9c11
    virtual ~EEE3DSnow();                   // +0x00 0xf9c87 (??_G 0xf9bf5)
    virtual long DrawPass6();               // +0x18 0xfa404: draws the flakes (fading out near the camera)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xf9e42
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xfa291: centre, transformation, radius (EEE3DRain's folds into it)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xfa579
    virtual long RepairPointers();          // +0x3c 0xfa5f3: the flakes saved as a count are respawned
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xfa663
    virtual long Step();                    // +0x4c 0xfa70d: the update (7: over and no flake left)
    virtual void Virtual64();               // +0x64 0xf9ef5: the graphic effect's parameters, the flake model
    virtual long AddFlake();                // +0x68 0xf9caf: a new flake over a random tile
    virtual long UpdateFlakes();            // +0x6c 0xfa782: spawns and moves the flakes

    std::vector<EE3DSnowDraw> mDraws;       // +0xb0
    std::vector<EEE3DSnowFlake> mFlakes;    // +0xc0
    unsigned long mFlakeCount;              // +0xd0 (saved; flakes spawned so far)
    unsigned long mLoadedFlakes;            // +0xd4 (Load: the flake count Virtual3C respawns)
    long mReservedD8[1];                    // +0xd8 never accessed (no EEE3DSnow function touches +0xd8); sizeof 0xdc
                                            // from new 0xdc in EEEffectManager 0x50673e/0x506978
};

#endif
