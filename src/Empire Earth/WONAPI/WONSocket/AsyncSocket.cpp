// Original-backed AsyncSocket transport family; grouping/filename provisional.
// New source only, all runtime behavior remains original-carved pending full gates.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// Native external data references, not new definitions. Types follow actual
// access widths/Interlocked-free under-lock arithmetic; names are provisional.

// FUNCTION: WON 0x006b0340
AsyncSocket::AsyncSocket(int inType)
{
    mSocket = INVALID_SOCKET;
    mLengthFieldSize = 4;
    mType = inType;
    mConnected = false;
    mOpen = false;
    mThreadCloseCallback = NULL;
    mFirstBlockedSendTime = 0;
    mLastRecvTime = time(NULL);
    mStatus = 100004;
    NetStats::IncrementNumConstructedSockets(1);
}

// FUNCTION: WON 0x006b0420
AsyncSocket* AsyncSocket::Duplicate()
{
    return new AsyncSocket(mType);
}

// FUNCTION: WON 0x006b04a0
AsyncSocket::~AsyncSocket()
{
    Fn2B1AD0();
    for (std::list<MsgTransform*>::iterator anItr = mMsgTransformQueue.begin();
         anItr != mMsgTransformQueue.end(); ++anItr)
        delete *anItr;
    NetStats::IncrementNumConstructedSockets(-1);
}

// FUNCTION: WON 0x006b0600
void AsyncSocket::SetLengthFieldSize(unsigned char inSize)
{
    mLengthFieldSize = inSize;
}

// FUNCTION: WON 0x006b0620
void AsyncSocket::Fn2B0620(MsgTransform* inTransform)
{
    AutoCrit aCrit(mDataCrit);
    mMsgTransformQueue.push_back(inTransform);
}

// FUNCTION: WON 0x006b0730
WONStatus AsyncSocket::Fn2B0730(ByteBufferPtr& inMessage)
{
    AutoCrit aCrit(mDataCrit);
    for (std::list<MsgTransform*>::reverse_iterator anItr = mMsgTransformQueue.rbegin();
         anItr != mMsgTransformQueue.rend(); ++anItr) {
        WONStatus aStatus = (*anItr)->Recv(inMessage);
        if (aStatus != 0)
            return aStatus;
    }
    return 0;
}

// FUNCTION: WON 0x006b07e0
WONStatus AsyncSocket::Fn2B07E0(ByteBufferPtr& inMessage)
{
    AutoCrit aCrit(mDataCrit);
    for (std::list<MsgTransform*>::iterator anItr = mMsgTransformQueue.begin();
         anItr != mMsgTransformQueue.end(); ++anItr) {
        WONStatus aStatus = (*anItr)->Send(inMessage);
        if (aStatus != 0)
            return aStatus;
    }
    return 0;
}

// FUNCTION: WON 0x006b0890
WONStatus AsyncSocket::Fn2B0890()
{
    WONStatus aStatus = WSAGetLastError();
    if (aStatus == WSAEWOULDBLOCK || aStatus == WSAEINPROGRESS)
        return 100001;
    mStatus = aStatus;
    return aStatus;
}

// FUNCTION: WON 0x006b08c0
bool AsyncSocket::Fn2B08C0()
{
    return mSocket != INVALID_SOCKET;
}

// FUNCTION: WON 0x006b08d0
bool AsyncSocket::Fn2B08D0()
{
    return mConnected;
}

// FUNCTION: WON 0x006b08e0
const IPAddr& AsyncSocket::GetDestAddr()
{
    return mDestAddr;
}

// FUNCTION: WON 0x006b1360
void AsyncSocket::Fn2B1360(unsigned long inWaitTime)
{
    if (inWaitTime == 0)
        return;
    AutoCrit aCrit(mDataCrit);
    if (!Fn2B08C0())
        return;
    timeval aTime;
    aTime.tv_sec = inWaitTime / 1000;
    aTime.tv_usec = (inWaitTime % 1000) * 1000;
    fd_set aSet;
    FD_ZERO(&aSet);
    FD_SET(mSocket, &aSet);
    aCrit.Leave();
    select(FD_SETSIZE, NULL, &aSet, NULL, &aTime);
}

