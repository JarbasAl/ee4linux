// Original-backed blocking operation family; source only, no runtime authority.
// SendBytes/SendMsg const-ownership candidates are excluded entirely.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006acc10
ServerOp::ServerOp()
{
    mMaxConnectTime=60000;
    mMaxSendTime=60000;
    mMaxRecvTime=60000;
    mClientKeySize=8;
    mUseClientKey=false;
}
// FUNCTION: WON 0x006ace20
void ServerOp::SetMaxTimes(int inConnect,int inSend,int inRecv)
{
    mMaxConnectTime=inConnect;
    mMaxSendTime=inSend;
    mMaxRecvTime=inRecv;
}
// FUNCTION: WON 0x006ace50
DWORD ServerOp::ConnectTime()
{
    DWORD aTime=mMaxConnectTime;
    if(mRunMode!=1 && TimeLeft()<aTime) aTime=TimeLeft();
    return aTime;
}
// FUNCTION: WON 0x006ace80
DWORD ServerOp::SendTime()
{
    DWORD aTime=mMaxSendTime;
    if(mRunMode!=1 && TimeLeft()<aTime) aTime=TimeLeft();
    return aTime;
}
// FUNCTION: WON 0x006aceb0
DWORD ServerOp::RecvTime()
{
    DWORD aTime=mMaxRecvTime;
    if(mRunMode!=1 && TimeLeft()<aTime) aTime=TimeLeft();
    return aTime;
}
// FUNCTION: WON 0x006acee0
void ServerOp::CopyMaxTimes(ServerOp* inSource)
{
    SetMaxTimes(inSource->mMaxConnectTime,inSource->mMaxSendTime,inSource->mMaxRecvTime);
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine missing session/profile/operation methods.
namespace WONAPI {
void ServerOp::SendMsgAsync(ByteBufferPtr theMsg, int theTrackId)
{
	SmartPtr<SendMsgOp> anOp = new SendMsgOp(theMsg, mSocket);
	Track(anOp,theTrackId);
	mSocket->Fn2B4740(anOp,-1);
}

void ServerOp::RecvMsgAsync(int theTrackId)
{
	SmartPtr<RecvMsgOp> anOp = new RecvMsgOp(mSocket);
	Track(anOp,theTrackId);
	mSocket->Fn2B4740(anOp,-1);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006accd0
// (public: virtual void * __thiscall WONAPI::ServerOp::`scalar deleting destructor'(unsigned int)    [??_GServerOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006accf0
// (public: virtual __thiscall WONAPI::ServerOp::~ServerOp(void)    [??1ServerOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ae9f0
// (public: __thiscall std::_Tree<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::AsyncOp> const ,clas)
