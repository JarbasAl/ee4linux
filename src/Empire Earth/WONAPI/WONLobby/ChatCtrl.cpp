// Reviewed original-backed controls; source-only, near code/support remain carved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233

// Blocker: canonical rich-text/TextArea reference contracts, event declarations,
// typed model/parser contracts and independent per-family flags calibration.
#include <windows.h>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/WONLobbyRichText.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyControls.h"
using namespace WONAPI;
// Actual original RTTI is global namespace ChatLink, VT452df8, allocation8c.
class ChatLink : public WONAPI::TextLink {
public:
    ChatLink(WONAPI::LobbyClient* inClient,WONAPI::Font* inFont,int inColor) : mKind(3)
    {
        mClient=inClient;
        mText=inClient->mName;
        mColor=inColor;
        mFont=inFont;
        mEnabled=true;
        mPrevious=NULL;
    }
    ChatLink(const WONAPI::GUIString& inText,unsigned short inId,WONAPI::Font* inFont,
        int inColor,WONAPI::LobbyServer* inServer) : mKind(0)
    {
        mText=inText;
        mColor=inColor;
        mGroupId=inId;
        mFont=inFont;
        mEnabled=true;
        mPrevious=NULL;
        if(inServer!=NULL) mAddress=inServer->mAddress;
    }
    ChatLink(WONAPI::LobbyFriend* inFriend,WONAPI::Font* inFont,int inColor) : mKind(4)
    {
        mFriend=inFriend;
        mText=inFriend->mName;
        mColor=inColor;
        mFont=inFont;
        mEnabled=true;
        mPrevious=NULL;
    }
    ChatLink(WONAPI::LobbyFriend* inFriend,const WONAPI::GUIString& inText,
        const WONAPI::IPAddr& inAddress,WONAPI::Font* inFont,int inColor) : mKind(5)
    {
        mFriend=inFriend;
        mAddress=inAddress;
        mText=inText;
        mColor=inColor;
        mFont=inFont;
        mEnabled=true;
        mPrevious=NULL;
    }
    ChatLink(int inKind,WONAPI::LobbyClient* inClient,const WONAPI::GUIString& inText,
        WONAPI::Font* inFont,int inColor) : mKind(inKind)
    {
        mClient=inClient;
        mText=inText;
        mColor=inColor;
        mFont=inFont;
        mEnabled=true;
        mPrevious=NULL;
    }
    // implicit normal destructor3b7190: original no own-vptr store; zero handwritten credit
    virtual bool IsLink(); //3b7160
    int mKind; //48
    WONAPI::IPAddr mAddress; //4c..74
    unsigned short mGroupId; //74
    WONAPI::SmartPtr<WONAPI::LobbyClient> mClient; //78
    WONAPI::SmartPtr<WONAPI::LobbyFriend> mFriend; //7c
    bool mEnabled; //80
    WONAPI::SmartPtr<ChatLink> mNext; //84 owned forward chain
    ChatLink* mPrevious; //88 borrowed; destructor never releases
};

// FUNCTION: WON 0x007b7160
bool ChatLink::IsLink() { return mEnabled; } //3b7160 7B

