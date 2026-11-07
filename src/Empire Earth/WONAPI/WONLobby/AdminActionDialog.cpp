// WONLobby AdminActionDialog -- original family3f4a60..3f525d; table bytes excluded.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {

AdminActionDialog::AdminActionDialog() {}
// FUNCTION: WON 0x7f4ad0
AdminActionDialog::~AdminActionDialog() {}
// FUNCTION: WON 0x7f4c90
void AdminActionDialog::UpdateDoActionButton()
{
    if (mExplanation->GetText().length() != 0 || mIsReasonOptional)
        mDoActionButton->SetEnabled(true);
    else
        mDoActionButton->SetEnabled(false);
}
// FUNCTION: WON 0x7f4ce0
void AdminActionDialog::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 21 && inEvent->mComponent == mExplanation.get()) {
        UpdateDoActionButton();
        return;
    }
    Dialog::HandleComponentEvent(inEvent);
}
// FUNCTION: WON 0x007f4d10
bool AdminActionDialog::GetApplyToServer()
{
    return mApplyToServer->IsChecked();
}
// Zero-credit pure forwarder: original3f4d20,11B.
// FUNCTION: WON 0x007f4d20
const GUIString& AdminActionDialog::GetExplanation()
{
    return mExplanation->GetText();
}
// FUNCTION: WON 0x7f4d30
unsigned long AdminActionDialog::GetNumSeconds()
{
    unsigned long aSeconds = 3600;
    switch(mDurationCombo->GetSelItem()) {
        case 0: aSeconds=1; break;
        case 1: aSeconds=5; break;
        case 2: aSeconds=10; break;
        case 3: aSeconds=30; break;
        case 4: aSeconds=0; break;
    }
    switch(mTimeSegmentCombo->GetSelItem()) {
        case 2: aSeconds*=24;
        case 1: aSeconds*=60;
        case 0: aSeconds*=60;
    }
    return aSeconds;
}
// FUNCTION: WON 0x7f4dc0
void AdminActionDialog::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType != 2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(aConfig, mExplanationHeader, "ExplanationHeader", __FILE__, 105);
    SafeGetComponent(aConfig, mExplanation, "Explanation", __FILE__, 106);
    SafeGetComponent(aConfig, mDurationHeader, "DurationHeader", __FILE__, 107);
    SafeGetComponent(aConfig, mDurationCombo, "DurationCombo", __FILE__, 108);
    SafeGetComponent(aConfig, mTimeSegmentCombo, "TimeSegmentCombo", __FILE__, 109);
    SafeGetComponent(aConfig, mDoActionButton, "DoActionButton", __FILE__, 110);
    SafeGetComponent(aConfig, mCancelButton, "CancelButton", __FILE__, 111);
    SafeGetComponent(aConfig, mApplyToRoom, "ApplyToRoomRadio", __FILE__, 112);
    SafeGetComponent(aConfig, mApplyToServer, "ApplyToServerRadio", __FILE__, 113);
    SafeGetComponent(aConfig, mTitleLabel, "TitleLabel", __FILE__, 114);
}
// FUNCTION: WON 0x7f4ee0
void AdminActionDialog::Reset(int inAction, LobbyClient* inTarget)
{
    mActionType=inAction;
    GUIString anAction;
    bool showDuration,enableReason,showScope;
    mApplyToServer->SetCheck(false,false);
    mApplyToRoom->SetCheck(true,false);
    mExplanation->SetText(GUIString(""),false);
    mDurationCombo->SetSelItem(1,false);
    mTimeSegmentCombo->SetSelItem(0,false);
    switch(mActionType) {
        case 15: anAction=::AdminActionDialog_Warn_String; showDuration=false; enableReason=true; mIsReasonOptional=false; showScope=false; break;
        case 8: anAction=::AdminActionDialog_Mute_String; showDuration=true; enableReason=true; mIsReasonOptional=true; showScope=true; break;
        case 14: anAction=::AdminActionDialog_UnMute_String; showDuration=false; enableReason=false; mIsReasonOptional=true; showScope=true; break;
        case 7: anAction=::AdminActionDialog_Kick_String; showDuration=false; enableReason=true; mIsReasonOptional=true; showScope=false; break;
        case 2: anAction=::AdminActionDialog_Ban_String; showDuration=true; enableReason=true; mIsReasonOptional=false; showScope=true; break;
    }
    mDurationHeader->SetVisible(showDuration);
    mDurationCombo->SetVisible(showDuration);
    mTimeSegmentCombo->SetVisible(showDuration);
    mApplyToServer->SetVisible(showScope);
    mApplyToRoom->SetVisible(showScope);
    mExplanation->SetEnabled(enableReason);
    if(enableReason) mExplanation->Fn2F5890();
    UpdateDoActionButton();
    GUIString aTitle=anAction;
    aTitle.append(GUIString(new GUIStringData(" ")));
    aTitle.append(inTarget->GetName());
    mTitleLabel->SetText(aTitle,true);
    if(mIsReasonOptional) mExplanationHeader->SetText(::AdminActionDialog_ExplanationOpt_String,true);
    else mExplanationHeader->SetText(::AdminActionDialog_ExplanationReq_String,true);
    mDoActionButton->SetText(anAction,true);
    Fn2F6290();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f4ab0
// (public: virtual void * __thiscall WONAPI::AdminActionDialog::`scalar deleting destructor'(unsigned int)    [??_GAdminActionDialog@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f4a60
// (public: __thiscall WONAPI::AdminActionDialog::AdminActionDialog(void)    [??0AdminActionDialog@WONAPI@@QAE@XZ])