// FUNCTION: WON 0x006b1410
void AsyncSocket::Fn2B1410(unsigned long inWaitTime)
{
    if (inWaitTime == 0)
        return;
    AutoCrit aCrit(mDataCrit);
    if (!Fn2B08C0())
        return;
    timeval aTime;
    aTime.tv_sec = inWaitTime / 1000;
    aTime.tv_usec = (inWaitTime % 1000) * 1000;
    fd_set aSet;
    FD_ZERO(&aSet);
    FD_SET(mSocket, &aSet);
    aCrit.Leave();
    select(FD_SETSIZE, &aSet, NULL, NULL, &aTime);
}

// Actual signed native length/count contract, admitted NET_A with FileTransfer consumer repair.
// FUNCTION: WON 0x006b14c0
WONStatus AsyncSocket::Send(const void* inData, int inLength, int* outSent)
{
    AutoCrit aCrit(mDataCrit);
    if (!Fn2B08C0())
        return 100016;
    int aSent = send(mSocket, static_cast<const char*>(inData), inLength, 0);
    if (aSent < 0) {
        if (mFirstBlockedSendTime == 0)
            mFirstBlockedSendTime = time(NULL);
        return Fn2B0890();
    }
    mFirstBlockedSendTime = 0;
    NetStats::IncrementBytesSent(aSent);
    if (outSent != NULL)
        *outSent = aSent;
    if (aSent < inLength)
        return mType == 1 ? 100017 : 100001;
    return 0;
}

// FUNCTION: WON 0x006b15e0
WONStatus AsyncSocket::Fn2B15E0(void* outData, int inLength, int* outReceived)
{
    AutoCrit aCrit(mDataCrit);
    if (!Fn2B08C0())
        return 100016;
    int aReceived = recv(mSocket, static_cast<char*>(outData), inLength, 0);
    if (aReceived < 0)
        return Fn2B0890();
    if (aReceived == 0)
        return 100020;
    NetStats::IncrementBytesReceived(aReceived);
    mLastRecvTime = time(NULL);
    if (outReceived != NULL)
        *outReceived = aReceived;
    if (aReceived != inLength && mType == 0)
        return 100001;
    return 0;
}

// FUNCTION: WON 0x006b1a10
void AsyncSocket::Fn2B1A10()
{
    if (Fn2B08C0()) {
        shutdown(mSocket, 1); // Winsock1 send-only shutdown, actual2b1a26
        if (closesocket(mSocket) != 0 && Fn2B0890() == 100001) {
            linger aLinger;
            aLinger.l_onoff = 1;
            aLinger.l_linger = 0;
            setsockopt(mSocket, SOL_SOCKET, SO_LINGER,
                reinterpret_cast<const char*>(&aLinger), sizeof(aLinger));
            closesocket(mSocket);
        }
        mSocket = INVALID_SOCKET;
        NetStats::IncrementNumOpenSockets(-1);
    }
    mConnected = false;
    mOpen = false;
}

// FUNCTION: WON 0x006b1ad0
void AsyncSocket::Fn2B1AD0()
{
    if (mThreadCloseCallback != NULL)
        mThreadCloseCallback(this);
    AutoCrit aCrit(mDataCrit);
    Virtual08();
    Fn2B1A10();
}

// FUNCTION: WON 0x006b1b50
void AsyncSocket::Close()
{
    if (mThreadCloseCallback != NULL)
        mThreadCloseCallback(this);
    AutoCrit aCrit(mDataCrit);
    Virtual08();
    Virtual0C();
    Fn2B1A10();
}
// Round3 additional original native socket behavior; source-only.

