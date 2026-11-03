// C:\EEX\Empire Earth\Graphics Thread\EEE3DDamageVent.h (RECONSTRUCTION; file name inferred).
// Smoke/fire jets from a damaged unit's vent tag points: more jets the lower its hit points (vtable 0x440558; RTTI
// EEE3DDamageVent > EEEffect3D; EEE3DDamageVent.cpp 0xea9e2..0xebbba, key "EEE3DDamageVentData"). sizeof 0x90. Names
// are provisional. Requires ee/EEEffect3D.h, ee/EEGraphicObject.h, <vector>.
#ifndef EE_EEE3DDAMAGEVENT_H
#define EE_EEE3DDAMAGEVENT_H

#include <vector>

class EEGraphicObject;
class EETagPointList;

// One jet (0x124 bytes; vector<EEE3DDamageVentPart> 0xea... members at the end of the unit). The implicit default ctor
// 0xeb4c0 constructs the four transformations; the copies are implicit too. Names are provisional.
struct EEE3DDamageVentPart {
    // 0xeb4e9: the jet's orientation from the vent point inFrom[mTag] towards inTo[mTag] (model space)
    void Aim(const EETagPointList* inFrom, const EETagPointList* inTo);

    long mTag;                              // +0x00 (the vent's tag point index)
    GETransformation mTransformation;       // +0x04 (the unit's location, moved along the jet)
    float mSpeed;                           // +0x44 (random in the effect's mF30..mF34)
    float mLength;                          // +0x48 (the current length, follows mSpeed * the damage scale)
    unsigned long mFrame;                   // +0x4c
    float mTime;                            // +0x50 the time left in the frame
    float mF54;                             // +0x54
    float mNextSubEffectTime;               // +0x58 the time to the next sub-effect
    float mDrawLength;                      // +0x5c (Virtual24 copies mF48)
    unsigned long mDrawFrame;               // +0x60 (Virtual24 copies mFrame)
    GETransformation mDrawTransformation;   // +0x64 (Virtual24 copies mTransformation)
    GETransformation mOrientation;          // +0xa4 (Aim)
    GETransformation mDrawOrientation;      // +0xe4 (Virtual24 copies mOrientation)
};

class EEE3DDamageVent : public EEEffect3D {
public:
    EEE3DDamageVent(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xeaa25 (kind 0x35)
    EEE3DDamageVent();                      // 0xeaa92
    virtual ~EEE3DDamageVent();             // +0x00 0xeaac0 (??_G 0xeaa76)
    virtual long DrawPass2();               // +0x08 (`return 0`: ICF-folded into 0xfca98)
    virtual long DrawPass4();               // +0x10 (likewise)
    virtual long DrawPass5();               // +0x14 (likewise)
    virtual long DrawPass6();               // +0x18 0xeabe0 (the draw)
    virtual long Queue(EEEffectQueue* inQueues);       // +0x24 0xeaaec (queue 4, by texture)
    virtual void PrepareQueue();            // +0x2c (empty: ICF-folded into 0xe123d)
    virtual long SetUp();                   // +0x30 0xeabb5 (loads the graphic)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);    // +0x38 0xeae0a
    // +0x40: EEEffect3D::Save (ICF-folded into EEE3DAdditivePass's 0xe8e01)
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);
    virtual long Step();                    // +0x4c 0xeae33 (adds/removes and moves the jets)

    static const UString sData;             // "EEE3DDamageVentData" (0x4e8af8; provisional name)

    EEGraphicObject* mGraphicObject;        // +0x74
    std::vector<EEE3DDamageVentPart> mParts;   // +0x78
    unsigned long mDrawCount;               // +0x88 (Virtual24 copies mParts.size())
    float mNextCountCheckTime;              // +0x8c the time to the next jet count check (2000 ms)
};

#endif