namespace WONAPI {
// FUNCTION: WON 0x007b6b80
ChatCtrl::ChatCtrl() : mRoomName(GetEmptyGUIStringData()) //3b6b80 251B
{
    mDefaultColor=0;
    mAcceptColor=0;
    mRoomSpecFlags=0;
    mErrorColor=0xff0000;
    mHelpColor=0x404040;
    mUserColor=0x800000;
    mEmoteColor=0x800080;
    mWhisperColor=0x8000;
    mWarningColor=0xff8000;
    mGroupColor=0x80;
    mMessageColor=0xff;
    mRejectColor=0xa0a0a0;
    mClientId=0xffff;
}
ChatCtrl::~ChatCtrl() {} //3b6ca0301B; ownVTstore3b6cbf,zeroquota,normalCOFFpending
// FUNCTION: WON 0x007b6f00
bool ChatCtrl::Fn3B6F00() { return (mRoomSpecFlags&16)!=0 ? true : false; } //12B

// FUNCTION: WON 0x007b6f10
void ChatCtrl::Fn3B6F10(const GUIString& inFormat,const GUIString*const* inStrings,int inNumStrings,
    TextSegmentBase*const* inSegments,int inNumSegments,bool inLineBreak) //3b6f10 185B
{
    int aColors[9]={mDefaultColor,mUserColor,mWhisperColor,mEmoteColor,mWarningColor,
        mErrorColor,mHelpColor,mGroupColor,mMessageColor};
    Font* aFonts[3]={mChatOutput->mFont.get(),mFont84.get(),mFont88.get()};
    LobbyFn34A1C0(mChatOutput.get(),inFormat,inStrings,inNumStrings,aColors,9,aFonts,3,
        inSegments,inNumSegments,inLineBreak);
}
// FUNCTION: WON 0x007b6fd0
void ChatCtrl::Fn3B6FD0(const GUIString& inFormat,const GUIString& inText,bool inLineBreak) //37B
{
    const GUIString* aText=&inText;
    Fn3B6F10(inFormat,&aText,1,NULL,0,inLineBreak);
}
// FUNCTION: WON 0x007b7000
void ChatCtrl::Fn3B7000(const GUIString& inFormat,TextSegmentBase* inSegment,bool inLineBreak) //37B
{
    TextSegmentBase* aSegment=inSegment;
    Fn3B6F10(inFormat,NULL,0,&aSegment,1,inLineBreak);
}
// FUNCTION: WON 0x007b7030
SmartPtr<ChatLink> ChatCtrl::Fn3B7030(LobbyClient* inClient) //3b7030 296B; ctor allocationguard
{
    return new ChatLink(inClient,mChatOutput->mFont.get(),mUserColor);
}
// FUNCTION: WON 0x007b98a0
void ChatCtrl::Fn3B98A0(ChatLink* inLink,int inColor,int inLinkedColor) //99B
{
    inLink->mColor=inColor;
    inLink->mEnabled=false;
    ChatLink* aLink=inLink->mNext.get();
    while(aLink!=NULL) {
        aLink->mEnabled=false;
        aLink->mColor=inLinkedColor;
        aLink=aLink->mNext.get();
    }
    aLink=inLink->mPrevious;
    while(aLink!=NULL) {
        aLink->mEnabled=false;
        aLink->mColor=inLinkedColor;
        aLink=aLink->mPrevious;
    }
    mChatOutput->Invalidate();
}
//3b6dd0..3b6ef9,297B; SafeGet types proved by actual RTTI targets.
// FUNCTION: WON 0x007b6dd0
void ChatCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(aConfig,mChatInput,"ChatInput",__FILE__,160);
        SafeGetComponent(aConfig,mSendButton,"SendButton",__FILE__,161);
        SafeGetComponent(aConfig,mChatOutput,"ChatOutput",__FILE__,162);
        mFont84=Fn2F5A30(mChatOutput->mFont.get(),1,0);
        mFont88=Fn2F5A30(mChatOutput->mFont.get(),2,0);
        Fn2F5360(1);
    }
}
// Canonical globals pending root names/data validation; original producer type GUIString.
//3b72e0..3b765e,894B; full allocation guards and 16-state local cleanup pending COFF.
// FUNCTION: WON 0x007b72e0
void ChatCtrl::Fn3B72E0(AnnounceClientLocationEvent* inEvent)
{
    if(inEvent->mHandled || (mRoomSpecFlags&inEvent->mRoomId)!=inEvent->mRoomId) return;
    LobbyClient* initialClient=inEvent->mClient.get(); //borrowed snapshot3b731e/23
    if(initialClient==NULL || !Fn2F5600()) return;
    inEvent->mHandled=true;
    LobbyServer* aServer=LobbyFn35B6F0();
    SmartPtr<ChatLink> aClientLink=Fn3B7030(inEvent->mClient.get());
    SmartPtr<ChatLink> aRoomLink=new ChatLink(inEvent->mRoom,initialClient->GetGroupId20(),
        mChatOutput->mLinkFont.get(),mGroupColor,aServer);
    SmartPtr<ChatLink> aServerLink=new ChatLink(inEvent->mServer,initialClient->mGroupId1E,
        mChatOutput->mLinkFont.get(),mGroupColor,aServer);
    TextSegmentBase* aLinks[3]={aRoomLink.get(),aServerLink.get(),aClientLink.get()};
    GUIString aFormat(GetEmptyGUIStringData());
    bool hasRoom=inEvent->mRoom.length()!=0;
    bool hasServer=inEvent->mServer.length()!=0;
    if(initialClient->Fn3A5470()) aFormat=::ChatCtrl_ClientIsAnonymous_String;
    else if(hasRoom) {
        if(hasServer) aFormat=::ChatCtrl_ClientInGameAndChat_String;
        else aFormat=::ChatCtrl_ClientInGame_String;
    } else if(hasServer) aFormat=::ChatCtrl_ClientInChat_String;
    else aFormat=::ChatCtrl_ClientNotOnServer_String;
    Fn3B6F10(aFormat,NULL,0,aLinks,3,true);
}

