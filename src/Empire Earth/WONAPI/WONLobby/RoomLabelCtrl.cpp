// Scratch entire RoomLabelCtrl family; generated implicit destructor0quota.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <string>
#include <list>
#include <map>
#include <vector>
#include <stdio.h>
#include <windows.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
namespace WONAPI {
// FUNCTION: WON 0x007c32b0
RoomLabelCtrl::RoomLabelCtrl() : mRoomSpecFlags(0),mUserCt(0) { mShowServer=true; }
// No explicit destructor: original3c33a0 has only member/base destruction.
// FUNCTION: WON 0x007c3480
void RoomLabelCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2)
    {
        ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig,mLabel,"Label",__FILE__,29);
        CalcLabel();
        Fn2F5360(1);
    }
}
// Scratch only. Original RoomLabelCtrl::CalcLabel3c34c0..3c3716 (598 nominalB).
// Needs canonical call-only four-argument formatter34a170, individually proved resources.
// FUNCTION: WON 0x007c34c0
void RoomLabelCtrl::CalcLabel()
{
    GUIString aFormat;
    char aCount[33] = "";
    if(mRoom.empty())
        aFormat = ::RoomLabelCtrl_LabelDisconnected_String;
    else
    {
        if(mPrefix.length()!=0) aFormat.append(::RoomLabelCtrl_LabelPrefix_String);
        if(mShowServer && mServer.length()!=0) aFormat.append(::RoomLabelCtrl_LabelServer_String);
        aFormat.append(mRoom);
        if(mUserCt>0)
        {
            sprintf(aCount,"%d",mUserCt);
            const GUIString& aSuffix = mUserCt==1 ? ::RoomLabelCtrl_LabelUser_String : ::RoomLabelCtrl_LabelUsers_String;
            aFormat.append(aSuffix);
        }
    }
    GUIString aCountText(new GUIStringData(aCount));
    mLabel->SetText(LobbyFn34A170(aFormat,mPrefix,mServer,aCountText),true);
}

// FUNCTION: WON 0x007c3720
void RoomLabelCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType)
    {
    case 100030:
        {
            JoinedRoomEvent* aEvent=static_cast<JoinedRoomEvent*>(inEvent);
            if((mRoomSpecFlags&aEvent->mRoomSpec)!=aEvent->mRoomSpec) return;
            mServer=aEvent->mServerName;
            mRoom=aEvent->mGroupName;
            if(mUserCt>=0) mUserCt=aEvent->mClients->mClients.size();
            CalcLabel();
        }
        return;
    case 100033:
        {
            LeftRoomEvent* aEvent=static_cast<LeftRoomEvent*>(inEvent);
            if((mRoomSpecFlags&aEvent->mRoomSpec)!=aEvent->mRoomSpec) return;
            mServer=GetEmptyGUIStringData();
            mRoom=GetEmptyGUIStringData();
            if(mUserCt>0) mUserCt=0;
            CalcLabel();
        }
        return;
    case 100006:
        {
            if(mUserCt<0) return;
            ClientChangedEvent* aEvent=static_cast<ClientChangedEvent*>(inEvent);
            if((mRoomSpecFlags&aEvent->mRoomSpec)!=aEvent->mRoomSpec) return;
            if(aEvent->IsAdd()) { ++mUserCt; CalcLabel(); return; }
            if(mUserCt>0 && aEvent->IsDelete()) --mUserCt;
            CalcLabel();
        }
        return;
    case 100054:
        mRoomSpecFlags|=static_cast<SetRoomSpecFlagsEvent*>(inEvent)->mFlags;
        return;
    default:
        Component::HandleComponentEvent(inEvent);
    }
}
}


// Source-local original RoomLabelCtrlConfig reference contract; no allocation authority.
namespace WONAPI {
// Reference-only source-local Config9c. No constructor/factory/new/byvalue/sizeof authority.
// RoomLabelCtrlConfig declaration hoisted to WONLobbyControlConfig.h; existing bodies unchanged.
// FUNCTION: WON 0x007c3930
bool RoomLabelCtrlConfig::Virtual24(const std::string& inKey)
{
    if(inKey=="PREFIX")
    {
        GUIString aPrefix=Fn3842F0(false);
        mRoomLabel->mPrefix=aPrefix;
        return true;
    }
    if(inKey=="GROUPTYPE")
    {
        GUIString aGroup=Fn3842F0(false);
        if(aGroup.CompareNoCase("CHAT",0)==0) mRoomLabel->mRoomSpecFlags=1;
        else if(aGroup.CompareNoCase("GAME",0)==0) mRoomLabel->mRoomSpecFlags=2;
        else throw ConfigObjectException("Invalid Group Type: "+(std::string)aGroup);
        return true;
    }
    if(inKey=="SHOWUSERCOUNT")
    {
        bool aShow=Fn3837F0();
        RoomLabelCtrl* aLabel=mRoomLabel;
        if(!aShow) aLabel->mUserCt=-1;
        else if(aLabel->mUserCt<0) aLabel->mUserCt=0;
        return true;
    }
    if(inKey=="SHOWSERVER")
    {
        bool aShow=Fn3837F0();
        mRoomLabel->mShowServer=aShow;
        return true;
    }
    return ContainerConfig::Virtual24(inKey);
}
// 39afb0 support ICF; source-only reference body, zero primary bytes.
void RoomLabelCtrlConfig::Virtual34(Component* inComponent)
{
    mRoomLabel=static_cast<RoomLabelCtrl*>(inComponent);
    ContainerConfig::Virtual34(inComponent);
}


}

namespace WONAPI {
// FUNCTION: WON 0x007655e0
ComponentConfig* RoomLabelCtrlConfig::CfgFactory()
{
    return new RoomLabelCtrlConfig;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c33a0
// (public: virtual __thiscall WONAPI::RoomLabelCtrl::~RoomLabelCtrl(void)    [??1RoomLabelCtrl@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c3380
// (public: virtual void * __thiscall WONAPI::RoomLabelCtrl::`scalar deleting destructor'(unsigned int)    [??_GRoomLabelCtrl@WONAPI@@UAEPAXI@Z])
