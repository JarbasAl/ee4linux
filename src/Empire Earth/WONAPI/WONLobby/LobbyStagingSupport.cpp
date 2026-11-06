// Complete native singleton orchestration; genuine external lifetime providers.
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
#include "ee/wonapi/WONLobbyAccountLogic.h"
#include "ee/wonapi/WONLobbyDialogLogic.h"
#include "ee/wonapi/WONLobbyLan.h"
#include "ee/wonapi/WONLobbyRoutingLogic.h"
#include "ee/wonapi/WONLobbyStaging.h"
#include "ee/wonapi/WONLobbyPing.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/WONCRC16.h"
#include "ee/wonapi/EELobbySound.h"
namespace WONAPI {
// Genuine SDK GetNetLogic helpers: adopted namespace spellings, exact native ABI.
// No shared declarations or replacement owner types are introduced.
// FUNCTION: WON 0x00761be0
NetLogic* LobbyFn361BE0(LobbyGameType type)
{
    if(gLobbyData.get()==NULL) return NULL;
    switch(static_cast<int>(type)) {
    case 1: case 3: return gLobbyData->mLanLogic.get();
    case 2: return gLobbyData->mRoutingLogic.get();
    default: return NULL;
    }
}
// FUNCTION: WON 0x00761c10
NetLogic* LobbyFn361C10()
{
    if(gLobbyData.get()==NULL) return NULL;
    return LobbyFn361BE0(gLobbyData->mCurGameType);
}
// FUNCTION: WON 0x00761c30
bool LobbyFn361C30(LobbyGame* game)
{
    if(gLobbyData.get()!=NULL) {
        gLobbyData->mCurGameType=static_cast<LobbyGameType>(game->mL2C);
        NetLogic* logic=LobbyFn361C10();
        if(logic!=NULL) return logic->Virtual18(game);
    }
    return false;
}
// FUNCTION: WON 0x00761c60
bool LobbyFn361C60(LobbyGame* game)
{
    if(gLobbyData.get()!=NULL) {
        gLobbyData->mCurGameType=static_cast<LobbyGameType>(game->mL2C);
        NetLogic* logic=LobbyFn361C10();
        if(logic!=NULL) return logic->Virtual1C(game);
    }
    return false;
}
// FUNCTION: WON 0x00761c90
LobbyClient* LobbyFn361C90()
{
    NetLogic* logic=LobbyFn361C10();
    if(logic==NULL) return NULL;
    return logic->Virtual20();
}
// FUNCTION: WON 0x00761dd0
void LobbyFn361DD0()
{
    if(gLobbyData.get()==NULL) return;
    if(LobbyFn361130()) gLobbyData->mRoutingLogic->Virtual4C();
    else if(LobbyFn361150()) gLobbyData->mLanLogic->Virtual4C();
}
// FUNCTION: WON 0x007612d0
void LobbyFn3612D0(LobbyGame* game) { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F08B0(game); }
// FUNCTION: WON 0x007612f0
void LobbyFn3612F0() { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F0410(); }
// FUNCTION: WON 0x00761330
bool LobbyFn361330(bool confirm) { return gLobbyData.get()!=NULL?gLobbyData->mStagingLogic->Fn3F0110(confirm):false; }
// FUNCTION: WON 0x00761420
void LobbyFn361420(unsigned short port) { if(gLobbyData.get()!=NULL) gLobbyData->mLobbyPort=port; }
// FUNCTION: WON 0x00761bc0
void LobbyFn361BC0(LobbyClient* client,bool ban) { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F10F0(client,ban); }
// FUNCTION: WON 0x00761e10
void LobbyFn361E10(ByteBuffer* data,LobbyClient* client) { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3EFD20(data,client); }
// FUNCTION: WON 0x00761e30
void LobbyFn361E30(ReadBuffer& data,LobbyClient* client) { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3EFB50(data,client); }
// FUNCTION: WON 0x00761e50
void LobbyFn361E50(bool showError) { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F0DE0(showError); }
// FUNCTION: WON 0x00761e70
void LobbyFn361E70(LobbyClient* client) { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F0F10(client); }
// FUNCTION: WON 0x00761e90
void LobbyFn361E90(LobbyClient* client,bool ban) { if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F0E10(client,ban); }
// FUNCTION: WON 0x00761ed0
void LobbyFn361ED0(bool joinDefault) { if(gLobbyData.get()!=NULL) gLobbyData->mRoutingLogic->EnterInternetScreen(joinDefault); }
}

