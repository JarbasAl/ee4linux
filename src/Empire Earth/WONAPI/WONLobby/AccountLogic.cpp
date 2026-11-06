// Original filename: C:\EEX\Empire Earth\WONAPI\WONLobby\AccountLogic.cpp.
// Provisional source-family subset: eight authored bodies,640 nominal original bytes.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// All eight primary ranges bind100 in the ordinary scoped-profile TU. This is
// source reconstruction only: the inactive constructor EH action has a receiver
// frame-offset difference, and generated SafeGet helpers retain string growth/guard
// differences. Preserve original-carved support; no whole-TU runtime authority.
// No partial asynchronous or private-core type is allocated by these active bodies.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyAccountLogic.h"
using namespace WONAPI;
namespace WONAPI {
// FUNCTION: WON 0x7db2a0
void AccountLogic::InitComponent(ComponentConfig* inConfig)
{
    SafeGetComponent(inConfig, mCreditsDialog, "CreditsDialog", __FILE__, 43);
    SafeGetComponent(inConfig, mTOUDialog, "TOUDialog", __FILE__, 44);
}
// FUNCTION: WON 0x7db150
AccountLogic::AccountLogic() { mNeedTOU = false; }
// FUNCTION: WON 0x7db1d0
AccountLogic::~AccountLogic() {}
// FUNCTION: WON 0x7db2e0
void AccountLogic::ShowCredits()
{
    mCreditsDialog->ResetScrolling();
    mCreditsDialog->StartScrolling();
    LobbyDoDialog(mCreditsDialog.get());
    mCreditsDialog->StopScrolling();
}
// FUNCTION: WON 0x7db310
bool AccountLogic::ShowTOU()
{
    mTOUDialog->Reset();
    return LobbyDoDialog(mTOUDialog.get()) == 1;
}
// FUNCTION: WON 0x7db330
bool AccountLogic::CheckLoginStatus()
{
    if (LobbyFn35B520() == -2814) {
        if (LobbyFn358EF0(::LobbyContainer_VersionOutOfDataTitle_String, ::LobbyContainer_VersionOutOfDataDetails_String, true) == 100001)
            LobbyFn3627F0(3);
        return false;
    }
    return true;
}
// FUNCTION: WON 0x7dc2e0
void AccountLogic::HandleControlEvent(int inControlId)
{
    switch (inControlId) {
    case 200001:
        if (CheckLoginStatus()) {
            LobbyFn3610D0();
            mNeedTOU = true;
        }
        break;
    case 200002:
        if (CheckLoginStatus()) LobbyFn3610F0();
        break;
    case 200003:
        if (CheckLoginStatus()) LobbyFn361110();
        break;
    }
}
// FUNCTION: WON 0x7dc340
void AccountLogic::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch (inEvent->mType) {
    case 100010: HandleCreateAccount(inEvent); break;
    case 100018: HandleLostPassword(inEvent); break;
    case 100019: HandleLostUsername(inEvent); break;
    case 100035: HandleLogin(inEvent); break;
    case 100040: QueryAccount(); break;
    case 100060: HandleUpdateAccount(inEvent); break;
    }
}
}

/*
Rejected/deferred source evidence below has ZERO implementation credit.
These two complete drafts remain inactive until the actual ServerOp94,
ServerRequestOpf4, DBProxyOp12c and request-operation144B layouts, virtual slots
and all owned lifetimes are admitted, together with canonical event/resource APIs.
No opaque buffers, size-only constructors, aliases or substitute allocations are
authorized. Other unreconstructed Account bodies remain original-carved: Create
3db370 (CreateAccountOp1398/ElGamal), Login3dba10 (GetCertOp11fc), Query3dbee0
and Update3dc160 (GetAccountOp118/UpdateAccountOp128).

// Complete deferred Account bodies. Scratch only; zero credit until full actual
// ServerOp/ServerRequestOp/DBProxyOp and request-operation144B contracts close.
// Original borrowed ServerContext is never retained as a separate local.
// 3db830,232B. Event identity/layout from controls producer3c0520/VT453a38.
void AccountLogic::HandleLostPassword(ComponentEvent* inEvent)
{
    ServerContext* account = LobbyFn35B1F0();
    if (account == NULL || !CheckLoginStatus())
        return;
    SmartPtr<RequestPasswordEmailOp> op = new RequestPasswordEmailOp(account);
    op->mUserName = static_cast<GetLostPasswordEvent*>(inEvent)->mUserName;
    LobbyFn358E70(op.get(), ::LobbyContainer_RetrievePasswordTitle_String, ::LobbyContainer_RetrievePasswordDetails_String, 0, false);
}
// 3db920,232B. Event identity/layout from controls producer3c0930/VT453b14.
void AccountLogic::HandleLostUsername(ComponentEvent* inEvent)
{
    ServerContext* account = LobbyFn35B1F0();
    if (account == NULL || !CheckLoginStatus())
        return;
    SmartPtr<RequestAccountInfoEmailOp> op = new RequestAccountInfoEmailOp(account);
    op->mEmail = static_cast<GetLostUserNameEvent*>(inEvent)->mEmail;
    LobbyFn358E70(op.get(), ::LobbyContainer_RetrieveUsernameTitle_String, ::LobbyContainer_RetrieveUsernameDetails_String, 0, false);
}

*/

