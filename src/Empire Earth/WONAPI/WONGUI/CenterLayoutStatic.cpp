// Complete original static layout convenience API; allocation/forwarding support only.
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
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// FUNCTION: WON 0x00737690
void CenterLayout::Center(Component* parent,Component* child,bool horz,bool vert)
{
    SmartPtr<CenterLayout> layout=new CenterLayout(parent,child,horz,vert);
    layout->Execute(parent);
}
}
