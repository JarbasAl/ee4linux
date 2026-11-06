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
#include "ee/wonapi/WONLobbyControls.h"
#include "ee/wonapi/WONLobbyDialogs.h"
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "LostDebug.h"
using namespace WONAPI;


// FUNCTION: WON 0x00763300
static Component* CreateAccountCtrlFactory()				{ return new CreateAccountCtrl; }
// FUNCTION: WON 0x00763360
static Component* DirectConnectCtrlFactory()				{ return new DirectConnectCtrl; }
// FUNCTION: WON 0x007633c0
static Component* GameStagingCtrlFactory()					{ return new GameStagingCtrl; }
// FUNCTION: WON 0x00763420
static Component* InitFailedCtrlFactory()					{ return new InitFailedCtrl; }
// FUNCTION: WON 0x00763480
static Component* InternetOptionsCtrlFactory()				{ return new LobbyOptionsCtrl(false); }
// FUNCTION: WON 0x007634e0
static Component* InternetScreenFactory()					{ return new RootScreen(false); }
// FUNCTION: WON 0x00763540
static Component* LanOptionsCtrlFactory()					{ return new LobbyOptionsCtrl(true); }
// FUNCTION: WON 0x007635a0
static Component* LanScreenFactory()						{ return new RootScreen(true); }
// FUNCTION: WON 0x00763600
static Component* LoginCtrlFactory()						{ return new LoginCtrl; }
// FUNCTION: WON 0x00763660
static Component* LoginHelpCtrlFactory()					{ return new LoginHelpCtrl; }
// FUNCTION: WON 0x007636c0
static Component* LoginScreenFactory()						{ return new LoginScreen; }
// FUNCTION: WON 0x00763720
static Component* LoginStatusCtrlFactory()					{ return new LoginStatusCtrl; }
// FUNCTION: WON 0x00763780
static Component* LostPasswordCtrlFactory()				{ return new LostPasswordCtrl; }
// FUNCTION: WON 0x007637e0
static Component* LostUserNameCtrlFactory()				{ return new LostUserNameCtrl; }
// FUNCTION: WON 0x00763840
static Component* MOTDCtrlFactory()						{ return new MOTDCtrl; }
// FUNCTION: WON 0x007638a0
static Component* QuickPlayCtrlFactory()					{ return new QuickPlayCtrl; }
// FUNCTION: WON 0x00763900
static Component* RoomCtrlFactory()						{ return new RoomCtrl; }
// FUNCTION: WON 0x00763960
static Component* UpdateAccountCtrlFactory()				{ return new UpdateAccountCtrl; }

