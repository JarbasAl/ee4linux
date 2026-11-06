// New native HTTP session/connection family; source filename/grouping is provisional.

// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <time.h>
#include <list>
#include <vector>
#include <map>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {

// FUNCTION: WON 0x006d28f0
void HTTPSession::AddOp(HTTPGetOp* inOp)
{
    AutoCrit guard(mDataCrit);
    HTTPConnectionPtr connection;
    HTTPConnectionPtr& stored = mConnectionMap[inOp->mHost];
    if (stored.get() == NULL) stored = new HTTPConnection(NULL);
    connection = stored;
    connection->QueueOp(inOp);
}

// FUNCTION: WON 0x006d2bc0
void HTTPSession::KillAll()
{
    AutoCrit guard(mDataCrit);
    for (ConnectionMap::iterator it=mConnectionMap.begin(); it!=mConnectionMap.end(); ++it)
        it->second->Kill();
}

// FUNCTION: WON 0x006d2c80
HTTPConnection::HTTPConnection(HTTPSession* inSession)
{
    mCurOp[1] = NULL;
    mCurOp[0] = NULL;
    mDoPipeline = true;
    mSession = inSession;
    mRetryMode = false;
    mNumPipelineFailures = 0;
    mNumPipelineSuccesses = 0;
}

// FUNCTION: WON 0x006d3600
void HTTPConnection::Kill()
{
    AutoCrit guard(mDataCrit);
    mRetryMode = false;
    mNumPipelineFailures = 0;
    mNumPipelineSuccesses = 0;
    mDoPipeline = true;
    for (int id=0; id<2; ++id) {
        if (mCurOp[id] != NULL) {
            mCurOp[id]->Kill(true);
            mCurOp[id] = NULL;
            mCurOpCompletion[id] = NULL;
        }
        for (OpQueue::iterator it=mOpQueue[id].begin(); it!=mOpQueue[id].end(); ++it) {
            HTTPGetOp* op = it->get();
            bool wasNotPending = op->mStatus != 100000;
            op->Kill(true);
            if (wasNotPending) op->ForceFinish(100002);
        }
        mOpQueue[id].clear();
    }
    if (mSocket.get() != NULL) {
        mSocket->Close();
        mSocket = NULL;
    }
}

// FUNCTION: WON 0x006d33a0
void HTTPConnection::QueueOp(HTTPGetOp* inOp)
{
    AutoCrit guard(mDataCrit);
    if (mDoPipeline) {
        inOp->mOnlySend = true;
        inOp->mOnlyRecv = false;
        QueueOp(inOp,0);
    } else {
        inOp->mOnlySend = false;
        inOp->mOnlyRecv = false;
        QueueOp(inOp,1);
    }
}
// FUNCTION: WON 0x006d3290
void HTTPConnection::QueueOp(HTTPGetOp* inOp,int inId)
{
    AutoCrit guard(mDataCrit);
    mOpQueue[inId].push_back(inOp);
    if (mOpQueue[inId].size() == 1) RunOp(inId);
}


// Additional HTTP session source intent; existing six bodies preserved separately.
// SCRATCH only, no canonical publication or normal capture yet.
// Existing HTTPConnection64/HTTPSession2c records are complete.

HTTPSession::HTTPSession()
{
}
// FUNCTION: WON 0x006d3430
void HTTPConnection::RunOp(int inId)
{
    AutoCrit guard(mDataCrit);
    if (mRetryMode || mOpQueue[inId].empty() || mCurOp[inId]!=NULL)
        return;
    SmartPtr<HTTPGetOp> op=mOpQueue[inId].front();
    mOpQueue[inId].pop_front();
    mCurOp[inId]=op.get();
    mCurOpCompletion[inId]=mCurOp[inId]->mCompletion;
    mCurOp[inId]->mCompletion=new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(
        StaticOpCallback,SmartPtr<RefCount>(this));
    mCurOp[inId]->SetCloseConnection(false);
    mCurOp[inId]->SetDoRangeResume(false);
    mCurOp[inId]->SetSocket(mSocket.get());
    mCurOp[inId]->Run(mCurOp[inId]->mTimeout);
}


// New native callback behavior; scratch only, no canonical publication or CL.