// small-operation cohort; canonical admitted8 prefix preserved exactly.
#include "ee/wonapi/WONLobbyEvents.h"
namespace WONAPI {
// FUNCTION: WON 0x007db830
void AccountLogic::HandleLostPassword(ComponentEvent* inEvent)
{
    ServerContext* account = LobbyFn35B1F0();
    if (account == NULL || !CheckLoginStatus())
        return;
    SmartPtr<RequestPasswordEmailOp> op = new RequestPasswordEmailOp(account);
    op->mUserName = static_cast<GetLostPasswordEvent*>(inEvent)->mUserName;
    LobbyFn358E70(op.get(), ::LobbyContainer_RetrievePasswordTitle_String, ::LobbyContainer_RetrievePasswordDetails_String, 0, 0);
}
// FUNCTION: WON 0x007db920
void AccountLogic::HandleLostUsername(ComponentEvent* inEvent)
{
    ServerContext* account = LobbyFn35B1F0();
    if (account == NULL || !CheckLoginStatus())
        return;
    SmartPtr<RequestAccountInfoEmailOp> op = new RequestAccountInfoEmailOp(account);
    op->mEmail = static_cast<GetLostUserNameEvent*>(inEvent)->mEmail;
    LobbyFn358E70(op.get(), ::LobbyContainer_RetrieveUsernameTitle_String, ::LobbyContainer_RetrieveUsernameDetails_String, 0, 0);
}
// FUNCTION: WON 0x007dbee0
void AccountLogic::QueryAccount()
{
    ServerContext* account = LobbyFn35B1F0();
    AuthContext* auth = GetLobbyAuthContext();
    if (account == NULL || auth == NULL)
        return;
    SmartPtr<GetAccountOp> op = new GetAccountOp(account,auth);
    ProfileData* profile = op->mProfileData.get();
    profile->AddNewsletterSub(1,false);
    LobbyFn358E70(op.get(),::LobbyContainer_QueryAccountTitle_String,::LobbyContainer_QueryAccountDetails_String,1,0);
    if (op->GetStatus() == 0)
        LobbyFn359100(new QueryAccountResultEvent(GUIString(op->mEmail),
                                                  profile->GetNewsletterSub(1)),true);
}

// FUNCTION: WON 0x007dc160
void AccountLogic::HandleUpdateAccount(ComponentEvent* inEvent)
{
    ServerContext* account = LobbyFn35B1F0();
    AuthContext* auth = GetLobbyAuthContext();
    if (account == NULL || auth == NULL)
        return;
    SmartPtr<UpdateAccountOp> op = new UpdateAccountOp(account,auth);
    UpdateAccountEvent* event = static_cast<UpdateAccountEvent*>(inEvent);
    op->mEmail = event->mEmail;
    op->mNewPassword = event->mNewPassword;
    op->mProfileData->AddNewsletterSub(1,event->mNewsletter);
    LobbyFn358E70(op.get(),::LobbyContainer_UpdateAccountTitle_String,::LobbyContainer_UpdateAccountDetails_String,0,0);
}

} //WONAPI

