// Original-backed RoutingConnection1c0 reconstruction; source-only pending actual COFF review.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "LostDebug.h"
// NEW actualRoutingConnection1c0 source family, NOT SmartRoutingConnection258.
namespace WONAPI {
// FUNCTION: WON 0x006ba050
RoutingConnection::RoutingConnection()
{
    mLengthFieldSize = 2;
    mAllDerivedOpsAreUnsolicited = true;
}

// Original2ba4b0..2ba62e has ownRoutingVT write and complete natural member/base
// destruction. Substantive normal destruction kind is separate from ordinary

// FUNCTION: WON 0x006ba4b0
RoutingConnection::~RoutingConnection() {}

// FUNCTION: WON 0x006bcb20
void RoutingConnection::Virtual08()
{
    for (RoutingOpList::iterator itr=mOpList.begin(); itr!=mOpList.end(); ++itr)
        (*itr)->Kill(true);
    mOpList.clear();
    ServerConnection::Virtual08();
}

// FUNCTION: WON 0x006bcbb0
void RoutingConnection::Virtual0C()
{
    mDefaultCompletion = NULL;
    for (int i=0; i<61; ++i)
        mCompletions[i] = NULL;
    ServerConnection::Virtual0C();
}

// These actual private declarations requested from sole Model header owner.
// FUNCTION: WON 0x006ba740
void RoutingConnection::CalcCompletion(RoutingOp* inOp)
{
    OpCompletionBasePtr completion = inOp->mCompletion;
    if (completion.get() == NULL) {
        completion = mCompletions[inOp->GetType()];
        if (completion.get() == NULL)
            completion = mDefaultCompletion;
    }
    inOp->mCompletion = completion;
}

// FUNCTION: WON 0x006ba630
void RoutingConnection::AddOp(RoutingOp* inOp)
{
    AutoCrit crit(mDataCrit);
    CalcCompletion(inOp);
    mOpList.push_back(inOp);
}

// FUNCTION: WON 0x006bd430
void RoutingConnection::CreateGroup(unsigned short inGroupId,
    const std::wstring& inName, const std::wstring& inPassword,
    unsigned short inMaxPlayers, unsigned long inFlags,
    unsigned char inJoinFlags, unsigned long inAsyncFlags,
    CompletionBase<AsyncOpPtr>* inCompletion)
{
    SmartPtr<RoutingCreateGroupOp> op = new RoutingCreateGroupOp(this);
    op->mGroupId = inGroupId;
    op->SetGroupName(inName);
    op->SetGroupPassword(inPassword);
    op->mMaxPlayers = inMaxPlayers;
    op->mGroupFlags = inFlags;
    op->mJoinFlags = inJoinFlags;
    op->mAsyncFlags = inAsyncFlags;
    op->mCompletion = inCompletion;
    op->Run(-1);
}

// FUNCTION: WON 0x006bd580
void RoutingConnection::JoinGroup(unsigned short inGroupId,
    const std::wstring& inPassword, const std::wstring& inComment,
    unsigned char inJoinFlags, unsigned char inMemberFlags,
    CompletionBase<AsyncOpPtr>* inCompletion)
{
    SmartPtr<RoutingJoinGroupOp> op = new RoutingJoinGroupOp(this);
    op->mGroupId = inGroupId;
    op->SetGroupPassword(inPassword);
    op->SetJoinComment(inComment);
    op->mJoinFlags = inJoinFlags;
    op->mMemberFlags = inMemberFlags;
    op->mCompletion = inCompletion;
    op->Run(-1);
}

}

