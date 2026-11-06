
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
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/WONLobbyRoutingLogic.h"
#include "ee/wonapi/WONLobbyLan.h"
namespace WONAPI {
// FUNCTION: WON 0x0075b730
ChatCommandParser* LobbyFn35B730(int flags)
{
    if(gLobbyData.get()==NULL) return NULL;
    ChatCommandParser* parser=gLobbyData->mChatCommandParser.get();
    LobbyClientList* clients=NULL;
    if(gLobbyData->mLobbyContainer->OnInternetScreen())
        clients=gLobbyData->mRoutingLogic->GetClientList();
    else if(gLobbyData->mLobbyContainer->OnLanScreen())
        clients=gLobbyData->mLanLogic->Fn3E0170();
    parser->SetClientList(clients,flags);
    return parser;
}
// FUNCTION: WON 0x00760ea0
Component* LobbyFn360EA0()
{
    if(gLobbyData.get()==NULL) return NULL;
    if(LobbyFn361130()) return gLobbyData->mLobbyContainer->mInternetScreen.get();
    if(LobbyFn361150()) return gLobbyData->mLobbyContainer->mLanScreen.get();
    if(LobbyFn361170()) return gLobbyData->mLobbyContainer->mLoginScreen.get();
    return NULL;
}
// FUNCTION: WON 0x00760f00
void LobbyFn360F00()
{
    if(gLobbyData.get()!=NULL) {
        gLobbyData->mLobbyContainer->ShowInternetScreen();
        LobbyEvent* event=new LobbyEvent(100014);
        gLobbyData->mLobbyContainer->mInternetScreen->Fn2F52D0(event);
    }
}
// FUNCTION: WON 0x00760f90
void LobbyFn360F90()
{
    if(gLobbyData.get()!=NULL) {
        gLobbyData->mLobbyContainer->ShowLanScreen();
        LobbyEvent* event=new LobbyEvent(100014);
        gLobbyData->mLobbyContainer->mLanScreen->Fn2F52D0(event);
    }
}
// FUNCTION: WON 0x007611b0
Container* GetLobbyRoot()
{
    return gLobbyData.get()!=NULL?gLobbyData->mLobbyContainer:SmartPtr<LobbyContainer>();
}
}
