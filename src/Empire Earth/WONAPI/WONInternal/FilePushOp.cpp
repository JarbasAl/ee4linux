// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK file push state machine.
// Missing unit proposed existing ServerOp/socket family profile.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONFilePush.h"



using namespace WONAPI;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FilePushOp::FilePushOp(const IPAddr &theAddr) :
	mConnectAddr(theAddr), mIsCompress(false), mBlockTransferSize(65535)
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
FilePushOp::~FilePushOp()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::CallbackHook(AsyncOp *theOp, int theId)
{
	if(theOp->GetStatus()!=0)
	{
		Fn29F220(theOp->GetStatus());
		return true;
	}

	switch(theId)
	{
		case TRACK_AUTH: 
			if(GetEncryptKey((PeerAuthOp*)theOp))
				SendStreamFileRequest(); 
			return true;
	
		case TRACK_STATUS_REPLY: 
		{
			RecvMsgOp *anOp = (RecvMsgOp*)theOp;
			HandleFactStatusReply(anOp->GetMsg());
			return true;
		}

		case TRACK_ACK:
		{
			RecvBytesOp *anOp = (RecvBytesOp*)theOp;
			HandleAck(anOp->GetBytes()->data()[0]);
			return true;
		}

	}
	
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::CheckError(WONStatus theStatus)
{
	if(theStatus!=(WONStatus)0)
	{
		Fn29F220(theStatus);
		return true;
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::GetEncryptKey(PeerAuthOp *theOp)
{
	AuthSession *aSession = theOp->GetSession();
	if(aSession==NULL)
	{
		Fn29F220((WONStatus)100013);
		return false;
	}

	mEncryptKey = aSession->GetKey();
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FilePushOp::DoProgressCompletion()
{
	if(mProgressCompletion.get()!=NULL)
		mProgressCompletion->Complete(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::HandleFactStatusReply(const ByteBuffer *theReply)
{
	ReadBuffer aReply(theReply->data(), theReply->length());
	WONStatus aStatus = (WONStatus)100004;
	try
	{
		unsigned char aHeaderType = aReply.ReadByte();
		unsigned short aServiceType = aReply.ReadShort();
		unsigned short aMessageType = aReply.ReadShort();
		aStatus = (WONStatus)aReply.ReadShort();
		if(CheckError(aStatus))
			return false;
	}
	catch(ReadBufferException&)
	{
		Fn29F220((WONStatus)100012);
		return false;
	}

	if(mTotalBytesSent > 0) // this is the second status reply so we're done
	{
		DoProgressCompletion();
		Fn29F220((WONStatus)0);
		return false;
	}
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::RecvFactStatusReply()
{
	if((mRunMode==OP_MODE_ASYNC))
	{
		RecvMsgAsync(TRACK_STATUS_REPLY);
		return false;
	}

	ByteBufferPtr aReply;
	WONStatus aStatus = mSocket->RecvMsg(aReply, RecvTime());
	if(CheckError(aStatus))
		return false;

	return HandleFactStatusReply(aReply);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::HandleAck(char theAck)
{
	if(theAck==1)
	{
		DoProgressCompletion();
		if((mRunMode==OP_MODE_ASYNC))
			SendFileChunk();

		return true;
	}

	return RecvFactStatusReply();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::RecvAck()
{
	if((mRunMode==OP_MODE_ASYNC))
	{
		SmartPtr<RecvBytesOp> anOp = new RecvBytesOp(1,mSocket);
		Track(anOp, TRACK_ACK);
		mSocket->Fn2B4740(anOp,-1);
		return false;
	}

	ByteBufferPtr aByte;
	WONStatus aStatus = mSocket->RecvBytes(aByte, 1,0xffffffff);
	if(CheckError(aStatus))
		return false;

	return HandleAck(aByte->data()[0]);

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::SendFileChunk()
{		
	WriteBuffer aBuf(4,false);
	aBuf.Reserve(mBlockTransferSize + 20);

	int aLen = fread(aBuf.data(),1,mBlockTransferSize,mFile.GetFile());
	WONStatus aStatus = (WONStatus)100004;

	// Encrypt bytes and pack them in aBuf
	if(aLen > 0)
	{
		ByteBufferPtr anEncrypt = mEncryptKey.Encrypt(aBuf.data(),aLen);
		aBuf.AppendBytes(anEncrypt->data(), anEncrypt->length());
	}

	// Send bytes
	mLastBytesSent = aLen;
	mTotalBytesSent += aLen;
	if((mRunMode==OP_MODE_ASYNC))
		mSocket->Fn2B4740((SocketOp*)Track(new SendBytesOp(aBuf.ToByteBuffer(true)),0),-1);
	else
	{
		aStatus = mSocket->SendBytes(aBuf.ToByteBuffer(true), SendTime());
		if(CheckError(aStatus))
			return false;
	}

	if(aLen>0)
		return RecvAck();
	else 
		return RecvFactStatusReply();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool FilePushOp::SendStreamFileRequest()
{
	WriteBuffer aMsg(mLengthFieldSize,true);
	aMsg.AppendByte(5);					// Small Message
	aMsg.AppendShort(5);				// Small Factory Service
	aMsg.AppendShort(21);				// FactStreamFile Msg
	aMsg.AppendString(mRemoteFileName);
	aMsg.AppendBool(mIsCompress);
	aMsg.AppendLong(mModifyTime.dwHighDateTime);
	aMsg.AppendLong(mModifyTime.dwLowDateTime);
	aMsg.AppendString(mComment);

	if((mRunMode==OP_MODE_ASYNC))
	{
		SendMsgAsync(aMsg.ToByteBuffer(true),TRACK_STREAM_REQUEST);
		RecvFactStatusReply();
		RecvAck();
		return false;
	}

	WONStatus aStatus = mSocket->SendMsg(aMsg.ToByteBuffer(true), SendTime());
	if(CheckError(aStatus))
		return false;

	if(!RecvFactStatusReply())
		return false;

	return RecvAck();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FilePushOp::Virtual04(void)
{
	mTotalBytesSent = 0;
	mLastBytesSent = 0;
	FILE *aFile = fopen(mFileName.c_str(),"rb");
	mFile.Attach(aFile);
	if(aFile==NULL)
	{
		Fn29F220((WONStatus)100007);
		return;
	}

	mSocket = new BlockingSocket(AsyncSocket::TCP);
	mSocket->SetLengthFieldSize(mLengthFieldSize);

	SmartPtr<PeerAuthOp> aPeerAuthOp = new PeerAuthOp(mConnectAddr, mAuthContext, mAuthType, mSocket,true);
	aPeerAuthOp->CopyMaxTimes(this);
	if((mRunMode==OP_MODE_ASYNC))
	{
		Track(aPeerAuthOp,TRACK_AUTH);
		aPeerAuthOp->Run(-1);
		return;
	}
	
	if(!aPeerAuthOp->RunBlock(TimeLeft()))
	{
		Fn29F220(aPeerAuthOp->GetStatus());
		return;
	}

	if(!GetEncryptKey(aPeerAuthOp))
		return;

	if(!SendStreamFileRequest())
		return;

	while(true)
	{
		if(!SendFileChunk())
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void FilePushOp::Virtual08(void)
{
	ServerOp::Virtual08();
	mFile.Detach();
}
