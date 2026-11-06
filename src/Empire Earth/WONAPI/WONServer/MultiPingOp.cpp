// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK ping family; no Native/primary/physical claim.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Missing unit: proposed existing server-operation family profile.
#include <windows.h>
#include <time.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONPing.h"
using namespace WONAPI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiPingOp::MultiPingOp()
{
	mBatchSize = 20;
	mMaxFailures = 3;
	mMaxPingTime = 3000;
	mMaxPingReplySize = 2048;
	mOrderListByPingTime = true;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::AddServer(const IPAddr &theAddr)
{
	mPingList.push_back(new MultiPingStruct(theAddr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::AddServers(const DirEntityList &theDir, const wchar_t *theNameFilter)
{
	wstring aNameFilter;
	if(theNameFilter!=NULL)
		aNameFilter = theNameFilter;

	DirEntityList::const_iterator anItr = theDir.begin();
	while(anItr!=theDir.end())
	{
		const DirEntity *anEntity = *anItr;
		if(anEntity->mType!='D')
		{
			if(aNameFilter.empty() || aNameFilter==anEntity->mName)
			{
				IPAddr anAddr = anEntity->GetNetAddrAsIP();
				if(anAddr.IsValid())
					mPingList.push_back(new MultiPingStruct(anAddr,anEntity));
			}
		}

		++anItr;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::ClearServers()
{
	mPingList.clear();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ByteBufferPtr MultiPingOp::GetRequest(MultiPingStruct *theStruct)
{
	theStruct->mPingId = rand();
	theStruct->mStartPingTick = GetTickCount();

	WriteBuffer aBuf(0,true);
	aBuf.AppendByte(3);
	aBuf.AppendByte(1);
	aBuf.AppendByte(5);
	aBuf.AppendLong(theStruct->mPingId);
	aBuf.AppendBool(false);
	return aBuf.ToByteBuffer(true);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
MultiPingOp::PingAction MultiPingOp::HandleResponse(MultiPingStruct *theStruct, const ByteBuffer *theResponse)
{
	ReadBuffer aBuf(theResponse->data(),theResponse->length());
	try
	{
		unsigned char aHeader = aBuf.ReadByte();
		unsigned char aService = aBuf.ReadByte();
		unsigned char aMsg = aBuf.ReadByte();

		if(aHeader!=3 || aService!=1 || aMsg!=6)
			return PingAction_InvalidReply;

		int anId = aBuf.ReadLong();
		if(anId!=theStruct->mPingId)
			return PingAction_InvalidReply;

		theStruct->mPingTime = GetTickCount()-theStruct->mStartPingTick;
		return PingAction_Done;
	}
	catch(ReadBufferException&)
	{
		return PingAction_InvalidReply;
	}

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::StaticRecvCallback(AsyncOpPtr theOp, SmartPtr<RefCount> theParam)
{
	MultiPingOp *thisOp = (MultiPingOp*)theParam.get();
	RecvBytesFromOp *aRecvOp = (RecvBytesFromOp*)theOp.get();
	thisOp->RecvCallback(aRecvOp);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::RecvCallback(RecvBytesFromOp *theOp)
{
	if(mStatus!=100000 || theOp->GetSocket()!=mSocket.get())
		return;

	if(!HandleResponsePrv(theOp->GetStatus(),theOp->GetBytes(),theOp->GetAddr()))
		Fn29F220(theOp->GetStatus());

	if(mPingMap.size()==0)
	{
		if(mPingList.size()==0)
			Fn29F220((WONStatus)0);
		else
			PingBatchAsync();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::StaticTimeoutCallback(AsyncOpPtr theOp, SmartPtr<RefCount> theParam)
{
	MultiPingOp *thisOp = (MultiPingOp*)theParam.get();
	thisOp->TimeoutCallback((AsyncOp*)theOp.get());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::TimeoutCallback(AsyncOp *theOp)
{
	if(mStatus!=100000 || theOp!=mRecvTimeoutOp.get())
		return;

	mRecvTimeoutOp = NULL;
	FinishBatch();
	PingBatchAsync();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::PrepareBatch()
{
	mPingMap.clear();

	MultiPingList::iterator anItr = mPingList.begin();
	int aCount = 0;

	while(anItr!=mPingList.end() && aCount<mBatchSize)
	{
		mPingMap[(*anItr)->mAddr] = *anItr;
		mPingList.erase(anItr++);
		aCount++;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::FinishBatch()
{
	MultiPingMap::iterator anItr = mPingMap.begin();
	while(anItr!=mPingMap.end())
	{
		MultiPingStruct *aStruct = anItr->second;
		aStruct->mNumFailures++;
		if(aStruct->mNumFailures >= mMaxFailures)
			mFinishedList.push_back(aStruct);
		else
			mPingList.push_back(aStruct);

		++anItr;
	}

	mPingMap.clear();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::PingBatchAsync()
{
	if(mRecvTimeoutOp.get()!=NULL)
	{
		mRecvTimeoutOp->Kill(true);
		mRecvTimeoutOp = NULL;
	}

	PrepareBatch();
	MultiPingMap::iterator anItr = mPingMap.begin();
	while(anItr!=mPingMap.end())
	{
		ByteBufferPtr aPing = GetRequest(anItr->second);
		mSocket->Fn2B4740(new SendBytesToOp(aPing,anItr->first), 0xffffffff);
		++anItr;
	}

	mRecvTimeoutOp = new AsyncOp;
	mRecvTimeoutOp->mCompletion = new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(StaticTimeoutCallback,this);
	mRecvTimeoutOp->Run(mMaxPingTime);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
DWORD MultiPingOp::ResponseTimeLeft(DWORD theStartTick)
{
	DWORD anElapsed = GetTickCount() - theStartTick;
	if(anElapsed > mMaxPingTime)
		return 0;


	DWORD anAbsTimeLeft = TimeLeft();
	DWORD aTimeLeft = mMaxPingTime - anElapsed;

	if(aTimeLeft < anAbsTimeLeft)
		return aTimeLeft;
	else
		return anAbsTimeLeft;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool MultiPingOp::HandleResponsePrv(WONStatus theStatus, const ByteBuffer *theResponse, const IPAddr &theAddr)
{
	if(theStatus!=(WONStatus)0 && theStatus!=(WONStatus)10040)
		return false;

	if(theStatus==(WONStatus)0)
	{
		MultiPingMap::iterator anItr = mPingMap.find(theAddr);
		if(anItr!=mPingMap.end())
		{
			PingAction anAction = HandleResponse(anItr->second,theResponse);
			if(anAction==PingAction_Done)
			{
				mFinishedList.push_back(anItr->second);
				mPingMap.erase(anItr);
			}
			else if(anAction==PingAction_More)
			{
				mPingList.push_back(anItr->second);
				mPingMap.erase(anItr);
			}
		}
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus MultiPingOp::PingBatchBlock()
{
	PrepareBatch();

	MultiPingMap::iterator anItr = mPingMap.begin();
	WONStatus aStatus;
	while(TimeLeft()>0 && anItr!=mPingMap.end())
	{
		ByteBufferPtr aPing = GetRequest(anItr->second);
		aStatus = mSocket->SendBytesTo(aPing, anItr->first, TimeLeft());
		if(aStatus!=(WONStatus)0)
			return aStatus;

		++anItr;
	}

	DWORD aStartTick = GetTickCount();
	while(ResponseTimeLeft(aStartTick)>0 && !mPingMap.empty())
	{
		ByteBufferPtr aReply;
		IPAddr anAddr;
		aStatus = mSocket->RecvBytesFrom(aReply,mMaxPingReplySize,anAddr,ResponseTimeLeft(aStartTick));
		if(!HandleResponsePrv(aStatus,aReply,anAddr))
			return aStatus;

	}

	return aStatus;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::Virtual04()
{
	srand(time(NULL));
	mSocket = new BlockingSocket(AsyncSocket::UDP);
	mSocket->Fn2B0D90(0,false);
	mFinishedList.clear();

	if(mRunMode==OP_MODE_ASYNC)
	{
		mSocket->Fn2B3BF0(new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(StaticRecvCallback,this));
		mSocket->Fn2B4500(new RecvBytesFromOp(mMaxPingReplySize));
		PingBatchAsync();
		return;
	}

	WONStatus aStatus;
	while(TimeLeft()>0 && !mPingList.empty())
	{
		aStatus = PingBatchBlock();
		FinishBatch();
		if(aStatus!=(WONStatus)0 && aStatus!=(WONStatus)100001)
		{
			Fn29F220(aStatus);
			return; 
		}
	}

	Fn29F220((WONStatus)0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void MultiPingOp::Virtual08()
{
	if(mSocket.get()!=NULL)
	{
		mSocket->Fn2B1AD0();
		mSocket = NULL;
	}

	if(mRecvTimeoutOp.get()!=NULL)
	{
		mRecvTimeoutOp->Kill(true);
		mRecvTimeoutOp = NULL;
	}

	FinishBatch();

	MultiPingList::iterator aListItr = mPingList.begin();
	while(aListItr!=mPingList.end())
	{
		mFinishedList.push_back(*aListItr);
		++aListItr;
	}
	mPingList.clear();

	if(mOrderListByPingTime)
	{
		typedef std::multimap<DWORD,MultiPingStructPtr> SortMap;
		SortMap aMap;
		aListItr = mFinishedList.begin();
		while(aListItr!=mFinishedList.end())
		{
			aMap.insert(SortMap::value_type((*aListItr)->mPingTime,*aListItr));
			++aListItr;
		}

		mFinishedList.clear();

		SortMap::iterator aMapItr = aMap.begin();
		while(aMapItr!=aMap.end())
		{
			mFinishedList.push_back(aMapItr->second);
			aMapItr++;
		}
	}

}
	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
SmartPtr<ServerContext> MultiPingOp::GetServerContext(bool discardNonResponsiveServers)
{
	SmartPtr<ServerContext> aContext = new ServerContext;
	MultiPingList::iterator anItr = mFinishedList.begin();
	while(anItr!=mFinishedList.end())
	{
		MultiPingStruct *aStruct = *anItr;
		if(aStruct->mPingTime!=-1 || !discardNonResponsiveServers)
			aContext->AddAddress(aStruct->mAddr);

		++anItr;
	}

	aContext->mB08 = false;
	return aContext;
}