// FUNCTION: WON 0x006b1700
WONStatus AsyncSocket::Fn2B1700(const void* inData, int inLength, const IPAddr& inAddress)
{
    AutoCrit aCrit(mDataCrit);
    if (mType == 0)
        return 100018;
    if (!Fn2B08C0()) {
        WONStatus aStatus = Fn2B0A90();
        if (aStatus != 0)
            return aStatus;
    }
    int aSent = sendto(mSocket, static_cast<const char*>(inData), inLength, 0,
        reinterpret_cast<const sockaddr*>(&inAddress.GetSockAddrIn()), 16);
    if (aSent < 0)
        return Fn2B0890();
    NetStats::IncrementBytesSent(aSent);
    return aSent == inLength ? 0 : 100017;
}

// FUNCTION: WON 0x006b1840
WONStatus AsyncSocket::Fn2B1840(void* outData, int inLength, IPAddr* outAddress, int* outReceived)
{
    AutoCrit aCrit(mDataCrit);
    if (mType == 0)
        return 100018;
    if (!Fn2B08C0())
        return 100016;
    sockaddr_in aFrom;
    int aFromLength = 16;
    int aReceived = recvfrom(mSocket, static_cast<char*>(outData), inLength, 0,
        reinterpret_cast<sockaddr*>(&aFrom), &aFromLength);
    if (aReceived >= 0) {
        NetStats::IncrementBytesReceived(aReceived);
        if (outAddress != NULL)
            outAddress->Fn2B2B00(aFrom);
        if (outReceived != NULL)
            *outReceived = aReceived;
        return 0;
    }
    return Fn2B0890();
}

// FUNCTION: WON 0x006b1980
void AsyncSocket::Fn2B1980()
{
    AutoCrit aCrit(mDataCrit);
    if (mType != 0) {
        sockaddr_in anAddress;
        memset(&anAddress, 0, sizeof(anAddress));
        if (connect(mSocket, reinterpret_cast<const sockaddr*>(&anAddress), 16) != 0)
            Fn2B1AD0();
    }
    else
        Fn2B1AD0();
}
// FUNCTION: WON 0x006b0a90
WONStatus AsyncSocket::Fn2B0A90()
{
    AutoCrit aCrit(mDataCrit);
    if (Fn2B08C0())
        Fn2B1AD0();
    mOpen = true;
    switch (mType) {
    case 0:
        mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        break;
    case 1:
        mSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        break;
    }
    if (mSocket == INVALID_SOCKET)
        return Fn2B0890();
    mStatus = 100004;
    NetStats::IncrementNumOpenSockets(1);
    linger aLinger;
    int aBroadcast;
    if (mType == 0) {
        aLinger.l_onoff = 0;
        if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER,
            reinterpret_cast<const char*>(&aLinger), 4) == SOCKET_ERROR)
            return Fn2B0890();
    }
    if (mType == 1) {
        aBroadcast = 1;
        if (setsockopt(mSocket, SOL_SOCKET, SO_BROADCAST,
            reinterpret_cast<const char*>(&aBroadcast), 4) == SOCKET_ERROR)
            return Fn2B0890();
    }
    unsigned long aNonBlocking = 1;
    if (ioctlsocket(mSocket, FIONBIO, &aNonBlocking) == SOCKET_ERROR)
        return Fn2B0890();
    Virtual04();
    return 0;
}


// FUNCTION: WON 0x006b0c90
WONStatus AsyncSocket::Fn2B0C90(const IPAddr& inAddress, bool inReuse)
{
    AutoCrit aCrit(mDataCrit);
    if (!Fn2B08C0()) {
        WONStatus aStatus = Fn2B0A90();
        if (aStatus != 0) return aStatus;
    }
    if (inReuse) {
        int aReuse = 1;
        if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR,
            reinterpret_cast<const char*>(&aReuse), 4) != 0)
            return Fn2B0890();
    }
    if (bind(mSocket, reinterpret_cast<const sockaddr*>(&inAddress.GetSockAddrIn()), 16) != 0)
        return Fn2B0890();
    return 0;
}