namespace WONAPI {
// Genuine missing SDK namespace entry points; retained canonical ABI spellings.
// FUNCTION: WON 0x00761260
void LobbyFn361260()
{
    if(!LobbyFn3613C0()) return;
    NetLogic* logic=LobbyFn361C10();
    LobbyGame* game=GetMyLobbyGame();
    if(logic!=NULL && game!=NULL) logic->Virtual30(game);
}
// FUNCTION: WON 0x00761290
void LobbyFn361290()
{
    if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3EF9B0();
}
// FUNCTION: WON 0x007612b0
void LobbyFn3612B0(bool inProgress)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3EFA80(inProgress);
}
// FUNCTION: WON 0x00761310
bool LobbyFn361310(bool confirm)
{
    return gLobbyData.get()!=NULL?gLobbyData->mStagingLogic->Fn3F0C60(confirm):false;
}
// FUNCTION: WON 0x00761370
LobbyClient* LobbyFn361370()
{
    return gLobbyData.get()!=NULL?gLobbyData->mStagingLogic->mMyClient.get():NULL;
}
// FUNCTION: WON 0x007613e0
void LobbyFn3613E0()
{
    if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F11F0();
}
// FUNCTION: WON 0x00761b60
void LobbyFn361B60(ByteBuffer* message)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3EFD60(message);
}
// FUNCTION: WON 0x00761b80
void LobbyFn361B80(ByteBuffer* message)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3EFDA0(message);
}
// FUNCTION: WON 0x00761ba0
void LobbyFn361BA0(LobbyClient* client,ByteBuffer* message)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3EFDE0(client,message);
}
}

namespace WONAPI {
// (SDK LobbyStaging::GetGame)
// FUNCTION: WON 0x00761350
LobbyGame* GetMyLobbyGame() { return gLobbyData.get()!=NULL?gLobbyData->mStagingLogic->mGame.get():NULL; }
// (SDK LobbyStaging::IAmCaptain)
// FUNCTION: WON 0x007613c0
bool LobbyFn3613C0() { return gLobbyData.get()!=NULL?gLobbyData->mStagingLogic->Fn3EFE10():false; }
// (SDK LobbyStaging::IsBehindFirewall)
// FUNCTION: WON 0x00761b20
bool LobbyFn361B20() { return gLobbyData.get()!=NULL?gLobbyData->mBehindFirewall:false; }
// (SDK LobbyStaging::IsFirewallTestDone)
// FUNCTION: WON 0x00761b40
bool LobbyFn361B40() { return gLobbyData.get()!=NULL?gLobbyData->mFirewallTestDone:false; }
}

namespace WONAPI {
// Genuine SDK LobbyStaging::GetMyPlayer; original16B, borrowed player from borrowed local client.
// FUNCTION: WON 0x00761390
LobbyPlayer* GetMyLobbyPlayer()
{
    LobbyClient* client=LobbyFn361370();
    return client!=NULL ? client->mPlayer.get() : NULL;
}
// Genuine SDK LobbyStaging::IAmReady; original20B.
// FUNCTION: WON 0x007613a0
bool LobbyFn3613A0()
{
    return gLobbyData.get()!=NULL ? gLobbyData->mStagingLogic->Fn3EFE30() : false;
}
// Genuine SDK LobbyStagingPrv network dispatch cohort; all parameters borrowed, no new ownership.
// FUNCTION: WON 0x00761cb0
void LobbyFn361CB0(ByteBuffer* message)
{
    NetLogic* logic=LobbyFn361C10();
    if(logic!=NULL) logic->Virtual24(message);
}
// FUNCTION: WON 0x00761cd0
void LobbyFn361CD0(ByteBuffer* message)
{
    NetLogic* logic=LobbyFn361C10();
    if(logic!=NULL) logic->Virtual28(message);
}
// FUNCTION: WON 0x00761cf0
void LobbyFn361CF0(LobbyClient* client,ByteBuffer* message)
{
    NetLogic* logic=LobbyFn361C10();
    if(logic!=NULL) logic->Virtual2C(client,message);
}
// FUNCTION: WON 0x00761d10
void LobbyFn361D10(LobbyGame* game)
{
    NetLogic* logic=LobbyFn361BE0(static_cast<LobbyGameType>(game->mL2C));
    if(logic!=NULL) logic->Virtual34(game);
}
// FUNCTION: WON 0x00761d30
void LobbyFn361D30(LobbyGame* game)
{
    NetLogic* logic=LobbyFn361BE0(static_cast<LobbyGameType>(game->mL2C));
    if(logic!=NULL) logic->Virtual40(game);
}
// FUNCTION: WON 0x00761d50
void LobbyFn361D50(LobbyClient* client,bool inBan)
{
    NetLogic* logic=LobbyFn361C10();
    if(logic!=NULL) logic->Virtual38(client,inBan);
}
// FUNCTION: WON 0x00761d70
void LobbyFn361D70(bool inClose)
{
    NetLogic* logic=LobbyFn361C10();
    if(logic!=NULL) logic->Virtual3C(inClose);
}
// FUNCTION: WON 0x00761d90
void LobbyFn361D90()
{
    NetLogic* logic=LobbyFn361C10();
    if(logic!=NULL) logic->Virtual44();
}
// FUNCTION: WON 0x00761db0
void LobbyFn361DB0()
{
    NetLogic* logic=LobbyFn361C10();
    if(logic!=NULL) logic->Virtual48();
}
// Genuine SDK LobbyStagingPrv::NotifyPingChange; original23B.
// FUNCTION: WON 0x00761eb0
void LobbyFn361EB0(LobbyGame* game)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mStagingLogic->Fn3F1000(game);
}
}

