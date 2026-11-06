// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK bounded/delayed byte receive operation.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONRecvSomeBytesOp.h"
namespace WONAPI {
DWORD RecvSomeBytesOp::mInitDelay = 0;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RecvSomeBytesOp::RecvSomeBytesOp(unsigned short theMaxBytes, AsyncSocket *theSocket, bool doDelay) : SocketOp(theSocket)
{
	mB58[0] = true;
	mMaxBytes = theMaxBytes;
	mCurRecv = NULL;

	mDoDelay = doDelay;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
RecvSomeBytesOp::~RecvSomeBytesOp()
{
	delete mCurRecv;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RecvSomeBytesOp::Virtual0C()
{
	if(mCurRecv!=NULL)
		delete mCurRecv;
	
	mCurRecv = new char[mMaxBytes];
	mStartTime = GetTickCount();

	return Virtual10();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
WONStatus RecvSomeBytesOp::Virtual10()
{
	mSocket->Fn2B1410(TimeLeft());

	int aRecvLen = 0;
	WONStatus aStatus;

	if (mDoDelay && mInitDelay>0 && GetTickCount()-mStartTime<mInitDelay)
		return 100001 /* SDK WS_TimedOut */;
	
	aStatus = mSocket->Fn2B15E0(mCurRecv,mMaxBytes,&aRecvLen);	
	if(aRecvLen>0)
	{
		mRecvBytes = new ByteBuffer(mCurRecv,aRecvLen,true);
		mCurRecv = NULL;
		aStatus = 0;
	}
	
	return aStatus;
}

}