//3b7660..3b7c5a,1530B; original31 cleanup states, independent branch allocation guards.
// FUNCTION: WON 0x007b7660
void ChatCtrl::Fn3B7660(AnnounceFriendEvent* inEvent)
{
    if((mRoomSpecFlags&inEvent->mLocation)!=inEvent->mLocation || inEvent->mFriend.get()==NULL) return;
    LobbyFriend* aFriend=inEvent->mFriend.get();
    if(inEvent->IsAdd()) {
        SmartPtr<ChatLink> aLink=new ChatLink(aFriend,mChatOutput->mFont.get(),mUserColor);
        Fn3B7000(::ChatCtrl_FriendJoined_String,aLink.get(),true);
        return;
    }
    if(inEvent->IsDelete()) {
        SmartPtr<ChatLink> aLink=new ChatLink(aFriend,mChatOutput->mFont.get(),mUserColor);
        Fn3B7000(::ChatCtrl_FriendLeft_String,aLink.get(),true);
        return;
    }
    if(inEvent->mHandled || !Fn2F5600()) return;
    inEvent->mHandled=true;
    IPAddr aAddress=aFriend->mAddress;
    GUIString aLocation=aFriend->mLocation;
    bool hasLocation=(aFriend->mFlags&3)!=0 && aAddress.IsValid() && aLocation.length()!=0;
    SmartPtr<ChatLink> aFriendLink=new ChatLink(aFriend,mChatOutput->mFont.get(),mUserColor);
    SmartPtr<ChatLink> aLocationLink;
    if(hasLocation) aLocationLink=new ChatLink(aFriend,aLocation,aAddress,mChatOutput->mFont.get(),mUserColor);
    TextSegmentBase* aLinks[2]={aFriendLink.get(),aLocationLink.get()};
    GUIString aFormat(GetEmptyGUIStringData());
    if(!hasLocation) aFormat=::ChatCtrl_FriendIsNotOnline_String;
    else if((aFriend->mFlags&1)!=0) {
        if((aFriend->mFlags&2)!=0) aFormat=::ChatCtrl_FriendIsInGameStaging_String;
        else aFormat=::ChatCtrl_FriendIsChatting_String;
    } else if((aFriend->mFlags&2)!=0) aFormat=::ChatCtrl_FriendIsPlaying_String;
    else aFormat=::ChatCtrl_FriendIsOn_String;
    Fn3B6F10(aFormat,NULL,0,aLinks,2,true);
}

//3b9910..3b9b7b,619B normal code; table24B starts3b9b7c, excluded credit.
// FUNCTION: WON 0x007b9910
void ChatCtrl::Fn3B9910(TextLinkActivatedEvent* inEvent)
{
    ChatLink* aLink=dynamic_cast<ChatLink*>(inEvent->mLink.get());
    if(aLink==NULL) return;
    ComponentEvent* anEvent;
    switch(aLink->mKind) {
    case 3:
        anEvent=new ClientActionEvent(18,mRoomSpecFlags,aLink->mClient.get());
        break;
    case 4:
        anEvent=new FriendActionEvent(5,aLink->mFriend.get(),mRoomSpecFlags);
        break;
    case 5:
        anEvent=new JoinFriendsRoomEvent(aLink->mFriend.get());
        break;
    case 0:
        Fn2F53F0(new JumpToRoomEvent(aLink->mText,aLink->mGroupId,aLink->mAddress),false);
        return;
    case 1:
        Fn3B98A0(aLink,mAcceptColor,mRejectColor);
        anEvent=new AcceptClientEvent(aLink->mClient.get(),true);
        break;
    case 2:
        Fn3B98A0(aLink,mAcceptColor,mRejectColor);
        anEvent=new AcceptClientEvent(aLink->mClient.get(),false);
        break;
    default: return;
    }
    Fn2F53F0(anEvent,false);
}

