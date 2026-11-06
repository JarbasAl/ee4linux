// Genuine SDK multilist bounds lookup and header base rendering adapter; support0.
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
// FUNCTION: WON 0x00702500
ListItem* MultiListItem::GetItem(int column) const
{
    if(column<0 || column>=mItems.size()) return NULL;
    return mItems[column].get();
}
// Shared original305e00/13B with ScrollAreaViewport::Paint; new nominal source only,0globalnative/0primary.
void MultiListHeader::Paint(Graphics& graphics) { Container::Paint(graphics); }
// Shared original304710/18B with RootContainer::VirtualAC; nominal source completion only,0new globalnative.
Component* MultiListHeader::VirtualAC(int x,int y)
{
    return Container::VirtualAC(x,y);
}
}
