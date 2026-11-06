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
// FUNCTION: WON 0x007285a0
void Image::DrawEntire(DisplayContext* context,int x,int y)
{
    Draw(context,x,y,0,0,mWidth,mHeight);
}
}