//3b81f0,248B; separate branch temporaries survive formatting then release.
// FUNCTION: WON 0x007b81f0
void ChatCtrl::Fn3B81F0(PlayerTeamChangedEvent* inEvent)
{
    if(!Fn3B6F00()) return;
    if(inEvent->IsAdd()) Fn3B7000(::ChatCtrl_TeamMemberJoined_String,Fn3B7030(inEvent->mClient.get()).get(),true);
    else if(inEvent->IsDelete()) Fn3B7000(::ChatCtrl_TeamMemberLeft_String,Fn3B7030(inEvent->mClient.get()).get(),true);
}
//3b85b0,819B. Repeated predicates retained as original calls, no cached semantic assumptions.
// FUNCTION: WON 0x007b85b0
void ChatCtrl::Fn3B85B0(RecvChatEvent* inEvent)
{
    if(inEvent->mSender.get()==NULL || mClientId==0xffff) return;
    if(inEvent->IsTeam()) {
        if(((mRoomSpecFlags|16)&inEvent->mRoomSpec)!=inEvent->mRoomSpec) return;
    } else {
        if((mRoomSpecFlags&inEvent->mRoomSpec)!=inEvent->mRoomSpec) return;
        if(Fn3B6F00() && !inEvent->IsEmote() && !inEvent->IsWarning()) return;
    }
    bool crossRoom=inEvent->IsTeam() && !Fn3B6F00();
    SmartPtr<ChatLink> aSender=Fn3B7030(inEvent->mSender.get());
    SmartPtr<ChatLink> aTarget;
    TextSegmentBase* aLinks[2]={aSender.get(),NULL};
    int count=1;
    if(inEvent->mTarget.get()!=NULL) {
        aTarget=Fn3B7030(inEvent->mTarget.get());
        aLinks[1]=aTarget.get();
        count=2;
    }
    const GUIString* aText=&inEvent->mText;
    GUIString aFormat(GetEmptyGUIStringData());
    if(inEvent->IsWhisper()) aFormat=crossRoom?::ChatCtrl_TeamEmote_String : ::ChatCtrl_Emote_String;
    else if(inEvent->IsEmote() && inEvent->mTarget.get()!=NULL) {
        if(inEvent->mSender->GetClientId()==mClientId)
            aFormat=inEvent->mTarget->mClientId==mClientId?::ChatCtrl_YouWhisperToYourself_String : ::ChatCtrl_YouWhisper_String;
        else aFormat=::ChatCtrl_SomeoneWhispersToYou_String;
    } else if(inEvent->IsWarning() && inEvent->mTarget.get()!=NULL) {
        if(inEvent->mSender->GetClientId()==mClientId)
            aFormat=inEvent->mTarget->mClientId==mClientId?::ChatCtrl_YouWarnYourself_String : ::ChatCtrl_YouWarnSomeone_String;
        else if(inEvent->mTarget->mClientId==mClientId) aFormat=::ChatCtrl_SomeoneWarnsYou_String;
        else aFormat=::ChatCtrl_SomeoneWarnsSomeone_String;
    } else aFormat=crossRoom?::ChatCtrl_TeamChat_String : ::ChatCtrl_NormalChat_String;
    if(aFormat.length()!=0) Fn3B6F10(aFormat,&aText,1,aLinks,count,true);
}
//3b88f0,161B.
// FUNCTION: WON 0x007b88f0
void ChatCtrl::Fn3B88F0(JoinedRoomEvent* inEvent)
{
    if((mRoomSpecFlags&inEvent->mRoomSpec)==inEvent->mRoomSpec) {
        mClientId=inEvent->mClientId;
        mRoomName=inEvent->mGroupName;
        mSendButton->SetEnabled(true);
        Fn3B6FD0(::ChatCtrl_EnteringRoom_String,inEvent->mGroupName,true);
        mChatOutput->LineBreak(10);
    }
}
//3b89a0,115B.
// FUNCTION: WON 0x007b89a0
void ChatCtrl::Fn3B89A0(LeftRoomEvent* inEvent)
{
    if((mRoomSpecFlags&inEvent->mRoomSpec)==inEvent->mRoomSpec) {
        mClientId=0xffff;
        mSendButton->SetEnabled(false);
        if(inEvent->mFlag) {
            if(inEvent->mForced) Fn3B6FD0(::ChatCtrl_Disconnected_String,mRoomName,true);
            else Fn3B6FD0(::ChatCtrl_LeavingRoom_String,mRoomName,true);
            mChatOutput->LineBreak(10);
        }
    }
}
//3b8bd0,31B.
// FUNCTION: WON 0x007b8bd0
void ChatCtrl::Fn3B8BD0(ChatCommandErrorEvent* inEvent)
{
    mChatOutput->AddSegment(inEvent->mText,mErrorColor,true);
}

