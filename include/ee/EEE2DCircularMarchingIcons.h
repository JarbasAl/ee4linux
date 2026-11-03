// C:\EEX\Empire Earth\Graphics Thread\EEE2DCircularMarchingIcons.h (RECONSTRUCTION; file name inferred). Marching
// icons on a circle (RTTI EEE2DCircularMarchingIcons > EEE2DMarchingIcons; vtable 0x43fa54;
// EEE2DCircularMarchingIcons.cpp 0xdf57c..0xdf68d). No members of its own: the radius is EEEffect2D::mF50 (its
// graphic effect's +0x28). Its ??_G 0xdf590 is the ICF-kept copy for EEE2DMarchingIcons's too (implicit dtor).
// Names are provisional. Requires ee/EEE2DMarchingIcons.h.
#ifndef EE_EEE2DCIRCULARMARCHINGICONS_H
#define EE_EEE2DCIRCULARMARCHINGICONS_H

#include "ee/EEE2DMarchingIcons.h"

class EEE2DCircularMarchingIcons : public EEE2DMarchingIcons {
public:
    EEE2DCircularMarchingIcons();           // 0xdf57c (the load-time ctor: kind 0xf)
    EEE2DCircularMarchingIcons(EEUnit* inUnit, EEGraphicEffect* inGraphicEffect);   // 0xdf5ac (kind 0x10)
    virtual void ComputeOffsets(float inAngle, std::vector<GE3DPoint>& outPoints);  // +0x54 0xdf63d

    void AddIcon(float inAngle, std::vector<GE3DPoint>& outPoints);   // 0xdf5d3 (provisional name)
};

#endif