// FUNCTION: WON 0x007639c0
static Component* AddFriendDialogFactory()					{ return new AddFriendDialog; }
// FUNCTION: WON 0x00763a20
static Component* AdminActionDialogFactory()				{ return new AdminActionDialog; }
// FUNCTION: WON 0x00763a80
static Component* BadUserDialogFactory()					{ return new BadUserDialog; }
// FUNCTION: WON 0x00763ae0
static Component* ChooseNetworkAdapterDialogFactory()		{ return new ChooseNetworkAdapterDialog; }
// FUNCTION: WON 0x00763b40
static Component* CreateGameDialogFactory()				{ return new CreateGameDialog; }
// FUNCTION: WON 0x00763ba0
static Component* CreateRoomDialogFactory()				{ return new CreateRoomDialog; }
// FUNCTION: WON 0x00763c00
static Component* CreditsDialogFactory()					{ return new CreditsDialog; }
// FUNCTION: WON 0x00763c60
static Component* DirectConnectDialogFactory()				{ return new DirectConnectDialog; }
// FUNCTION: WON 0x00763cc0
static Component* LanNameDialogFactory()					{ return new LanNameDialog; }
// FUNCTION: WON 0x00763d20
static Component* LanPortDialogFactory()					{ return new LanPortDialog; }
// FUNCTION: WON 0x00763d80
static Component* OkCancelDialogFactory()					{ return new OkCancelDialog; }
// FUNCTION: WON 0x00763de0
static Component* PasswordDialogFactory()					{ return new PasswordDialog; }
// FUNCTION: WON 0x00763e40
static Component* RoomDialogFactory()						{ return new RoomDialog; }
// FUNCTION: WON 0x00763ea0
static Component* StatusDialogFactory()					{ return new StatusDialog; }
// FUNCTION: WON 0x00763f00
static Component* TOUDialogFactory()						{ return new TOUDialog; }

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00763f60
static ResourceConfigTablePtr GetLobbyResourceConfigTable(ResourceConfigTable *theTable)
{
	ResourceConfigTablePtr aTable = LobbyResource_Init(theTable);
	WONGUIConfig_Init(aTable.get());	
	BrowserComponentConfig::Init(aTable.get());

	aTable->AddAllocator("ChatCtrl",ChatCtrlConfig::CfgFactory,ChatCtrlConfig::CompFactory);
	aTable->AddAllocator("CreateAccountCtrl",ContainerConfig::CfgFactory,CreateAccountCtrlFactory);
	aTable->AddAllocator("DirectConnectCtrl",ContainerConfig::CfgFactory,DirectConnectCtrlFactory);
	aTable->AddAllocator("FriendsListCtrl",FriendsListCtrl::CfgFactory,FriendsListCtrl::CompFactory);
	aTable->AddAllocator("GameBrowserCtrl",GameBrowserCtrlConfig::CfgFactory,GameBrowserCtrlConfig::CompFactory);
	aTable->AddAllocator("GameStagingCtrl",ContainerConfig::CfgFactory,GameStagingCtrlFactory);
	aTable->AddAllocator("GameUserListCtrl",GameUserListCtrlConfig::CfgFactory,GameUserListCtrlConfig::CompFactory);
	aTable->AddAllocator("InitFailedCtrl",ContainerConfig::CfgFactory,InitFailedCtrlFactory);
	aTable->AddAllocator("InternetOptionsCtrl",ContainerConfig::CfgFactory,InternetOptionsCtrlFactory);
	aTable->AddAllocator("InternetScreen",ContainerConfig::CfgFactory,InternetScreenFactory);
	aTable->AddAllocator("LanOptionsCtrl",ContainerConfig::CfgFactory,LanOptionsCtrlFactory);
	aTable->AddAllocator("LanScreen",ContainerConfig::CfgFactory,LanScreenFactory);
	aTable->AddAllocator("LoginCtrl",DialogConfig::CfgFactory,LoginCtrlFactory);
	aTable->AddAllocator("LoginHelpCtrl",ContainerConfig::CfgFactory,LoginHelpCtrlFactory);	
	aTable->AddAllocator("LoginScreen",ContainerConfig::CfgFactory,LoginScreenFactory);
	aTable->AddAllocator("LoginStatusCtrl",ContainerConfig::CfgFactory,LoginStatusCtrlFactory);
	aTable->AddAllocator("LostPasswordCtrl",ContainerConfig::CfgFactory,LostPasswordCtrlFactory);
	aTable->AddAllocator("LostUserNameCtrl",ContainerConfig::CfgFactory,LostUserNameCtrlFactory);
	aTable->AddAllocator("MOTDCtrl",ContainerConfig::CfgFactory,MOTDCtrlFactory);
	aTable->AddAllocator("QuickPlayCtrl",DialogConfig::CfgFactory,QuickPlayCtrlFactory);
	aTable->AddAllocator("RoomCtrl",ContainerConfig::CfgFactory,RoomCtrlFactory);
	aTable->AddAllocator("RoomLabelCtrl",RoomLabelCtrlConfig::CfgFactory,RoomLabelCtrlConfig::CompFactory);
	aTable->AddAllocator("ServerListCtrl",ServerListCtrlConfig::CfgFactory,ServerListCtrlConfig::CompFactory);
	aTable->AddAllocator("UpdateAccountCtrl",ContainerConfig::CfgFactory,UpdateAccountCtrlFactory);
	aTable->AddAllocator("UserListCtrl",UserListCtrlConfig::CfgFactory,UserListCtrlConfig::CompFactory);

	aTable->AddAllocator("AddFriendDialog",DialogConfig::CfgFactory,AddFriendDialogFactory);
	aTable->AddAllocator("AdminActionDialog",DialogConfig::CfgFactory,AdminActionDialogFactory);
	aTable->AddAllocator("BadUserDialog",DialogConfig::CfgFactory,BadUserDialogFactory);
	aTable->AddAllocator("ChooseNetworkAdapterDialog",DialogConfig::CfgFactory,ChooseNetworkAdapterDialogFactory);
	aTable->AddAllocator("CreditsDialog",CreditsDialogConfig::CfgFactory,CreditsDialogFactory);
	aTable->AddAllocator("CreateGameDialog",DialogConfig::CfgFactory,CreateGameDialogFactory);
	aTable->AddAllocator("CreateRoomDialog",DialogConfig::CfgFactory,CreateRoomDialogFactory);
	aTable->AddAllocator("DirectConnectDialog",DialogConfig::CfgFactory,DirectConnectDialogFactory);
	aTable->AddAllocator("LanNameDialog",DialogConfig::CfgFactory,LanNameDialogFactory);
	aTable->AddAllocator("LanPortDialog",DialogConfig::CfgFactory,LanPortDialogFactory);
	aTable->AddAllocator("OkCancelDialog",DialogConfig::CfgFactory,OkCancelDialogFactory);
	aTable->AddAllocator("PasswordDialog",DialogConfig::CfgFactory,PasswordDialogFactory);
	aTable->AddAllocator("RoomDialog",DialogConfig::CfgFactory,RoomDialogFactory);
	aTable->AddAllocator("StatusDialog",DialogConfig::CfgFactory,StatusDialogFactory);
	aTable->AddAllocator("TOUDialog",DialogConfig::CfgFactory,TOUDialogFactory);


	aTable->AddAllocator("CrossPromotionButton",CrossPromotionButtonConfig::CfgFactory,CrossPromotionButtonConfig::CompFactory);
	aTable->AddAllocator("ImageTextButton",ImageTextButtonConfig::CfgFactory,ImageTextButtonConfig::CompFactory);
	aTable->AddAllocator("MaxSplitterButton",MaxSplitterButtonConfig::CfgFactory,MaxSplitterButtonConfig::CompFactory);
	// LOST-DEBUG: none: exact only
	EE_LOST_DEBUG(3);

	aTable->AddAllocator("GameOptionsCtrl",ContainerConfig::CfgFactory,ContainerConfig::CompFactory);
	aTable->AddAllocator("PlayerOptionsCtrl",ContainerConfig::CfgFactory,ContainerConfig::CompFactory);

	return aTable;
}