//3b94f0..3b9673,387B. Actual cdecl unsigned duration/unit helper; no EH.
// FUNCTION: WON 0x007b94f0
unsigned char LobbyCtrlFn3B94F0(unsigned long inSeconds,bool inRoundUp,unsigned long* outValue)
{
    if(inSeconds==0) { *outValue=0;return 6; }
    if(inRoundUp) {
        if(inSeconds<=60) { *outValue=1;return 0; }
        if(inSeconds<=3540) {
            *outValue=inSeconds/60;
            if(inSeconds%60>0) ++*outValue;
            return 1;
        }
        if(inSeconds<=3600) { *outValue=1;return 2; }
        if(inSeconds<=82800) {
            *outValue=inSeconds/3600;
            if(inSeconds%3600>0) ++*outValue;
            return 3;
        }
        if(inSeconds<=86400) { *outValue=1;return 4; }
        *outValue=inSeconds/86400;
        if(inSeconds%86400>0) ++*outValue;
        return 5;
    }
    if(inSeconds<120) { *outValue=1;return 0; }
    if(inSeconds<3600) { *outValue=inSeconds/60;return 1; }
    if(inSeconds<7200) { *outValue=1;return 2; }
    if(inSeconds<86400) { *outValue=inSeconds/3600;return 3; }
    if(inSeconds<172800) { *outValue=1;return 4; }
    *outValue=inSeconds/86400;
    return 5;
}
//3b9680,541B. AlertEvent canonical neutral field names retained (room/result/duration here).
// FUNCTION: WON 0x007b9680
void ChatCtrl::Fn3B9680(AlertEvent* inEvent)
{
    if((mRoomSpecFlags&inEvent->mKind)!=inEvent->mKind) return;
    GUIString aFormat(GetEmptyGUIStringData());
    const GUIString* aText=&inEvent->mText;
    switch(inEvent->mValue) {
    case 0:
        aFormat=::ChatCtrl_ServerAlert_String;
        break;
    case 1: {
        unsigned long value=0;
        unsigned char unit=LobbyCtrlFn3B94F0(inEvent->mSeverity,false,&value);
        aFormat=::LobbyResource_GetString((LobbyResourceId)(unit+0x86));
        if(value!=0) {
            char text[20];
            sprintf(text,"%lu",value);
            GUIString number(new GUIStringData(text));
            const GUIString* arg=&number;
            Fn3B6F10(aFormat,&arg,1,NULL,0,true);
            aFormat=::ChatCtrl_AlertComment_String;
        }
        break;
    }
    default:
        aFormat=::ChatCtrl_ServerShutdownAborted_String;
        Fn3B6F10(aFormat,NULL,0,NULL,0,true);
        aFormat=::ChatCtrl_AlertComment_String;
        break;
    }
    if(aText->length()!=0) Fn3B6F10(aFormat,&aText,1,NULL,0,true);
}

//3b82f0,695B. The handled write precedes parser lookup and all later failures.
// FUNCTION: WON 0x007b82f0
void ChatCtrl::Fn3B82F0(SetChatInputEvent* inEvent)
{
    if(inEvent->mHandled || (mRoomSpecFlags&inEvent->mRoomSpec)!=inEvent->mRoomSpec || !Fn2F5600()) return;
    inEvent->mHandled=true;
    ChatCommandParser* parser=LobbyFn35B710();
    if(parser==NULL) return;
    mChatInput->Fn2F5890();
    GUIString input(GetEmptyGUIStringData());
    input.append(parser->mPrefix);
    input.append(parser->GetCommandString(inEvent->mCommand));
    input.append(GUIString(" \""));
    input.append(inEvent->mText);
    input.append(GUIString(new GUIStringData("\" ")));
    mChatInput->SetText(input,false);
    mChatInput->SelectRange(-1,-1);
    if(mChatInput->GetText().find(input,0)!=-1) return;
    if(mChatInput->GetText()[0]!=parser->mPrefix)
        input.append(mChatInput->GetText());
    mChatInput->SetText(input,false);
    mChatInput->SelectRange(-1,-1);
}

