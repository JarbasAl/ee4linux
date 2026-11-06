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
// Genuine SDK GetDefaultTimeout: native 35b5a0 returns a constant, with no state.
// FUNCTION: WON 0x0075b5a0
int LobbyFn35B5A0() { return 60000; }
// FUNCTION: WON 0x00758ef0
int LobbyFn358EF0(const GUIString& title,const GUIString& text,bool sound)
{
    return gLobbyData.get()!=NULL?gLobbyData->mDialogLogic->ShowOkCancel(title,text,sound):100002;
}
// FUNCTION: WON 0x00758e70
void LobbyFn358E70(AsyncOp* op,const GUIString& title,const GUIString& text,int mode,unsigned long timeout)
{
    if(gLobbyData.get()!=NULL) {
        DialogLogic* logic=gLobbyData->mDialogLogic.get();
        logic->RunOpWithStatus(op,LobbyFn35B5A0(),title,text,mode,timeout);
    }
}
// FUNCTION: WON 0x00758ed0
void LobbyFn358ED0(const GUIString& title,WONStatus status)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mDialogLogic->ShowError(title,status);
}
// FUNCTION: WON 0x0075b9a0
void LobbyFn35B9A0(Sound* sound)
{
    if(gLobbyData.get()!=NULL && !gLobbyData->mSuspended && sound!=NULL) sound->Fn329BD0();
}
// FUNCTION: WON 0x0075b900
void LobbyFn35B900(LobbyGame* game,bool force)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mPingLogic->Fn3E6B50(game,false,force);
}
// FUNCTION: WON 0x0075b920
void LobbyFn35B920(LobbyGame* game)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mPingLogic->Fn3E6B50(game,true,true);
}
// FUNCTION: WON 0x0075b960
void LobbyFn35B960(int unused)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mPingLogic->Fn3E6DB0(unused);
}
// FUNCTION: WON 0x0075b710
ChatCommandParser* LobbyFn35B710()
{
    return gLobbyData.get()!=NULL?gLobbyData->mChatCommandParser.get():NULL;
}
// FUNCTION: WON 0x0075b8f0
UDPManager* LobbyFn35B8F0()
{
    return gLobbyData.get()!=NULL?gLobbyData->mUDPManager.get():NULL;
}
// FUNCTION: WON 0x0075b600
void LobbyFn35B600(long& systemTime,long& gameTime)
{
    if(gLobbyData.get()!=NULL) {
        systemTime=gLobbyData->mSysTOUTime;
        gameTime=gLobbyData->mGameTOUTime;
    }
}
// FUNCTION: WON 0x0075b980
void LobbyFn35B980()
{
    if(gLobbyData.get()!=NULL) gLobbyData->mInitLogic->KillFirewallTest();
}
// FUNCTION: WON 0x0075b6b0
LobbyServerList* LobbyFn35B6B0()
{
    return gLobbyData.get()!=NULL?gLobbyData->mRoutingLogic->mServerList.get():NULL;
}
// FUNCTION: WON 0x0075b6d0
LobbyGroup* LobbyFn35B6D0()
{
    return gLobbyData.get()!=NULL?gLobbyData->mRoutingLogic->mChatGroup.get():NULL;
}
}

namespace WONAPI {
// Genuine missing SDK namespace entry points; retained canonical ABI spellings.
// FUNCTION: WON 0x0075b6f0
LobbyServer* LobbyFn35B6F0()
{
    return gLobbyData.get()!=NULL?gLobbyData->mRoutingLogic->mCurrentServer.get():NULL;
}
}

namespace WONAPI {
// GenuineSDKStringUtil.h declaration, source-local only. Provider body is the separate real StringUtil.cpp unit below.
const std::string& GetEmptyStr();
// FUNCTION: WON 0x0075b580
unsigned short GetLobbyProductId()
{
    if(gLobbyData.get()!=NULL) return gLobbyData->mLanProductId;
    return 0;
}
// (SDK LobbyMisc::DoCDKeyCheck)
// FUNCTION: WON 0x0075b540
bool LobbyFn35B540() { return gLobbyData.get()!=NULL?gLobbyData->mConfig->mCDKeyCheck:false; }
// (SDK LobbyMisc::IsCDKeyValid)
// FUNCTION: WON 0x0075b560
bool LobbyFn35B560() { return gLobbyData.get()!=NULL?gLobbyData->mCDKey.mValid:false; }
// SUPPORT-ONLY: genuine SDK CPP LobbyMisc::GetSysTOUPath; actual native20B, zero primary.
const std::string& LobbyFn35B630() { return gLobbyData.get()!=NULL?gLobbyData->mSysTOUPath:GetEmptyStr(); }
// SUPPORT-ONLY: genuine SDK CPP LobbyMisc::GetGameTOUPath; actual native20B, zero primary.
const std::string& LobbyFn35B650() { return gLobbyData.get()!=NULL?gLobbyData->mGameTOUPath:GetEmptyStr(); }
// (SDK LobbyMisc::GetNumPingsToDo)
// FUNCTION: WON 0x0075b940
int LobbyFn35B940() { return gLobbyData.get()!=NULL?gLobbyData->mPingLogic->Fn3E6B40():0; }
// (SDK LobbyDialog::UserCanceledLastStatusOp)
// FUNCTION: WON 0x00758f50
bool LobbyFn358F50() { return gLobbyData.get()!=NULL?gLobbyData->mDialogLogic->mStatusCancelled:false; }
}

namespace WONAPI {
// Genuine SDK LobbyDialog::DoDialog; original27B. Borrowed generic Component, no local ownership.
// FUNCTION: WON 0x00758e50
int LobbyDoDialog(Component* inDialog)
{
    return gLobbyData.get()!=NULL ? gLobbyData->mDialogLogic->DoDialog(inDialog) : -1;
}
// Genuine SDK LobbyDialog::DoErrorDialog; original28B.
// FUNCTION: WON 0x00758eb0
void LobbyMessageBox(const GUIString& inTitle,const GUIString& inText)
{
    if(gLobbyData.get()!=NULL) gLobbyData->mDialogLogic->ShowError(inTitle,inText);
}
// Genuine SDK LobbyDialog::DoYesNoDialog; original39B, cancel100002 without lobby.
// FUNCTION: WON 0x00758f20
int LobbyMessageBoxYesNo(const GUIString& inTitle,const GUIString& inText,bool inWarningSound)
{
    return gLobbyData.get()!=NULL ? gLobbyData->mDialogLogic->ShowYesNo(inTitle,inText,inWarningSound) : 100002;
}
// Genuine SDK LobbyMisc::RunOp; original24B. Existing canonical Run is native29f6d0/SDKRunAsync.
// Canonical RunAsync29f610 is the distinct timer operation and must not be substituted here.
// FUNCTION: WON 0x0075b5b0
void LobbyFn35B5B0(AsyncOp* inOp,unsigned long inTimeout)
{
    if(inTimeout==0) inTimeout=LobbyFn35B5A0();
    inOp->Run(static_cast<long>(inTimeout));
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0075b630
// (class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const & __cdecl WONAPI::LobbyFn35B630(void)    [?LobbyFn35B630)
// TEMPLATE: WON 0x0075b650
// (class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const & __cdecl WONAPI::LobbyFn35B650(void)    [?LobbyFn35B650)
