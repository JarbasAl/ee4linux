// Original skinned list/header/area composition; raw owned results follow the native API.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyMSSkinImage.h"
#include "ee/wonapi/WONLobbySkinListControls.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00715b20
WONListCtrl* WONSkin::GetListCtrl()
{
    WONListCtrl* list=mListCtrl->Duplicate();
    list->CopyAttributes(mScrollContainer.get());
    list->SetHeader(new WONHeaderControl(this));
    MultiListArea* area=new MultiListArea;
    list->SetScrollArea(area);
    list->SetScrollbar(GetScrollbar(true));
    list->SetScrollbar(GetScrollbar(false));
    list->GetHeader()->SetMultiListArea(area);
    return list;
}
}
