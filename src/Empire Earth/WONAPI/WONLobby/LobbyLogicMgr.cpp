// Actual SDK logic manager: a single owning list and real dispatch behavior.
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
#include "ee/wonapi/WONLobbyLogic.h"
namespace WONAPI {
// Actual list push_back adapter3e5c10, generated/container support0.
// FUNCTION: WON 0x007e5c10
void LobbyLogicMgr::AddLogic(LobbyLogic* logic)
{
    mLogicList.push_back(logic);
}
// FUNCTION: WON 0x007e5d40
void LobbyLogicMgr::Init(ComponentConfig* config)
{
    LogicList::iterator itr=mLogicList.begin();
    while(itr!=mLogicList.end()) {
        (*itr)->InitComponent(config);
        ++itr;
    }
}
// FUNCTION: WON 0x007e5d80
void LobbyLogicMgr::Kill()
{
    LogicList::iterator itr=mLogicList.begin();
    while(itr!=mLogicList.end()) {
        (*itr)->Kill();
        ++itr;
    }
}
// FUNCTION: WON 0x007e5db0
void LobbyLogicMgr::DispatchControlEvent(int control)
{
    LogicList::iterator itr=mLogicList.begin();
    while(itr!=mLogicList.end()) {
        (*itr)->HandleControlEvent(control);
        ++itr;
    }
}
// FUNCTION: WON 0x007e5df0
void LobbyLogicMgr::DispatchComponentEvent(ComponentEvent* event)
{
    LogicList::iterator itr=mLogicList.begin();
    while(itr!=mLogicList.end()) {
        (*itr)->HandleComponentEvent(event);
        ++itr;
    }
}
// FUNCTION: WON 0x007e5e30
void LobbyLogicMgr::DispatchTimerEvent()
{
    LogicList::iterator itr=mLogicList.begin();
    while(itr!=mLogicList.end()) {
        // Canonical existing slot0c is named Reset; same actual no-argument slot.
        (*itr)->Reset();
        ++itr;
    }
}
// FUNCTION: WON 0x007e5e60
void LobbyLogicMgr::HandleComponentEvent(ComponentEvent* event)
{
    switch(event->mType) {
    default:
        DispatchComponentEvent(event);
        break;
    case 24: {
        ListItemClickedEvent* click=static_cast<ListItemClickedEvent*>(event);
        if(click->mItem.get()==NULL || click->mButton!=0) return;
        MenuItem* menu=dynamic_cast<MenuItem*>(click->mItem.get());
        if(menu!=NULL) DispatchControlEvent(menu->mMenuId);
        break;
    }
    case 15:
        DispatchControlEvent(event->GetControlId());
        break;
    }
}
}

namespace WONAPI {
// SUPPORT-ONLY: EXE 0x00766b30 (genuine SDK explicit default constructor)
// Complete nonpolymorphic0c owner; automatic owningLogicList construction.
LobbyLogicMgr::LobbyLogicMgr() {}
}