namespace WONAPI {
// GetLobbyResourceConfigTable is the file-local function above.

// FUNCTION: WON 0x00765ac0
bool WONLobbyPrv::Init(ResourceConfigTable* input,void (*initFunc)(ComponentConfig*))
{
    mInitError=GetEmptyGUIStringData();
    ConfigParser parser;
    parser.mWarningsAsErrors=true;
    if(!parser.OpenFile(WONAPICore::GetDefaultFileDirectory()+LobbyConfig::mDefaultFileName)) {
        mInitError=GUIString("Couldn't find "+LobbyConfig::mDefaultFileName);
        return false;
    }
    if(!mConfig->ParseTopLevel(parser,NULL)) {
        mInitError=GUIString(LobbyConfig::mDefaultFileName+" error: ");
        mInitError.append(GUIString(parser.GetAbortReason()));
        return false;
    }
    mLobbyPort=mConfig->mLobbyPort;
    SmartPtr<ContainerConfig> componentConfig=new ContainerConfig;
    mLobbyContainer=new LobbyContainer;
    ComponentConfig::SetResourceFolder(WONAPICore::GetDefaultFileDirectory()+mConfig->mSC8);
    SmartPtr<ResourceConfigTable> resources=GetLobbyResourceConfigTable(input);
    ServerListCtrl::InitColumnMap();
    std::map<std::string,std::string>::iterator itr=mConfig->mMEC.begin();
    while(itr!=mConfig->mMEC.end()) {
        parser.AddDefine(itr->first,itr->second,false);
        ++itr;
    }
    if(!componentConfig->ReadConfig_Unsafe(mLobbyContainer.get(),"_LobbyMaster.cfg",resources.get(),&parser)) {
        mInitError=componentConfig->mConfigError;
        return false;
    }
    try {
        mLogicMgr.Init(componentConfig.get());
        if(initFunc!=NULL) initFunc(componentConfig.get());
    }
    catch(ConfigObjectException& error) {
        mInitError=GUIString(error.mText);
        return false;
    }
    CRC16 crc;
    crc.Put(mConfig->mS78.data(),mConfig->mS78.length());
    crc.Put(mConfig->mS98.data(),mConfig->mS98.length());
    mProductId=crc.Get();
    crc.Reset();
    crc.Put(mConfig->mS78.data(),mConfig->mS78.length());
    crc.Put(mConfig->mSA8.data(),mConfig->mSA8.length());
    mLanProductId=crc.Get();
    mChatCommandParser->InitCommands();
    return true;
}

// FUNCTION: WON 0x007660f0
void WONLobbyPrv::Destroy()
{
    mLogicMgr.Kill();
    if(mUDPManager.get()!=NULL) {
        mUDPManager->Close();
        mUDPManager=NULL;
    }
    if(mAuthContext.get()!=NULL) {
        mAuthContext->Kill();
        mAuthContext=NULL;
    }
}

// Additional native WONLobbyPrv method; append only after Root leases new RVA.
// FUNCTION: WON 0x00766010
void WONLobbyPrv::InitAuth(const std::wstring& loginCommunity,const char* hashFileListStr)
{
    mHasSetHashFileList=false;
    mHashFileList.clear();
    if(hashFileListStr!=NULL) {
        StringParser parser(hashFileListStr);
        parser.ReadStrings(mHashFileList);
    }
    mLoginCommunity=loginCommunity;
    mAuthContext->SetCommunity(mLoginCommunity);
    if(mConfig->mCDKeyCheck) {
        mCDKey.SetProductString(mConfig->mS78);
        mCDKey.LoadFromRegistry(2,gCDKeyRegistryKey);
        mAuthContext->SetCDKey(mLoginCommunity,mCDKey);
    }
}


// Complete original orchestration; requires Root's actual child-owner allocation grants.
// No member initialization is invented for fields the native constructor leaves untouched.
// FUNCTION: WON 0x007628c0
WONLobbyPrv::WONLobbyPrv()
{
    mLobbyPort=8888;
    mSysTOUTime=mGameTOUTime=0;
    mBehindFirewall=false;
    mFirewallTestDone=false;
    mSuspended=false;
    mCurGameType=(LobbyGameType)0;
    mLobbyStatus=(LobbyStatus)0;
    mRunMode=(LobbyRunMode)1;
    mProductId=0;
    mLanProductId=0;

    mSysTOUPath=WONAPICore::GetDefaultFileDirectory()+"_wonsystou_m.txt";
    mGameTOUPath=WONAPICore::GetDefaultFileDirectory()+"_wongametou_m.txt";

    mConfig=LobbyConfig::mAllocator();
    mPersistentData=new LobbyPersistentData;
    mChatCommandParser=new ChatCommandParser;
    mUDPManager=new UDPManager;
    mAccountServers=new ServerContext;
    mAuthServers=new ServerContext;
    mDirServers=new ServerContext;
    mRoomDirServers=new ServerContext;
    mFirewallDetectServers=new ServerContext;
    mPatchServers=new ServerContext;
    mVersionStatus=100004;
    mAuthContext=new AuthContext;
    mAuthContext->SetServerContext(mAuthServers.get());

    mAccountLogic=new AccountLogic;
    mDialogLogic=new DialogLogic;
    mInitLogic=new InitLogic;
    mRoutingLogic=new RoutingLogic;
    mStagingLogic=new StagingLogic;
    mLanLogic=new LanLogic;
    mPingLogic=new PingLogic;

    mLogicMgr.AddLogic(mAccountLogic.get());
    mLogicMgr.AddLogic(mDialogLogic.get());
    mLogicMgr.AddLogic(mStagingLogic.get());
    mLogicMgr.AddLogic(mInitLogic.get());
    mLogicMgr.AddLogic(mLanLogic.get());
    mLogicMgr.AddLogic(mRoutingLogic.get());
    mLogicMgr.AddLogic(mPingLogic.get());
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00763080
// (public: virtual void * __thiscall WONAPI::WONLobbyPrv::`scalar deleting destructor'(unsigned int)    [??_GWONLobbyPrv@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00765180
// (public: static class WONAPI::Component * __cdecl WONAPI::ChatCtrlConfig::CompFactory(void)    [?CompFactory@ChatCtrlConfig@WONAPI@@SAPAVComponent@2@XZ)
// TEMPLATE: WON 0x00765520
// (public: static class WONAPI::Component * __cdecl WONAPI::GameUserListCtrlConfig::CompFactory(void)    [?CompFactory@GameUserListCtrlConfig@WONAPI@@SAP)
// TEMPLATE: WON 0x00765580
// (public: static class WONAPI::Component * __cdecl WONAPI::RoomLabelCtrlConfig::CompFactory(void)    [?CompFactory@RoomLabelCtrlConfig@WONAPI@@SAPAVComp)
// TEMPLATE: WON 0x007659a0
// (public: static class WONAPI::Component * __cdecl WONAPI::CrossPromotionButtonConfig::CompFactory(void)    [?CompFactory@CrossPromotionButtonConfig@WON)
// TEMPLATE: WON 0x00765a00
// (public: static class WONAPI::Component * __cdecl WONAPI::UserListCtrlConfig::CompFactory(void)    [?CompFactory@UserListCtrlConfig@WONAPI@@SAPAVCompon)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00765260
// (public: static class WONAPI::ComponentConfig * __cdecl WONAPI::CreditsDialogConfig::CfgFactory(void)    [?CfgFactory@CreditsDialogConfig@WONAPI@@SAPAV)
// TEMPLATE: WON 0x00765380
// (public: static class WONAPI::Component * __cdecl WONAPI::GameBrowserCtrlConfig::CompFactory(void)    [?CompFactory@GameBrowserCtrlConfig@WONAPI@@SAPAV)
// TEMPLATE: WON 0x007656a0
// (public: static class WONAPI::Component * __cdecl WONAPI::ServerListCtrlConfig::CompFactory(void)    [?CompFactory@ServerListCtrlConfig@WONAPI@@SAPAVCo)
// TEMPLATE: WON 0x00765760
// (public: static class WONAPI::Component * __cdecl WONAPI::ImageTextButtonConfig::CompFactory(void)    [?CompFactory@ImageTextButtonConfig@WONAPI@@SAPAV)