namespace WONAPI {
// FUNCTION: WON 0x006bf590
void RoutingConnection::BanClient(unsigned short inGroupId, bool inOn,
    unsigned long inSeconds, const std::wstring& inExplanation,
    unsigned short inClientId, CompletionBase<AsyncOpPtr>* inCompletion)
{
    SmartPtr<RoutingBanClientOp> op = new RoutingBanClientOp(this);
    op->mGroupId = inGroupId;
    op->mIsBanned = inOn;
    op->mBanTime = inSeconds;
    op->SetBanComment(inExplanation);
    op->mClientId = inClientId;
    op->mBanByWONId = false;
    op->mCompletion = inCompletion;
    op->Run(-1);
}
// FUNCTION: WON 0x006bf6b0
void RoutingConnection::BanClientByWONId(unsigned short inGroupId, bool inOn,
    unsigned long inSeconds, const std::wstring& inExplanation,
    unsigned long inWONId, CompletionBase<AsyncOpPtr>* inCompletion)
{
    SmartPtr<RoutingBanClientOp> op = new RoutingBanClientOp(this);
    op->mGroupId = inGroupId;
    op->mIsBanned = inOn;
    op->mBanTime = inSeconds;
    op->SetBanComment(inExplanation);
    op->mWONId = inWONId;
    op->mBanByWONId = true;
    op->mWONIdIsCDKeyId = false;
    op->mCompletion = inCompletion;
    op->Run(-1);
}
// FUNCTION: WON 0x006bf7d0
void RoutingConnection::MuteClient(unsigned short inGroupId, bool inOn,
    unsigned long inSeconds, const std::wstring& inExplanation,
    unsigned short inClientId, CompletionBase<AsyncOpPtr>* inCompletion)
{
    SmartPtr<RoutingMuteClientOp> op = new RoutingMuteClientOp(this);
    op->mGroupId = inGroupId;
    op->mIsMuted = inOn;
    op->mMuteTime = inSeconds;
    op->SetMuteComment(inExplanation);
    op->mClientId = inClientId;
    op->mMuteByWONId = false;
    op->mCompletion = inCompletion;
    op->Run(-1);
}
// FUNCTION: WON 0x006bf8f0
void RoutingConnection::MuteClientByWONId(unsigned short inGroupId, bool inOn,
    unsigned long inSeconds, const std::wstring& inExplanation,
    unsigned long inWONId, CompletionBase<AsyncOpPtr>* inCompletion)
{
    SmartPtr<RoutingMuteClientOp> op = new RoutingMuteClientOp(this);
    op->mGroupId = inGroupId;
    op->mIsMuted = inOn;
    op->mMuteTime = inSeconds;
    op->SetMuteComment(inExplanation);
    op->mWONId = inWONId;
    op->mMuteByWONId = true;
    op->mWONIdIsCDKeyId = false;
    op->mCompletion = inCompletion;
    op->Run(-1);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006bcc30
void RoutingConnection::Register(const std::wstring& name,const std::wstring& password,
    unsigned long reconnectId,unsigned long asyncFlags,unsigned long registerFlags,
    unsigned long clientFlags,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingRegisterClientOp> op=new RoutingRegisterClientOp(this,name,password,
        reconnectId,asyncFlags,registerFlags,clientFlags);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bcd40
void RoutingConnection::GetClientList(CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingGetClientListOp> op=new RoutingGetClientListOp(this);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bcf90
void RoutingConnection::GetGroupList(unsigned char flags,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingGetGroupListOp> op=new RoutingGetGroupListOp(this,flags);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bd1e0
void RoutingConnection::GetMembersOfGroup(unsigned short id,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingGetMembersOfGroupOp> op=new RoutingGetMembersOfGroupOp(this,id);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bd850
void RoutingConnection::LeaveGroup(unsigned short id,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingLeaveGroupOp> op=new RoutingLeaveGroupOp(this,id);
    op->SetCompletion(completion);
    op->Run(-1);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006bd6b0
void RoutingConnection::CancelJoinGroup(unsigned short id,bool leave,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingCancelJoinGroupOp> op=new RoutingCancelJoinGroupOp(this,id,leave);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bd9f0
void RoutingConnection::AcceptClient(unsigned short group,unsigned short client,bool on,
    const std::wstring& explanation,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingAcceptClientOp> op=new RoutingAcceptClientOp(this);
    op->mGroupId=group;
    op->mClientId=client;
    op->mAccepted=on;
    op->SetAcceptComment(explanation);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bdb00
void RoutingConnection::InviteClient(unsigned short group,unsigned short client,bool on,
    const std::wstring& explanation,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingInviteClientOp> op=new RoutingInviteClientOp(this);
    op->mGroupId=group;
    op->mClientId=client;
    op->mInvited=on;
    op->SetInviteComment(explanation);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bdc10
void RoutingConnection::SetGroupFlags(unsigned short id,unsigned long mask,unsigned long flags,
    unsigned long asyncMask,unsigned long asyncFlags,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingSetGroupFlagsOp> op=new RoutingSetGroupFlagsOp(this,id,mask,flags,asyncMask,asyncFlags);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bddd0
void RoutingConnection::SendServerAlert(const std::wstring& text,CompletionBase<AsyncOpPtr>* completion)
{
    // LOST-DEBUG: none: exact only.
    EE_LOST_DEBUG(2);
    SmartPtr<RoutingSendServerAlertOp> op=new RoutingSendServerAlertOp(this,text);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006be010
void RoutingConnection::StartServerShutdown(const std::wstring& text,unsigned long seconds,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingStartServerShutdownOp> op=new RoutingStartServerShutdownOp(this,text,seconds);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006be200
void RoutingConnection::AbortServerShutdown(CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingAbortServerShutdownOp> op=new RoutingAbortServerShutdownOp(this);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006be390
void RoutingConnection::SetClientFlags(unsigned long mask,unsigned long flags,
    unsigned long asyncMask,unsigned long asyncFlags,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingSetClientFlagsOp> op=new RoutingSetClientFlagsOp(this,(unsigned short)mask,flags,asyncMask,asyncFlags);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006be540
void RoutingConnection::SendChat(const std::wstring& text,unsigned long flags,unsigned short recipient,CompletionBase<AsyncOpPtr>* completion)
{
    std::list<unsigned short> recipients;
    recipients.push_back(recipient);
    SendChatTo(text,(unsigned short)flags,recipients,completion);
}
// FUNCTION: WON 0x006be6a0
void RoutingConnection::SendChatTo(const std::wstring& text,unsigned short flags,
    const std::list<unsigned short>& recipients,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingSendChatOp> op=new RoutingSendChatOp(this);
    op->SetText(text);
    op->SetFlags(flags);
    op->SetRecipients(recipients);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006be910
void RoutingConnection::SendData(const ByteBuffer* data,unsigned char flags,unsigned short recipient,CompletionBase<AsyncOpPtr>* completion)
{
    std::list<unsigned short> recipients;
    recipients.push_back(recipient);
    SendData(data,flags,recipients,completion);
}
// FUNCTION: WON 0x006bea70
void RoutingConnection::SendData(const ByteBuffer* data,unsigned char flags,
    const std::list<unsigned short>& recipients,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingSendDataOp> op=new RoutingSendDataOp(this);
    // SDK ConstSmartPtr retains a mutable T* internally from its const input.
    op->SetData(data);
    op->SetFlags(flags);
    op->SetRecipients(recipients);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bf3f0
void RoutingConnection::BecomeModerator(bool on,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingBecomeModeratorOp> op=new RoutingBecomeModeratorOp(this,on);
    op->SetCompletion(completion);
    op->Run(-1);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006bc850
void RoutingConnection::Virtual18(const ByteBuffer* inData)
{
    AutoCrit crit(mDataCrit);
    SmartPtr<RoutingOp> op;
    RoutingOpList::iterator itr;
    bool unsolicited = false;
    WONStatus status;
    try {
        ReadBuffer msg(inData->data(), inData->length());
        msg.ReadByte();
        unsigned char service = msg.ReadByte();
        unsigned char msgType = msg.ReadByte();
        bool derived = service != 7;
        int savedPos = msg.pos();
        op = GetUnsolicitedOp(msgType, derived);
        if (op.get() != NULL) {
            mOpList.push_front(op);
            unsolicited = true;
        }
        for (itr=mOpList.begin(); itr!=mOpList.end(); ++itr) {
            op = *itr;
            if (derived == op->mIsDerivedServerOp) {
            status = op->Virtual10(msgType, msg);
            if (status == (WONStatus)100105) break;
            if (status != (WONStatus)100104) {
                mOpList.erase(itr);
                crit.Leave();
                op->SetMode(OP_MODE_BLOCK);
                if (unsolicited) op->ForceFinish(status);
                else op->Fn29F220(status);
                Virtual2C(op.get());
                op->Complete();
                break;
            }
            msg.SetPos(savedPos);
            }
        }
    } catch (ReadBufferException&) {
        if (op.get() != NULL) {
            mOpList.erase(itr);
            crit.Leave();
            op->SetMode(OP_MODE_BLOCK);
            if (unsolicited) op->ForceFinish(status);
            else op->Fn29F220(status);
            op->Complete();
        }
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x006becd0
void RoutingConnection::CreateDataObject(unsigned char flags,unsigned short link,const std::string& type,
    const std::wstring& name,const ByteBuffer* data,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingCreateDataObjectOp> op=new RoutingCreateDataObjectOp(this);
    op->mFlags=flags;
    op->mLinkId=link;
    op->SetDataType(type);
    op->SetDataName(name);
    // Genuine SDK ConstSmartPtr storage bridge, no payload mutation.
    op->mData=data;
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bee30
void RoutingConnection::ReplaceDataObject(unsigned short link,const std::string& type,
    const std::wstring& name,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingDeleteDataObjectOp> op=new RoutingDeleteDataObjectOp(this);
    op->mLinkId=link;
    op->SetDataType(type);
    op->SetDataName(name);
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bef50
void RoutingConnection::ModifyDataObject(unsigned short link,const std::string& type,
    const std::wstring& name,unsigned short offset,bool insert,const ByteBuffer* data,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingModifyDataObjectOp> op=new RoutingModifyDataObjectOp(this);
    op->mLinkId=link;
    op->SetDataType(type);
    op->SetDataName(name);
    op->mOffset=offset;
    op->mIsInsert=insert;
    // Genuine SDK ConstSmartPtr storage bridge, no payload mutation.
    op->mData=data;
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bf0c0
void RoutingConnection::Fn2BF0C0(unsigned short link,const std::string& type,unsigned long flags,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingReadDataObjectOp> op=new RoutingReadDataObjectOp(this);
    op->mSubscribe=false;
    op->mLinkId=link;
    op->SetDataType(type);
    op->mFlag=(unsigned char)flags;
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bf1d0
void RoutingConnection::ReadDataObject(unsigned short link,const std::string& type,unsigned long flags,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingReadDataObjectOp> op=new RoutingReadDataObjectOp(this);
    op->mSubscribe=true;
    op->mLinkId=link;
    op->SetDataType(type);
    op->mFlag=(unsigned char)flags;
    op->SetCompletion(completion);
    op->Run(-1);
}
// FUNCTION: WON 0x006bf2e0
void RoutingConnection::Fn2BF2E0(unsigned short link,const std::string& type,unsigned long flags,CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingUnsubscribeDataObjectOp> op=new RoutingUnsubscribeDataObjectOp(this);
    op->mLinkId=link;
    op->SetDataType(type);
    op->mFlags=(unsigned char)flags;
    op->SetCompletion(completion);
    op->Run(-1);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006ba830
SmartPtr<RoutingOp> RoutingConnection::GetUnsolicitedOp(unsigned char messageType,bool derived)
{
    SmartPtr<RoutingOp> op;
    if(!derived) {
        switch(messageType) {
        case 6: op=new RoutingClientFlagsChangedOp(this); break;
        case 7: op=new RoutingClientJoinedGroupOp(this); break;
        case 8: op=new RoutingClientJoinedServerOp(this); break;
        case 9: op=new RoutingClientLeftGroupOp(this); break;
        case 10: op=new RoutingClientLeftServerOp(this); break;
        case 14: op=new RoutingDataObjectCreatedOp(this); break;
        case 15: op=new RoutingDataObjectDeletedOp(this); break;
        case 16: op=new RoutingDataObjectModifiedOp(this); break;
        case 31: op=new RoutingGroupCaptainChangedOp(this); break;
        case 32: op=new RoutingGroupCreatedOp(this); break;
        case 33: op=new RoutingGroupDeletedOp(this); break;
        case 34: op=new RoutingGroupFlagsChangedOp(this); break;
        case 35: op=new RoutingGroupInvitationOp(this); break;
        case 36: op=new RoutingGroupJoinAttemptOp(this); break;
        case 37: op=new RoutingGroupMaxPlayersChangedOp(this); break;
        case 71: op=new RoutingGroupMemberCountOp(this); break;
        case 38: op=new RoutingGroupNameChangedOp(this); break;
        case 39: op=new RoutingGroupObserverCountOp(this); break;
        case 44: op=new RoutingMemberFlagsChangedOp(this); break;
        case 47: op=new RoutingPeerChatOp(this); break;
        case 48: op=new RoutingPeerDataOp(this); break;
        case 57: op=new RoutingServerAlertOp(this); break;
        case 58: op=new RoutingServerShutdownAbortedOp(this); break;
        case 59: op=new RoutingServerShutdownStartedOp(this); break;
        case 69: op=new RoutingYouWereBannedOp(this); break;
        case 70: op=new RoutingYouWereMutedOp(this); break;
        }
    } else
        op=Virtual28(messageType);
    // LOST-DEBUG: none: exact only.
    EE_LOST_TRACE(op.get());
    EE_LOST_TRACE(op.get());
    if(op.get()!=NULL)
        CalcCompletion(op.get());
    return op;
}
}

namespace WONAPI {
// FUNCTION: WON 0x006ba100
RoutingClientInfoPtr RoutingConnection::Virtual1C()
{
    return new RoutingClientInfo;
}
// FUNCTION: WON 0x006ba230
RoutingMemberInfoPtr RoutingConnection::Virtual20()
{
    return new RoutingMemberInfo;
}
// FUNCTION: WON 0x006ba300
RoutingGroupInfoPtr RoutingConnection::Virtual24()
{
    return new RoutingGroupInfo;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK CD-key-ID operation dispatch.
void RoutingConnection::BanClientByCDKeyId(unsigned short groupId,bool isBanned,
    unsigned long seconds,const std::wstring& comment,unsigned long cdKeyId,
    CompletionBase<AsyncOpPtr>* completion)
{
    SmartPtr<RoutingBanClientOp> op=new RoutingBanClientOp(this);
    op->mGroupId=groupId;
    op->mIsBanned=isBanned;
    op->mBanTime=seconds;
    op->SetBanComment(comment);
    op->mWONId=cdKeyId;
    op->mBanByWONId=true;
    op->mWONIdIsCDKeyId=true;
    op->SetCompletion(completion);
    op->Run(-1);
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing routing operation APIs.
#include "ee/wonapi/WONRoutingGroupAdmin.h"
#include "ee/wonapi/WONRoutingDetectFirewallOp.h"
#include "ee/wonapi/WONRoutingDisconnectClientOp.h"
namespace WONAPI {
void RoutingConnection::DisconnectClient(CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingDisconnectClientOp> anOp = new RoutingDisconnectClientOp(this);
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::GetClientCDKeyIdList(CompletionBase<AsyncOpPtr> *theCompletion, bool bDoSubscribe)
{
	SmartPtr<RoutingReadDataObjectOp> anOp = new RoutingReadDataObjectOp(this);
	anOp->mDataType="_KeyId";
	anOp->mLinkId=0xffff;
	anOp->mSubscribe=bDoSubscribe;
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::GetClientListEx(CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingGetClientListOp> anOp = new RoutingGetClientListOp(this);
    anOp->mIsEx=true;
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);	
}

void RoutingConnection::SetGroupName(unsigned short theGroupId, const std::wstring &theName, CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingSetGroupNameOp> anOp = new RoutingSetGroupNameOp(this,theGroupId,theName);
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::SetGroupMaxPlayers(unsigned short theGroupId, unsigned short theMaxPlayers, CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingSetGroupMaxPlayersOp> anOp = new RoutingSetGroupMaxPlayersOp(this,theGroupId,theMaxPlayers);
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::RelinquishCaptaincy(unsigned short theGroupId, unsigned short theNewCaptainId, CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingRelinquishCaptaincyOp> anOp = new RoutingRelinquishCaptaincyOp(this,theGroupId,theNewCaptainId);
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::SendServerAlertWithRecipientList(const std::wstring &theAlertText, std::list<unsigned short>& recipientList, CompletionBase<AsyncOpPtr> *theCompletion)
{
	if (recipientList.empty())
		return;

	SmartPtr<RoutingSendServerAlertOp> anOp = new RoutingSendServerAlertOp(this,theAlertText);

	std::list<unsigned short>::const_iterator anItr = recipientList.begin();
	for (; anItr != recipientList.end(); ++anItr)
		anOp->mRecipientList.push_back(*anItr);

	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::DetectFirewall(unsigned short theListenPort, unsigned long theMaxConnectSeconds, bool doListen, CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingDetectFirewallOp> anOp = new RoutingDetectFirewallOp(this);
	anOp->SetListenPort(theListenPort);
	anOp->SetMaxConnectWaitTime(theMaxConnectSeconds);
	anOp->SetDoListen(doListen);
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::MuteClientByCDKeyId(unsigned short theGroupId, bool isMuted, unsigned long theSeconds, 
	const std::wstring &theComment, unsigned long theCDKeyId, CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingMuteClientOp> anOp = new RoutingMuteClientOp(this);
	anOp->mGroupId=theGroupId;
	anOp->mIsMuted=isMuted;
	anOp->mMuteTime=theSeconds;
	anOp->mMuteComment=theComment;
	anOp->mWONId=theCDKeyId;
    anOp->mMuteByWONId=true;
    anOp->mWONIdIsCDKeyId=true;
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}

void RoutingConnection::GetBadUserList(RoutingBadUserListType theListType, unsigned short theGroupId, unsigned char theFlags,
									   CompletionBase<AsyncOpPtr> *theCompletion)
{
	SmartPtr<RoutingGetBadUserListOp> anOp = new RoutingGetBadUserListOp(this,theListType,theGroupId,theFlags);
	anOp->mCompletion=theCompletion;
	anOp->Run(-1);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ba3f0
// (public: virtual __thiscall WONAPI::RoutingGroupInfo::~RoutingGroupInfo(void)    [??1RoutingGroupInfo@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ba490
// (protected: virtual void * __thiscall WONAPI::RoutingConnection::`scalar deleting destructor'(unsigned int)    [??_GRoutingConnection@WONAPI@@MAEPAXI@Z)
// TEMPLATE: WON 0x006bb290
// (public: virtual __thiscall WONAPI::RoutingClientJoinedServerOp::~RoutingClientJoinedServerOp(void)    [??1RoutingClientJoinedServerOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bd320
// (public: virtual __thiscall WONAPI::RoutingGetMembersOfGroupOp::~RoutingGetMembersOfGroupOp(void)    [??1RoutingGetMembersOfGroupOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bce50
// (public: virtual long __thiscall WONAPI::RoutingGetClientListOp::GetType(void)const     [?GetType@RoutingGetClientListOp@WONAPI@@UBEJXZ])
// TEMPLATE: WON 0x006bd0a0
// (public: virtual long __thiscall WONAPI::RoutingGetGroupListOp::GetType(void)const     [?GetType@RoutingGetGroupListOp@WONAPI@@UBEJXZ])
// TEMPLATE: WON 0x006bd2f0
// (public: virtual long __thiscall WONAPI::RoutingGetMembersOfGroupOp::GetType(void)const     [?GetType@RoutingGetMembersOfGroupOp@WONAPI@@UBEJXZ])
// TEMPLATE: WON 0x006bfa10
// (public: virtual long __thiscall WONAPI::RoutingGetBadUserListOp::GetType(void)const     [?GetType@RoutingGetBadUserListOp@WONAPI@@UBEJXZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ba280
// (public: virtual void * __thiscall WONAPI::RoutingMemberInfo::`scalar deleting destructor'(unsigned int)    [??_GRoutingMemberInfo@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006ba2a0
// (public: virtual __thiscall WONAPI::RoutingMemberInfo::~RoutingMemberInfo(void)    [??1RoutingMemberInfo@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006ba3d0
// (public: virtual void * __thiscall WONAPI::RoutingGroupInfo::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupInfo@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006bb050
// (public: virtual __thiscall WONAPI::RoutingOp::~RoutingOp(void)    [??1RoutingOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bd0b0
// (public: virtual void * __thiscall WONAPI::RoutingGetGroupListOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGetGroupListOp@WONAPI@@UAEP)
// TEMPLATE: WON 0x006bd0d0
// (public: virtual __thiscall WONAPI::RoutingGetGroupListOp::~RoutingGetGroupListOp(void)    [??1RoutingGetGroupListOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bd7b0
// (public: virtual void * __thiscall WONAPI::RoutingCancelJoinGroupOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingCancelJoinGroupOp@WONAPI)
// TEMPLATE: WON 0x006bd7d0
// (public: virtual __thiscall WONAPI::RoutingCancelJoinGroupOp::~RoutingCancelJoinGroupOp(void)    [??1RoutingCancelJoinGroupOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bd950
// (public: virtual void * __thiscall WONAPI::RoutingLeaveGroupOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingLeaveGroupOp@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x006bd970
// (public: virtual __thiscall WONAPI::RoutingLeaveGroupOp::~RoutingLeaveGroupOp(void)    [??1RoutingLeaveGroupOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bdd30
// (public: virtual void * __thiscall WONAPI::RoutingSetGroupFlagsOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingSetGroupFlagsOp@WONAPI@@UA)
// TEMPLATE: WON 0x006bdd50
// (public: virtual __thiscall WONAPI::RoutingSetGroupFlagsOp::~RoutingSetGroupFlagsOp(void)    [??1RoutingSetGroupFlagsOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006be2f0
// (public: virtual void * __thiscall WONAPI::RoutingAbortServerShutdownOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingAbortServerShutdownO)
// TEMPLATE: WON 0x006be310
// (public: virtual __thiscall WONAPI::RoutingAbortServerShutdownOp::~RoutingAbortServerShutdownOp(void)    [??1RoutingAbortServerShutdownOp@WONAPI@@UAE@X)
// TEMPLATE: WON 0x006be4a0
// (public: virtual void * __thiscall WONAPI::RoutingSetClientFlagsOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingSetClientFlagsOp@WONAPI@@)
// TEMPLATE: WON 0x006be4c0
// (public: virtual __thiscall WONAPI::RoutingSetClientFlagsOp::~RoutingSetClientFlagsOp(void)    [??1RoutingSetClientFlagsOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bf4f0
// (public: virtual void * __thiscall WONAPI::RoutingBecomeModeratorOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingBecomeModeratorOp@WONAPI)
// TEMPLATE: WON 0x006bf510
// (public: virtual __thiscall WONAPI::RoutingBecomeModeratorOp::~RoutingBecomeModeratorOp(void)    [??1RoutingBecomeModeratorOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bb0e0
// (public: virtual void * __thiscall WONAPI::RoutingClientFlagsChangedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingClientFlagsChangedOp@)
// TEMPLATE: WON 0x006bb100
// (public: virtual __thiscall WONAPI::RoutingClientFlagsChangedOp::~RoutingClientFlagsChangedOp(void)    [??1RoutingClientFlagsChangedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb190
// (public: virtual void * __thiscall WONAPI::RoutingClientJoinedGroupOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingClientJoinedGroupOp@WO)
// TEMPLATE: WON 0x006bb1b0
// (public: virtual __thiscall WONAPI::RoutingClientJoinedGroupOp::~RoutingClientJoinedGroupOp(void)    [??1RoutingClientJoinedGroupOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb270
// (public: virtual void * __thiscall WONAPI::RoutingClientJoinedServerOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingClientJoinedServerOp@)
// TEMPLATE: WON 0x006bb340
// (public: virtual void * __thiscall WONAPI::RoutingClientLeftGroupOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingClientLeftGroupOp@WONAPI)
// TEMPLATE: WON 0x006bb360
// (public: virtual __thiscall WONAPI::RoutingClientLeftGroupOp::~RoutingClientLeftGroupOp(void)    [??1RoutingClientLeftGroupOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb3f0
// (public: virtual void * __thiscall WONAPI::RoutingClientLeftServerOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingClientLeftServerOp@WONA)
// TEMPLATE: WON 0x006bb410
// (public: virtual __thiscall WONAPI::RoutingClientLeftServerOp::~RoutingClientLeftServerOp(void)    [??1RoutingClientLeftServerOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb4a0
// (public: virtual void * __thiscall WONAPI::RoutingDataObjectCreatedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingDataObjectCreatedOp@WO)
// TEMPLATE: WON 0x006bb4c0
// (public: virtual __thiscall WONAPI::RoutingDataObjectCreatedOp::~RoutingDataObjectCreatedOp(void)    [??1RoutingDataObjectCreatedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb5e0
// (public: virtual void * __thiscall WONAPI::RoutingDataObjectDeletedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingDataObjectDeletedOp@WO)
// TEMPLATE: WON 0x006bb600
// (public: virtual __thiscall WONAPI::RoutingDataObjectDeletedOp::~RoutingDataObjectDeletedOp(void)    [??1RoutingDataObjectDeletedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb6f0
// (public: virtual void * __thiscall WONAPI::RoutingDataObjectModifiedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingDataObjectModifiedOp@)
// TEMPLATE: WON 0x006bb710
// (public: virtual __thiscall WONAPI::RoutingDataObjectModifiedOp::~RoutingDataObjectModifiedOp(void)    [??1RoutingDataObjectModifiedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb830
// (public: virtual void * __thiscall WONAPI::RoutingGroupCaptainChangedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupCaptainChangedO)
// TEMPLATE: WON 0x006bb850
// (public: virtual __thiscall WONAPI::RoutingGroupCaptainChangedOp::~RoutingGroupCaptainChangedOp(void)    [??1RoutingGroupCaptainChangedOp@WONAPI@@UAE@X)
// TEMPLATE: WON 0x006bb8e0
// (public: virtual void * __thiscall WONAPI::RoutingGroupCreatedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupCreatedOp@WONAPI@@UAEP)
// TEMPLATE: WON 0x006bb900
// (public: virtual __thiscall WONAPI::RoutingGroupCreatedOp::~RoutingGroupCreatedOp(void)    [??1RoutingGroupCreatedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bb9c0
// (public: virtual void * __thiscall WONAPI::RoutingGroupDeletedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupDeletedOp@WONAPI@@UAEP)
// TEMPLATE: WON 0x006bb9e0
// (public: virtual __thiscall WONAPI::RoutingGroupDeletedOp::~RoutingGroupDeletedOp(void)    [??1RoutingGroupDeletedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bba70
// (public: virtual void * __thiscall WONAPI::RoutingGroupFlagsChangedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupFlagsChangedOp@WO)
// TEMPLATE: WON 0x006bba90
// (public: virtual __thiscall WONAPI::RoutingGroupFlagsChangedOp::~RoutingGroupFlagsChangedOp(void)    [??1RoutingGroupFlagsChangedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bbb20
// (public: virtual void * __thiscall WONAPI::RoutingGroupInvitationOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupInvitationOp@WONAPI)
// TEMPLATE: WON 0x006bbb40
// (public: virtual __thiscall WONAPI::RoutingGroupInvitationOp::~RoutingGroupInvitationOp(void)    [??1RoutingGroupInvitationOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bbc00
// (public: virtual void * __thiscall WONAPI::RoutingGroupJoinAttemptOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupJoinAttemptOp@WONA)
// TEMPLATE: WON 0x006bbc20
// (public: virtual __thiscall WONAPI::RoutingGroupJoinAttemptOp::~RoutingGroupJoinAttemptOp(void)    [??1RoutingGroupJoinAttemptOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bbce0
// (public: virtual void * __thiscall WONAPI::RoutingGroupMemberCountOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupMemberCountOp@WONA)
// TEMPLATE: WON 0x006bbd00
// (public: virtual __thiscall WONAPI::RoutingGroupMemberCountOp::~RoutingGroupMemberCountOp(void)    [??1RoutingGroupMemberCountOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bbd90
// (public: virtual void * __thiscall WONAPI::RoutingGroupMaxPlayersChangedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupMaxPlayersCh)
// TEMPLATE: WON 0x006bbdb0
// (public: virtual __thiscall WONAPI::RoutingGroupMaxPlayersChangedOp::~RoutingGroupMaxPlayersChangedOp(void)    [??1RoutingGroupMaxPlayersChangedOp@WONA)
// TEMPLATE: WON 0x006bbe40
// (public: virtual void * __thiscall WONAPI::RoutingGroupNameChangedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupNameChangedOp@WONA)
// TEMPLATE: WON 0x006bbe60
// (public: virtual __thiscall WONAPI::RoutingGroupNameChangedOp::~RoutingGroupNameChangedOp(void)    [??1RoutingGroupNameChangedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bbf20
// (public: virtual void * __thiscall WONAPI::RoutingGroupObserverCountOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGroupObserverCountOp@)
// TEMPLATE: WON 0x006bbf40
// (public: virtual __thiscall WONAPI::RoutingGroupObserverCountOp::~RoutingGroupObserverCountOp(void)    [??1RoutingGroupObserverCountOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bbfd0
// (public: virtual void * __thiscall WONAPI::RoutingMemberFlagsChangedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingMemberFlagsChangedOp@)
// TEMPLATE: WON 0x006bbff0
// (public: virtual __thiscall WONAPI::RoutingMemberFlagsChangedOp::~RoutingMemberFlagsChangedOp(void)    [??1RoutingMemberFlagsChangedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bc080
// (public: virtual void * __thiscall WONAPI::RoutingPeerChatOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingPeerChatOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006bc0a0
// (public: virtual __thiscall WONAPI::RoutingPeerChatOp::~RoutingPeerChatOp(void)    [??1RoutingPeerChatOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bc1a0
// (public: virtual void * __thiscall WONAPI::RoutingPeerDataOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingPeerDataOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006bc1c0
// (public: virtual __thiscall WONAPI::RoutingPeerDataOp::~RoutingPeerDataOp(void)    [??1RoutingPeerDataOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bc270
// (public: virtual void * __thiscall WONAPI::RoutingServerAlertOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingServerAlertOp@WONAPI@@UAEPAX)
// TEMPLATE: WON 0x006bc290
// (public: virtual __thiscall WONAPI::RoutingServerAlertOp::~RoutingServerAlertOp(void)    [??1RoutingServerAlertOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bc350
// (public: virtual void * __thiscall WONAPI::RoutingServerShutdownAbortedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingServerShutdownAbor)
// TEMPLATE: WON 0x006bc370
// (public: virtual __thiscall WONAPI::RoutingServerShutdownAbortedOp::~RoutingServerShutdownAbortedOp(void)    [??1RoutingServerShutdownAbortedOp@WONAPI@)
// TEMPLATE: WON 0x006bc430
// (public: virtual void * __thiscall WONAPI::RoutingServerShutdownStartedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingServerShutdownStar)
// TEMPLATE: WON 0x006bc450
// (public: virtual __thiscall WONAPI::RoutingServerShutdownStartedOp::~RoutingServerShutdownStartedOp(void)    [??1RoutingServerShutdownStartedOp@WONAPI@)
// TEMPLATE: WON 0x006bc510
// (public: virtual void * __thiscall WONAPI::RoutingYouWereBannedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingYouWereBannedOp@WONAPI@@UA)
// TEMPLATE: WON 0x006bc530
// (public: virtual __thiscall WONAPI::RoutingYouWereBannedOp::~RoutingYouWereBannedOp(void)    [??1RoutingYouWereBannedOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bc5f0
// (public: virtual void * __thiscall WONAPI::RoutingYouWereMutedOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingYouWereMutedOp@WONAPI@@UAEP)
// TEMPLATE: WON 0x006bc610
// (public: virtual __thiscall WONAPI::RoutingYouWereMutedOp::~RoutingYouWereMutedOp(void)    [??1RoutingYouWereMutedOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bce60
// (public: virtual void * __thiscall WONAPI::RoutingGetClientListOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGetClientListOp@WONAPI@@UA)
// TEMPLATE: WON 0x006bce80
// (public: virtual __thiscall WONAPI::RoutingGetClientListOp::~RoutingGetClientListOp(void)    [??1RoutingGetClientListOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bd300
// (public: virtual void * __thiscall WONAPI::RoutingGetMembersOfGroupOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGetMembersOfGroupOp@WO)
// TEMPLATE: WON 0x006bdf00
// (public: virtual void * __thiscall WONAPI::RoutingSendServerAlertOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingSendServerAlertOp@WONAPI)
// TEMPLATE: WON 0x006bdf20
// (public: virtual __thiscall WONAPI::RoutingSendServerAlertOp::~RoutingSendServerAlertOp(void)    [??1RoutingSendServerAlertOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006be130
// (public: virtual void * __thiscall WONAPI::RoutingStartServerShutdownOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingStartServerShutdownO)
// TEMPLATE: WON 0x006be150
// (public: virtual __thiscall WONAPI::RoutingStartServerShutdownOp::~RoutingStartServerShutdownOp(void)    [??1RoutingStartServerShutdownOp@WONAPI@@UAE@X)
// TEMPLATE: WON 0x006be800
// (public: virtual void * __thiscall WONAPI::RoutingSendChatOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingSendChatOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006be820
// (public: virtual __thiscall WONAPI::RoutingSendChatOp::~RoutingSendChatOp(void)    [??1RoutingSendChatOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bebc0
// (public: virtual void * __thiscall WONAPI::RoutingSendDataOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingSendDataOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006bebe0
// (public: virtual __thiscall WONAPI::RoutingSendDataOp::~RoutingSendDataOp(void)    [??1RoutingSendDataOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006bfa20
// (public: virtual void * __thiscall WONAPI::RoutingGetBadUserListOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGetBadUserListOp@WONAPI@@)
// TEMPLATE: WON 0x006bfa40
// (public: virtual __thiscall WONAPI::RoutingGetBadUserListOp::~RoutingGetBadUserListOp(void)    [??1RoutingGetBadUserListOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ba1a0
// (public: virtual void * __thiscall WONAPI::RoutingClientInfo::`scalar deleting destructor'(unsigned int)    [??_GRoutingClientInfo@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006ba1c0
// (public: virtual __thiscall WONAPI::RoutingClientInfo::~RoutingClientInfo(void)    [??1RoutingClientInfo@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bb0d0
// (public: virtual long __thiscall WONAPI::RoutingClientFlagsChangedOp::GetType(void)const )
// TEMPLATE: WON 0x006bb180
// (public: virtual long __thiscall WONAPI::RoutingClientJoinedGroupOp::GetType(void)const )
// TEMPLATE: WON 0x006bb260
// (public: virtual long __thiscall WONAPI::RoutingClientJoinedServerOp::GetType(void)const )
// TEMPLATE: WON 0x006bb330
// (public: virtual long __thiscall WONAPI::RoutingClientLeftGroupOp::GetType(void)const )
// TEMPLATE: WON 0x006bb3e0
// (public: virtual long __thiscall WONAPI::RoutingClientLeftServerOp::GetType(void)const )
// TEMPLATE: WON 0x006bb490
// (public: virtual long __thiscall WONAPI::RoutingDataObjectCreatedOp::GetType(void)const )
// TEMPLATE: WON 0x006bb5d0
// (public: virtual long __thiscall WONAPI::RoutingDataObjectDeletedOp::GetType(void)const )
// TEMPLATE: WON 0x006bb6e0
// (public: virtual long __thiscall WONAPI::RoutingDataObjectModifiedOp::GetType(void)const )
// TEMPLATE: WON 0x006bb820
// (public: virtual long __thiscall WONAPI::RoutingGroupCaptainChangedOp::GetType(void)const )
// TEMPLATE: WON 0x006bb8d0
// (public: virtual long __thiscall WONAPI::RoutingGroupCreatedOp::GetType(void)const )
// TEMPLATE: WON 0x006bb9b0
// (public: virtual long __thiscall WONAPI::RoutingGroupDeletedOp::GetType(void)const )
// TEMPLATE: WON 0x006bba60
// (public: virtual long __thiscall WONAPI::RoutingGroupFlagsChangedOp::GetType(void)const )
// TEMPLATE: WON 0x006bbb10
// (public: virtual long __thiscall WONAPI::RoutingGroupInvitationOp::GetType(void)const )
// TEMPLATE: WON 0x006bbbf0
// (public: virtual long __thiscall WONAPI::RoutingGroupJoinAttemptOp::GetType(void)const )
// TEMPLATE: WON 0x006bbcd0
// (public: virtual long __thiscall WONAPI::RoutingGroupMemberCountOp::GetType(void)const )
// TEMPLATE: WON 0x006bbd80
// (public: virtual long __thiscall WONAPI::RoutingGroupMaxPlayersChangedOp::GetType(void)const )
// TEMPLATE: WON 0x006bbe30
// (public: virtual long __thiscall WONAPI::RoutingGroupNameChangedOp::GetType(void)const )
// TEMPLATE: WON 0x006bbf10
// (public: virtual long __thiscall WONAPI::RoutingGroupObserverCountOp::GetType(void)const )
// TEMPLATE: WON 0x006bbfc0
// (public: virtual long __thiscall WONAPI::RoutingMemberFlagsChangedOp::GetType(void)const )
// TEMPLATE: WON 0x006bc070
// (public: virtual long __thiscall WONAPI::RoutingPeerChatOp::GetType(void)const )
// TEMPLATE: WON 0x006bc190
// (public: virtual long __thiscall WONAPI::RoutingPeerDataOp::GetType(void)const )
// TEMPLATE: WON 0x006bc260
// (public: virtual long __thiscall WONAPI::RoutingServerAlertOp::GetType(void)const )
// TEMPLATE: WON 0x006bc340
// (public: virtual long __thiscall WONAPI::RoutingServerShutdownAbortedOp::GetType(void)const )
// TEMPLATE: WON 0x006bc420
// (public: virtual long __thiscall WONAPI::RoutingServerShutdownStartedOp::GetType(void)const )
// TEMPLATE: WON 0x006bc500
// (public: virtual long __thiscall WONAPI::RoutingYouWereBannedOp::GetType(void)const )
// TEMPLATE: WON 0x006bc5e0
// (public: virtual long __thiscall WONAPI::RoutingYouWereMutedOp::GetType(void)const )
// TEMPLATE: WON 0x006ca2c0
// (protected: void __thiscall std::list<class WONAPI::SmartPtr<class WONAPI::RoutingOp>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::Routing)
// TEMPLATE: WON 0x006ca410
// (public: class std::list<struct WONAPI::RoutingBadUser,class std::allocator<struct WONAPI::RoutingBadUser> >::iterator __thiscall std::list<struct WONA)
// TEMPLATE: WON 0x006cbaf0
// (public: void __thiscall std::list<unsigned short,class std::allocator<unsigned short> >::assign(class std::list<unsigned short,class std::allocator<un)
// TEMPLATE: WON 0x006c9600
// (public: void __thiscall std::list<class WONAPI::SmartPtr<class WONAPI::RoutingOp>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::RoutingOp>)
// TEMPLATE: WON 0x007322c0
// (protected: __thiscall std::_Tree_val<struct std::less<unsigned short>,class std::allocator<struct std::pair<unsigned short const ,class WONAPI::SmartP)
