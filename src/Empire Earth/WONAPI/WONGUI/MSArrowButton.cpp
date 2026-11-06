// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;
// FUNCTION: WON 0x006ff150
void MSArrowButton::Paint(Graphics& g) {
    Component::Paint(g);
    bool down=(mButtonFlags&0x20)!=0;
    g.SetSysColor(StandardColor_3DFace);
    g.FillRect(0,0,Width(),Height());
    LobbyDrawFrame(g,0,0,Width(),Height(),down);
    if(down) MSGraphicsUtil::DrawArrow(g,mDirection,1,1,Width(),Height(),IsDisabled());
    else MSGraphicsUtil::DrawArrow(g,mDirection,0,0,Width(),Height(),IsDisabled());
}
