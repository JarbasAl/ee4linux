// Original-owned WON control configuration handlers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x007986d0
bool WONScrollbarConfig::Virtual24(const std::string& key)
{
    if(key=="TRANSPARENT") {
        if(Fn3837F0()) {
            Button* rect=new Button;
            mScrollbar->mScrollRect=rect;
        }
    } else return ScrollbarConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x00798b20
bool WONHeaderControlConfig::Virtual24(const std::string& key)
{
    if(MultiListHeaderConfig::Virtual24(key)) return true;
    if(key=="SETSTANDARDCOLUMN") {
        int column=ReadInt(); EnsureComma();
        GUIString title=Fn3842F0(false);
        mHeader->SetColumn(column,title,NULL);
    } else return false;
    return true;
}
// FUNCTION: WON 0x00798c00
bool WONListCtrlConfig::Virtual24(const std::string& key)
{
    if(key=="HEADERHEIGHT") {
        int height=ReadInt(); mListCtrl->mHeaderHeight=height;
    } else if(key=="NUMCOLUMNS") {
        MultiListArea* area=static_cast<MultiListArea*>(mListCtrl->mScrollArea.get());
        int count=ReadInt(); area->SetNumColumns(count);
    } else if(key=="DYNAMICSIZECOLUMN") {
        MultiListArea* area=static_cast<MultiListArea*>(mListCtrl->mScrollArea.get());
        int column=ReadInt(); area->mDynamicSizeColumn=column;
    } else if(key=="LISTFLAGS") {
        ListAreaConfig::ReadListFlags(this,static_cast<MultiListArea*>(mListCtrl->mScrollArea.get()));
    } else if(key=="SETSTANDARDCOLUMN") {
        int column=ReadInt(); EnsureComma();
        GUIString title=Fn3842F0(false); EnsureComma();
        int width=ReadInt(); mListCtrl->SetStandarColumnHeader(column,title,width);
    } else if(key=="SETCOLUMN") {
        int column=ReadInt(); EnsureComma();
        Component* component=ReadComponent(); EnsureComma();
        int width=ReadInt(); mListCtrl->SetColumnHeader(column,component,width);
    } else return WONScrollContainerConfig::Virtual24(key);
    return true;
}
}
