// Reviewed original-backed controls, source-only; generated support has zeroquota.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SCRATCH whole original family. Source-kind, exact family flags and full COFF pending.
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
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
CreateAccountCtrl::CreateAccountCtrl() {} //3ba290,104B; empty ctor zeroquota
// Implicit destructor3ba320,455B; fourteen reverse owned releases then Container.
// Original3ba4f0..3ba67a,394B. Actual file lines30..43.
// FUNCTION: WON 0x007ba4f0
void CreateAccountCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mUserNameInput,"UserNameInput",__FILE__,30);
        SafeGetComponent(config,mUserNameDesc,"UserNameDesc",__FILE__,31);
        SafeGetComponent(config,mPasswordInput,"PasswordInput",__FILE__,32);
        SafeGetComponent(config,mPasswordDesc,"PasswordDesc",__FILE__,33);
        SafeGetComponent(config,mConfirmPasswordInput,"ConfirmPasswordInput",__FILE__,34);
        SafeGetComponent(config,mConfirmPasswordDesc,"ConfirmPasswordDesc",__FILE__,35);
        SafeGetComponent(config,mEmailInput,"EmailInput",__FILE__,36);
        SafeGetComponent(config,mEmailDesc,"EmailDesc",__FILE__,37);
        SafeGetComponent(config,mMonthInput,"MonthInput",__FILE__,38);
        SafeGetComponent(config,mDayInput,"DayInput",__FILE__,39);
        SafeGetComponent(config,mYearInput,"YearInput",__FILE__,40);
        SafeGetComponent(config,mBirthdateDesc,"BirthdateDesc",__FILE__,41);
        SafeGetComponent(config,mNewsletterCheck,"NewsletterCheck",__FILE__,42);
        SafeGetComponent(config,mCreateButton,"CreateButton",__FILE__,43);
        UpdateCreateButton();
    }
}
// Original3ba680..3ba698,24B. Receiver is intentionally unused.
// FUNCTION: WON 0x007ba680
bool CreateAccountCtrl::Fn3BA680(bool inVisible,Label* inLabel)
{
    inLabel->SetVisible(inVisible);
    return inVisible;
}
// Original3ba6a0..3ba78f,239B; shared call-only name is Core owner's contract.
// FUNCTION: WON 0x007ba6a0
bool LobbyFn3BA6A0(const std::string& inEmail)
{
    if(inEmail.empty()) return false;
    std::string::size_type at=inEmail.find('@',1);
    if(at==std::string::npos) return false;
    std::string::size_type dot=inEmail.find('.',at);
    if(dot==std::string::npos) return false;
    std::string::size_type length=inEmail.length();
    if(length>dot+1) return true;
    return false;
}
// Original3ba790..3ba800,112B, all three text/numeric reads occur before validation.
// FUNCTION: WON 0x007ba790
bool CreateAccountCtrl::Fn3BA790(InputBox* inMonth,InputBox* inDay,InputBox* inYear)
{
    int month=inMonth->GetText().mData->ToInt(0);
    int day=inDay->GetText().mData->ToInt(0);
    int year=inYear->GetText().mData->ToInt(0);
    if(month<=0||day<=0||year<=1889||month>12||day>31||year>1999) return false; return true;
}
// Original3ba800..3ba989,393B. Each validation executes, no boolean short-circuit chain.
// FUNCTION: WON 0x007ba800
void CreateAccountCtrl::UpdateCreateButton()
{
    bool invalid=false;
    if(Fn3BA680(mUserNameInput->GetText().length()<2,mUserNameDesc.get())) invalid=true;
    if(Fn3BA680(mPasswordInput->GetText().length()<4,mPasswordDesc.get())) invalid=true;
    {
        Label* desc=mConfirmPasswordDesc.get();
        InputBox* confirm=mConfirmPasswordInput.get();
        const GUIString& password=mPasswordInput->GetText();
        if(Fn3BA680(password!=confirm->GetText(),desc)) invalid=true;
    }
    bool emailInvalid;
    {
        const std::string& email=mEmailInput->GetText();
        Label* desc=mEmailDesc.get();
        emailInvalid=Fn3BA680(!LobbyFn3BA6A0(email),desc);
    }
    if(emailInvalid) invalid=true;
    if(Fn3BA680(!Fn3BA790(mMonthInput.get(),mDayInput.get(),mYearInput.get()),mBirthdateDesc.get())) invalid=true;
    mCreateButton->SetEnabled(!invalid);
}
// Original3ba990..3bac0f,639B; actual canonical CreateAccountEvent58.
// FUNCTION: WON 0x007ba990
void CreateAccountCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==21) { UpdateCreateButton(); return; }
    if(inEvent->mType==15&&inEvent->mComponent==mCreateButton.get()) {
        SmartPtr<CreateAccountEvent> event=new CreateAccountEvent;
        event->mUserName=mUserNameInput->GetText();
        event->mPassword=mPasswordInput->GetText();
        event->mEmail=mEmailInput->GetText();
        event->mDay=mDayInput->GetText().mData->ToInt(0);
        event->mMonth=mMonthInput->GetText().mData->ToInt(0);
        event->mYear=mYearInput->GetText().mData->ToInt(0);
        event->mNewsletter=mNewsletterCheck->mChecked;
        Fn2F53F0(event.get(),false);
    } else Component::HandleComponentEvent(inEvent);
}
}
// Event normal/deleting destructors3bac50/3bac30 are this producer's support.
// Adjacent3bad40 is an unrelated ServerDetailsItem factory, excluded here.
// Implicit source form requires full normal COFF proof;
// no handwritten quota. No empty explicit destructor or raw payload view added.

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007bac10
// (public: __thiscall WONAPI::LobbyEvent::LobbyEvent(int)    [??0LobbyEvent@WONAPI@@QAE@H@Z])
// TEMPLATE: WON 0x007bac50
// (public: virtual __thiscall WONAPI::CreateAccountEvent::~CreateAccountEvent(void)    [??1CreateAccountEvent@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007ba300
// (public: virtual void * __thiscall WONAPI::CreateAccountCtrl::`scalar deleting destructor'(unsigned int)    [??_GCreateAccountCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bac30
// (public: virtual void * __thiscall WONAPI::CreateAccountEvent::`scalar deleting destructor'(unsigned int)    [??_GCreateAccountEvent@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007ba320
// (public: virtual __thiscall WONAPI::CreateAccountCtrl::~CreateAccountCtrl(void)    [??1CreateAccountCtrl@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007ba290
// (public: __thiscall WONAPI::CreateAccountCtrl::CreateAccountCtrl(void)    [??0CreateAccountCtrl@WONAPI@@QAE@XZ])
