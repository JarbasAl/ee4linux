// Full genuine MenuItem38 allocation uses its original external constructor/lifetime.
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
// FUNCTION: WON 0x006fd480
MenuItem* PopupMenu::AddItem(const GUIString& text,NativeImage* icon,ComponentListener* listener,int id)
{
    SmartPtr<MenuItem> item=new MenuItem(text,listener);
    item->mMenuId=id;
    item->mImage=icon;
    InsertItem(item.get());
    SetSizeEqualToTotal(-1,-1);
    return item.get();
}
}
