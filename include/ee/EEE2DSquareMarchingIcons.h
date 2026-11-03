// C:\EEX\Empire Earth\Graphics Thread\EEE2DSquareMarchingIcons.h (RECONSTRUCTION; file name inferred). Marching
// icons on a square (RTTI EEE2DSquareMarchingIcons > EEE2DMarchingIcons; vtable 0x4400dc;
// EEE2DSquareMarchingIcons.cpp 0xe72fa..0xe753d). No members of its own: EEEffect2D::mF50 is the square's half
// diagonal (sqrt(2 r r) of its graphic effect's +0x28). The dtor is declared (it stores the vptr; ??_G 0xe730e calls
// it out of line). Names are provisional. Requires ee/EEE2DMarchingIcons.h.
#ifndef EE_EEE2DSQUAREMARCHINGICONS_H
#define EE_EEE2DSQUAREMARCHINGICONS_H

#include "ee/EEE2DMarchingIcons.h"

class EEE2DSquareMarchingIcons : public EEE2DMarchingIcons {
public:
    EEE2DSquareMarchingIcons();             // 0xe72fa (the load-time ctor: kind 0xf)
    EEE2DSquareMarchingIcons(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xe732a (kind 0xf)
    virtual ~EEE2DSquareMarchingIcons();    // 0xe7365 (??_G 0xe730e)
    virtual void ComputeOffsets(float inAngle, std::vector<GE3DPoint>& outPoints);  // +0x54 0xe74ed

    void AddIcon(float inAngle, std::vector<GE3DPoint>& outPoints);   // 0xe7370 (provisional name)
};

#endif
