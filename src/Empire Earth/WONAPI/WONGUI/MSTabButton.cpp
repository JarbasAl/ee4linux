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
// FUNCTION: WON 0x006feed0
void MSTabButton::Virtual0C(int& width,int& height) {
    MSButtonBase::Virtual0C(width,height);
    width+=16;
    height+=5+mLowerAmount;
}
// FUNCTION: WON 0x006fef00
void MSTabButton::Paint(Graphics& g) {
    Component::Paint(g);
    MSGraphicsUtil::DrawTab(g,0,0,Width(),Height());
    DrawButtonText(g,0,0);
}
