// Reviewed original-backed controls; source-only, near code/support remain carved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233

#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/EELobbySound.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
//3be930,56B plus separate20B jump table; source-level free helper.
// FUNCTION: WON 0x007be930
static int GetConnectionSpeed(int inSelection)
{
    if (inSelection<0) return 5;
    switch (inSelection) {
    case 0:return 5;
    case 1:return 15;
    case 2:return 25;
    case 3:return 35;
    case 4:return 45;
    default:return 55;
    }
}
//3bef30,36B.
// FUNCTION: WON 0x007bef30
static int GetConnectionSpeedIndex(int inSpeed)
{
    int i;
    for (i=1;i<6;i++)
        if (GetConnectionSpeed(i)>inSpeed) break;
    return i-1;
}
//3bdfc0,140B; real18 owning pointer defaults, bool is ctor arg.
// FUNCTION: WON 0x007bdfc0
LobbyOptionsCtrl::LobbyOptionsCtrl(bool inIsLan) { mIsLan = inIsLan; }
//3be070,490B; reverse18 pointers, then Container;18EHstates required.
LobbyOptionsCtrl::~LobbyOptionsCtrl() {} //ownVTstore3be08f; zeroquota,normalCOFFpending
//3be260,1490B,11states. Local button owns Reset/Restore/Remove in turn.
// FUNCTION: WON 0x007be260
void LobbyOptionsCtrl::InitComponent(ComponentInit& inInit)
{
    if (inInit.mType==2) {
        SmartPtr<Button> button;
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mChatSoundEffectsCheck,"ChatSoundEffectsCheck",2,__FILE__,95);
        SafeGetComponent(config,mConnectionSpeedCombo,"ConnectionSpeedCombo",__FILE__,96);
        SafeGetComponent(config,mCrossPromotionCheck,"CrossPromotionCheck",3,__FILE__,97);
        SafeGetComponent(config,mFirewalledIcon,"FirewalledIcon",__FILE__,98);
        SafeGetComponent(config,mLobbyMusicCheck,"LobbyMusicCheck",5,__FILE__,99);
        SafeGetComponent(config,mLobbySoundEffectsCheck,"LobbySoundEffectsCheck",6,__FILE__,100);
        SafeGetComponent(config,mLocalAddressDetailLabel,"LocalAddressDetailLabel",__FILE__,101);
        SafeGetComponent(config,mPublicAddressDetailLabel,"PublicAddressDetailLabel",__FILE__,102);
        SafeGetComponent(config,button,"ResetFiltersButton",8,__FILE__,103);
        SafeGetComponent(config,button,"RestoreDefaultsButton",9,__FILE__,104);
        SafeGetComponent(config,mShowConfirmationsCheck,"ShowConfirmationsCheck",10,__FILE__,105);
        SafeGetComponent(config,mShowGamesInProgressCheck,"ShowGamesInProgressCheck",11,__FILE__,106);
        SafeGetComponent(config,mStagingSoundEffectsCheck,"StagingSoundEffectsCheck",12,__FILE__,107);
        SafeGetComponent(config,mTabSelectCombo,"TabSelectCombo",__FILE__,108);
        if (mIsLan) {
            SafeGetComponent(config,mLANBroadcastDetailLabel,"LANBroadcastDetailLabel",__FILE__,112);
            SafeGetComponent(config,button,"RemoveLanNameButton",7,__FILE__,113);
            SafeGetComponent(config,mUserNameCombo,"UserNameCombo",__FILE__,114);
        } else {
            SafeGetComponent(config,mAnonymousToFriendsCheck,"AnonymousToFriendsCheck",0x30d56,__FILE__,118);
            SafeGetComponent(config,mAutoPingGamesCheck,"AutoPingGamesCheck",1,__FILE__,119);
            SafeGetComponent(config,mDoAutoAFKCheck,"DoAutoAFKCheck",4,__FILE__,120);
            SafeGetComponent(config,mFilterDirtyWordsCheck,"FilterDirtyWordsCheck",0x30d55,__FILE__,121);
        }
        mLocalAddressDetailLabel->SetText(GUIString(IPAddr::GetLocalAddr().GetHostString(true)),true);
        Fn2F5360(1);
    }
}
//3be840,239B.
// FUNCTION: WON 0x007be840
void LobbyOptionsCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 27:
        if (inEvent->mComponent==mTabSelectCombo.get()) {
            LobbyFn35E470(mTabSelectCombo->mEdit->GetText());return;
        } else if(inEvent->mComponent==mConnectionSpeedCombo.get()) {
            LobbyFn35E600(GetConnectionSpeed(mConnectionSpeedCombo->GetSelItem()));return;
        } else if(inEvent->mComponent==mUserNameCombo.get()) {
            LobbyFn35D6C0(mUserNameCombo->mEdit->GetText());return;
        }
        break;
    case 15:
        if(HandleButton(inEvent->mComponent)) return;
        break;
    case 100014: SyncOptions();return;
    case 100050: RefreshLanOptions();return;
    case 100055: RefreshPublicAddress();return;
    }
    Component::HandleComponentEvent(inEvent);
}
//3bec80,368B. Actual map<GUIString,long,NoCase> type from core owner; canonical gate pending.
// FUNCTION: WON 0x007bec80
void LobbyOptionsCtrl::RefreshLanOptions() {
    if(mIsLan) {
        mUserNameCombo->Clear(true);
        const LobbyLanNameInfoMap& names=LobbyFn35D660();
        for(LobbyLanNameInfoMap::const_iterator i=names.begin();i!=names.end();++i)
            mUserNameCombo->AddString(i->first,-1);
        ComboBox* combo=mUserNameCombo.get(); //original ESI snapshot3bed24 before getter
        combo->SetTextAndSel(LobbyFn35D640(),true);
        char port[20]; //original20-byte local extent
        sprintf(port,"%d",LobbyFn35E3C0());
        mLANBroadcastDetailLabel->SetText(GUIString(new GUIStringData(port)),true);
    }
}
//3bedf0,311B. Notification booleans are original-specific, do not normalize.
// FUNCTION: WON 0x007bedf0
void LobbyOptionsCtrl::RefreshOptions()
{
    if(mIsLan) RefreshLanOptions();
    else {
        mFilterDirtyWordsCheck->SetCheck(LobbyFn35E2F0(),false);
        mDoAutoAFKCheck->SetCheck(LobbyFn35E300(),false);
        mAnonymousToFriendsCheck->SetCheck(LobbyFn35E390(),true);
        mAutoPingGamesCheck->SetCheck(LobbyFn35E370(),true);
    }
    mChatSoundEffectsCheck->SetCheck(LobbyFn35E330(),true);
    mConnectionSpeedCombo->SetSelItem(GetConnectionSpeedIndex(LobbyFn35E3A0()),false);
    mTabSelectCombo->SetTextAndSel(LobbyFn35E2D0(),false);
    mLobbyMusicCheck->SetCheck(LobbyFn35E350(),true);
    mLobbySoundEffectsCheck->SetCheck(LobbyFn35E320(),true);
    mShowConfirmationsCheck->SetCheck(LobbyFn35E360(),true);
    mCrossPromotionCheck->SetCheck(LobbyFn35E310(),true);
    mShowGamesInProgressCheck->SetCheck(LobbyFn35E380(),true);
    mStagingSoundEffectsCheck->SetCheck(LobbyFn35E340(),true);
}
//3bef60,34B.
// FUNCTION: WON 0x007bef60
void LobbyOptionsCtrl::SyncOptions()
{
    if(!LobbyFn361170() && LobbyFn361150()==mIsLan) RefreshOptions();
}
//3be980 HandleButton and3bef90 RefreshPublicAddress pending actual Sound/Image

}
// DEFERRED COMPLETE BODY 3be980,715B +48B table, zero admitted credit.
// Root G closes Sound globals/Image/widget types and Component::Fn2F52D0;
// root has approved Sound wrapper declarations. Remaining blockers: canonical
// PData LANmap/reset calls, own source gate and normal fullTU COFF capture.
namespace WONAPI {
// FUNCTION: WON 0x007be980
bool LobbyOptionsCtrl::HandleButton(Component* inComponent)
{
    if(inComponent==NULL) return false;
    Button* button=dynamic_cast<Button*>(inComponent);
    if(button==NULL) return false;
    bool checked=button->mChecked!=0;
    switch(inComponent->mControlId) {
    case 3:LobbyFn35E450(checked);break;
    case 4:LobbyFn35E430(checked);return true;
    case 1:LobbyFn35E5A0(checked);return true;
    case 11:
        LobbyFn35E5C0(checked);
        LobbyFn359120(100059,true);
        return true;
    case 9:
        LobbyFn35E6A0();
        LobbyFn35B9C0(LobbyFn35E3E0());
        RefreshOptions();
        return true;
    case 8: {
        LobbyFn35DF00();
        Component* screen=LobbyFn360EA0();
        if(screen!=NULL) screen->Fn2F52D0(new LobbyEvent(100034));
        return true;
    }
    case 5:
        LobbyFn35E550(checked);
        if(checked) ::LobbyGlobal_LobbyMusic_Sound->Resume(); // actual Resume wrapper
        else ::LobbyGlobal_LobbyMusic_Sound->Stop(); // actual Stop wrapper
        return true;
    case 6:
        LobbyFn35E4E0(checked);
        ::LobbyGlobal_ButtonClick_Sound->Fn329BF0(!checked); // actual SetMuted, not looping
        if(!checked) ::LobbyGlobal_SierraLogon_Sound->Stop();
        return true;
    case 2:LobbyFn35E510(checked);return true;
    case 12:LobbyFn35E530(checked);return true;
    case 10:LobbyFn35E580(checked);return true;
    case 7:LobbyFn35D770();RefreshLanOptions();return true;
    case 0x30d55:LobbyFn35E410(checked);break;
    case 0x30d56:LobbyFn35E5E0(checked);break;
    }
    return false;
}

// DEFERRED COMPLETE BODY 3bef90..3bf18d,509B, zero admitted credit.
// Root G closes actual ImageComponent::VirtualB0(Image*) and both Image globals.
// Remaining blockers: own source gate and full normal allocation/EH/data review.
// FUNCTION: WON 0x007bef90
void LobbyOptionsCtrl::RefreshPublicAddress()
{
    if(LobbyFn361B40()) {
        if(LobbyFn361B20()) {
            mFirewalledIcon->VirtualB0(::LobbyGlobal_Firewall_Image.get());
            mFirewalledIcon->Invalidate();
            mFirewalledIcon->SetVisible(true);
        } else mFirewalledIcon->SetVisible(false);
    } else {
        mFirewalledIcon->VirtualB0(::LobbyGlobal_Searching_Image.get());
        mFirewalledIcon->Invalidate();
        mFirewalledIcon->SetVisible(true);
    }
    IPAddr address=LobbyFn361990();
    if(!address.IsValid()) {
        if(!LobbyFn361B40())
            mPublicAddressDetailLabel->SetText(::LobbyOptions_CheckingAddress_String,true);
        else mPublicAddressDetailLabel->SetText(::LobbyOptions_AddressUnavailable_String,true);
    } else {
        const std::string& host=address.GetHostString(true); //temporary exists BEFORE raw new
        mPublicAddressDetailLabel->SetText(GUIString(new GUIStringData(host)),true);
    }
}
} //namespace WONAPI
// Allocation guard in valid branch protects new GUIStringData before construction;
// outline GUIString(std::string) cannot silently own missing caller cleanup.

/* Rejected earlier implicit-source inference; zero handwritten credit.
The blanket removal of an explicit declaration was wrong for this family.
Actual original3be08f stores45354c before reverse18-member cleanup. The explicit
empty body above is the corrected natural candidate. Full normal COFF must
confirm all actual actions/hot writes; no destructor marker or quota is added.
*/

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007ce1a0
// (void __cdecl WONAPI::SafeGetComponent(class WONAPI::ComponentConfig *,class WONAPI::SmartPtr<class WONAPI::Button> &,char const *,int,char const *,int)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007be050
// (public: virtual void * __thiscall WONAPI::LobbyOptionsCtrl::`scalar deleting destructor'(unsigned int)    [??_GLobbyOptionsCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007be070
// (public: virtual __thiscall WONAPI::LobbyOptionsCtrl::~LobbyOptionsCtrl(void)    [??1LobbyOptionsCtrl@WONAPI@@UAE@XZ])