//3b7c60,593B. New-room constructor argument evaluation remains under raw allocation guard.
// FUNCTION: WON 0x007b7c60
void ChatCtrl::Fn3B7C60(GroupInvitationEvent* inEvent)
{
    if((mRoomSpecFlags&inEvent->mKind)!=inEvent->mKind) return;
    SmartPtr<ChatLink> aClient=Fn3B7030(inEvent->mClient.get());
    SmartPtr<ChatLink> aRoom=new ChatLink(inEvent->mGameName,inEvent->mGameId,
        mChatOutput->mLinkFont.get(),mGroupColor,LobbyFn35B6F0());
    TextSegmentBase* aLinks[2]={aClient.get(),aRoom.get()};
    const GUIString* aText[2]={&inEvent->mGameName,&inEvent->mComment};
    int id=inEvent->mFlag?(inEvent->mInvite?0x8f:0x90):(inEvent->mInvite?0x8d:0x8e);
    if(inEvent->mComment.length()!=0) id+=4;
    GUIString aFormat(::LobbyResource_GetString((LobbyResourceId)(id)));
    Fn3B6F10(aFormat,aText,2,aLinks,2,true);
}
//3b7ec0,805B. Forward chain retains accept; reverse chain borrows decline, avoiding ownership cycle.
// FUNCTION: WON 0x007b7ec0
void ChatCtrl::Fn3B7EC0(GroupJoinAttemptEvent* inEvent)
{
    if((mRoomSpecFlags&inEvent->mKind)!=inEvent->mKind || Fn3B6F00()) return;
    SmartPtr<ChatLink> aClient=Fn3B7030(inEvent->mClient.get());
    SmartPtr<ChatLink> aAccept=new ChatLink(1,inEvent->mClient.get(),::ChatCtrl_Accept_String,
        mChatOutput->mLinkFont.get(),mMessageColor);
    SmartPtr<ChatLink> aDecline=new ChatLink(2,inEvent->mClient.get(),::ChatCtrl_Reject_String,
        mChatOutput->mLinkFont.get(),mMessageColor);
    aAccept->mPrevious=aDecline.get();
    aDecline->mNext=aAccept.get();
    const GUIString* aText=&inEvent->mComment;
    TextSegmentBase* aLinks[3]={aClient.get(),aAccept.get(),aDecline.get()};
    GUIString aFormat(inEvent->mComment.length()!=0?::ChatCtrl_SomeoneAsksToJoinExpl_String : ::ChatCtrl_SomeoneAsksToJoin_String);
    Fn3B6F10(aFormat,&aText,1,aLinks,3,true);
}

//3b8a20,390B code; separate7-entry table3b8ba8..3b8bc4, excluded primary credit.
// FUNCTION: WON 0x007b8a20
void ChatCtrl::Fn3B8A20(ChatCommandParser* inParser)
{
    GUIString error(GetEmptyGUIStringData());
    switch(inParser->mError) {
    case 3:error=LobbyFormatString(::RoutingLogic_ClientNotFound_String,inParser->mTargetName);break;
    case 4:error=LobbyFormatString(::RoutingLogic_ClientAmbiguous_String,inParser->mTargetName);break;
    case 1:error=::RoutingLogic_InvalidCommand_String;break;
    case 2:error=::RoutingLogic_RequireName_String;break;
    case 7:error=::RoutingLogic_BadTimeSpec_String;break;
    }
    if(error.length()==0) error=::RoutingLogic_Error_String;
    mChatOutput->AddSegment(error,mErrorColor,true);
}
//3b9240,674B. Number temporary dies before optional comment output; format lives across both.
// FUNCTION: WON 0x007b9240
void ChatCtrl::Fn3B9240(ClientModeratedEvent* inEvent)
{
    if((mRoomSpecFlags&inEvent->mRoomSpec)!=inEvent->mRoomSpec) return;
    GUIString format(GetEmptyGUIStringData());
    SmartPtr<ChatLink> client=Fn3B7030(inEvent->mClient.get());
    TextSegmentBase* link=client.get();
    unsigned long value=0;
    if(inEvent->mFlag) {
        switch(inEvent->mModerationKind) {
        case 0:format=::LobbyResource_GetString((LobbyResourceId)(LobbyCtrlFn3B94F0(inEvent->mDuration,true,&value)+0x64));break;
        case 2:format=::LobbyResource_GetString((LobbyResourceId)(LobbyCtrlFn3B94F0(inEvent->mDuration,true,&value)+0x74));break;
        case 3:format=::ChatCtrl_YouWereUnmuted_String;break;
        default:format=::ChatCtrl_YouWereKicked_String;break;
        }
    } else {
        switch(inEvent->mModerationKind) {
        case 0:format=::LobbyResource_GetString((LobbyResourceId)(LobbyCtrlFn3B94F0(inEvent->mDuration,true,&value)+0x6c));break;
        case 2:format=::LobbyResource_GetString((LobbyResourceId)(LobbyCtrlFn3B94F0(inEvent->mDuration,true,&value)+0x7c));break;
        case 3:format=::ChatCtrl_SomeoneWasUnmuted_String;break;
        default:format=::ChatCtrl_SomeoneWasKicked_String;break;
        }
    }
    if(value==0) {
        const GUIString* text=&inEvent->mRoomName;
        Fn3B6F10(format,&text,1,&link,1,true);
    } else {
        char text[20];
        sprintf(text,"%lu",value);
        GUIString number(text);
        const GUIString* args[2]={&inEvent->mRoomName,&number};
        Fn3B6F10(format,args,2,&link,1,true);
    }
    if(inEvent->mComment.length()!=0) {
        format=::ChatCtrl_ModeratorComment_String;
        const GUIString* text=&inEvent->mComment;
        Fn3B6F10(format,&text,1,NULL,0,true);
    }
}