void HTTPConnection::StaticOpCallback(AsyncOpPtr inOp,SmartPtr<RefCount> inConnection)
{
    static_cast<HTTPConnection*>(inConnection.get())->OpCallback(
        static_cast<HTTPGetOp*>(inOp.get()));
}

// FUNCTION: WON 0x006d2e80
void HTTPConnection::OpCallback(HTTPGetOp* inOp)
{
    int anId;
    for (anId=0;anId<2;++anId)
        if (mCurOp[anId]==inOp)
            break;
    if (anId>=2)
        return;
    mCurOp[anId]=NULL;
    OpCompletionBasePtr aCompletion=mCurOpCompletion[anId];
    mCurOpCompletion[anId]=NULL;
    inOp->mCompletion=aCompletion;

    if (!mRetryMode) {
        mSocket=inOp->GetSocket();
        if (anId==1 && mSocket.get()!=NULL) {
            if (++mNumPipelineSuccesses>2)
                mNumPipelineFailures=0;
        }
        if (mSocket.get()==NULL && mDoPipeline &&
            (!mOpQueue[1].empty() || mCurOp[1]!=NULL || mCurOp[0]!=NULL)) {
            AutoCrit aGuard(mDataCrit);
            mRetryMode=true;
            if (mCurOp[0]!=NULL)
                mCurOp[0]->Kill(true);
            if (mCurOp[1]!=NULL)
                mCurOp[1]->Kill(true);
        }
    }

    if (mRetryMode) {
        if (anId!=0 && inOp->mStatus==0) {
            if (aCompletion.get()!=NULL)
                aCompletion->Complete(AsyncOpPtr(inOp));
        }
        else
            QueueOp(inOp);

        if (mCurOp[0]==NULL && mCurOp[1]==NULL) {
            AutoCrit aGuard(mDataCrit);
            int aLastQueue=1;
            if (mDoPipeline && ++mNumPipelineFailures>2) {
                mDoPipeline=false;
                aLastQueue=0;
            }
            for (int i=1;i>=aLastQueue;--i) {
                OpQueue aQueue=mOpQueue[i];
                mOpQueue[i].clear();
                for (OpQueue::iterator anItr=aQueue.begin();anItr!=aQueue.end();++anItr)
                    QueueOp(anItr->get());
            }
            mRetryMode=false;
            RunOp(mDoPipeline?0:1);
        }
    }
    else {
        if (anId==0 && mSocket.get()!=NULL && inOp->mStatus==100097) {
            inOp->SetOnlyRecv();
            QueueOp(inOp,1);
        }
        else {
            if (aCompletion.get()!=NULL)
                aCompletion->Complete(AsyncOpPtr(inOp));
            mLastUseTime=time(NULL);
        }
        RunOp(anId);
    }
}

}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing session/profile/operation methods.
namespace WONAPI {
int HTTPSession::GetNumOps()
{
	AutoCrit aCrit(mDataCrit);
	int aNumOps = 0;
	ConnectionMap::iterator anItr = mConnectionMap.begin();
	while(anItr!=mConnectionMap.end())
	{
		HTTPConnection *aConnection = anItr->second;
		aNumOps += aConnection->GetNumOps();
		++anItr;
	}

	return aNumOps;
}

int HTTPConnection::GetNumOps()
{
	int aNumOps = 0;
	for(int i=0; i<2; i++)
	{
		aNumOps += mOpQueue[i].size();
		if(mCurOp[i]!=NULL)
			aNumOps++;
	}

	return aNumOps;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d2d50
// (public: virtual __thiscall WONAPI::HTTPConnection::~HTTPConnection(void)    [??1HTTPConnection@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d2d30
// (public: virtual void * __thiscall WONAPI::HTTPConnection::`scalar deleting destructor'(unsigned int)    [??_GHTTPConnection@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d27b0
// (public: __thiscall WONAPI::HTTPSession::HTTPSession(void)    [??0HTTPSession@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006d2870
// (public: virtual __thiscall WONAPI::HTTPSession::~HTTPSession(void)    [??1HTTPSession@WONAPI@@UAE@XZ])

// TEMPLATE: WON 0x006d2850
// (HTTPSession scalar deleting destructor)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d2b30
// (public: __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::SmartPtr<cla)
