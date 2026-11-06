// Palette deep duplicate. Original Palette constructor/destructor ownership retained.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <cstring>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x00749040
SmartPtr<Palette> Palette::Duplicate()
{
    Palette *aPalette = new Palette(mNumColors);
    memcpy(aPalette->mColors,mColors,mNumColors*sizeof(DWORD));
    return aPalette;
}
}
