// Original-backed config-gated client flag transition/event; no partial private object allocation.
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
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyConfig.h"
namespace WONAPI {
// Existing ordinary persistence function, native35d100/cdecl.
void LobbyFn35D100(const GUIString&,bool);
// FUNCTION: WON 0x0075b850
void LobbyFn35B850(LobbyClient* client,bool flag,int roomSpec)
{
    WONLobbyPrv* data=gLobbyData.get();
    if(data==NULL || !data->mConfig->mCDKeyCheck) return;
    if(client->mB23==flag) return;
    client->mB23=flag;
    LobbyFn359100(new ClientChangedEvent(client,roomSpec,2),true);
}
// FUNCTION: WON 0x0075b7b0
void LobbyFn35B7B0(LobbyClient* client,bool flag,int roomSpec)
{
    if(gLobbyData.get()==NULL) return;
    if(client->mB22==flag) return;
    client->mB22=flag;
    LobbyFn35D100(client->mName,flag);
    LobbyFn359100(new ClientChangedEvent(client,roomSpec,2),true);
}

}