//3b8bf0,1562B code; jump/selector payload3b920c..3b9235 excluded primary credit.
// FUNCTION: WON 0x007b8bf0
void ChatCtrl::Fn3B8BF0()
{
    if(mChatInput->GetText().empty() || mSendButton->IsDisabled()) return;
    ChatCommandParser* parser=LobbyFn35B730(mRoomSpecFlags);
    if(parser==NULL) return;
    GUIString input=mChatInput->GetText();
    mChatInput->Fn2F9140(input,true);
    mChatInput->Clear(true);
    if(!parser->HandleInput(input)) {
        Fn3B8A20(parser);
        return;
    }
    switch(parser->mCommand) {
    case 20:
        Fn3B6F10(parser->GetHelp(mClientId),NULL,0,NULL,0,true);
        return;
    case 19:
        mChatOutput->Reset(false);
        return;
    case 18:
        if(parser->mText.length()!=0) break;
        {
            GUIString command(GetEmptyGUIStringData());
            command.append(parser->mPrefix);
            command.append(parser->GetCommandString(2));
            command.append(GUIString(" \""));
            command.append(parser->mTargetName);
            command.append(GUIString(new GUIStringData("\" ")));
            mChatInput->SetText(command,false);
            mChatInput->SelectRange(-1,-1);
        }
        return;
    case 21: {
        LobbyClientIterator iterator(parser->mClients.get());
        LobbyClient* me=LobbyFn361370();
        if(me!=NULL) {
            GUIString names(GetEmptyGUIStringData());
            while(iterator.HasMore()) {
                LobbyClient* client=iterator.Next();
                if(me->Fn3A55C0(client)) {
                    if(names.length()!=0) names.append(GUIString(new GUIStringData(", ")));
                    names.append(client->mName);
                }
            }
            Fn3B6FD0(::ChatCtrl_TeamMembers_String,names,true);
        }
        return;
    }
    case 5:
        if(parser->mTargetName.length()!=0) break;
        {
            GUIString names(GetEmptyGUIStringData());
            for(LobbyIgnoredNameSet::const_iterator i=LobbyFn35CFC0().begin();i!=LobbyFn35CFC0().end();++i) {
                if(names.length()!=0) names.append(GUIString(new GUIStringData(", ")));
                names.append(*i);
            }
            Fn3B6FD0(::ChatCtrl_IgnoreList_String,names,true);
        }
        return;
    }
    Fn2F5380(100012,1);
}


// Original3b9c80,330B; owned selector/table ends3b9e0c are not body credit.
// FUNCTION: WON 0x007b9c80
void ChatCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 17: Fn3B9910(static_cast<TextLinkActivatedEvent*>(inEvent)); return;
    case 15:
        if(inEvent->mComponent==mSendButton.get()) { Fn3B8BF0(); return; }
        break;
    case 20:
        if(inEvent->mComponent==mChatInput.get()) { Fn3B8BF0(); return; }
        break;
    case 100001: Fn3B9680(static_cast<AlertEvent*>(inEvent)); return;
    case 100003: Fn3B7660(static_cast<AnnounceFriendEvent*>(inEvent)); return;
    case 100004: Fn3B72E0(static_cast<AnnounceClientLocationEvent*>(inEvent)); return;
    case 100007: Fn3B9240(static_cast<ClientModeratedEvent*>(inEvent)); return;
    case 100009: Fn3B8BD0(static_cast<ChatCommandErrorEvent*>(inEvent)); return;
    case 100022: Fn3B7C60(static_cast<GroupInvitationEvent*>(inEvent)); return;
    case 100023: Fn3B7EC0(static_cast<GroupJoinAttemptEvent*>(inEvent)); return;
    case 100030: Fn3B88F0(static_cast<JoinedRoomEvent*>(inEvent)); return;
    case 100033: Fn3B89A0(static_cast<LeftRoomEvent*>(inEvent)); return;
    case 100039: Fn3B81F0(static_cast<PlayerTeamChangedEvent*>(inEvent)); return;
    case 100044: Fn3B85B0(static_cast<RecvChatEvent*>(inEvent)); return;
    case 100048: Fn3B82F0(static_cast<SetChatInputEvent*>(inEvent)); return;
    case 100054: mRoomSpecFlags|=static_cast<SetRoomSpecFlagsEvent*>(inEvent)->mFlags; return;
    }
    Component::HandleComponentEvent(inEvent);
}

}

/* Rejected source-kind candidates; zero handwritten credit.
ChatLink::~ChatLink() {}
JumpToRoomEvent::~JumpToRoomEvent() {}
Blocker: complete originals3b7190..3b72d6 and3b9bc0..3b9c78 have no own
452df8/452e30 vptr stores. Implicit member/base destruction is retained.
Normal COFF must establish exact generated bodies/EH before admission.
*/

