// Reviewed original-backed controls; source-only, near code/support remain carved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SCRATCH ONLY. Canonical InputBox/Dialog/event/persistence contracts incomplete.
// No compile or production authority. Provisional method names follow evidence.
#include <string>
#include <list>
#include <map>
#include <vector>
#include <windows.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
//3bf190,50B. Dialog base is real, pointers zero by SmartPtr defaults.
// FUNCTION: WON 0x007bf190
LoginCtrl::LoginCtrl() : mCanLogin(false) {}
//3bf1f0,261B. Reverse4 owned pointers, then Dialog's member94+Container.
// Implicit destructor support is generated from the actual class/vtable.
//3bf300..3bf38c,140B.
// FUNCTION: WON 0x007bf300
void LoginCtrl::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType == 2) {
        ComponentConfig* config = static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config, mUserNameCombo, "UserNameCombo", __FILE__, 31);
        SafeGetComponent(config, mPasswordInput, "PasswordInput", __FILE__, 32);
        SafeGetComponent(config, mRememberPasswordCheck, "RememberPasswordCheck", __FILE__, 33);
        SafeGetComponent(config, mLoginButton, "LoginButton", __FILE__, 34);
        UpdateLoginButton();
        Fn2F5360(1);
    }
}
//3bf390..3bf3f4,100B.
// FUNCTION: WON 0x007bf390
void LoginCtrl::UpdateLoginButton()
{
    mLoginButton->SetEnabled(mCanLogin && mUserNameCombo->mEdit->GetText().length() != 0 &&
        mPasswordInput->GetText().length() != 0);
}
//3bf400..3bf4c6,198B. Canonical typed map/record from persistence owner.
// FUNCTION: WON 0x007bf400
void LoginCtrl::FillUserNames()
{
    mUserNameCombo->Clear(true);
    const LobbyUserInfoMap& users=LobbyFn35CE60();
    long newest=0;
    ListItem* latest=NULL;
    for(LobbyUserInfoMap::const_iterator i=users.begin();i!=users.end();++i) {
        ListItem* item=mUserNameCombo->AddString(i->first,-1);
        if(i->second.mLastUseTime>newest) { newest=i->second.mLastUseTime;latest=item; }
    }
    if(latest!=NULL) {
        mUserNameCombo->SetSelItem(mUserNameCombo->mListArea->GetItemPos(latest),false);
        LoadPassword();
    }
}
//3bf4d0..3bf5e6,278B.
// FUNCTION: WON 0x007bf4d0
void LoginCtrl::LoadPassword()
{
    LobbyConfig* config = GetLobbyConfig();
    if (config != NULL && config->mPublic) {
        mPasswordInput->Clear(true);
        mRememberPasswordCheck->SetVisible(false);
        mRememberPasswordCheck->SetCheck(false,false);
        return;
    }
    GUIString password = LobbyFn35CEC0(mUserNameCombo->mEdit->GetText());
    mPasswordInput->SetText(password,true);
    mPasswordInput->SelectRange(0,-1);
    mRememberPasswordCheck->SetCheck(password.mData->mLength!=0,false);
    mRememberPasswordCheck->SetVisible(true);
}
//3bf5f0..3bf75e,366B.
// FUNCTION: WON 0x007bf5f0
void LoginCtrl::SavePassword()
{
    GUIString userName = mUserNameCombo->mEdit->GetText();
    GUIString password(GetEmptyGUIStringData());
    if (mRememberPasswordCheck->IsChecked())
        password=mPasswordInput->GetText();
    if (userName.length()!=0)
        LobbyFn35C910(userName,password,true);
}
//3bf760..3bf7a5,69B. Event exact RTTI naming/producer still pending; no allocation.
// FUNCTION: WON 0x007bf760
void LoginCtrl::SetCredentials(SetLoginFieldsEvent* inEvent)
{
    mUserNameCombo->SetText(inEvent->mUserName,false);
    mPasswordInput->SetText(inEvent->mPassword,false);
    mRememberPasswordCheck->SetCheck(false,false);
    UpdateLoginButton();
}
//3bf7b0..3bf807,87B.
// FUNCTION: WON 0x007bf7b0
void LoginCtrl::HandleLoginInitStatus(LoginInitStatusEvent* inEvent)
{
    if (inEvent->mKind==1) {
        mCanLogin=false;
        UpdateLoginButton();
        SetVisible(true);
    } else if (inEvent->mKind==2) {
        mCanLogin=true;
        UpdateLoginButton();
    } else if (inEvent->mKind==3)
        SetVisible(false);
}
//3bf810..3bfb23,787B; full caller allocation guard and8 EH states required.
// FUNCTION: WON 0x007bf810
void LoginCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch (inEvent->mType) {
    case 21:
        if (inEvent->mComponent==mUserNameCombo.get() &&
            mUserNameCombo->mEdit->GetText()[0]=='@' &&
            mPasswordInput->GetText().length()!=0)
            mPasswordInput->Clear(false);
        UpdateLoginButton();
        return;
    case 20:
        if (mLoginButton->IsDisabled()) {
            if (inEvent->mComponent==mUserNameCombo.get())
                mPasswordInput->Fn2F5890();
            else if (inEvent->mComponent==mPasswordInput.get())
                mUserNameCombo->Fn2F5890();
            return;
        }
        // Original event20 falls into the event15 source checks when enabled.
    case 15:
        if (inEvent->mComponent==mLoginButton.get()) {
            Fn2F53F0(new LoginEvent(mUserNameCombo->mEdit->GetText(),
                                  mPasswordInput->GetText(),
                                  mRememberPasswordCheck->mChecked!=0),false);
            return;
        } else if (inEvent->mComponent==mRememberPasswordCheck.get()) {
            SavePassword();
            return;
        }
        break;
    case 27:
        if (inEvent->mComponent==mUserNameCombo.get()) LoadPassword();
        return;
    case 100036:
        HandleLoginInitStatus(static_cast<LoginInitStatusEvent*>(inEvent));
        return;
    case 100051:
        SetCredentials(static_cast<SetLoginFieldsEvent*>(inEvent));
        return;
    case 100052:
        FillUserNames();
        return;
    }
    Dialog::HandleComponentEvent(inEvent);
}
}

/* Rejected source-kind candidate; zero handwritten credit.
LoginEvent::~LoginEvent() {}
Blocker: full original3bfb50..3bfc12 has no own4536ec vptr store, only
RefCount at3bfbfe. Keep implicit derived destructor; do not restore this body
without exact normal COFF evidence under independently calibrated flags.
*/

/* Rejected empty explicit destructor candidates; zero handwritten credit.
LoginCtrl::~LoginCtrl() {}
Blocker: complete original normal destructors contain no own derived-vptr store.
Implicit member/base destruction is the source candidate; require actual normal
COFF functions/relocations/EH actions before admission.
*/

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bfb50
// (public: virtual __thiscall WONAPI::LoginEvent::~LoginEvent(void)    [??1LoginEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bf1d0
// (public: virtual void * __thiscall WONAPI::LoginCtrl::`scalar deleting destructor'(unsigned int)    [??_GLoginCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bf1f0
// (public: virtual __thiscall WONAPI::LoginCtrl::~LoginCtrl(void)    [??1LoginCtrl@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007bfb30
// (public: virtual void * __thiscall WONAPI::LoginEvent::`scalar deleting destructor'(unsigned int)    [??_GLoginEvent@WONAPI@@UAEPAXI@Z])
