// C:\EEX\Low-Level Engine\Graphics Engine\GELight.cpp (name from lle/GELight.h's out-of-line members)
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// No address range of its own (sorts between GEFontBitmap.cpp and GELine.cpp at 0x100166d3): both members are
// ICF-folded into identical accessor bodies kept in NELagLossSimulator.cpp (0x1003d63e get, 0x1003d65d set). No key
// chains: the unit includes no rasterizer header.
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/GE2DPoint.h"
#include "lle/GEVector.h"
#include "lle/GE3DPoint.h"
#include "lle/GEColor.h"
#include "lle/GELight.h"

float GELight::GetAttenuation0() const
{
    return this->mAttenuation0;
}

void GELight::SetAttenuation0(float inAttenuation)
{
    this->mAttenuation0 = inAttenuation;
}