/* Rejected earlier implicit-source inference; zero handwritten credit.
The blanket removal of an explicit declaration was wrong for ChatCtrl.
Actual3b6cbf stores452d28 before reverse member cleanup. The explicit empty body
above is the corrected natural candidate. Full normal COFF must confirm all
actions/hot writes; no destructor marker or quota is added. ChatLink and
JumpToRoomEvent separately have no own-derived-vptr store and stay implicit candidates.
*/

namespace WONAPI {
// REFERENCE ONLY actual ChatCtrlConfig9c/VT451c38, ContainerConfig98/14slots.
// Never allocate, construct, sizeof, copy/pass by value, or claim factory/vtable authority.
// ChatCtrlConfig declaration hoisted to WONLobbyControlConfig.h; existing bodies unchanged.
// Original3b9e10..3ba288,1144B, no switch table or cold continuation.
// FUNCTION: WON 0x007b9e10
bool ChatCtrlConfig::Virtual24(const std::string& inKey)
{
    if(ContainerConfig::Virtual24(inKey))
        return true;
    if(inKey=="GROUPTYPE") {
        GUIString group=Fn3842F0(false);
        if(group.mData->CompareNoCase("CHAT",0)==0)
            mChatCtrl->mRoomSpecFlags=1;
        else if(group.mData->CompareNoCase("GAME",0)==0)
            mChatCtrl->mRoomSpecFlags=2;
        else if(group.mData->CompareNoCase("TEAM",0)==0)
            mChatCtrl->mRoomSpecFlags=0x12;
        else {
            throw ConfigObjectException("Invalid Group Type: "+(std::string)group);
        }
        return true;
    }
    if(inKey=="CTRLLINKID")
        return true;
    if(inKey=="COLOR_DEFAULT") mChatCtrl->mDefaultColor=Fn385510();
    else if(inKey=="COLOR_ERROR") mChatCtrl->mErrorColor=Fn385510();
    else if(inKey=="COLOR_HELP") mChatCtrl->mHelpColor=Fn385510();
    else if(inKey=="COLOR_USER") mChatCtrl->mUserColor=Fn385510();
    else if(inKey=="COLOR_EMOTE") mChatCtrl->mEmoteColor=Fn385510();
    else if(inKey=="COLOR_WHISPER") mChatCtrl->mWhisperColor=Fn385510();
    else if(inKey=="COLOR_WARNING") mChatCtrl->mWarningColor=Fn385510();
    else if(inKey=="COLOR_GROUP") mChatCtrl->mGroupColor=Fn385510();
    else if(inKey=="COLOR_MESSAGE") mChatCtrl->mMessageColor=Fn385510();
    else if(inKey=="COLOR_ACCEPT") mChatCtrl->mAcceptColor=Fn385510();
    else if(inKey=="COLOR_REJECT") mChatCtrl->mRejectColor=Fn385510();
    else return false;
    return true;
}
// Original3c8380..3c8399,25B; source-kind registry/template uncertainty, zeroquota.
void ChatCtrlConfig::Virtual34(Component* inComponent)
{
    ContainerConfig::Virtual34(inComponent);
    mChatCtrl=static_cast<ChatCtrl*>(inComponent);
}
} //namespace WONAPI

namespace WONAPI {
// FUNCTION: WON 0x007651e0
ComponentConfig* ChatCtrlConfig::CfgFactory()
{
    return new ChatCtrlConfig;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007b6ca0
// (public: virtual __thiscall WONAPI::ChatCtrl::~ChatCtrl(void)    [??1ChatCtrl@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007b7190
// (public: virtual __thiscall ChatLink::~ChatLink(void)    [??1ChatLink@@UAE@XZ])
// TEMPLATE: WON 0x007b9bc0
// (public: virtual __thiscall WONAPI::JumpToRoomEvent::~JumpToRoomEvent(void)    [??1JumpToRoomEvent@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007cd8a0
// TEMPLATE: WON 0x007cda20

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007b6c80
// (public: virtual void * __thiscall WONAPI::ChatCtrl::`scalar deleting destructor'(unsigned int)    [??_GChatCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007b9ba0
// (public: virtual void * __thiscall WONAPI::JumpToRoomEvent::`scalar deleting destructor'(unsigned int)    [??_GJumpToRoomEvent@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007b7170
// (public: virtual void * __thiscall ChatLink::`scalar deleting destructor'(unsigned int)    [??_GChatLink@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00765240
// (public: virtual void * __thiscall WONAPI::ChatCtrlConfig::`scalar deleting destructor'(unsigned int))