// FUNCTION: WON 0x006b0e60
WONStatus AsyncSocket::Fn2B0E60(const IPAddr& inAddress)
{
    if (!inAddress.IsValid())
        return 100014;
    AutoCrit aCrit(mDataCrit);
    if (mType == 0 && mConnected)
        Fn2B1980();
    if (mSocket == INVALID_SOCKET) {
        WONStatus aStatus = Fn2B0A90();
        if (aStatus != 0) return aStatus;
        if (WONAPI::IPAddr::GetLocalHost() != 0) {
            aStatus = Fn2B0D90(0, 0);
            if (aStatus != 0) return aStatus;
        }
    }
    mDestAddr = inAddress;
    mConnected = true;
    if (connect(mSocket, reinterpret_cast<const sockaddr*>(&inAddress.GetSockAddrIn()), 16) == SOCKET_ERROR)
        return Fn2B0890();
    return 0;
}

// FUNCTION: WON 0x006b0fc0
WONStatus AsyncSocket::Fn2B0FC0(int inBacklog)
{
    AutoCrit aCrit(mDataCrit);
    if (!Fn2B08C0()) return 100016;
    if (mType == 1) return 100019;
    if (listen(mSocket, inBacklog) != 0)
        return Fn2B0890();
    return 0;
}

// FUNCTION: WON 0x006b1060
WONStatus AsyncSocket::Fn2B1060(SmartPtr<AsyncSocket>& outSocket)
{
    AutoCrit aCrit(mDataCrit);
    sockaddr_in aFrom;
    int aFromLength = 16;
    SOCKET aSocket = accept(mSocket, reinterpret_cast<sockaddr*>(&aFrom), &aFromLength);
    if (aSocket == INVALID_SOCKET)
        return Fn2B0890();
    linger aLinger;
    aLinger.l_onoff = 0;
    setsockopt(aSocket, SOL_SOCKET, SO_LINGER,
        reinterpret_cast<const char*>(&aLinger), 4);
    unsigned long aNonBlocking = 1;
    ioctlsocket(aSocket, FIONBIO, &aNonBlocking);
    outSocket = Duplicate();
    outSocket->mSocket = aSocket;
    outSocket->mDestAddr = IPAddr(aFrom);
    outSocket->mOpen = true;
    outSocket->mConnected = true;
    NetStats::IncrementNumOpenSockets(1);
    return 0;
}

// FUNCTION: WON 0x006b1220
WONStatus AsyncSocket::Fn2B1220(unsigned long inWaitTime)
{
    AutoCrit aCrit(mDataCrit);
    if (!Fn2B08C0()) return 100016;
    SOCKET aSocket = mSocket;
    aCrit.Leave();
    timeval aTime;
    aTime.tv_sec = inWaitTime / 1000;
    aTime.tv_usec = (inWaitTime % 1000) * 1000;
    fd_set aWriteSet;
    fd_set anExceptSet;
    FD_ZERO(&aWriteSet);
    FD_ZERO(&anExceptSet);
    FD_SET(aSocket, &aWriteSet);
    FD_SET(aSocket, &anExceptSet);
    int aResult = select(FD_SETSIZE, NULL, &aWriteSet, &anExceptSet, &aTime);
    int anError = 0;
    int aLength = 4;
    if (FD_ISSET(aSocket, &anExceptSet)) {
        getsockopt(mSocket, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&anError), &aLength);
        return anError == 0 ? 100021 : anError;
    }
    if (aResult == 1)
        return 0;
    return 100001;
}

// SUPPORT-ONLY: complete native sockaddr preparation overload2b0d90.
// Canonical second parameter remains int; actual archived SDK contract is bool.
// FUNCTION: WON 0x006b0d90
WONStatus AsyncSocket::Fn2B0D90(unsigned short inPort, bool inReuse)
{
    sockaddr_in anAddress;
    memset(&anAddress, 0, sizeof(anAddress));
    anAddress.sin_family = AF_INET;
    anAddress.sin_addr.s_addr = htonl(WONAPI::IPAddr::GetLocalHost());
    anAddress.sin_port = htons(inPort);
    return Fn2B0C90(IPAddr(anAddress), inReuse);
}

}

