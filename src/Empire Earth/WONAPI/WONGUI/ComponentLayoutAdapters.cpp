// Genuine Container layout adapters; support0.
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
// FUNCTION: WON 0x006f6100
void Container::AddLayoutPoint(LayoutPoint* point)
{
    point->mParent=this;
    mChildLayout.AddLayoutPoint(point);
}
// FUNCTION: WON 0x006f6120
void Container::Fn2F6120(int id,bool enabled)
{
    mChildLayout.EnableInstructions(id,enabled);
}
// FUNCTION: WON 0x006f6290
void Container::Fn2F6290()
{
    Virtual08();
    Invalidate();
}
}