// complete actual CreateAccountOp1398 consumer; other deferred bodies remain original-owned.
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/EELobbyGameCode.h" //genuine LobbyMessageBox call declaration
namespace WONAPI {
// FUNCTION: WON 0x007db370
void AccountLogic::HandleCreateAccount(ComponentEvent* inBaseEvent)
{
    if(!CheckLoginStatus()) return;
    if(mNeedTOU) {
        if(!ShowTOU()) return;
        mNeedTOU=false;
    }
    ServerContext* servers=LobbyFn35B1F0();
    if(!servers) return;
    SmartPtr<CreateAccountOp> op=new CreateAccountOp(servers);
    CreateAccountEvent* event=static_cast<CreateAccountEvent*>(inBaseEvent);
    op->SetUserName(event->mUserName);
    op->SetPassword(event->mPassword);
    op->SetEmail(event->mEmail);
    SmartPtr<ProfileData> profile=new ProfileData;
    profile->AddNewsletterSub(1,event->mNewsletter);
    profile->SetBirthDate(event->mMonth,event->mDay,event->mYear);
    op->SetProfileData(profile.get());
    LobbyFn358E70(op.get(),::LobbyContainer_CreateAccountTitle_String,::LobbyContainer_CreateAccountDetails_String,0,0);
    if(op->GetStatus()!=0) return;
    LobbyUserInfo* user;
    {
        GUIString emptyPassword(new GUIStringData(""));
        GUIString userName(new GUIStringData(event->mUserName));
        user=LobbyFn35C910(userName,emptyPassword,false);
    }
    if(user) LobbyFn35B600(user->mValue04,user->mValue08);
    LobbyFn35E660(true);
    LobbyFn359120(100052,true);
    LobbyFn359100(new SetLoginFieldsEvent(GUIString(event->mUserName),
        GUIString(event->mPassword),false),true);
}
}

// complete login workflow using actual external-owned GetCertOp11fc.
namespace WONAPI {
// FUNCTION: WON 0x007dba10
void AccountLogic::HandleLogin(ComponentEvent* inBaseEvent)
{
    AuthContext* auth=GetLobbyAuthContext();
    Window* window=LobbyFn361230();
    if(!auth || !window) return;
    int keyMod=window->mKeyMod;
    if((keyMod&7)==7) { ShowCredits();return; }
    LoginEvent* event=static_cast<LoginEvent*>(inBaseEvent);
    if(event->mUserName.empty() || !CheckLoginStatus()) return;
    bool admin=false;
    if(event->mUserName[0]==L'@') {
        admin=true;
        event->mUserName=event->mUserName.substr(1);
    }
    if(WStringCompareNoCase(event->mUserName,mLastUserName)!=0) {
        LobbyUserInfo* user;
        {
            GUIString name(new GUIStringData(event->mUserName));
            user=LobbyFn35CF20(name);
        }
        if((!user || LobbyFn35B670(user->mValue04,user->mValue08)) && !ShowTOU()) return;
        mLastUserName=event->mUserName;
    }
    auth->SetUserName(event->mUserName);
    auth->SetPassword(event->mPassword);
    if(LobbyFn35B540() && !LobbyFn35B560()) {
        LobbyMessageBox(::AccountLogic_InvalidCDKeyTitle_String,::AccountLogic_InvalidCDKeyDetails_String);
        return;
    }
    if(admin) auth->AddCommunity(std::wstring(L"Admin"));
    else auth->RemoveCommunity(std::wstring(L"Admin"));
    SmartPtr<GetCertOp> op=new GetCertOp(auth);
    LobbyFn358E70(op.get(),::LobbyContainer_LoginTitle_String,::LobbyContainer_LoginDetails_String,1,0);
    if(op->GetStatus()!=0) return;
    auth->Fn2DEB30(true);
    GUIString password(GetEmptyGUIStringData());
    if(event->mRemember) password=GUIString(new GUIStringData(event->mPassword));
    LobbyUserInfo* user;
    {
        GUIString name(new GUIStringData(event->mUserName));
        user=LobbyFn35C910(name,password,false);
    }
    if(user) {
        LobbyFn35B600(user->mValue04,user->mValue08);
        LobbyFn35E660(true);
        LobbyFn35CF90(user);
    }
    LobbyFn359120(100052,true);
    LobbyFn361ED0((keyMod&4)==0);
}
}

// SafeGetComponent COMDATs this unit references first (placed after the last logic unit)
// TEMPLATE: WON 0x007f30d0
// TEMPLATE: WON 0x007f3250

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007db1b0
// (public: virtual void * __thiscall WONAPI::AccountLogic::`scalar deleting destructor'(unsigned int)    [??_GAccountLogic@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007db760
// (public: virtual void * __thiscall WONAPI::SetLoginFieldsEvent::`scalar deleting destructor'(unsigned int)    [??_GSetLoginFieldsEvent@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x007db780
// (public: virtual __thiscall WONAPI::SetLoginFieldsEvent::~SetLoginFieldsEvent(void)    [??1SetLoginFieldsEvent@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007dc0b0
// (public: virtual void * __thiscall WONAPI::QueryAccountResultEvent::`scalar deleting destructor'(unsigned int)    [??_GQueryAccountResultEvent@WONAPI@@)
// TEMPLATE: WON 0x007dc0d0
// (public: virtual __thiscall WONAPI::QueryAccountResultEvent::~QueryAccountResultEvent(void)    [??1QueryAccountResultEvent@WONAPI@@UAE@XZ])