// Private Round7 missing AsyncSocket source completion; all definitions support0.
namespace WONAPI {
// FUNCTION: WON 0x006b0610
unsigned char AsyncSocket::GetLengthFieldSize() const
{
    return mLengthFieldSize;
}
// FUNCTION: WON 0x006b08f0
unsigned short AsyncSocket::GetLocalPort()
{
    if(!Fn2B08C0()) return 0;
    sockaddr address;
    int length=sizeof(address);
    int result=getsockname(mSocket,&address,&length);
    if(result!=0) return 0;
    return ntohs(reinterpret_cast<sockaddr_in*>(&address)->sin_port);
}
// FUNCTION: WON 0x006b0a50
WONStatus AsyncSocket::Fn2B0A50(bool inAllowBroadcast)
{
    int option=inAllowBroadcast;
    int result=setsockopt(mSocket,SOL_SOCKET,SO_BROADCAST,
        reinterpret_cast<const char*>(&option),sizeof(option));
    if(result==SOCKET_ERROR) return Fn2B0890();
    return 0;
}
}

namespace WONAPI {
// True SDK static counter; native zero-initialized DWORD565f34, no CRT constructor.
int AsyncSocket::mWinsockStartCount=0;
// FUNCTION: WON 0x006b0940
WONStatus AsyncSocket::StartWinsock()
{
    if(mWinsockStartCount!=0)
    {
        ++mWinsockStartCount;
        return 0;
    }
    WSADATA data;
    int result=WSAStartup(MAKEWORD(1,1),&data);
    if(result!=0) return result;
    ++mWinsockStartCount;
    return 0;
}
// FUNCTION: WON 0x006b0980
WONStatus AsyncSocket::StopWinsock()
{
    if(mWinsockStartCount<=0 || --mWinsockStartCount!=0)
        return 0;
    return WSACleanup();
}
}

namespace WONAPI {

// FUNCTION: WON 0x006b1350
void AsyncSocket::WaitForAccept(unsigned long timeout)
{
    Fn2B1410(timeout);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006b0a40
AsyncSocket::SocketType AsyncSocket::GetType()
{
    return (SocketType)mType;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK missing executable methods.
namespace WONAPI {
int AsyncSocket::TimeLeft(DWORD theTotalTime, DWORD theStartTime)
{
	int deltaTime = GetTickCount() - theStartTime;
	if(deltaTime >= theTotalTime)
		return 0;
	else
		return theTotalTime - deltaTime;
}

void AsyncSocket::RemoveAllMsgTransforms()
{
	AutoCrit aCrit(mDataCrit);
	std::list<MsgTransform*>::iterator anItr = mMsgTransformQueue.begin();
	while(anItr!=mMsgTransformQueue.end())
	{
		delete *anItr;
		++anItr;
	}
	mMsgTransformQueue.clear();
}

bool AsyncSocket::IsOpen()
{
	return mOpen;
}

void AsyncSocket::SetType(SocketType theType) 
{
	AutoCrit aCrit(mDataCrit);
	if(mType!=theType)
		Fn2B1AD0();

	mType = theType;
}

WONStatus AsyncSocket::SetNagling(bool on) 
{
	int aVal, aSockOpt = !on;
	aVal = setsockopt(mSocket,IPPROTO_TCP,TCP_NODELAY,(char*) &aSockOpt,sizeof(aSockOpt));

	if(aVal==SOCKET_ERROR) 
		return Fn2B0890();
	else
		return (WONStatus)0;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006b0480
// (public: virtual void * __thiscall WONAPI::AsyncSocket::`scalar deleting destructor'(unsigned int)    [??_GAsyncSocket@WONAPI@@UAEPAXI@Z])
