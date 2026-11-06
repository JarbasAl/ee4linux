// New R3 UpdateAccount whole source; actual ordinary-object order/EH/data review pending.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {

UpdateAccountCtrl::UpdateAccountCtrl() {}
// Original3c9d20..3c9eed,461B. Own-vptr3c9d3f; normal automatic cleanup definition.
// FUNCTION: WON 0x007c9d20
UpdateAccountCtrl::~UpdateAccountCtrl() {}
// Original3c9ef0..3ca078,392B.
// FUNCTION: WON 0x007c9ef0
void UpdateAccountCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(config,mUserNameInput,"UserNameInput",__FILE__,39);
    SafeGetComponent(config,mOldPasswordInput,"OldPasswordInput",__FILE__,40);
    SafeGetComponent(config,mPasswordInput,"PasswordInput",__FILE__,41);
    SafeGetComponent(config,mConfirmPasswordInput,"ConfirmPasswordInput",__FILE__,42);
    SafeGetComponent(config,mEmailInput,"EmailInput",__FILE__,43);
    SafeGetComponent(config,mNewsletterCheck,"NewsletterCheck",__FILE__,44);
    SafeGetComponent(config,mUserNameDesc,"UserNameDesc",__FILE__,45);
    SafeGetComponent(config,mOldPasswordDesc,"OldPasswordDesc",__FILE__,46);
    SafeGetComponent(config,mPasswordDesc,"PasswordDesc",__FILE__,47);
    SafeGetComponent(config,mConfirmPasswordDesc,"ConfirmPasswordDesc",__FILE__,48);
    SafeGetComponent(config,mEmailDesc,"EmailDesc",__FILE__,49);
    SafeGetComponent(config,mQueryButton,"QueryButton",__FILE__,50);
    SafeGetComponent(config,mUpdateButton,"UpdateButton",__FILE__,51);
    mUserNameInput->SetEnabled(false);
    CheckInput();
    Fn2F5360(1);
}
// Original3ca080..3ca096,22B, static cdecl. Condition copied before callback.
// FUNCTION: WON 0x007ca080
bool UpdateAccountCtrl::SetDesc(bool inCondition,Label* inLabel)
{
    inLabel->SetVisible(inCondition);
    return inCondition;
}
// Original3ca0a0..3ca243,419B. All receiver snapshots follow native evaluation order.
// FUNCTION: WON 0x007ca0a0
void UpdateAccountCtrl::CheckInput()
{
    bool error=false;
    bool queryAllowed=true;
    Label* oldDesc=mOldPasswordDesc.get();
    if(SetDesc(mOldPasswordInput->GetText()!=mActualOldPassword,oldDesc)) {
        error=true;
        queryAllowed=false;
    }
    bool shortPassword=mPasswordInput->GetText().length()!=0 && mPasswordInput->GetText().length()<4;
    if(SetDesc(shortPassword,mPasswordDesc.get())) error=true;
    Label* confirmDesc=mConfirmPasswordDesc.get();
    InputBox* confirm=mConfirmPasswordInput.get();
    const GUIString& password=mPasswordInput->GetText();
    if(SetDesc(password!=confirm->GetText(),confirmDesc)) error=true;
    bool badEmail=false;
    if(mEmailInput->GetText().length()!=0)
        badEmail=!LobbyFn3BA6A0(mEmailInput->GetText());
    if(SetDesc(badEmail,mEmailDesc.get())) error=true;
    mUpdateButton->SetEnabled(!error);
    mQueryButton->SetEnabled(queryAllowed);
}
// Original3ca250..3ca3df,399B. Requires canonical AuthContext name50/password60 proof.
// FUNCTION: WON 0x007ca250
void UpdateAccountCtrl::HandleEnteredScreen()
{
    AuthContext* context=GetLobbyAuthContext();
    if(context==NULL) return;
    mUserNameInput->SetText(GUIString(new GUIStringData(context->mName)),true);
    mOldPasswordInput->Clear(true);
    mPasswordInput->Clear(true);
    mConfirmPasswordInput->Clear(true);
    mEmailInput->Clear(true);
    mNewsletterCheck->SetCheck(false,false);
    mActualOldPassword=GUIString(new GUIStringData(context->mPassword));
}
// Original3ca3e0..3ca40f,47B. Reload newsletter receiver after SetText callback.
// FUNCTION: WON 0x007ca3e0
void UpdateAccountCtrl::HandleQueryAccountResult(QueryAccountResultEvent* inEvent)
{
    mEmailInput->SetText(inEvent->mEmail,true);
    mNewsletterCheck->SetCheck(inEvent->mNewsletter,false);
}
// Original3ca410..3ca5ce,446B. New event reads borrowed GUIString refs after rawnew.
// FUNCTION: WON 0x007ca410
void UpdateAccountCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 21: CheckInput(); return;
    case 15:
        if(inEvent->mComponent==mQueryButton.get()) {
            Fn2F5380(100040,false);
            return;
        }
        if(inEvent->mComponent==mUpdateButton.get()) {
            Fn2F53F0(new UpdateAccountEvent(mEmailInput->GetText(),mPasswordInput->GetText(),mNewsletterCheck->mChecked!=0),false);
        }
        return;
    case 100014: HandleEnteredScreen(); return;
    case 100041: HandleQueryAccountResult(static_cast<QueryAccountResultEvent*>(inEvent)); return;
    default: Component::HandleComponentEvent(inEvent); return;
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c9d00
// (public: virtual void * __thiscall WONAPI::UpdateAccountCtrl::`scalar deleting destructor'(unsigned int)    [??_GUpdateAccountCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007ca5d0
// (public: virtual void * __thiscall WONAPI::UpdateAccountEvent::`scalar deleting destructor'(unsigned int)    [??_GUpdateAccountEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007ca5f0
// (public: virtual __thiscall WONAPI::UpdateAccountEvent::~UpdateAccountEvent(void)    [??1UpdateAccountEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c9c40
// (public: __thiscall WONAPI::UpdateAccountCtrl::UpdateAccountCtrl(void)    [??0UpdateAccountCtrl@WONAPI@@QAE@XZ])
