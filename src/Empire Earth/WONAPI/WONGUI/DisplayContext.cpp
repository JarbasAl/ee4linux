// Target DisplayContext bitmap factory, corroborated by original platform SDK.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x007cedd0 (143B genuine native factory, primary0).
SmartPtr<NativeImage> DisplayContext::CreateImage(int width,int height)
{
    return new MSNativeImage(this,width,height);
}
}
