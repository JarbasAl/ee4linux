// C:\EEX\Empire Earth\Graphics Thread\EEE3DShield.h (RECONSTRUCTION; file name inferred). A scrolling shield model at
// a unit's tag point, fading while the unit loses hit points (vtable 0x440ce8; RTTI EEE3DShield > EEEffect3D;
// EEE3DShield.cpp 0xf923f..0xf9871). Its tag-point update is EEE3DBlastWave's
// FollowUnit (ICF-identical). Names are provisional. Requires ee/EEEffect3D.h.
#ifndef EE_EEE3DSHIELD_H
#define EE_EEE3DSHIELD_H

class EEGraphicObject;

class EEE3DShield : public EEEffect3D {
public:
    EEE3DShield(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xf9282 (kind 0x45)
    EEE3DShield();                          // 0xf92e2
    virtual ~EEE3DShield();                 // +0x00 0xf9305 (??_G 0xf92c6)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 0xf942a (the draw: scrolls the texture v by mDrawScroll)
    virtual long DrawPass6();               // +0x18 (`return 0`)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xf932c (queue 3, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xf93bd
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xf966d
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 0xf9701
    virtual long Step();                    // +0x4c 0xf9775 (7: the unit is dead or the age reached 1)

    void FollowUnit();                      // the transformation from the unit's tag point (ICF: 0xe904a)

    static const UString sData;             // "EEE3DShield-Data" (0x4e8cc0; provisional name)

    GE3DPoint mLocation;                    // +0x74 (the position the draw uses)
    float mScroll;                          // +0x80 (saved)
    float mDrawScroll;                      // +0x84 (Virtual24 copies mScroll)
    float mAge;                             // +0x88 0..1 (saved)
    float mDrawAge;                         // +0x8c
    float mOpacity;                         // +0x90 (saved)
    float mDrawOpacity;                     // +0x94
    long mHitPoints;                        // +0x98 the unit's at the last update (saved)
    EEGraphicObject* mGraphicObject;        // +0x9c
    bool mScrollingDown;                    // +0xa0 (saved from version 0.0xb4)
};

#endif
