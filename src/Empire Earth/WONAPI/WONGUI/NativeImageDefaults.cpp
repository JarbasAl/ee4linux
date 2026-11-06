// Complete original-backed GUI values; source-only, no runtime substitution.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Genuine SDK null-return defaults, identical typed null-return behavior at two native sites; zero new native credit.
// FUNCTION: WON 0x0072b9d0
SmartPtr<NativeIcon> NativeImage::GetIcon() { return NULL; }
// FUNCTION: WON 0x00728bd0
SmartPtr<RawImage32> NativeImage::GetRaw() { return NULL; }
}
