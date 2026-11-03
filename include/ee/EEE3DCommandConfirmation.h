// C:\EEX\Empire Earth\Graphics Thread\EEE3DCommandConfirmation.h (RECONSTRUCTION; file name inferred). The ring
// under a commanded unit or at a commanded point, coloured by the command (vtable 0x440458; RTTI
// EEE3DCommandConfirmation > EEEffect2D; EEE3DCommandConfirmation.cpp 0xe9bee..0xea703: six file-scope colours, no
// key). sizeof 0x8c. Names are provisional. Requires ee/EEEffect2D.h, lle/GEColor.h.
#ifndef EE_EEE3DCOMMANDCONFIRMATION_H
#define EE_EEE3DCOMMANDCONFIRMATION_H

class EEGraphicObject;
class EEComplexUnit;
template <class T> class U2DPoint; typedef U2DPoint<long> EETileCoordinate;
class GETexture;

// 0x241658 (cdecl): whether inUnit cannot stand on inTile (the confirmation shows the "blocked" colour when it can
// and the tile is visible) (provisional name; meaning inferred)
// (EECanStandOn, ee/EEAreaUtilities.h)

// EEROUnits.cpp's three command textures (0x5294e0/0x5294e4/0x5294e8, loaded there; the confirmation draws with them:
// they are not unit-local) (provisional names)
extern GETexture* sTexture4E0;
extern GETexture* sTexture4E4;
extern GETexture* sTexture4E8;

class EEE3DCommandConfirmation : public EEEffect2D {
public:
    // 0xe9d1e: at the unit (inGoalIndex: its goal queue, 0 for the current one)
    EEE3DCommandConfirmation(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect, unsigned long inGoalIndex, long inKind);
    // 0xe9d91: at a point
    EEE3DCommandConfirmation(EEUnit* inUnit, const GE3DPoint& inLocation, EEGraphicEffect* inGraphicEffect,
                             unsigned long inGoalIndex, long inKind);
    virtual ~EEE3DCommandConfirmation();    // +0x00 0xe9e20 (??_G 0xe9d75)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 0xea201 (the draw)
    virtual long DrawPass5();               // +0x14 (`return 0`)
    virtual long DrawPass6();               // +0x18 (`return 0`)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xe9e42 (queue 2, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xe9eb5 (the colour from the command)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xea1fc (0)
    virtual long RepairPointers();          // +0x3c (`return 0`)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x40 (0xea1fc)
    virtual long Step();                    // +0x4c 0xea47e (ages, follows the unit, animates)

    GEColor mColor;                         // +0x54
    EEGraphicObject* mGraphicObject;        // +0x60
    long mState;                            // +0x64 (the effect's mL64: 0 the unit's goal, 1..4 fixed colours)
    long mDrawState;                        // +0x68 (Virtual24 copies mState)
    float mAge;                             // +0x6c milliseconds
    float mDrawAge;                         // +0x70 (Virtual24 copies mAge)
    float mStartSize;                       // +0x74
    float mEndSize;                         // +0x78
    float GetTime() const { return this->mTime; }  // inline (Virtual4C loads it as a value: fld; faddp)
    float mTime;                            // +0x7c the time into the frame
    unsigned long mFrame;                   // +0x80
    unsigned long mDrawFrame;               // +0x84 (Virtual24 copies mFrame)
    unsigned long mGoalIndex;               // +0x88 (an index into the complex unit's mGoalQueues)
};

#endif
