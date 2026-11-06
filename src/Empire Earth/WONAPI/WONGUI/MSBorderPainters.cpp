// Original MS control border painting on borrowed external-owned receivers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyMSBorderPainters.h"

namespace WONAPI {
// FUNCTION: WON 0x00700860
void MSEtchedFrame::Paint(Graphics& g)
{
    Component::Paint(g);
    g.SetSysColor(StandardColor_3DHilight);
    g.DrawRect(0,0,Width(),Height());
    g.DrawRect(1,1,Width()-1,Height()-1);
    g.SetSysColor(StandardColor_3DShadow);
    g.DrawRect(0,0,Width()-1,Height()-1);
}

// FUNCTION: WON 0x006ffad0
void MSInputBox::Paint(Graphics& g)
{
    InputBox::Paint(g);
    MSGraphicsUtil::DrawFrame3(g,0,0,Width(),Height(),true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006ffb00
void MSInputBox::Virtual0C(int& width,int& height) { width=100; height=22; }
}
