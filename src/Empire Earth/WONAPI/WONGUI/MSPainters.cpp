// Source-only standard MS popup/dialog background paint; no runtime authority.
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
#include "ee/wonapi/WONLobbyMSPainters.h"
namespace WONAPI {
// FUNCTION: WON 0x00700450
void MSPopup::Paint(Graphics& g)
{
    Component::Paint(g);
    g.SetSysColor(11);
    g.FillRect(0,0,Width(),Height());
    g.SetSysColor(2);
    g.DrawLine(1,1,Width()-2,1);
    g.DrawLine(1,1,1,Height()-2);
    g.SetSysColor(0);
    g.DrawLine(Width()-1,0,Width()-1,Height()-1);
    g.DrawLine(1,Height()-1,Width()-1,Height()-1);
    g.SetSysColor(3);
    g.DrawLine(Width()-2,0,Width()-2,Height()-2);
    g.DrawLine(2,Height()-2,Width()-2,Height()-2);
}
// FUNCTION: WON 0x00700520
void MSDlgBack::Paint(Graphics& g)
{
    Component::Paint(g);
    g.SetSysColor(1);
    g.FillRect(0,0,Width(),Height());
}
}
