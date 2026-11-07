// WONAPI routing-server API as the game's WON front end (EEWonNet.cpp, 0xcd4ee..0xd3ff4) sees it (RECONSTRUCTION;
//). WONAPI proper (0x29deb0..) is not decompiled: class names come from RTTI
// (.?AVRoutingConnection@WONAPI@@, ...), members and offsets from EEWonNet.cpp's use; methods are named by address
// where the WONAPI name is unknown. Every name PROVISIONAL. Requires <string>, <list>, <map>, ee/EEWonNet.h.
#ifndef EE_WONAPI_WONROUTING_H
#define EE_WONAPI_WONROUTING_H

#include "ee/wonapi/WONBuffer.h"
#include "ee/wonapi/WONElGamal.h"
#include "ee/wonapi/WONBlowfish.h"
#include "ee/wonapi/WONAuthPeerData.h" //actual complete const-owner for LightGetCertOp inline setter
#include "ee/wonapi/WONStringCompare.h"
#include <stdio.h> //actual FILE* receiver field
#include <set> //actual SmartRoutingConnection IgnoreSet
#ifndef EE_LOST_TRACE // as LostDebug.h; that header's struct here shifts identifier-count rows (0x7245f0)
#define EE_LOST_TRACE(expr) ((void)(expr))
#endif

namespace WONAPI {

typedef long WONStatus;
enum AsyncRunMode { OP_MODE_ASYNC=1, OP_MODE_BLOCK=2 }; //actual SDK/native enum32
void StringToUpperCaseInPlace(std::string&); //2a69a0
typedef long RoutingOpType;
enum AuthType { AUTH_TYPE_NONE=0, AUTH_TYPE_SESSION=1, AUTH_TYPE_PERSISTENT=2, AUTH_TYPE_PERSISTENT_NOCRYPT=3 };

// 0x29deb0 (WONStatus.cpp, the first WONAPI object): the status text
const char* WONStatusToString(WONStatus inStatus);


template <class T> class CompletionBase;

class AsyncOp;
class AsyncOpContainer;
typedef std::multimap<unsigned long, SmartPtr<AsyncOp> > AsyncOpTimeoutMap;

// Actual AsyncOp allocation0x44; originalctor29ef80/dtor29f000, three vtable slots.
class AsyncOp : public RefCount {
public:
    WONStatus Fn29F500(long inMode,unsigned long inTimeout); //29f500, actual status return/ret8

    AsyncOp();                              // 0x29ef80
    virtual ~AsyncOp();                     // 0x29f000
    virtual void Virtual04();
    virtual void Virtual08();
    // (: AsyncOp's own vtable 0x84c834 has these three slots only; SocketOp and RoutingOp each introduce their
    // +0x0c/+0x10/+0x14: SocketOp's +0x14 returns 0, AcceptOp's a new AcceptOp; RoutingOp's +0x14 is GetType)

    WONStatus GetStatus() const { return mStatus; }
    CompletionBase<SmartPtr<AsyncOp> >* GetCompletion() const { return mCompletion; }
    unsigned long GetTimeout() const { return mTimeout; }
    void SetCompletion(CompletionBase<SmartPtr<AsyncOp> >* theCompletion) { mCompletion=theCompletion; }
    void SetMode(AsyncRunMode); //native29f190, actual enum32/ret4 conditional setter, call-only
    void ForceFinish(WONStatus inStatus); //29f2f0, actual full status argument
    void RunAsync(unsigned long inTimeout); //29f610, DWORD timeout/ret4
    void Fn29F220(WONStatus inStatus); // original29f220: completes pending status under lock
    void Fn29F1B0(unsigned long inTimeout); //29f1b0, timeout DWORD at2c/ret4
    bool RunBlock(unsigned long inTimeout); //native29f6e0, true20Bexternalwrapper, support0
    void Run(long inTimeout);               // 0x29f6d0
    void Kill(bool inFlag);                 // 0x29f1e0
    void SetFlag3A(bool inFlag);            // 0x29f1d0
    void AddToTimerMap(AsyncOpTimeoutMap&, unsigned long); //29f080, ret8
    void RemoveFromTimerMap(bool); //29f160, ret4
    void QueueInOpContainer(); //29f200, actual container/core dispatch
    void Complete(); //29f3a0
    void StartRunAsync(); //29f4c0
    unsigned long TimeLeft();               // 0x29f700
    DWORD RealTimeLeft();

    CriticalSection mLock;                  // +0x08, originalInitialize/DeleteCriticalSection
    SmartPtr<CompletionBase<SmartPtr<AsyncOp> > > mCompletion;     // +0x20
    AsyncOpContainer* mContainer;           // +0x24, nonowning; Queue29f200
    unsigned long mStartTime;               // +0x28, GetTickCount
    unsigned long mTimeout;                 // +0x2c, unsignedelapsed comparison29f700
    bool IsAsync() const { return mRunMode==1; }
    long mRunMode;                          // +0x30
    //: 0x291983 (an inline COMDAT of EELobbyFileTransfer.cpp): finished without error
#ifdef EE_DINKUM_SXL233
    bool Succeeded() { return mStatus==0 && !mB39; }
#else
    bool Succeeded();                       // 0x691983
#endif

    WONStatus mStatus;                      // +0x34
    bool mCompletionPending;                // +0x38
    bool mB39;                              // +0x39
    bool mFlag3A;                           // +0x3a; byte3b is normalalignment
    AsyncOpTimeoutMap* mTimeoutMap;         // +0x3c, nonowning
    AsyncOpTimeoutMap::iterator mTimeoutItr; // +0x40, associateddeadline entry
};
typedef SmartPtr<AsyncOp> AsyncOpPtr;

// Complete target Event8 (vptr0, HANDLE4); native29f7c0 and embedded atContainer1c.
class Event {
public:
    explicit Event(bool manualReset=false,bool initiallySet=false)
        { mEvent=CreateEventA(NULL,manualReset,initiallySet,NULL); }
    virtual ~Event() {CloseHandle(mEvent);}
    void Set() {SetEvent(mEvent);}
    void Reset() {ResetEvent(mEvent);}
    bool WaitFor(unsigned long milliseconds)
        {return WaitForSingleObjectEx(mEvent,milliseconds,TRUE)==WAIT_OBJECT_0;}
    HANDLE mEvent; //04
};
// Complete target38-byte container, sole virtual destructor. Ctor29f730, dtor29f830.
class AsyncOpContainer {
public:
    AsyncOpContainer();
    virtual ~AsyncOpContainer();
    void QueueOp(AsyncOp*); //29f9b0
    void PurgeOps(); //29f900
    void Pump(unsigned long,int inMaxOps=0); //29fad0
    CriticalSection mDataCrit; //04..1c
    Event mSignal; //1c..24
    std::list<AsyncOpPtr> mOpQueue; //24..30
    AsyncOpPtr mCurPumpOp; //30
    bool mPurging; //34; naturalpadding35..37
};

class SocketOp;
class MsgTransform {
public:
    // Pinned SDK explicit empty virtual dtor; target deleting2ad88032B stores ownVT44ca2c.
    virtual ~MsgTransform() {}
    virtual WONStatus Send(ByteBufferPtr&) { return 0; } //VT+4=2ad870, ret4
    virtual WONStatus Recv(ByteBufferPtr&) { return 0; } //VT+8=2ad870, ret4
};

// Genuine SDK NetStats static-only class; no instance layout or virtual API.
class NetStats
{
protected:
	static CriticalSection mDataCrit;

	static bool mEnabled;

	static int mNumConstructedSockets;
	static int mNumOpenSockets;

	static __int64 mNumBytesSent;
	static __int64 mNumBytesReceived;
	static __int64 mNumMessagesSent;
	static __int64 mNumMessagesReceived;

public:
	static void Enable(bool isEnabled) { mEnabled = isEnabled; }

	static void IncrementNumConstructedSockets(int theAmount) { if(!mEnabled) return; mDataCrit.Enter(); mNumConstructedSockets+=theAmount; mDataCrit.Leave(); }
	static void IncrementNumOpenSockets(int theAmount) { if(!mEnabled) return; mDataCrit.Enter(); mNumOpenSockets+=theAmount; mDataCrit.Leave(); }

	static void IncrementBytesSent(int theAmount) { if(!mEnabled) return; mDataCrit.Enter(); mNumBytesSent+=theAmount; mDataCrit.Leave(); }
	static void IncrementBytesReceived(int theAmount) { if(!mEnabled) return; mDataCrit.Enter(); mNumBytesReceived+=theAmount; mDataCrit.Leave(); }
	static void IncrementMessagesSent(int theAmount) { if(!mEnabled) return; mDataCrit.Enter(); mNumMessagesSent+=theAmount; mDataCrit.Leave(); }
	static void IncrementMessagesReceived(int theAmount) { if(!mEnabled) return; mDataCrit.Enter(); mNumMessagesReceived+=theAmount; mDataCrit.Leave(); }

	static int GetNumConstructedSockets() { return mNumConstructedSockets; }
	static int GetNumOpenSockets() { return mNumOpenSockets; }

	static __int64 GetNumBytesSent() { return mNumBytesSent; }
	static __int64 GetNumBytesReceived() { return mNumBytesReceived; }
	static __int64 GetNumMessagesSent() { return mNumMessagesSent; }
	static __int64 GetNumMessagesReceived() { return mNumMessagesReceived; }
};

class AsyncSocket : public RefCount {
public:
    enum SocketType { TCP, UDP }; //genuine SDK type, same 32-bit native domain
    SocketType GetType(); //2b0a40, complete4B native nonvirtual API
    static int TimeLeft(DWORD,DWORD);
    void RemoveAllMsgTransforms();
    bool IsOpen();
    void SetType(SocketType);
    WONStatus SetNagling(bool);
    static WONStatus StartWinsock(); //2b0940 actual SDK cdecl, external source owner
    static WONStatus StopWinsock(); //2b0980 actual SDK cdecl, external source owner
    static int mWinsockStartCount; //565f34 actual4B zero static, pinned genuine SDK
    static bool HasInternetConnection();
    WONStatus Fn2B0A50(bool); //2b0a50, native broadcast socketoption
    WONStatus Fn2B0A90(); //2b0a90, native create
    WONStatus Fn2B1840(void*,int,IPAddr*,int*); //2b1840, signed count/output on success
    void Fn2B1980(); //2b1980, disconnect
    WONStatus Fn2B0C90(const IPAddr&,bool); //2b0c90, address bind/reuse
    WONStatus Fn2B0E60(const IPAddr&); //2b0e60, connect
    WONStatus Fn2B1060(SmartPtr<AsyncSocket>&); //2b1060, owning accepted output reference
    WONStatus Fn2B1220(unsigned long); //2b1220, native connect wait
    void WaitForAccept(unsigned long); //2b1350/13; genuine SDK API

    AsyncSocket(int inType); //2b0340; fresh factory2b0420 new74
    virtual ~AsyncSocket(); //ordinary2b04a0; scalar deleting2b0480
    virtual void Virtual04(); //326b60; actual open-site2b0c69, SDK OpenHook
    virtual void Virtual08(); //326b60; close2b1b1d/kill2b1b9d, SDK CloseHook
    virtual void Virtual0C(); //326b60; kill2b1ba4, SDK KillHook
    virtual AsyncSocket* Duplicate(); //2b0420, factory result consumed2b111c
    // Existing API ownership migration; exact original addresses preserved.
    WONStatus Fn2B0D90(unsigned short,bool); // bind to the local host address: (port, allowReuse)
    WONStatus Fn2B0FC0(int); //existing canonical2b0fc0 listen ABI
    void Fn2B1360(unsigned long); //existing canonical2b1360
    WONStatus Send(const void*,int,int*); //existing canonical2b14c0 ABI, nominal signedness deferred
    void Fn2B1AD0(); //2b1ad0
    void Close(); //existing canonical name2b1b50; nominal SDK Kill kept separate
    unsigned short GetLocalPort(); //2b08f0, native call2cf882, SDK confirms no args
    unsigned char GetLengthFieldSize() const; //native2b0610, actualcall2b5b10
    void SetLengthFieldSize(unsigned char); //2b0600, bytearg->50/ret4
    bool Fn2B08D0(); //2b08d0, byte51 ->AL; SDK IsConnected corroboration
    const IPAddr& GetDestAddr(); //2b08e0, lea28/ret; SDK nonconst method
    void Fn2B0620(MsgTransform*); //2b0620, owns raw transform pushed into list
    void Fn2B1410(unsigned long); //2b1410, wait-for-read timeout
    void Fn2B1A10(); //2b1a10, ClosePrv handle shutdown/close
    WONStatus Fn2B0890(); //2b0890, socket WSA status mapping
    bool Fn2B08C0(); //2b08c0, handle20 != INVALID_SOCKET
    WONStatus Fn2B0730(ByteBufferPtr&); //recv transforms reverse-list slot08
    WONStatus Fn2B07E0(ByteBufferPtr&); //send transforms forward-list slot04
    WONStatus Fn2B1700(const void*,int,const IPAddr&); //sendto
    WONStatus Fn2B15E0(void*,int,int*); //recv

    CriticalSection mDataCrit; //08..1f
    SOCKET mSocket; //20
    int mType; //24, fullDWORD argument/factory forwarding
    IPAddr mDestAddr; //28..4f, complete28
    unsigned char mLengthFieldSize; //50
    bool mConnected; //51
    bool mOpen; //52; natural padding53
    WONStatus mStatus; //54, target-only status relative to archived SDK
    long mLastRecvTime; //58, target32-bit time_t, ctor and recv write time(NULL)
    long mFirstBlockedSendTime; //5c, send-zero/test/time/reset proof
    std::list<MsgTransform*> mMsgTransformQueue; //60 allocator,64head,68size; ownsraw virtualobjects
    typedef void (*CloseCallback)(AsyncSocket*);
    CloseCallback mThreadCloseCallback; //6c, cdecl caller-pops4 at2b1aed/1b6d
    SmartPtr<RefCount> mThreadData; //70, targetintrusiveownedABI + pinned SDK RefCountPtr; root accepted threshold
};



// a WON operation on a socket (RTTI SocketOp > AsyncOp, vtable 0x84c424; its inline members are emitted by
// EELobbyFileTransfer.cpp 0x29151c..0x291555, 0x2914c2)
class SocketOp : public AsyncOp {
public:
    static bool mRunAsyncImmediatelyDef; //genuine SDK bool static, native565f08 virtual-zero byte
    AsyncSocket* GetSocket() { return mSocket.get(); }
    void SetSocket(AsyncSocket* theSocket) { mSocket=theSocket; }
    SocketOp(AsyncSocket* inSocket) : mB44(false), mSocket(inSocket)
    {
        SetFlag3A(mRunAsyncImmediatelyDef);
        for (int i = 0; i < 3; i++) {
            mB58[i] = false;
            mL48[i] = -1;
        }
    }
    // Implicit destructor: original29153d has no own-vptr store.
    virtual void Virtual04();               // +0x04 0x2b6070 actualRunHook
    virtual void Virtual08();               // +0x08 0x2b60c0
    //: +0x0c starts the operation, +0x10 continues it (SendFileOp 0x2915d2 / 0x291613 return a WONStatus)
    virtual WONStatus Virtual0C() { return Virtual10(); }   // +0x0c 0x29151c
    virtual WONStatus Virtual10() = 0;      // +0x10 (_purecall)
    // +0x14 (vtable 0x84c424: 0x1ad67e `xor eax, eax`, ICF): a fresh copy of the op, none by default (AcceptOp's
    // 0x291ab7 news one on the same socket, name PROVISIONAL)
    virtual AsyncOp* Duplicate() { return NULL; }

    bool mB44;                              // +0x44
    long mL48[3];                           // +0x48
    SmartPtr<AsyncSocket> mSocket;          // +0x54
    bool mB58[3];                           // +0x58
};

// closes its socket
class CloseOp : public SocketOp {
public:
    CloseOp(AsyncSocket* inSocket) : SocketOp(inSocket) {}
    virtual WONStatus Virtual10() { mSocket->Fn2B1AD0(); return 0; }    // +0x10 0x291993
};

// waits for a connection on its (listening) socket
class AcceptOp : public SocketOp {
public:
    AcceptOp(AsyncSocket* inSocket) : SocketOp(inSocket) { mB58[0] = true; }
    AsyncSocket* GetAcceptedSocket() { return mAcceptedSocket.get(); }
    virtual WONStatus Virtual10();          // +0x10 0x2b0310 (WONAPI)
    virtual AsyncOp* Duplicate() { return new AcceptOp(mSocket.get()); }   // +0x14 0x291ab7

    SmartPtr<AsyncSocket> mAcceptedSocket;  // +0x5c
};

// connects its socket to an address (RTTI ConnectOp > SocketOp, vtable 0x84c480; 0x84 bytes: EELobbyFileTransfer
// 0x291b79 news it and inlines the ctor: SocketOp(NULL), the IPAddr copy ctor 0x291d59 at +0x5c, the vptr, then
// +0x59 and +0x5a = true, argument order PROVISIONAL)
class ConnectOp : public SocketOp {
public:
    ConnectOp(const IPAddr& inAddr, AsyncSocket* inSocket = NULL) : SocketOp(inSocket), mAddr(inAddr)
    {
        mB58[1] = true;
        mB58[2] = true;
    }
    virtual WONStatus Virtual0C();          // +0x0c 0x2b2320 (WONAPI)
    virtual WONStatus Virtual10();          // +0x10 0x2b2330 (WONAPI)

    IPAddr mAddr;                           // +0x5c
};

// Full original-backed receive operation fields; no opaque storage tails.
class RecvBytesOp : public SocketOp {
public:
    WONStatus Fn2B5000(); //2b5000
    WONStatus Fn2B50C0(); //2b50c0
    WONStatus Fn2B5410(WONStatus); //2b5410, native filefinish
    WONStatus Fn2B54A0(WONStatus); //2b54a0, native finish

    RecvBytesOp(unsigned long inLength, AsyncSocket* inSocket = NULL); //2b4de0
    // Implicit dtor2ab5a0 must naturally inline in RecvMsgOp dtor2ab470.
    virtual void Virtual08(); //2b5580
    virtual WONStatus Virtual0C(); //2b4ee0
    virtual WONStatus Virtual10(); //2b4ef0
    virtual AsyncOp* Duplicate(); //2b4f00
    void Fn2B4F90(CompletionBase<AsyncOpPtr>*, long inInterval);
    ByteBufferPtr GetBytes() const; //2b1df0, owning return by value
    void SetFilePath(const std::string& theFilePath) { mPath=theFilePath; }
    void SetAppendFile(bool theVal) { mAppend=theVal; }
    void SetBinaryFile(bool isBinary) { mBinary=isBinary; }
    void SetFileModifyTime(time_t theTime) { mFileModifyTime=theTime; }
    void SetSkipBytes(bool skip) { mB80=skip; }
    WriteBuffer mWriteBuffer; //5c,18
    ByteBufferPtr mReceivedData; //74
    unsigned long mLength; //78
    unsigned long mBytesRead; //7c
    bool mB80; //80, provisional semantics
    FILE* mFile; //84, natural alignment before pointer
    std::string mPath; //88
    bool mAppend; //98
    bool mOwnFile; //99
    bool mBinary; //9a
    long mFileModifyTime; //9c, actual32-bit time_t, natural alignment before scalar
    SmartPtr<CompletionBase<AsyncOpPtr> > mProgressCompletion; //a0
    long mProgressInterval; //a4, existing setter signature
    unsigned long mBytesSinceProgress; //a8
};
class RecvMsgOp : public RecvBytesOp {
public:
    WONStatus Fn2B5B50(); //2b5b50
    WONStatus Fn2B5D20(); //2b5d20

    RecvMsgOp(AsyncSocket* inSocket = NULL) : RecvBytesOp(0, inSocket) {}
    // Implicit dtor2ab470; no own-vptr store, full base cleanup inlined.
    virtual WONStatus Virtual0C(); //2b5b00
    virtual WONStatus Virtual10(); //2b5e20
    virtual AsyncOp* Duplicate(); //2ab3d0
    ByteBufferPtr GetMsg() const; //2ab770, owning return by value
    ByteBufferPtr mMessage; //ac
    int mState; //b0, ctor uninitialized, RunHook sets1
};

// NEXT-window complete class proposal only, not canonical. Native VT44ca78 six slots.
class RecvCRMsgOp : public SocketOp {
public:
    RecvCRMsgOp(AsyncSocket* inSocket=NULL) : SocketOp(inSocket),mBuffer(0,true) {mB58[0]=true;}
    virtual WONStatus Virtual0C(); //2b5970,62
    virtual WONStatus Virtual10(); //2b59b0,330 ending ret2b5af9, excludes6align
    virtual AsyncOp* Duplicate() {return new RecvCRMsgOp(mSocket.get());} //2b2180,209 support0
    ByteBufferPtr GetMsg() const {return mMessage;} //own accessor2b1df0 ICF-qualified only, support0
    WriteBuffer mBuffer; //5c..73
    ByteBufferPtr mMessage; //74, total78
    //Implicit normal2b2280/157 has NO own-vptr store: SP74,Buffer5c,SocketSP54,AsyncBase.
};

// what a RecvBytesOp's progress completion (+0xa0) gets: 0x2b520d news one (0x48 bytes; RTTI RecvChunkOp : AsyncOp,
// vtable 0x84cae4) with +0x44 = the receiving op
class RecvChunkOp : public AsyncOp {
public:
    RecvChunkOp(RecvBytesOp* inOp) : mRecvOp(inOp) {} //actual new48/base44/owning44; support0
    virtual void Virtual04() { Fn29F220(0); } //2b5370, actual8B override, existingslot04; support0

    SmartPtr<RecvBytesOp> mRecvOp;          // +0x44
};



// the completion interface (vtable 0x43e4a0: ??_G, pure Complete)
template <class T> class CompletionBase : public RefCount {
public:
    virtual void Complete(T theParam) = 0;  // +0x04
};
// Actual two-slot completion16 for AsyncOpPtr/SmartPtr<RefCount>; support quota0.
// Additive full template only. Existing CompletionBase stays unchanged/implicit.
// Complete actual LightGetCertOp48: AsyncOp44 plus ConstPeer44, no private tail.
// PeerAuthOp::AsyncRefreshCert native2e74a0 allocates48 and calls the existing AsyncOp constructor.
// Natural implicit constructor/destructor match native2e74e0..2e74f2 and2e7580 owned-pointer/base cleanup;

class LightGetCertOp : public AsyncOp {
    ConstSmartPtr<AuthPeerData> mPeerData; //44..48, native GetCertCallback2de677
public:
    void SetPeerData(const AuthPeerData* data) { mPeerData=data; }
    const AuthPeerData* GetPeerData() const { return mPeerData; }
};

// Actual ParamCompletion<AsyncOpPtr,SmartPtr<RefCount>> allocation10,
// callback8/ownedparamc, VT44ca3c -> deleting2aeab0, Complete2ae840.
// Native ctor2ae7e0 consumes the by-value parameter and copies it into mParam.
// Normal destructor2aead0 has no own-vptr store; natural implicit cleanup required.
template <class ResultType, class ParamType>
class ParamCompletion : public CompletionBase<ResultType> {
    typedef void (*Callback)(ResultType, ParamType);
    Callback mCallback;
    ParamType mParam;
public:
    ParamCompletion(Callback inCallback, ParamType inParam)
        : mCallback(inCallback), mParam(inParam) {}
    virtual void Complete(ResultType inResult) { mCallback(inResult, mParam); }
};

typedef SmartPtr<CompletionBase<AsyncOpPtr> > OpCompletionBasePtr;

class QueueSocket : public AsyncSocket {
public:
    void Fn2B3D50(long inTimeout); //2b3d50, StartCloseTimer
    void Fn2B3ED0(); //2b3ed0, CancelCloseTimer
    static void Fn2B3F80(AsyncOpPtr inOp, SmartPtr<RefCount> inParam); //owning value callback
    void Fn2B4010(AsyncOp* inTimer); //2b4010
    static void Fn2B4190(AsyncOpPtr inOp, SmartPtr<RefCount> inParam); //owning value callback
    void Fn2B4220(SocketOp* inOp); //2b4220
    void Fn2B45B0(); //2b45b0, RunRepeatOp
    void Fn2B48B0(); //2b48b0, RunQueueOp
    QueueSocket(int inType); //2b3930; fresh factory2b39d0 newa0
    // IMPLICIT destructor: ordinary2b3a60 has no own-vptr store and naturally inlines
    // into BlockingSocket/LobbyLanSocket; do not add bare out-of-line dtor declaration.
    virtual void Virtual04(); //2b4c00, byte9c=false,ret
    virtual void Virtual08(); //2b4c10,noargs/void, SDK CloseHook
    virtual void Virtual0C(); //2b4d40,noargs/void, SDK KillHook
    virtual AsyncSocket* Duplicate(); //2b39d0
    void Fn2B3BF0(CompletionBase<AsyncOpPtr>*); //2b3bf0 writes7c; repeat completion
    void SetCloseCompletion(CompletionBase<AsyncOpPtr>*); //2b3ca0 writes80
    void SetQueueCompletion(CompletionBase<AsyncOpPtr>*);
    void QueueOpFast(SocketOp*,DWORD inTimeout=0xffffffff);
    void Fn2B4500(SocketOp*); //2b4500, repeat op84; never broad AsyncOp*
    void Fn2B4740(SocketOp*,long inTimeout); //2b4740, accesses/rebinds op54
    void QueueClose(long inTimeout); //2b4a70, distinct from existing Close()2b1b50

    SmartPtr<CompletionBase<AsyncOpPtr> > mDefaultQueueCompletion; //74
    SmartPtr<CompletionBase<AsyncOpPtr> > mCurQueueCompletion; //78
    SmartPtr<CompletionBase<AsyncOpPtr> > mRepeatCompletion; //7c
    SmartPtr<CompletionBase<AsyncOpPtr> > mCloseCompletion; //80
    SmartPtr<SocketOp> mRepeatOp; //84
    SmartPtr<SocketOp> mQueueOp; //88
    AsyncOpPtr mCloseTimer; //8c, original StartCloseTimer new44/AsyncOpctor
    std::list<SmartPtr<SocketOp> > mOpQueue; //90 allocator,94head,98count
    bool mB9C; //9c, target extra flag; natural padding9d..9f
};

class BlockingSocket : public QueueSocket {
public:
    WONStatus Connect(const IPAddr&,unsigned long); //2b1c10
    WONStatus SendBytes(const ByteBuffer*,unsigned long); //2b1d00
    WONStatus SendBytesTo(const ByteBuffer*,const IPAddr&,unsigned long);
    WONStatus RecvBytesFrom(ByteBufferPtr&,unsigned short,IPAddr&,unsigned long);
    WONStatus RecvMsg(ByteBufferPtr&,unsigned long); //2b1f10
    WONStatus RecvBytes(ByteBufferPtr&,DWORD,DWORD inTimeout=0xffffffff);
    WONStatus RecvCRMsg(ByteBufferPtr&,unsigned long); //2b2040
    WONStatus SendMsg(const ByteBuffer*,unsigned long); //2b1e30, actual SDK call-only contract
    BlockingSocket(int inType); //2b1bd0, just QueueSocket(inType), ownvptr
    // IMPLICIT destructor normal2aad90(375B), no own-vptr; inlines Queue members
    // then calls actual AsyncSocket::~2b04a0. Scalar deleting2b1bf0.
    virtual AsyncSocket* Duplicate(); //2aab50, newa0 and forwards type24
};

// Complete original-backed networking operation/manager additions.

// TD4d4198, VT44c9bc, total6c. Constructor body3e0e80 is ordinary
// compiler-emitted inline support. Implicit destructor2abad0 has no own-vptr store.
class SendBytesOp : public SocketOp {
public:
    SendBytesOp(const ByteBuffer* inData, AsyncSocket* inSocket = NULL)
        : SocketOp(inSocket),
          mReadBuffer(inData->data(), inData->length()),
          mBuffer(inData)
    { mB58[1] = true; }
    virtual WONStatus Virtual0C(); //2b5e50
    virtual WONStatus Virtual10(); //2b5e70
    ReadBuffer mReadBuffer; //5c, exact0c
    ByteBufferPtr mBuffer; //68
};

// TD4d41c0, VT44c9a0, total70. Implicit destructor2abb80 has no own-vptr store.
class SendMsgOp : public SendBytesOp {
public:
    SendMsgOp(const ByteBuffer* inMessage, AsyncSocket* inSocket = NULL)
        : SendBytesOp(NULL, inSocket), mMessage(inMessage) {}
    virtual WONStatus Virtual0C(); //2b5fa0
    virtual WONStatus Virtual10(); //2b6020
    ByteBufferPtr mMessage; //6c
};

// TD4d4040, VT44c948, total88. Actual inline allocation/ctor3e469e..4793.
// Implicit destructor2aa980 destroys IPAddr60, buffer5c, socket54, AsyncOp.
class SendBytesToOp : public SocketOp {
public:
    SendBytesToOp(const ByteBuffer* inData, const IPAddr& inAddress,
                  AsyncSocket* inSocket = NULL)
        : SocketOp(inSocket), mBuffer(inData), mAddress(inAddress)
    { mB58[1] = true; }
    virtual WONStatus Virtual10(); //2b5f10
    ByteBufferPtr mBuffer; //5c
    IPAddr mAddress; //60, exact28
};

// TD4d4440, VT44caf4, total90. Original out-of-line constructor2b55a0.
// Explicit native destructor2b56f0 stores ownVT and deletes raw array5c before owned members.
class RecvBytesFromOp : public SocketOp {
public:
    RecvBytesFromOp(unsigned short inMaxLength, AsyncSocket* inSocket = NULL);
    virtual ~RecvBytesFromOp(); //2b56f0, EXPLICIT native ownVT/raw-array delete before owned members
    virtual void Virtual04(); //2b57d0
    virtual WONStatus Virtual10(); //2b5820
    virtual AsyncOp* Duplicate(); //2b5660, actual fresh same-class allocation
    SmartPtr<ByteBuffer> GetBytes() const; //2aa920, owning51B sret/AddRef, field60
    const IPAddr& GetAddr() const { return mFromAddress; }
    char* mRawBuffer; //5c, owned array: allocate/transfer/clear; explicit native destructor deletes remainder
    SmartPtr<ByteBuffer> mReceivedData; //60, initializednull
    unsigned short mMaxLength; //64, actual first constructor argument
    IPAddr mFromAddress; //68, natural two-byte alignment before this member
};

// TARGET UDP records: four slots each; archived UDP header's extra
// RetransmitHook is a version difference and MUST NOT be inserted here.
class UDPManager;
class UDPManagerRecvOp : public AsyncOp {
public:
    UDPManagerRecvOp(); //2b68b0, actualdefault91B
    UDPManagerRecvOp(const IPAddr&); //2b6a10
    bool HasVerifier();
    void SetUDPManager(UDPManager* theManager);
    void SetSocket(QueueSocket* theSocket);
    const IPAddr& GetFromAddr();
    void SetFromAddr(const IPAddr& theAddr);
    const ByteBuffer* GetMsg() { return mReceivedData.get(); }
    void SetMsg(const ByteBuffer* theMsg) { mReceivedData=theMsg; }
    // Implicit destructor2b6930, no own-vptr store; natural owned-member cleanup.
    virtual void Virtual04(); //2b6fb0
    virtual void Virtual08(); //2b6f60
    virtual WONStatus Virtual0C(ReadBuffer&); //2b6c10, actual new slot0c
    SmartPtr<QueueSocket> mSocket; //44
    SmartPtr<UDPManager> mManager; //48
    bool mOwnSocket; //4c; originalctor leaves until native Run/setup
    IPAddr mAddress; //50..77, naturalalignment4d..4f
    ByteBufferPtr mReceivedData; //78
    typedef WONStatus (*MessageVerifier)(ReadBuffer&, void*);
    MessageVerifier mVerifier; //7c, initializedNULL
    void* mVerifierContext; //80; originalctor leaves until setter
    void Fn2B6BF0(MessageVerifier,void*); //2b6bf0
    void Fn2B6C30(); //2b6c30
    static void Fn2B6D70(AsyncOpPtr,SmartPtr<RefCount>); //2b6d70, owning values
    void Fn2B6E60(RecvBytesFromOp*); //2b6e60

};

class UDPManagerSendRecvOp : public AsyncOp {
public:
    static WONStatus Fn2B7250(ReadBuffer&,void*); //2b7250
    void Fn2B7260(); //2b7260
    static void Fn2B7550(AsyncOpPtr,SmartPtr<RefCount>); //2b7550, owning values
    void Fn2B75E0(AsyncOp*); //2b75e0

    UDPManagerSendRecvOp(); //2b70b0
    void SetUDPManager(UDPManager* theManager);
    void SetMsgTimeout(DWORD theTimeout);
    void SetNumRetransmits(int theNum);
    void SetDestAddr(const IPAddr& theAddr);
    const IPAddr& GetDestAddr() { return mAddress; }
    // Implicit destructor2b7140, no own-vptr; naturally inlined into globalPingGameOp.
    virtual void Virtual04(); //2b7470
    virtual void Virtual08(); //2b7630
    virtual WONStatus Virtual0C(ReadBuffer&); //2ad870, actual defaultstatus0/ret4
    SmartPtr<UDPManager> mManager; //44
    SmartPtr<QueueSocket> mSocket; //48
    ByteBufferPtr mOutgoing; //4c
    IPAddr mAddress; //50..77
    SmartPtr<UDPManagerRecvOp> mRecvOp; //78, NOT a ByteBuffer
    int mRetryLimit; //7c, actualsignedcompare + pinnednominalint
    int mAttemptCount; //80, actualRunHook clears; ctorleavesuninitialized
    unsigned long mReceiveTimeout; //84, original10000; SDKDWORDnominalcorroboration
};

class UDPManager : public RefCount {
public:
    WONStatus Fn2B79C0(int,bool); //2b79c0, pinned nominal signedint port
    static void Fn2B8230(AsyncOpPtr,SmartPtr<RefCount>); //2b8230, owning values
    void Fn2B8320(RecvBytesFromOp*); //2b8320, native receive callback

    UDPManager(); //2b76b0, original-carved
    void SetDefaultCompletion(CompletionBase<AsyncOpPtr>* theCompletion) { mDefaultCompletion=theCompletion; }
    void Send(const ByteBuffer*, const IPAddr&); //2b7ec0, void/ret8, currentPing draftname; SDKSendBytesTo spelling separatelyprovisional
    void Close(); //2b78e0
    void SetAutoRecv(bool); //2b7b40
    void QueueOp(UDPManagerRecvOp*,unsigned long); //2b7c50
    void CleanOps(); //2b8000
    // Implicit virtual destructor; target2b77b0 has NO own-derived-vptr store.
    typedef std::list<SmartPtr<UDPManagerRecvOp> > OpList;
    typedef std::multimap<IPAddr, SmartPtr<UDPManagerRecvOp> > OpMap;
    void Erase(const OpList::iterator&); //2b8160 actual clean-list iterator preserving erase, support0
    void Erase(const OpMap::iterator&); //2b81b0 actual clean-map iterator preserving erase, support0
    SmartPtr<QueueSocket> mSocket; //08
    SmartPtr<CompletionBase<AsyncOpPtr> > mDefaultCompletion; //0c
    SmartPtr<UDPManagerRecvOp> mAutoRecvOp; //10
    OpList::iterator mCleanListItr; //14, initialized mOpList.begin()
    OpMap::iterator mCleanMapItr; //18, initialized mOpMap.begin()
    OpList mOpList; //1c, actual listnode0c with retained pointer+08
    OpMap mOpMap; //28, actualnode3c: IPAddrkey0c,retainedpointer34,treeflags38/39
    int mMaxRecvBytes; //34, initialized1024
    bool mAutoRecv; //38, initializedfalse; naturalpadding39..3b
};

// Genuine SDK inline UDP APIs; definitions follow complete UDPManager for typed SmartPtr ownership.
inline bool UDPManagerRecvOp::HasVerifier() { return mVerifier!=NULL; }
inline void UDPManagerRecvOp::SetUDPManager(UDPManager* theManager) { mManager=theManager; }
inline void UDPManagerRecvOp::SetSocket(QueueSocket* theSocket) { mSocket=theSocket; mOwnSocket=false; }
inline const IPAddr& UDPManagerRecvOp::GetFromAddr() { return mAddress; }
inline void UDPManagerRecvOp::SetFromAddr(const IPAddr& theAddr) { mAddress=theAddr; }
inline void UDPManagerSendRecvOp::SetUDPManager(UDPManager* theManager) { mManager=theManager; }
inline void UDPManagerSendRecvOp::SetMsgTimeout(DWORD theTimeout) { mReceiveTimeout=theTimeout; }
inline void UDPManagerSendRecvOp::SetNumRetransmits(int theNum) { mRetryLimit=theNum; }
inline void UDPManagerSendRecvOp::SetDestAddr(const IPAddr& theAddr) { mAddress=theAddr; }

// a completion calling a member of T (RTTI LobbyCompletion<T> > LobbyCompletionBase > CompletionBase<AsyncOpPtr>;
// vtables 0x84c49c / 0x84c4ac; r23 PROVISIONAL member names)
class LobbyCompletionBase : public CompletionBase<AsyncOpPtr> {
public:
    LobbyCompletionBase() throw() {}
    virtual void Complete(AsyncOpPtr theParam);     // +0x04 0x357d70 (calls +0x08)
    virtual void Run(AsyncOp* inOp) {}      // +0x08
};

template <class T> class LobbyCompletion : public LobbyCompletionBase {
public:
    typedef void (T::*Callback)(AsyncOp* inOp);
    LobbyCompletion(Callback inCallback, T* inObject) : mCallback(inCallback), mObject(inObject) {}
    virtual void Run(AsyncOp* inOp) { (mObject.mPtr->*mCallback)(inOp); }

    Callback mCallback;                     // +0x08
    SmartPtr<T> mObject;                    // +0x0c
};


// RTTI LogicCompletionBase4558cc derives LobbyCompletionBase; distinct flag-bearing subclass.
class LogicCompletionBase : public LobbyCompletionBase {
public:
    LogicCompletionBase() : mCheckTracker(false) {}
    bool mCheckTracker;                    // +0x08; original3df809 and3e5b6e
};

template <class T> class LogicCompletion : public LogicCompletionBase {
public:
    typedef void (T::*Callback)(AsyncOp* inOp);
    LogicCompletion(Callback inCallback, T* inObject) : mCallback(inCallback), mObject(inObject) {}
    virtual void Run(AsyncOp* inOp)
    {
        if (mObject->mKilled) return;
        if (mCheckTracker && !mObject->Fn3E5C00(inOp)) return;
        (mObject.mPtr->*mCallback)(inOp);
    }
    Callback mCallback;                    // +0x0c
    SmartPtr<T> mObject;                    // +0x10; original allocation0x14
};

// PARTIAL reference-only HTTPConnection, actual RTTI44d3a4 > RefCount.
// SmartPtr map ownership is legal; no constructor/new/sizeof/value-copy authority.
class HTTPGetOp;
class HTTPSession;
class HTTPGetOp;
class HTTPConnection : public RefCount {
public:
    HTTPConnection(HTTPSession* inSession = NULL); //2d2c80,new64 at2d2a97
    // Implicit ordinary destructor2d2d50(150B), NO own-vptr store; actual one-slot VT44d3a4.
    void Kill(); //2d3600
    void QueueOp(HTTPGetOp*); //2d33a0,ret4
    void QueueOp(HTTPGetOp*,int); //2d3290,ret8
    void RunOp(int); //2d3430,ret4
    int GetNumOps();
    static void StaticOpCallback(AsyncOpPtr,SmartPtr<RefCount>); //2d2df0, two owning by-value args
protected:
    void OpCallback(HTTPGetOp*); //2d2e80, actual1030B; pinned SDK protected nominal
public:
    CriticalSection mDataCrit; //08..20
    HTTPSession* mSession; //20 NONOWNING, ctor raw assignment
    SmartPtr<BlockingSocket> mSocket; //24
    int mNumPipelineFailures; //28
    int mNumPipelineSuccesses; //2c
    bool mDoPipeline; //30; natural alignment31..33
    time_t mLastUseTime; //34, constructor leaves uninitialized
    typedef std::list<SmartPtr<HTTPGetOp> > OpQueue;
    OpQueue mOpQueue[2]; //38,44
    bool mRetryMode; //50; natural alignment51..53
    HTTPGetOp* mCurOp[2]; //54,58 NONOWNING, native Kill nulls after killing
    SmartPtr<CompletionBase<AsyncOpPtr> > mCurOpCompletion[2]; //5c,60
};
typedef SmartPtr<HTTPConnection> HTTPConnectionPtr;

// Full2c: actual constructor2d27b0, map destruction2d2870.
// Implicit dtor:2d2870 has no own vptr store; final store is RefCount at2d28d0.
// SDK HTTPEngine.h independently has no explicit destructor declaration.
class HTTPSession : public RefCount {
public:
    HTTPSession();                          //2d27b0, out of line
    void AddOp(HTTPGetOp*);                  //2d28f0, native complete-op queue
    void KillAll();                         //2d2bc0, existing spelling retained
    int GetNumOps();
    CriticalSection mDataCrit;              //08..20
    typedef std::map<std::string,HTTPConnectionPtr,StringLessNoCase> ConnectionMap;
    ConnectionMap mConnectionMap;           //20..2c
};
typedef SmartPtr<HTTPSession> HTTPSessionPtr;


class RoutingConnection;
class PeerAuthOp;

// a client of the routing server (+8 its name)
struct RoutingClientInfo : public RefCount {
    RoutingClientInfo() : mClientId(0),mFlags(0),mUserId(0),mAddress(0),mTrustLevel(0) {} //actual SDK/native inline default
    std::wstring mName;                     // +0x08
    unsigned short mClientId;               // +0x18
    unsigned long mFlags;                   // +0x1c (Client init 0x3a5350)
    unsigned long mUserId;                  // +0x20
    unsigned long mAddress;                 // +0x24, native CDKeyId (historical spelling retained)
    unsigned short mTrustLevel; //28, actualbasefactory alloc2c/storesWORD28
};
typedef SmartPtr<RoutingClientInfo> RoutingClientInfoPtr;

// Actual borrowed SmartRoutingClientInfo30, native Smartfactory2bfe60/alloc30.
// SDK/native complete30 owner for narrowly reviewed factory2bfe60; destructor remains implicit.
struct SmartRoutingClientInfo : public RoutingClientInfo {
    SmartRoutingClientInfo() : RoutingClientInfo(),mNumGroups(0),mIgnored(false) {}
    unsigned short mNumGroups; //2c
    bool mIgnored; //2e, naturalalignment2f
};
typedef std::map<unsigned short, RoutingClientInfoPtr> RoutingClientMap;

// a member of a group: the client it is
struct RoutingMemberInfo : public RefCount {
    RoutingMemberInfo() : mClientId(0),mFlags(0) {} //actual SDK/native inline default; mClientInfo default NULL
    unsigned short mClientId;               // +0x08
    unsigned char mFlags; //0a actual BYTE; naturalpadding0b; native2c5da5/2c64e7 writesBYTE
    RoutingClientInfoPtr mClientInfo;       // +0x0c
};
typedef SmartPtr<RoutingMemberInfo> RoutingMemberInfoPtr;
typedef std::map<unsigned short, RoutingMemberInfoPtr> RoutingMemberMap;

// a group (room or game) of the routing server
struct RoutingGroupInfo : public RefCount {
    RoutingGroupInfo() : mId(0),mParentId(0xffff),mCaptainId(0),mMaxClients(0),mFlags(0),mAsyncFlags(0),mObserverCount(0),mMaxPlayers(0) {} //SDK/native; historical protected names retained
    std::wstring mName;                     // +0x08
    unsigned short mId;                     // +0x18
    unsigned short mParentId; //1a, SDK mParentId; native2ba361 storesffff
    unsigned short mCaptainId;             // +0x1c
    unsigned short mMaxClients;            // +0x1e
    unsigned long mFlags;                   // +0x20 (0x800: the room is public; 0x8 in byte +0x21: a chat room)
    unsigned long mAsyncFlags; //24 native group reply storesDWORD
    unsigned short mObserverCount; //28 native storesWORD
    unsigned short mMaxPlayers;             // +0x2a
    RoutingMemberMap mMembers;              // +0x2c
};
typedef SmartPtr<RoutingGroupInfo> RoutingGroupInfoPtr;
typedef std::map<unsigned short, RoutingGroupInfoPtr> RoutingGroupMap;

// a data object of the routing server (the game's room/game descriptions)
struct RoutingDataObject : public RefCount {
    RoutingDataObject() : mLinkId(0) {} //native new30/inlinector2c81c9..2c8209; support0
    unsigned short mLinkId;                 // +0x08
    std::string mDataType;                  // +0x0c
    std::wstring mName;                     // +0x1c
    ByteBufferPtr mData;                    // +0x2c

    unsigned long GetDataLength() const { return mData->length(); }
    const char* GetData() const { return mData->data(); }
};
typedef SmartPtr<RoutingDataObject> RoutingDataObjectPtr;
typedef std::list<RoutingDataObjectPtr> RoutingDataObjectList;

// Actual RoutingOp base and proved reference-only payload prefixes.
// Partial derived operations are NEVER allocated, sized, constructed, destroyed or emitted as RTTI/vtables here.
class RoutingOp : public AsyncOp {
public:
    RoutingOp();
    RoutingOp(RoutingConnection*);
    // Destructor remains implicit; do not force a separate out-of-line body.
    virtual void Virtual04(); //RunHook2c7d20
    // Virtual08 CleanupHook inherited from actualAsyncOp; no invented extra slot.
    virtual void Virtual0C(); //SendRequest default326b60, void()
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //HandleReply2bcd30 ret8
    virtual RoutingOpType GetType() const; //2c7b10, target slot14
    void InitSendMsg(unsigned char); //2c7bc0
    void SendMsg(); //2c7c00
    void AddOp(); //2c7d10
    RoutingMemberInfoPtr GetNewMemberInfo(); //native2c4660 delegates connection slot20
    RoutingGroupInfoPtr GetNewGroupInfo(); //native2c5bb0 delegates connection slot24, true owned sret
    RoutingClientInfoPtr GetNewClientInfo(); // native2c4640; owned sret delegates connection slot1c
    RoutingConnection* GetConnection() { return mConnection.get(); }
    WriteBuffer mSendMsg; //44..5b, complete canonical size18
    bool mIsDerivedServerOp; //5c, natural alignment5d..5f
    SmartPtr<RoutingConnection> mConnection; //60, destructor owns release
};

// Additional protected-consumer prefixes; reference-only, no allocation/RTTI/vtable emission.
// Immutable SDK headers only supply nominal names; target excerpts are authoritative.
// legacy_list_ops_target.txt and legacy_list_ops_contracts.json retain ctor/parser/dtor proof.
class RoutingGetClientListOp : public RoutingOp {
public:
    RoutingGetClientListOp(RoutingConnection* c) : RoutingOp(c),mIsEx(false) {} //full74, native2c88e4..2c8917
    virtual RoutingOpType GetType()const { return (RoutingOpType)20; } //native type20, six-byte support0
    virtual void Virtual0C(); // native2c56d0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native2c57a0
    RoutingClientMap mClientMap; //64, typed dtor2bcecb->d2add; typed insert2c58b0->d1c02
    bool mIsEx; //70, native2bcdab storesfalse; actualnativeallocation74
};
// Native complete administrative operation records88/88/78; SDK/native lifetime proved.
class RoutingBanClientOp : public RoutingOp {
public:
    RoutingBanClientOp(RoutingConnection*); //2c4320,111B,ret4
    virtual void Virtual0C(); //actual six-slot native table44d0c8
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&);
    virtual RoutingOpType GetType() const; //type3, native6B/support0
    unsigned short mGroupId; //64, ctorffff
    bool mIsBanned; //66, ctortrue; alignment67
    unsigned long mBanTime; //68, ctor0
    std::wstring mBanComment; //6c..7b
    void SetBanComment(const std::wstring& v) { mBanComment=v; }
    void SetGroupId(unsigned short v) { mGroupId=v; }
    void SetBanTime(unsigned long v) { mBanTime=v; }
    void SetClientId(unsigned short v) { mClientId=v; mBanByWONId=false; }
    void SetWONId(unsigned long v,bool inIsCDKeyId) { mWONId=v; mBanByWONId=true; mWONIdIsCDKeyId=inIsCDKeyId; }
    bool mBanByWONId; //7c, ctortrue
    bool mWONIdIsCDKeyId; //7d, ctorfalse; alignment7e..7f
    unsigned long mWONId; //80, ctor0
    unsigned short mClientId; //84, ctor0; natural alignment86..87
    // Actual ordinary2c43c0 is implicit/no own-vptr write. No explicit dtor.
public:
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short GetClientId() const { return mClientId; }
};
class RoutingMuteClientOp : public RoutingOp {
public:
    RoutingMuteClientOp(RoutingConnection*); //2c78c0,111B,ret4
    virtual void Virtual0C(); //actual six-slot native table44d1c0
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&);
    virtual RoutingOpType GetType() const; //type38, native6B/support0
    unsigned short mGroupId; //64
    bool mIsMuted; //66
    unsigned long mMuteTime; //68
    std::wstring mMuteComment; //6c
    void SetMuteComment(const std::wstring& v) { mMuteComment=v; }
    void SetGroupId(unsigned short v) { mGroupId=v; }
    void SetMuteTime(unsigned long v) { mMuteTime=v; }
    void SetClientId(unsigned short v) { mClientId=v; mMuteByWONId=false; }
    void SetWONId(unsigned long v,bool inIsCDKeyId) { mWONId=v; mMuteByWONId=true; mWONIdIsCDKeyId=inIsCDKeyId; }
    void SetIsMuted(bool v) { mIsMuted=v; }
    bool mMuteByWONId; //7c
    bool mWONIdIsCDKeyId; //7d
    unsigned long mWONId; //80
    unsigned short mClientId; //84
    // Actual ordinary2c7960 implicit/no own-vptr write. Full native allocation88.
public:
    unsigned short GetGroupId() const { return mGroupId; }
    bool GetIsMuted() const { return mIsMuted; }
    unsigned short GetClientId() const { return mClientId; }
};

struct RoutingBadUser {
    RoutingBadUser() : mWONId(0), mExpirationDiff(0), mModeratorWONId(0) {} // native2c5429..2c5475
    std::wstring mName; //0..f
    unsigned long mWONId; //10
    unsigned long mExpirationDiff; //14
    unsigned long mModeratorWONId; //18
    std::wstring mModeratorName; //1c..2b
    std::wstring mModeratorComment; //2c..3b
    // Natural implicit cleanup/copy; no invented out-of-line destructor.
};
typedef std::list<RoutingBadUser> RoutingBadUserList;

enum RoutingBadUserListType {
    RoutingBadUserListType_BannedUsers=1,
    RoutingBadUserListType_MutedUsers=2
};

class RoutingGetBadUserListOp : public RoutingOp {
public:
    RoutingGetBadUserListOp(RoutingConnection* c,RoutingBadUserListType kind,
                          unsigned short group,unsigned char flags)
      :RoutingOp(c),mListType(kind),mGroupId(group),mFlags(flags) {}
    virtual void Virtual0C(); //2c5320, slot0c
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c5370, slot10
    virtual RoutingOpType GetType()const { return (RoutingOpType)19; } //2bfa10 type19
    RoutingBadUserListType mListType; //64, fullenum32
    unsigned short mGroupId; //68
    unsigned char mFlags; //6a
    RoutingBadUserList mBadUserList; //6c allocator byte/head70/count74, full78
    // Implicit normal2bfa40 has no own-vptr write; deleting2bfa20 calls it.
public:
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned char GetFlags() const { return mFlags; }
};


// Actual type22 VT44cf20, ordinarydtor2bd320.
// Reply2c63f0 comparesgroup64, writesflags68/69 andobservercount66.
// Reference-only record/list input for dialogs. No record new/sizeof/value lifetime authority.


class RoutingGetMembersOfGroupOp : public RoutingOp {
public:
    RoutingGetMembersOfGroupOp(RoutingConnection* c,unsigned short id) : RoutingOp(c),mGroupId(id) {}
    RoutingGetMembersOfGroupOp(RoutingConnection* c) : RoutingOp(c) {}
    virtual void Virtual0C(); //44cf20+0c native
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c63f0
    virtual RoutingOpType GetType() const { return (RoutingOpType)22; } //22

    unsigned short mGroupId; //64, reply2c6447 comparesrequestgroup
    unsigned short GetGroupId() const { return mGroupId; }
    void SetGroupId(unsigned short v) { mGroupId=v; }
    unsigned short mObserverCount; //66, response2c6491 fullwordstore
    unsigned short GetObserverCount() const { return mObserverCount; }
    bool mHasClientNames; //68, response2c6480 storesReadByte&1
    bool mHasClientFlags; //69, response2c6487 stores(ReadByte>>1)&1
    RoutingMemberMap mMembers; //6c, typed dtor2bd36b->d2a1e; naturalalignment6a..6b
    const RoutingMemberMap& GetMembers() const { return mMembers; }
};

class RoutingGetGroupListOp : public RoutingOp {
public:
    void ParseReplyExceptForStatus(ReadBuffer&); //2c5c10
    RoutingMemberInfoPtr GetNewMemberInfo(); //ownhelper2c5a60, nullconnection fallback
    RoutingGroupInfoPtr GetNewGroupInfo(); //ownhelper2c5ad0, nullconnection fallback
    // Only inline constructor needed by actual consumer; all behavior from target:
    RoutingGetGroupListOp() : RoutingOp(),mFlags(0),mParentGroupId(0xffff) {}
    bool ParseGroupListObj(const void*,unsigned long); //2c5f70, existing original external
    RoutingGetGroupListOp(RoutingConnection* inConnection,unsigned short inFlags)
      : RoutingOp(inConnection),mFlags(inFlags),mParentGroupId(0xffff) {}
    virtual void Virtual0C(); //nativeSendRequest, actualslot from44cf04
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c5bd0
    virtual RoutingOpType GetType() const { return (RoutingOpType)21; } //type21
    bool IncludeName() const { return (mFlags & 1) ? true : false; }
    bool IncludeCaptain() const { return (mFlags & 2) ? true : false; }
    bool IncludeMaxPlayers() const { return (mFlags & 4) ? true : false; }
    bool IncludeFlags() const { return (mFlags & 8) ? true : false; }
    bool IncludeAsyncFlags() const { return (mFlags & 0x10) ? true : false; }
    bool IncludeMemberCount() const { return (mFlags & 0x20) ? true : false; }
    bool IncludeObserverCount() const { return (mFlags & 0x40) ? true : false; }
    bool IncludeMembers() const { return (mFlags & 0x80) ? true : false; }
    unsigned short mFlags; //64
    unsigned short mParentGroupId; //66
    RoutingGroupMap mGroupMap; //68..73
    const RoutingGroupMap& GetGroupMap() const { return mGroupMap; }
    // Implicit destructor, no explicit empty/out-of-line decl requested.
public:
    unsigned short GetFlags() const { return mFlags; }
    void SetFlags(unsigned short v) { mFlags=v; }
};

class RoutingLeaveGroupOp : public RoutingOp {
public:
    RoutingLeaveGroupOp(RoutingConnection* c,unsigned short id=0)
      : RoutingOp(c),mGroupId(id) {}
    virtual void Virtual0C(); //nativeSendRequest fromVT44cf58
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //sharedphysical2c8e90
    virtual RoutingOpType GetType()const; //35
    unsigned short mGroupId; //64
public:
    unsigned short GetGroupId() const { return mGroupId; }
};

// Model-owned canonical proposal; reference-only until root allocation gate.
// Actual RTTI .?AVRoutingRegisterClientOp@WONAPI@@, VT44d21c, type42.
// Base RoutingOp64; ctor2c8450; ordinary dtor2c8570; true allocationac3ee504.
// Target ctor+writer connection_lifetime.txt; full lifetime/EH packet retained.

class RoutingRegisterClientOp : public RoutingOp {
public:
    RoutingRegisterClientOp(RoutingConnection*); //2c8450
    RoutingRegisterClientOp(RoutingConnection*,const std::wstring&,const std::wstring&,
        unsigned long,unsigned long,unsigned long,unsigned long); //2c8680 ret1c; leavesclientId94 untouched
    virtual void Virtual0C(); //2c87a0
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c8840
    virtual RoutingOpType GetType() const; //42
    std::wstring mClientName; //64..73
    std::wstring mServerPassword; //74..83
    unsigned long mReconnectId; //84: ctor2c8502 DWORD, request2c87f2 DWORD->AppendLong; reply2c88a6 DWORD
    unsigned long GetReconnectId() const { return mReconnectId; }
    unsigned long mAsyncMessageFlags; //88: ctor2c8508,request2c8800 DWORD
    unsigned long mRegisterFlags; //8c: ctor2c850e,request2c880e DWORD
    unsigned long mClientFlags; //90: ctor2c8514,request2c881c DWORD
    unsigned short mClientId; //94: ctor2c851a WORD, reply2c888b stores ReadShort AX
    RoutingClientMap mClientMap; //98: true typed map9c head/a0 count
    int mReplyCount; //a4: uninitialized by ctor, SendRequest2c87aa sets0
    int mNumRepliesNeeded; //a8: SendRequest2c87b6 sets1,2c87c2 sets2
public:
    unsigned short GetClientId() const { return mClientId; }
    const std::wstring& GetClientName() const { return mClientName; }
    unsigned long GetClientFlags() const { return mClientFlags; }
    unsigned long GetAsyncMessageFlags() const { return mAsyncMessageFlags; }
    const RoutingClientMap& GetClientMap() const { return mClientMap; }
};

// Old protected EEWonNet spelled +84 ULONG "mClientId". That is a semantic
// misname for reconnect ID. Its original DWORD+84 consumer MUST stay+84 and keep
// ULONG width during migration. Do not redirect it to the real ushort+94 ID.
// No alias, union, padding or partial allocating constructor is proposed.



// RTTI44ceb0; type6; processor2c4580 reads short64,long68.
// Routing3e7a30 GetClient64, writes client flags+10 from68.
// CANONICAL ORIGINAL: .?AVRoutingClientFlagsChangedOp@WONAPI@@; vtable0x44ceb0; processor0x2c4580; type6
class RoutingClientFlagsChangedOp : public RoutingOp {
public:
    RoutingClientFlagsChangedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 6; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c4580
    unsigned short mClientId; //64
    unsigned short GetClientId() const { return mClientId; }
    unsigned long mClientFlags; //68, compiler's natural 2-byte alignment hole
    unsigned long GetClientFlags() const { return mClientFlags; }
};

// RTTI44ce94; type7; processor2c45c0 reads group64/client66/memberbyte68.
// Later true fields6cwstring,7cflags,80bool exist but are not needed by this prefix.
// Routing3e8030 separately resolves group64/client66 and applies member68.
// CANONICAL ORIGINAL: .?AVRoutingClientJoinedGroupOp@WONAPI@@; vtable0x44ce94; processor0x2c45c0; type7
class RoutingClientJoinedGroupOp : public RoutingOp {
public:
    RoutingClientJoinedGroupOp(RoutingConnection* connection) : RoutingOp(connection),mClientFlags(0),mHasClientFlags(false) {}
    virtual RoutingOpType GetType() const { return 7; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c45c0
    unsigned short mGroupId; //64
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short mClientId; //66
    unsigned char mMemberFlags; //68
    std::wstring mClientName; //6c, target2c4610
    unsigned long mClientFlags; //7c, target2c4621
    bool mHasClientFlags; //80, target2c4624
public:
    unsigned short GetClientId() const { return mClientId; }
    unsigned char GetMemberFlags() const { return mMemberFlags; }
    const std::wstring& GetClientName() const { return mClientName; }
    unsigned long GetClientFlags() const { return mClientFlags; }
    bool HasClientFlags() const { return mHasClientFlags; }
};

// RTTI44ce78; type8; processor2c4700 calls actual info getter2c4640 then
// owns RoutingClientInfo64; body parses client fields through that pointee.
// Routing3e7d60 passes raw64 to LobbyServer::Fn3A9F50(RoutingClientInfo*).
// CANONICAL ORIGINAL: .?AVRoutingClientJoinedServerOp@WONAPI@@; vtable0x44ce78; processor0x2c4700; type8
class RoutingClientJoinedServerOp : public RoutingOp {
public:
    RoutingClientJoinedServerOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 8; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c4700
    static void ParseExClientData(ReadBuffer&, RoutingClientInfo*); // native2c4680 cdecl
    SmartPtr<RoutingClientInfo> mClientInfo; //64
    const std::wstring& GetClientName() const { if(mClientInfo.get()!=NULL) return mClientInfo->mName; else return GetEmptyWStr(); }
};

// RTTI44ce5c; type9; processor2c4800 reads two shorts.
// Routing3e8160 resolves group64 then removes client66 with owned returned SmartPtr.
// CANONICAL ORIGINAL: .?AVRoutingClientLeftGroupOp@WONAPI@@; vtable0x44ce5c; processor0x2c4800; type9
class RoutingClientLeftGroupOp : public RoutingOp {
public:
    RoutingClientLeftGroupOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 9; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c4800
    unsigned short mGroupId; //64
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short mClientId; //66
    unsigned short GetClientId() const { return mClientId; }
    unsigned char mMemberFlags; //68, genuine SDK field; actualfactory alloc6c; parser intentionally leaves unwritten
public:
    unsigned char GetMemberFlags() const { return mMemberFlags; }
};

// RTTI44ce40; type10; processor2c4840 reads one short; Routing3e7e80 removes client64.
// CANONICAL ORIGINAL: .?AVRoutingClientLeftServerOp@WONAPI@@; vtable0x44ce40; processor0x2c4840; type10
class RoutingClientLeftServerOp : public RoutingOp {
public:
    RoutingClientLeftServerOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 10; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c4840
    unsigned short mClientId; //64
public:
    unsigned short GetClientId() const { return mClientId; }
};

// RTTI44ce24; actual type13; processor2c4e40 reads short64,string68,wstring78,
// ReadBuffer2a4110 returned intrusive ByteBuffer assigned88 with full ownership.
// Routing3e8ad0 calls existing by-value GetData helperd06f1, with actual postcall release.
// CANONICAL ORIGINAL: .?AVRoutingDataObjectCreatedOp@WONAPI@@; vtable0x44ce24; processor0x2c4e40; type13
class RoutingDataObjectCreatedOp : public RoutingOp {
public:
    RoutingDataObjectCreatedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 13; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c4e40
    unsigned short mLinkId; //64
    std::string mDataType; //68
    std::wstring mName; //78
    ConstSmartPtr<ByteBuffer> mData; //88
    // SUPPORT actuald06f1: returns owned pointer in caller-provided sret; no class lifetime.
    ByteBufferPtr GetData() const { return mData; }
    unsigned short GetLinkId() const { return mLinkId; } //actual scalar value accessor
};

// Actual type14, RTTI and processor2c4f30 in op_contracts.json; two actual strings.
// Routing3e8d10 compares string68 and removes game link64; does not touch string78.
// CANONICAL ORIGINAL: .?AVRoutingDataObjectDeletedOp@WONAPI@@; vtable0x44ce08; processor0x2c4f30; type14
class RoutingDataObjectDeletedOp : public RoutingOp {
public:
    RoutingDataObjectDeletedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 14; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c4f30
    unsigned short mLinkId; //64
    std::string mDataType; //68
    std::wstring mName; //78
    unsigned short GetLinkId() const { return mLinkId; } //actual scalar value accessor
};

// RTTI44cdd0; type23; processor2c6c10 reads group64/newcaptain66.
// Routing3e7c20 passes captain66 to actual LobbyGroup::Fn3A8F80 with old-client outptr.
// CANONICAL ORIGINAL: .?AVRoutingGroupCaptainChangedOp@WONAPI@@; vtable0x44cdd0; processor0x2c6c10; type23
class RoutingGroupCaptainChangedOp : public RoutingOp {
public:
    RoutingGroupCaptainChangedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 23; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c6c10
    unsigned short mGroupId; //64
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short mCaptainId; //66
    unsigned short GetCaptainId() const { return mCaptainId; }
};

// RTTI44cdb4; type24; processor2c6c50:short64,wstring68,short78,long7c,long80.
// Existing getter2c6cb0 returns SmartPtr<RoutingGroupInfo> BY VALUE, proven sret use
// and release in Routing3e8f80. Return is neither raw pointer nor caller-owned info.
// CANONICAL ORIGINAL: .?AVRoutingGroupCreatedOp@WONAPI@@; vtable0x44cdb4; processor0x2c6c50; type24
class RoutingGroupCreatedOp : public RoutingOp {
public:
    RoutingGroupCreatedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 24; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c6c50
    unsigned short mGroupId; //64
    std::wstring mName; //68
    unsigned short mMaxClients; //78 (nominal semantics require getter read crosscheck)
    unsigned long mFlags; //7c (nominal semantics provisional)
    unsigned long mAsyncFlags; //80 (nominal semantics provisional)
    RoutingGroupInfoPtr GetGroupInfo(); //2c6cb0, CALL-ONLY existing carved method
public:
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned long GetFlags() const { return mFlags; }
    unsigned long GetAsyncFlags() const { return mAsyncFlags; }
    unsigned short GetMaxClients() const { return mMaxClients; }
    const std::wstring& GetName() const { return mName; }
};

// RTTI44cd98; type25; processor2c6da0 reads one short; Routing3e9040 removes group64.
// CANONICAL ORIGINAL: .?AVRoutingGroupDeletedOp@WONAPI@@; vtable0x44cd98; processor0x2c6da0; type25
class RoutingGroupDeletedOp : public RoutingOp {
public:
    RoutingGroupDeletedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 25; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c6da0
    unsigned short mGroupId; //64
public:
    unsigned short GetGroupId() const { return mGroupId; }
};

// RTTI44cd60; type27; processor2c6e10 reads two shorts,bool and wstring.
// Routing3e9100 resolves GAME by64 and CLIENT by66 (different order from JoinAttempt).
// CANONICAL ORIGINAL: .?AVRoutingGroupInvitationOp@WONAPI@@; vtable0x44cd60; processor0x2c6e10; type27
class RoutingGroupInvitationOp : public RoutingOp {
public:
    RoutingGroupInvitationOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 27; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c6e10
    unsigned short mGroupId; //64
    unsigned short mClientId; //66
    bool mInvited; //68
    std::wstring mComment; //6c
public:
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short GetClientId() const { return mClientId; }
    bool GetInvited() const { return mInvited; }
    const std::wstring& GetComment() const { return mComment; }
};

// RTTI44cd44; type28; processor2c6e60 reads two shorts,wstring68,byte78.
// Routing3e92d0 compares66 to current game group, resolves CLIENT by64. Order matters.
// CANONICAL ORIGINAL: .?AVRoutingGroupJoinAttemptOp@WONAPI@@; vtable0x44cd44; processor0x2c6e60; type28
class RoutingGroupJoinAttemptOp : public RoutingOp {
public:
    RoutingGroupJoinAttemptOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 28; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c6e60
    unsigned short mClientId; //64
    unsigned short mGroupId; //66
    std::wstring mComment; //68
    unsigned char mFlags; //78
public:
    unsigned short GetClientId() const { return mClientId; }
    unsigned short GetGroupId() const { return mGroupId; }
    const std::wstring& GetComment() const { return mComment; }
    unsigned char GetFlags() const { return mFlags; }
};

// RTTI44ccb8; type36; processor2c7650 reads group64,client66,memberbyte68.
// Routing3e7b40 writes LobbyClient+1c or+1d based on actual LobbyGroup kind.
// CANONICAL ORIGINAL: .?AVRoutingMemberFlagsChangedOp@WONAPI@@; vtable0x44ccb8; processor0x2c7650; type36
class RoutingMemberFlagsChangedOp : public RoutingOp {
public:
    RoutingMemberFlagsChangedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 36; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c7650
    unsigned short mGroupId; //64
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short mClientId; //66
    unsigned short GetClientId() const { return mClientId; }
    unsigned char mMemberFlags; //68
    unsigned char GetMemberFlags() const { return mMemberFlags; }
};

// RTTI44cc9c; type39; processor2c7d40 explicitly reads FLAGS AS A WORD.
// Flags100 drives Routing3e7770: old uchar bitfield+gap is not a faithful representation.
// List78 has actual0c-byte nodes, ushort atnode8, count80. Root-owned game adapter
// migration must replace mL80 reads with list.size/empty, bit2 reads with flag&4.
// CANONICAL ORIGINAL: .?AVRoutingPeerChatOp@WONAPI@@; vtable0x44cc9c; processor0x2c7d40; type39
class RoutingPeerChatOp : public RoutingOp {
public:
    RoutingPeerChatOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 39; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c7d40
    unsigned short mClientId; //64 sender
    unsigned short mFlags; //66
    std::wstring mText; //68
    std::list<unsigned short> mRecipients; //78
public:
    unsigned short GetClientId() const { return mClientId; }
    unsigned short GetFlags() const { return mFlags; }
    bool IsWhisper() const { return (mFlags & 4) ? true : false; }
    const std::wstring& GetText() const { return mText; }
    const std::list<unsigned short>& GetRecipients() const { return mRecipients; }
};

// RTTI44cc80; type40; processor2c7e00 ReadShort64, ReadBuffer2a4110 thenowned buffer68.
// Actual getter3e79f0 returns SmartPtr by value and Routing3e7940 releases after use.
// CANONICAL ORIGINAL: .?AVRoutingPeerDataOp@WONAPI@@; vtable0x44cc80; processor0x2c7e00; type40
class RoutingPeerDataOp : public RoutingOp {
public:
    RoutingPeerDataOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 40; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c7e00
    unsigned short mClientId; //64
    ByteBufferPtr mData; //68
    ByteBufferPtr GetData() const { return mData; }
public:
    unsigned short GetClientId() const { return mClientId; }
};

// RTTI44cc64; type48; processor2c8b80 ReadWString into64.
// CANONICAL ORIGINAL: .?AVRoutingServerAlertOp@WONAPI@@; vtable0x44cc64; processor0x2c8b80; type48
class RoutingServerAlertOp : public RoutingOp {
public:
    RoutingServerAlertOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 48; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c8b80
    std::wstring mText; //64
};

// RTTI44cc48; type49; processor2c8bb0 validates only, but actual dtor2bc370
// destroys the default-constructed COW WSTRING64 (pointer68,length6c,reserved70).
// Routing3e89b0 reads this actual member even though no text came from wire.
// CANONICAL ORIGINAL: .?AVRoutingServerShutdownAbortedOp@WONAPI@@; vtable0x44cc48; processor0x2c8bb0; type49
class RoutingServerShutdownAbortedOp : public RoutingOp {
public:
    RoutingServerShutdownAbortedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 49; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c8bb0
    std::wstring mText; //64
};

// RTTI44cc2c; type50; processor2c8bd0 ReadWString64 then ReadLong74.
// CANONICAL ORIGINAL: .?AVRoutingServerShutdownStartedOp@WONAPI@@; vtable0x44cc2c; processor0x2c8bd0; type50
class RoutingServerShutdownStartedOp : public RoutingOp {
public:
    RoutingServerShutdownStartedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 50; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c8bd0
    std::wstring mText; //64
    unsigned long mSeconds; //74
public:
    unsigned long GetSeconds() const { return mSeconds; }
};

// RTTI44cc10; type58; processor2c8eb0 reads short64,bool66,wstring6c,long68.
// Routing3e82b0 ignores an unban; duration0 or>=10 meanskind0,1..9 meanskind1.
// CANONICAL ORIGINAL: .?AVRoutingYouWereBannedOp@WONAPI@@; vtable0x44cc10; processor0x2c8eb0; type58
class RoutingYouWereBannedOp : public RoutingOp {
public:
    RoutingYouWereBannedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 58; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c8eb0
    unsigned short mGroupId; //64
    bool mIsBanned; //66
    unsigned long mDuration; //68
    std::wstring mComment; //6c
public:
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned long GetDuration() const { return mDuration; }
    const std::wstring& GetComment() const { return mComment; }
};

// Actual type59, RTTI and processor2c8f00 in op_contracts.json.
// Same wire widths as banned; Routing3e8580 uses bool66 to selectkind2or3.
// CANONICAL ORIGINAL: .?AVRoutingYouWereMutedOp@WONAPI@@; vtable0x44cbf4; processor0x2c8f00; type59
class RoutingYouWereMutedOp : public RoutingOp {
public:
    RoutingYouWereMutedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 59; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c8f00
    unsigned short mGroupId; //64
    bool mIsMuted; //66
    unsigned long mDuration; //68
    std::wstring mComment; //6c
public:
    unsigned short GetGroupId() const { return mGroupId; }
    bool GetIsMuted() const { return mIsMuted; }
    unsigned long GetDuration() const { return mDuration; }
    const std::wstring& GetComment() const { return mComment; }
};

// Reference-only; RTTI type41 VT44d1f8, ctor2c8000/native2bf1d0.
// Constructor/string/list reader proof: read_data_op_evidence.txt.
// Allocation88 is original evidence, NOT permission to allocate this partial declaration.
// CANONICAL ORIGINAL: .?AVRoutingReadDataObjectOp@WONAPI@@; vtable0x44d1f8; processor0x2c8130; type41
class RoutingReadDataObjectOp : public RoutingOp {
public:
    RoutingReadDataObjectOp(RoutingConnection*); //actual native full owner constructor
    virtual void Virtual0C();
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&);
    virtual RoutingOpType GetType() const;
    void ParseReplyExceptForStatus(ReadBuffer&); //native2c8170,500B
    unsigned short mLinkId; //64, originalnative2bf231
    std::string mDataType; //68, ctorzero6c/70/74,nativeassign2bf24d
    void SetDataType(const std::string& v) { mDataType=v; }
    unsigned char mFlag; //78 native flag mask; target AppendByte2c8117, SDK mFlags
    bool mSubscribe; //79, native2bf22d
    RoutingDataObjectList mDataObjects; //7c, head80,count84; reader2c8170
public:
    unsigned short GetLinkId() const { return mLinkId; }
};


// Reference-only protected-consumer prefixes, no allocation/lifetime/RTTI authority.
// every_op_processors.txt and every_op_contracts.json retain RTTI, wire parser and type.
class RoutingDataObjectModifiedOp : public RoutingOp {
public:
    RoutingDataObjectModifiedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 15; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c4f80
    unsigned short mLinkId; //64 ReadShort2c4fb8
    std::string mDataType; //68 ReadString2c4fc4
    std::wstring mName; //78 ReadWString2c4fd1
    unsigned short mOffset; //88 ReadShort2c4fdf, not ULONG
    bool mIsInsert; //8a ReadByte2c4ff4; nominalSDKbool
    ByteBufferPtr mData; //8c owned parserassign2c5045
    unsigned short GetLinkId() const { return mLinkId; } //BY VALUE, preserves map-key temporary
    unsigned short GetOffset() const { return mOffset; }
    bool GetIsInsert() const { return mIsInsert; }
    ByteBufferPtr GetData() const { return mData; } //owned BY VALUE, keep temporary lifetime
};
// Borrowed-only notification receivers; parser implementation does not allocate them.
class RoutingGroupFlagsChangedOp : public RoutingOp {
public:
    RoutingGroupFlagsChangedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 26; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c6dd0
    unsigned short mGroupId; //64
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned long mGroupFlags; //68
    unsigned long GetGroupFlags() const { return mGroupFlags; }
    unsigned long mAsyncFlags; //6c
};
class RoutingGroupMemberCountOp : public RoutingOp {
public:
    RoutingGroupMemberCountOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 30; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c6eb0
    unsigned short mGroupId; //64
    unsigned short mMemberCount; //66
public:
    unsigned short GetGroupId() const { return mGroupId; }
};
class RoutingGroupNameChangedOp : public RoutingOp {
public:
    RoutingGroupNameChangedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 31; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c6f30
    unsigned short mGroupId; //64
    unsigned short GetGroupId() const { return mGroupId; }
    std::wstring mName; //68
};
class RoutingGroupObserverCountOp : public RoutingOp {
public:
    RoutingGroupObserverCountOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 32; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c6f70
    unsigned short mGroupId; //64
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short mObserverCount; //66
    unsigned short GetObserverCount() const { return mObserverCount; }
};

// Actual type29, NOT30. VT44cd28, wire25, processor2c6ef0.
class RoutingGroupMaxPlayersChangedOp : public RoutingOp {
public:
    RoutingGroupMaxPlayersChangedOp(RoutingConnection* connection) : RoutingOp(connection) {}
    virtual RoutingOpType GetType() const { return 29; } //actual native slot14, support0
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); // native 0x2c6ef0
    unsigned short mGroupId; //64 store2c6f12
    unsigned short mMaxPlayers; //66 store2c6f1b
    unsigned short GetMaxPlayers() const { return mMaxPlayers; }
    unsigned short GetGroupId() const { return mGroupId; } //BY VALUE, not constref
    unsigned short GetNewMaxPlayers() const { return mMaxPlayers; }
};
// Complete98 records: actual native allocations3ed6c9/3ed9a1, six-slot tables.
// Implicit ordinary destructors:2c4b40/2c7250 have no own-vptr store.
class RoutingCreateGroupOp : public RoutingOp {
public:
    void CheckLeaveGroup(short); //2c4cb0

    RoutingCreateGroupOp(RoutingConnection*); //2c4a80
    virtual void Virtual0C(); //2c4c20 SendRequest
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c4d90 HandleReply
    virtual RoutingOpType GetType() const { return (RoutingOpType)12; } //2c4b10 type12
    unsigned short mGroupId; //64
    unsigned short mParentGroupId; //66
    std::wstring mGroupName; //68
    void SetGroupName(const std::wstring& v) { mGroupName=v; }
    std::wstring mGroupPassword; //78
    void SetGroupPassword(const std::wstring& v) { mGroupPassword=v; }
    unsigned short mMaxPlayers; //88
    unsigned char mJoinFlags; //8a, alignment8b
    unsigned long mGroupFlags; //8c
    unsigned long mAsyncFlags; //90
    bool mLeaveGroupOnCancel; //94, natural tail95..97
    void SetGroupFlags(unsigned long v) { mGroupFlags=v; }
    void SetJoinFlags(unsigned char v) { mJoinFlags=v; }
    void SetLeaveGroupOnCancel(bool v) { mLeaveGroupOnCancel=v; }
    unsigned short GetGroupId() const { return mGroupId; }
};
class RoutingJoinGroupOp : public RoutingOp {
public:
    void SetGroupId(unsigned short v) { mGroupId=v; }
    RoutingJoinGroupOp(RoutingConnection*); //2c7140
    virtual void Virtual0C(); //2c7370 SendRequest
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //2c73e0 HandleReply
    virtual RoutingOpType GetType() const { return (RoutingOpType)34; } //2c7220 type34
    unsigned short mGroupId; //64, alignment66..67
    std::wstring mGroupPassword; //68
    void SetGroupPassword(const std::wstring& v) { mGroupPassword=v; }
    std::wstring mJoinComment; //78
    void SetJoinComment(const std::wstring& v) { mJoinComment=v; }
    unsigned char mJoinFlags; //88
    unsigned char GetJoinFlags() const { return mJoinFlags; }
    unsigned char mMemberFlags; //89, alignment8a..8b
    RoutingMemberMap mMemberMap; //8c, size98
    unsigned short GetGroupId() const { return mGroupId; }
public:
    unsigned char GetMemberFlags() const { return mMemberFlags; }
};
// Native request operation receivers. Borrowed-only until a separate full ctor/allocation/lifetime packet; no operation allocations by serializer cohort.
class RoutingCancelJoinGroupOp : public RoutingOp {
public:
    RoutingCancelJoinGroupOp(RoutingConnection* c,unsigned short id=0,bool leave=true) : RoutingOp(c),mGroupId(id),mLeaveIfAlreadyInGroup(leave) {}
    virtual void Virtual0C(); //native0x2c4540 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type5 existing slot14
    unsigned short mGroupId; //64
    bool mLeaveIfAlreadyInGroup; //66
public:
    unsigned short GetGroupId() const { return mGroupId; }
};
class RoutingSetGroupFlagsOp : public RoutingOp {
public:
    RoutingSetGroupFlagsOp(RoutingConnection* c,unsigned short id=0,unsigned long mask=0,unsigned long flags=0,unsigned long asyncMask=0,unsigned long asyncFlags=0) : RoutingOp(c),mGroupId(id),mGroupFlagMask(mask),mGroupFlags(flags),mAsyncFlagMask(asyncMask),mAsyncFlags(asyncFlags) {}
    virtual void Virtual0C(); //native0x2c8c60 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type52 existing slot14
    unsigned short mGroupId; //64
    unsigned long mGroupFlagMask; //68
    unsigned long mGroupFlags; //6c
    unsigned long mAsyncFlagMask; //70
    unsigned long mAsyncFlags; //74
public:
    unsigned short GetGroupId() const { return mGroupId; }
};
class RoutingSendServerAlertOp : public RoutingOp {
public:
    RoutingSendServerAlertOp(RoutingConnection* c,const std::wstring& text) : RoutingOp(c),mAlertText(text) {}
    virtual void Virtual0C(); //native0x2c8b20 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type47 existing slot14
    std::wstring mAlertText; //64
    std::list<unsigned short> mRecipientList; //74
};
class RoutingStartServerShutdownOp : public RoutingOp {
public:
    RoutingStartServerShutdownOp(RoutingConnection* c,const std::wstring& text,unsigned long seconds) : RoutingOp(c),mAlertText(text),mSecondsUntilShutdown(seconds) {}
    virtual void Virtual0C(); //native0x2c8cc0 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type56 existing slot14
    std::wstring mAlertText; //64
    unsigned long mSecondsUntilShutdown; //74
};
class RoutingAbortServerShutdownOp : public RoutingOp {
public:
    RoutingAbortServerShutdownOp(RoutingConnection* c) : RoutingOp(c) {}
    virtual void Virtual0C(); //native0x2c4170 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type1 existing slot14
};
class RoutingSetClientFlagsOp : public RoutingOp {
public:
    RoutingSetClientFlagsOp(RoutingConnection* c,unsigned short mask=0,unsigned long flags=0,unsigned long asyncMask=0,unsigned long asyncFlags=0) : RoutingOp(c),mClientFlagMask(mask),mClientFlags(flags),mAsyncMessageFlagMask(asyncMask),mAsyncMessageFlags(asyncFlags) {}
    virtual void Virtual0C(); //native0x2c8c10 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type51 existing slot14
    unsigned long mClientFlagMask; //64
    unsigned long mClientFlags; //68
    unsigned long mAsyncMessageFlagMask; //6c
    unsigned long mAsyncMessageFlags; //70
};
class RoutingSendChatOp : public RoutingOp {
public:
    RoutingSendChatOp(RoutingConnection* c) : RoutingOp(c),mFlags(0) {}
    virtual void Virtual0C(); //native0x2c8a20 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type44 existing slot14
    unsigned short mFlags; //64
    std::wstring mText; //68
    void SetFlags(unsigned short v) { mFlags=v; }
    void SetRecipients(const std::list<unsigned short>& v) { mRecipients=v; }
    void SetText(const std::wstring& v) { mText=v; }
    std::list<unsigned short> mRecipients; //78
public:
    unsigned short GetFlags() const { return mFlags; }
};
class RoutingSendDataOp : public RoutingOp {
public:
    RoutingSendDataOp(RoutingConnection* c) : RoutingOp(c),mFlags(0) {}
    virtual void Virtual0C(); //native0x2c8aa0 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type46 existing slot14
    unsigned char mFlags; //64
    void SetFlags(unsigned char v) { mFlags=v; }
    void SetData(const ByteBuffer* v) { mData=v; }
    void SetRecipients(const std::list<unsigned short>& v) { mRecipients=v; }
    ConstSmartPtr<ByteBuffer> mData; //68
    std::list<unsigned short> mRecipients; //6c
public:
    unsigned char GetFlags() const { return mFlags; }
};
class RoutingBecomeModeratorOp : public RoutingOp {
public:
    RoutingBecomeModeratorOp(RoutingConnection* c,bool on=false) : RoutingOp(c),mModeratorOn(on) {}
    virtual void Virtual0C(); //native0x2c4510 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type4 existing slot14
    bool mModeratorOn; //64
};
class RoutingAcceptClientOp : public RoutingOp {
public:
    RoutingAcceptClientOp(RoutingConnection*); //native2c4190,89B
    virtual void Virtual0C(); //native0x2c42d0 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type2 existing slot14
    unsigned short mGroupId; //64
    unsigned short mClientId; //66
    bool mAccepted; //68
    std::wstring mAcceptComment; //6c
    void SetAcceptComment(const std::wstring& v) { mAcceptComment=v; }
public:
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short GetClientId() const { return mClientId; }
};
class RoutingDeleteDataObjectOp : public RoutingOp {
public:
    RoutingDeleteDataObjectOp(RoutingConnection*); //actual native full owner constructor
    virtual void Virtual0C(); //native0x2c5210 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type16 existing slot14
    unsigned short mLinkId; //64
    std::string mDataType; //68
    void SetDataType(const std::string& v) { mDataType=v; }
    std::wstring mDataName; //78
    void SetDataName(const std::wstring& v) { mDataName=v; }
public:
    unsigned short GetLinkId() const { return mLinkId; }
};
class RoutingInviteClientOp : public RoutingOp {
public:
    RoutingInviteClientOp(RoutingConnection*); //native2c6fb0,89B
    virtual void Virtual0C(); //native0x2c70f0 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type33 existing slot14
    unsigned short mGroupId; //64
    unsigned short mClientId; //66
    bool mInvited; //68
    std::wstring mInviteComment; //6c
    void SetInviteComment(const std::wstring& v) { mInviteComment=v; }
public:
    unsigned short GetGroupId() const { return mGroupId; }
    unsigned short GetClientId() const { return mClientId; }
    bool GetInvited() const { return mInvited; }
};
class RoutingModifyDataObjectOp : public RoutingOp {
public:
    RoutingModifyDataObjectOp(RoutingConnection*); //actual native full owner constructor
    virtual void Virtual0C(); //native0x2c7840 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type37 existing slot14
    unsigned short mLinkId; //64
    std::string mDataType; //68
    void SetDataType(const std::string& v) { mDataType=v; }
    std::wstring mDataName; //78
    void SetDataName(const std::wstring& v) { mDataName=v; }
    unsigned short mOffset; //88
    bool mIsInsert; //8a
    ByteBufferPtr mData; //8c
public:
    unsigned short GetLinkId() const { return mLinkId; }
};
class RoutingUnsubscribeDataObjectOp : public RoutingOp {
public:
    RoutingUnsubscribeDataObjectOp(RoutingConnection*); //actual native full owner constructor
    virtual void Virtual0C(); //native0x2c8e40 existing slot0c
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&); //shared physical2c8e90 slot10
    virtual RoutingOpType GetType() const; //native type57 existing slot14
    unsigned short mLinkId; //64
    std::string mDataType; //68
    void SetDataType(const std::string& v) { mDataType=v; }
    unsigned char mFlags; //78
public:
    unsigned short GetLinkId() const { return mLinkId; }
    unsigned char GetFlags() const { return mFlags; }
};



// Actual REQUEST11, distinct from notificationCreated13 and read/subscription41.
// Native2becd0 alloc8c/ctor2c4870; ctor writes VT44d0e4. SDK and target agree.
class RoutingCreateDataObjectOp : public RoutingOp {
public:
    RoutingCreateDataObjectOp(RoutingConnection*); //actual native full owner constructor
    virtual void Virtual0C();
    virtual WONStatus Virtual10(unsigned char, ReadBuffer&);
    virtual RoutingOpType GetType() const;
    unsigned char mFlags; //64 native2bed34; NOT linkID64
    unsigned short mLinkId; //66 native2bed37, ctor2c48c4 zeroWORD
    std::string mDataType; //68
    void SetDataType(const std::string& v) { mDataType=v; }
    std::wstring mDataName; //78
    void SetDataName(const std::wstring& v) { mDataName=v; }
    ByteBufferPtr mData; //88 ctor2c48bezero; native2bedad ownedassign
    unsigned short GetLinkId() const { return mLinkId; } //BYVALUE, trueoffset66
    ByteBufferPtr GetData() const { return mData; } //ownedBYVALUE, preservetemporary releases
public:
    unsigned char GetFlags() const { return mFlags; }
};

class RoutingGetUserStatusOp : public UDPManagerSendRecvOp {
public:
    typedef std::map<unsigned long,unsigned char> FriendStatusMap;
    RoutingGetUserStatusOp(const IPAddr&,UDPManager*); //2c6630/ret8
    virtual void Virtual04(); //RunHook2c6970
    virtual WONStatus Virtual0C(ReadBuffer&); //UnpackMsg2c6ab0
    void AddWONUserId(unsigned long); //2c68c0/ret4
    const FriendStatusMap& GetFriendStatusMap() const { return mFriendStatusMap; }
    FriendStatusMap mFriendStatusMap; //88 allocator/header8c/count90, full94
    // No explicit destructor: ordinary2c6750 has NOown-vptr write and inlines
    // genuine map destruction followed by complete base88 member/base cleanup.
};


inline const char* GetBufferData(const ByteBufferPtr& inBuffer)
{
    return inBuffer->data();
}

// the WON server list the account ops use (??_G 0xceaec, dtor 0xceb08; sizeof 0x3c).: layout from
// the dtor and the base game's out-of-line ctor (base exe 0x4c772a; AoC's WONAPI inlines it: 0x2ddb70, 0x3628c0);
// member names PROVISIONAL
typedef std::map<std::wstring,DirEntityList> DirEntityMap;
typedef std::set<IPAddr> AddrSet;
class ServerContext : public RefCount {
public:
    ServerContext() : mB08(true), mB09(false) {}
    bool AddAddress(const IPAddr& inAddress); //2ac4b0, actual AL success
    void Clear(); //2ac390
    void NotifyFailed(const IPAddr&); //2ac980
    bool AddAddressesFromDir(const DirEntityList&,const wchar_t* inNameFilter=NULL); //2acab0/332
    void RemoveAllAddresses(); //2acc00, genuineSDKname/existing5Bbody, zero new body
    unsigned long GetNumAddresses() { return mPingMap.size(); }
    typedef std::list<IPAddr> AddrList;
    void CopyAddresses(AddrList&); //2ac6d0, complete685B behavior
    bool RemoveAddress(const IPAddr&);
    void CopyAddresses(AddrSet&);
    bool AddAddressesFromDir(const DirEntityMap&,const std::wstring&,const wchar_t* inNameFilter=NULL);
    bool AtomicClearAndAddAddressesFromDir(const DirEntityList&,const wchar_t* inNameFilter=NULL);
    bool AtomicClearAndAddAddressesFromDir(const DirEntityMap&,const std::wstring&,const wchar_t* inNameFilter=NULL);

    bool mB08;                              // +0x08
    bool mB09;                              // +0x09
    CriticalSection mLock;                  // +0x0c
    std::map<IPAddr, AddrList::iterator> mPingMap; //24; actual inverse priority-list iterator, not integer
    AddrList mAddresses; //30, actual owned IPAddr priority list
};

// AsyncOpWithTracker's op tracker (; the lock, then the maps: ~AsyncOpWithTracker 0x4cff95 addresses the
// maps from the lock (+0x18) and destroys them first; names and key types PROVISIONAL)
// Original2a9140/2a9270 insert without unique-key rejection: BOTH multimaps.
struct AsyncOpTrackerEntry;
typedef std::multimap<unsigned long, AsyncOpTrackerEntry*> AsyncOpTrackerEntryMap;
typedef std::multimap<AsyncOpPtr, AsyncOpTrackerEntryMap::iterator> AsyncOpTrackerOpMap;
// Originalallocation8, constructor2a9380: owningop then inverse-op-map iterator.
struct AsyncOpTrackerEntry {
    AsyncOpTrackerEntry(const AsyncOpPtr& op,const AsyncOpTrackerOpMap::iterator& opItr) : mOp(op),mOpItr(opItr) {}
    AsyncOpPtr mOp;                         // +0x00
    AsyncOpTrackerOpMap::iterator mOpItr;   // +0x04
};
class AsyncOpTrackerMaps {
public:
    typedef AsyncOpTrackerEntryMap EntryMap;
    typedef AsyncOpTrackerOpMap OpMap;
    typedef EntryMap::iterator iterator;
    typedef EntryMap::const_iterator const_iterator;
    EntryMap::iterator begin() { return mEntries.begin(); }
    EntryMap::iterator end() { return mEntries.end(); }
    EntryMap::const_iterator begin() const { return mEntries.begin(); }
    EntryMap::const_iterator end() const { return mEntries.end(); }
    int size() const { return mEntries.size(); }
    // Genuine SDK BiMap methods on this existing complete adopted owner.
    EntryMap::iterator erase(EntryMap::iterator); //2a8e70/146, support dependency
    OpMap::iterator erase_val(OpMap::iterator); //genuine SDK inline dependency; no RVA claim
    OpMap::iterator erase_val(OpMap::iterator,OpMap::iterator); //2a9040/245, authored support
    AsyncOpTrackerMaps(bool inFlag1, bool inFlag2) : mB18(inFlag1), mB19(inFlag2) {} // inline (LobbyLogic 0x7e5930 expands it; 0x6d9d20 is a refused copy)
    ~AsyncOpTrackerMaps()
    {
        // LOST-DEBUG: none: exact only; budget grid of 0x6accf0, 0x6d3d50 (k=14..18 of these reads; they conflicted through every other form)
        EE_LOST_TRACE(mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18+mB18);
        for (EntryMap::iterator anItr=mEntries.begin();anItr!=mEntries.end();++anItr)
            delete (*anItr).second;
    }
    EntryMap mEntries;                      // +0x00
    OpMap mOps;                             // +0x0c, valueisEntryMap::iterator
    bool mB18;                             // +0x18, ctor2d9dc2
    bool mB19;                             // +0x19, ctor2d9dc9
};
class AsyncOpTracker {
public:
    typedef AsyncOpTrackerMaps OpMap;
    int GetNumTrack() { return mMaps.size(); }
    const OpMap& GetOpMap() { return mMaps; }
    // LOST-DEBUG: budget census of two rows (0x6acc10, 0x6d0170): this ctor costs 28..66 IL more than its code
    AsyncOpTracker() : mMaps(true, false) { EE_LOST_TRACE(mMaps.mB18); EE_LOST_TRACE(mMaps.mB18); EE_LOST_TRACE(mMaps.mB18); EE_LOST_TRACE(mMaps.mB18); }
    AsyncOp* AddOp(AsyncOp* inOp, unsigned long inId); // original29fc80
    bool RemoveOp(AsyncOp* inOp);             // original29fe50
    void KillAll();                          // original29ffa0
    CriticalSection mLock;                  // +0x00
    AsyncOpTrackerMaps mMaps;               // +0x18
};

// an op that tracks sub-ops
class AsyncOpWithTracker : public AsyncOp {
private:
    class TrackerCompletion; //actual nested16 owner, RTTI44c854/native2a0100
    friend class TrackerCompletion;
    void Callback(AsyncOp*,int); //2a00d0 pending/remove/virtualcallback dispatcher
public:
    AsyncOp* Track(AsyncOp*,int); //2a0100, SDK protected nominal; returns tracker AddOp result
    virtual void Virtual08();               //slot08 CleanupHook2a0290
    virtual bool CallbackHook(AsyncOp* inOp,int inParam); //slot0c, bool/ret8
    AsyncOpTracker mTracker;                //44..78
};

class BlockingSocket;
// Complete original-backed ServerOp94 contract; construction allowed by native proof.
// PeerAuthOp complete2244 owner below; constructors/destructors/protocol remain external.
// Exact target RTTI: PeerAuthOp>ServerOp>AsyncOpWithTracker>AsyncOp>RefCount.
// peerauth_rtti.txt: VT44d4b0 four actual slots, ServerOpVT44c9e8 four slots.
// Canonical AsyncOpWithTracker78 must be complete before this prefix.
// Complete original ServerOp94: ctor2acc10, implicit dtor2accf0 (no own-vptr).
class ServerOp : public AsyncOpWithTracker {
public:
    void SetMaxTimes(int,int,int); //2ace20, cohesive support0
    BlockingSocket* GetSocket() { return mSocket.get(); } //genuine primary SDK borrowed accessor, no refcount changes
    void SetAuthType(AuthType theType,AuthContext* theContext=NULL) { mAuthType=theType; mAuthContext=theContext; }
    void SetLengthFieldSize(unsigned char theLengthFieldSize) { mLengthFieldSize=theLengthFieldSize; }
    unsigned char GetLengthFieldSize() { return mLengthFieldSize; }
protected:
    void SendMsgAsync(ByteBufferPtr,int inTrackId=0);
    void RecvMsgAsync(int inTrackId=0);
public:
    DWORD ConnectTime(); //2ace50
    DWORD SendTime(); //2ace80
    DWORD RecvTime(); //2aceb0
    void CopyMaxTimes(ServerOp*); //2acee0, cohesive support0
    ServerOp(); //2acc10, constructor leaves mAuthType/mLengthFieldSize untouched
    // CallbackHook slot0c inherited from actual AsyncOpWithTracker; no own ServerOp override.
    SmartPtr<BlockingSocket> mSocket; //78
    SmartPtr<AuthContext> mAuthContext; //7c
    AuthType mAuthType; //80
    unsigned char mLengthFieldSize; //84
    unsigned char mClientKeySize; //85=8
    bool mUseClientKey; //86=false, natural alignment87
    void SetClientKeySize(unsigned char size,bool useClientKey) { mClientKeySize=size;mUseClientKey=useClientKey; }
    DWORD mMaxConnectTime; //88=60000
    DWORD mMaxSendTime; //8c=60000
    DWORD mMaxRecvTime; //90=60000
};

// Actual complete AuthSession10cc: constructor2e27b0 copies by-value Blowfish10b4;
// dtor2e2a80 destroys actual RawBuffers and RefCount, no opaque private lifetime.
class AuthSession : public RefCount {
protected:
    AuthType mAuthType; //08
    unsigned char mLengthFieldSize; //0c, natural alignment0d
    unsigned short mId; //0e
    unsigned short mInSeq; //10
    unsigned short mOutSeq; //12
    Blowfish mKey; //14..10c8
    time_t mLastUseTime; //10c8, extent10cc
    virtual ~AuthSession(); //2e2a80
public:
    AuthSession(AuthType, unsigned short, Blowfish, unsigned char); //2e27b0/RET10c0
    WONStatus Encrypt(ByteBufferPtr&); //2e2b40
    WONStatus Decrypt(ByteBufferPtr&); //2e2d60
    time_t GetLastUseTime() const { return mLastUseTime; }
    const Blowfish& GetKey() const { return mKey; }
    AuthType GetAuthType() const { return mAuthType; }
    unsigned short GetSessionId() const { return mId; }
    void SetLengthFieldSize(unsigned char size) { mLengthFieldSize = size; }
    bool IsEncrypted() { return (mAuthType==AUTH_TYPE_SESSION || mAuthType==AUTH_TYPE_PERSISTENT); } //genuine SDK inline; same native AuthSession08 predicate as PeerAuthServerOp::Success
};
// Complete target CryptTransform8; native new8 at2ad6d7, VT44ca1c, SDK CryptTransform.h.
// No AuthSession value/allocation authority; actual retained reference field only.
class CryptTransform : public MsgTransform {
    SmartPtr<AuthSession> mSession; //04, size08; implicit dtor2ad8e0 releases then MsgTransform dtor
public:
    CryptTransform(AuthSession* inSession) : mSession(inSession) {} //native inline, support0
    virtual WONStatus Send(ByteBufferPtr& inMessage) { return mSession->Encrypt(inMessage); } //2ad8b0, support0
    virtual WONStatus Recv(ByteBufferPtr& inMessage) { return mSession->Decrypt(inMessage); } //2ad8a0, support0
};

class ServerRequestOp : public ServerOp {
public:
    ServerRequestOp(const IPAddr&); //2ad050
    void Init(); //2acf10, support0
    void SetServerContext(ServerContext*); //2ad2e0, support0
    void SetAddr(const IPAddr&); //2ad340
    IPAddr GetAddr() const; //2ad410, support0
    void FinishSaveSession(WONStatus); //2ad5b0
    void SetLastServerError(WONStatus); //2ad660
    bool TryConnect(); //2ad6b0
    bool TryPeerToPeerAuth(); //2ad950, native external full PeerAuth owner
    bool TryNextServer(); //2add80
    void QueueSocketOp(SocketOp*,DWORD); //2adae0, native external
    WONStatus Send(); //2ae050
    WONStatus Recv(); //2ae2d0
    bool CheckExpiredSession(); //2ae200
    ServerRequestOp(ServerContext*); //2acf60
    const IPAddr& GetCurAddr() const { return mCurAddr; } //actual borrowedb0 getter, support0
    virtual void Virtual04(); //2ae390
    virtual void Virtual08(); //2ae4c0
    virtual bool CallbackHook(AsyncOp*,int); //2adb10
    virtual WONStatus Virtual10(); //2ad020
    virtual WONStatus Virtual14(); //2c7b10
    virtual void Virtual18(); //326b60
protected:
    WONStatus InvalidReplyHeader(); //2ad6a0, true protected native16B; returns100057
    virtual ~ServerRequestOp(); //2ad110, explicit own-vptr store
public:
    typedef std::list<IPAddr> AddrList;
    AddrList mAddrList; //94
    AddrList::iterator mAddrItr; //a0
    SmartPtr<AuthSession> mCurSession; //a4
    SmartPtr<AuthSession> mUsedSession; //a8
    bool mReusingSession; //ac
    bool mSessionExpired; //ad, natural alignmentae/af
    IPAddr mCurAddr; //b0..d8
    bool mUseAuth2; //d8
    bool mCloseConnection; //d9, natural alignmentda/db
    WONStatus mGetCertStatus; //dc
    bool mUseLastServerErrorForFinishStatus; //e0, natural alignmente1..e3
    WONStatus mLastServerError; //e4
    SmartPtr<ServerContext> mServerContext; //e8
    ByteBufferPtr mRequest; //ec
    ByteBufferPtr mResponse; //f0..f4
};

// Native complete f8 owner, RTTI/VT44d138, primary2c5ff0/6150/6270.
class RoutingGetGroupListReqOp : public ServerRequestOp {
public:
    RoutingGetGroupListReqOp(const IPAddr&,unsigned char inFlags=0);
    virtual WONStatus Virtual10(); //GetNextRequest
    virtual WONStatus Virtual14(); //CheckResponse
    SmartPtr<RoutingGetGroupListOp> mOp; //f4, implicit destructor releases before base
};

// Actual AoC directory operation types. Legacy DEAD GetDirOp above remains unrepaired/version-specific.
// SCRATCH fragment for Model-owned canonical header window only.
// Reuse existing canonical DirEntityList/SmartPtr, never duplicate these aliases.
typedef std::set<std::string> DirDataTypeSet;
// Genuine SDK stateless parser; native static cdecl2f0510.
class DirEntityReplyParser {
public:
    static WONStatus ParseSingleEntityReply(const void*, unsigned long, DirEntityPtr&); //same-SDK complete single-entity parser; SOURCE_ONLY_NO_NATIVE_ID
    static WONStatus ParseMultiEntityReply(const void*, unsigned long, DirEntityList&, unsigned char* inSequence = NULL);
    static bool OnlyDirs(DWORD); //native2f04f0 support0
    static bool OnlyServices(DWORD); //native2f0500 support0
};

class GetEntityRequest : public RefCount {
protected:
    virtual ~GetEntityRequest(); //2f0da0, explicit ownVT44d4f0 store2f0dc2
    DWORD mFlags; //08, actual default constructor-inline flags06020837
    std::wstring mPath; //0c..1c
    DirDataTypeSet mDataTypes; //1c..28
    DirEntityList mDirEntityList; //28..34
    DirEntityMap mDirEntityMap; //34..40
    WONStatus mStatus; //40..44; constructor leaves untouched
public:
    // Actual constructor is inline in original AddPath2f18f0, no invented native RVA.
    // No source allocation/construction of this child record is authorized by this fragment.
    GetEntityRequest() { mFlags=0x06020837; }
    enum PACKFLAG { Pack_GetDirOp, Pack_GetMultiDirOp };
    void SetPath(const std::wstring& inPath) { mPath=inPath; }
    void SetFlags(DWORD inFlags) { mFlags=inFlags; }
    void SetDataTypes(const DirDataTypeSet& inTypes) { mDataTypes=inTypes; }
    void AddDataType(const std::string& type) { mDataTypes.insert(type); }
    void Reset(); //2f1070, clears result list/map only
    void Pack(WriteBuffer*,PACKFLAG); //2f0fa0
    WONStatus ParseMultiEntityReply(const void*,unsigned long); //2f1160, original parser only
    WONStatus GetStatus() { return mStatus; }
    const DirEntityList& GetDirEntityList() { return mDirEntityList; }
    const DirEntityMap& GetDirEntityMap(); //sameSDK full lazy grouping algorithm, SourceOnlyNoNativeID
};
typedef SmartPtr<GetEntityRequest> GetEntityRequestPtr;
// Actual AoC GetDirOpf8: RTTI .?AVGetDirOp@WONAPI@@, VT44d4f8.
// Replaces DEAD base-game118 prefix; affected legacy GameCode requires actual API migration.
class GetDirOp : public ServerRequestOp {
private:
    void Init(); //native2f1190, complete ownedGetEntityRequest44 initialization
    GetEntityRequestPtr mGetEntityRequest; //f4..f8
protected:
    virtual WONStatus Virtual10(); //native2f13d0, GetNextRequest
    virtual WONStatus Virtual14(); //native2f14e0, CheckResponse
    virtual void Virtual18(); //native2f13c0, Reset support0
public:
    GetDirOp(ServerContext*); //native2f12d0,89B; ONLY surviving native constructor
    void SetPath(const std::wstring& path) { mGetEntityRequest->SetPath(path); }
    void SetFlags(DWORD flags) { mGetEntityRequest->SetFlags(flags); }
    void AddDataType(const std::string& type) { mGetEntityRequest->AddDataType(type); }
    const DirEntityList& GetDirEntityList() { return mGetEntityRequest->GetDirEntityList(); }
    // Actual implicit ordinary2f1350 releases child thenServerRequestOp; no own-vptr write.
};
typedef std::list<GetEntityRequestPtr> GetEntityRequestList;
class GetMultiDirOp : public ServerRequestOp {
protected:
    virtual WONStatus Virtual10(); //2f1a30
    virtual WONStatus Virtual14(); //2f1b90
    virtual void Virtual18(); //2f1790
private:
    void Init(); //2f1520, genuine native protocol initialization, support0
    GetEntityRequestList mRequestList; //f4..100
    GetEntityRequestList::iterator mCurRequest; //100=0
    DWORD mFlags; //104, native ctor leaves untouched
    DirDataTypeSet mDataTypes; //108..114
    int mNumFailedRequests; //114=0, complete118
public:
    GetMultiDirOp(ServerContext* inContext); //2f1540, actual full207B
    // Implicit destructor native2f1630/350, no own-vptr store.
    void SetFlags(DWORD inFlags) { mFlags=inFlags; }
    void AddPath(const std::wstring& inPath); //2f18f0
    void AddRequest(GetEntityRequest*); //2f17c0
    const GetEntityRequestList& GetRequestList() { return mRequestList; }
    // Original inherited current-address access is exactly b0.
    // Prefer shared ServerRequestOp's canonical nominal GetCurAddr accessor once admitted.
};

// Exact direct RTTI parent is ServerOp, not AsyncOpWithTracker or RoutingOp.
// No own fields used by protected consumer; actual PeerAuth own IPAddr begins94.
// Producer ServerConnection::Connect2abc40 new2244 at2abc81, ctor2e6a10 at2abcb3.
// Ctor calls ServerOp2acc10 at2e6a30; inherited pointer78 release/addref/write
// shown2e6b10..2e6b61, source argument BlockingSocket* (pinned header corroborates).
// Complete actual PeerAuthClient2180: no vptr, two genuine Blowfish10b4 owners.
class PeerAuthClient {
    ConstSmartPtr<AuthPeerData> mPeerData; //00
    AuthType mAuthType; //04, native constructor untouched
    Blowfish mSecretA; //08..10bc
    Blowfish mSecretB; //10bc..2170
    unsigned char mLengthFieldSize; //2170, native constructor untouched
    bool mUseClientSecret; //2171=false
    unsigned char mClientKeySize; //2172=8
    bool mUseAuth2; //2173=true
    SmartPtr<AuthCertificateBase> mServerCertificate; //2174, actual mutable SDK owner
    SmartPtr<AuthSession> mSession; //2178
public:
    enum PeerAuthClientState { STATE_NOT_STARTED=0, STATE_AWAITING_CHALLENGE=1, STATE_AWAITING_COMPLETE=2 } mState; //217c
private:
    WONStatus HandleChallenge1(ReadBuffer&,ByteBufferPtr&); //2e5cc0 actualRET8
    WONStatus GetChallenge2(ByteBufferPtr&); //2e5fb0 actualRET4
    WONStatus HandleComplete(ReadBuffer&); //2e61d0 actualRET4
public:
    ByteBufferPtr Start(const AuthPeerData*,AuthType,unsigned char); //2e6730 actualowningsret RET10
    WONStatus HandleRecvMsg(const void*,unsigned long,ByteBufferPtr&); //2e68d0 actualRET0c
    PeerAuthClient(); //2e5c30 external125, no new implementation
    void Reset(); //2e5cb0 external state reset
    AuthSession* GetSession() { return mSession.get(); }
    const AuthCertificateBase* GetServerCertificate() { return mServerCertificate.get(); }
    PeerAuthClientState GetState() { return mState; }
    void SetUseAuth2(bool value) { mUseAuth2=value; }
    void SetClientKeySize(unsigned char size,bool useSecret=true) { mClientKeySize=size;mUseClientSecret=useSecret; }
};
// Actual RTTI PeerAuthOp >ServerOp >AsyncOpWithTracker >AsyncOp >RefCount.
// Complete2244 owner only; ctor/dtor/protocol remain external original-owned.
// Complete cached-native PeerAuthServer2184, no vptr or opaque storage.
class PeerAuthServer {
    ConstSmartPtr<AuthPeerData> mPeerData; //00
    AuthType mAuthType; //04, native constructor untouched
    Blowfish mSecretA; //08..10bc
    Blowfish mSecretB; //10bc..2170
    unsigned char mLengthFieldSize; //2170, native constructor untouched; natural alignment2171..2173
    SmartPtr<AuthCertificateBase> mClientCertificate; //2174, actual mutable SDK BasePtr
    SmartPtr<AuthSession> mSession; //2178
    bool mUseAuth2; //217c=true; natural alignment217d..217f
public:
    enum PeerAuthServerState { STATE_NOT_STARTED=0,STATE_AWAITING_REQUEST=1,STATE_AWAITING_CHALLENGE2=2 } mState; //2180=0
private:
    WONStatus GetChallenge1(ByteBufferPtr&); //2e7dd0 actualRET4, pure wire/encryption body
    WONStatus HandleRequest(ReadBuffer&,ByteBufferPtr&); //2e80a0 actualRET8, complete certificate owner allocations
    WONStatus GetComplete(WONStatus,ByteBufferPtr&); //2e8300 actualRET8, complete AuthSession owner allocation
    WONStatus HandleChallenge2(ReadBuffer&,ByteBufferPtr&); //2e8630 actualRET8
public:
    void Start(const AuthPeerData*,unsigned char); //2e7d30 actualRET8 external original protocol initializer
    WONStatus HandleRecvMsg(const void*,unsigned long,ByteBufferPtr&); //2e87c0 actualRET0c original protocol dispatcher
    PeerAuthServer(); //2e7cc0 external106, no protocol/lifecycle implementation
    const AuthCertificateBase* GetClientCertificate() { return mClientCertificate.get(); }
    AuthSession* GetSession() { return mSession.get(); }
    PeerAuthServerState GetState() { return mState; }
    void SetUseAuth2(bool value) { mUseAuth2=value; }
};
// Actual RTTI PeerAuthServerOp >ServerOp >AsyncOpWithTracker >AsyncOp >RefCount.
// Complete2240 owner; protocol/constructor/destructor remain external original-owned.
class PeerAuthServerOp : public ServerOp {
    bool CheckStatus(WONStatus); //2e8fa0 original boolRET4 status/Finish helper
    void AsyncRun(); //2e8fc0 original asynchronous receive setup
    void Success(); //2e9370 original session transform installation/completion
    virtual bool CallbackHook(AsyncOp*,int); //2e8d70 actual fourth slot; protocol external
protected:
    virtual ~PeerAuthServerOp(); //2e8bc0 actual422, own-vptr then real owners
    virtual void Virtual04(); //2e9050 RunHook
    virtual void Virtual08(); //2e7cb0 shared actual CleanupHook
public:
    PeerAuthServerOp(AuthContext*,BlockingSocket*); //2e8950 native230, actualRET8
    // Native-backed typed field view for the borrowed consumer; original nonstatic source access/names not inferred.
    IPAddr mConnectAddr; //94..bc, actual default IPAddr28 constructed2b2350
    PeerAuthServer mPeerAuthServer; //bc..2240; clientCertificate2230/session2234/state223c
};

class PeerAuthOp : public ServerOp {
    IPAddr mConnectAddr; //94..bc, actual28 owned address/string
    WONStatus mGetCertStatus; //bc, native constructor untouched
    ConstSmartPtr<AuthPeerData> mPeerData; //c0
    PeerAuthClient mPeerAuthClient; //c4..2244
    enum PeerAuthTrack { PeerAuth_Track_Socket=1, PeerAuth_Track_GetCert=2 };
    bool CheckStatus(WONStatus); //2e8fa0 genuine ICF-shared bool/Finish helper, RET4
    void AsyncRetry(); //2e71a0
    void AsyncSend(const ByteBuffer*); //2e7240, RET4
    void AsyncConnect(); //2e7370
    void AsyncRefreshCert(); //2e74a0
    void AsyncRun(); //2e75f0
    void Success(); //2e7c40
    virtual bool CallbackHook(AsyncOp*,int); //2e6ed0 actual fourth slot, no new slots
protected:
    virtual ~PeerAuthOp(); //2e6d10 actual437 own-vptr destructor
    virtual void Virtual04(); //2e76d0 RunHook
    virtual void Virtual08(); //2e7cb0 CleanupHook
public:
    PeerAuthOp(const IPAddr&,AuthContext*,AuthType,BlockingSocket*,bool=true); //2e6a10 native370, actualret20
    AuthSession* GetSession() { return mPeerAuthClient.GetSession(); }
    BlockingSocket* GetSocket() { return mSocket.get(); }
    Auth2Certificate* GetServerCertificate() { return (Auth2Certificate*)mPeerAuthClient.GetServerCertificate(); }
    WONStatus GetGetCertStatus() const { return mGetCertStatus; }
};

// Full15c HTTP operation; native layout/lifetime/vtable proof in R3Account gate.
class FileReader;
class FileWriter;
// Complete native cache entry68, ctor2d1260; implicit dtor2d1330(no ownVT store).
class HTTPCacheEntry : public RefCount {
    friend class HTTPCache;
    int mId; //08
    bool mIsValid; //0c,naturalpadding0d..0f
    std::string mRemoteLocation; //10
    std::string mLocalPath; //20
    std::string mFileName; //30
    std::string mFileType; //40
    std::string mEntityTag; //50
    time_t mExpirationTime; //60
    time_t mLastAccessTime; //64
public:
    HTTPCacheEntry(); //2d1260
    void ReadFromFile(FileReader&); //2d1450
    void WriteToFile(FileWriter&); //2d14c0
    bool IsValid(); //2d1540, existing tinygetter stays call-only
    void SetValid(bool inValid) {mIsValid=inValid;}
    void SetExpirationTime(time_t inTime) {mExpirationTime=inTime;}
    void SetEntityTag(const std::string& inTag) {mEntityTag=inTag;}
    const std::string& GetLocalPath() {return mLocalPath;}
    const std::string& GetEntityTag() {return mEntityTag;}
    time_t GetExpirationTime() {return mExpirationTime;}
};
// Complete native cache34. Target has NO later-SDK mCreatedDirectory boolean.
// Native constructor2d1670 and singleton allocation2d192d prove sizeof34.
class HTTPCache : public RefCount {
protected:
    std::string mCachePath; //08
    typedef std::map<std::string,SmartPtr<HTTPCacheEntry> > CacheLocationMap;
    typedef std::map<int,SmartPtr<HTTPCacheEntry> > CacheIdMap;
    CacheLocationMap mCacheLocationMap; //18
    CacheIdMap mCacheIdMap; //24
    int mNextFreeId; //30
    void ReadCacheMap(); //2d1a10
    void WriteCacheMap(); //2d1ca0
    int GetFreeId(); //2d2170
    virtual ~HTTPCache(); //2d1820 explicitownVT then WriteCacheMap
public:
    HTTPCache(const std::string&,bool inFullPath=false); //2d1670
    void SetCachePath(const std::string&,bool); //2d1ef0 always creates dir; empty path returns before read
    HTTPCacheEntry* GetEntry(const std::string&); //2d2210 rawborrowed ret4, mapsretain
    static HTTPCache* GetGlobalCache(); //2d1900, actualnoarg native getter
    static void SetGlobalCache(HTTPCache*);
    void UpdateEntry(HTTPCacheEntry*); //2fa500, genuine native version-empty call
};
class HTTPGetOp : public AsyncOpWithTracker {
private:
    enum TrackType { HTTP_Track_Connect=1,HTTP_Track_RecvStatus=2,HTTP_Track_RecvHeader=3,
        HTTP_Track_RecvContent=4,HTTP_Track_Redirect=5,HTTP_Track_RecvChunkLength=6,
        HTTP_Track_RecvChunkCRLF=7,HTTP_Track_RecvFooter=8,HTTP_Track_SendRequest=9 };
    bool ExtractHeaderLine(const char*); //2d47e0
    bool SendRequest(); //2d5270
    void AsyncRecvCRMsg(TrackType); //2d6170
    bool DoConnect(); //2d5090
    bool ExtractChunkLength(const char*); //2d6240
    bool RecvChunkLength(); //2d6430
    bool DoneRecvContent(RecvBytesOp*); //2d65c0
    void RecvContent(); //2d6730, actual full receiver; call-only until nextcohort
    void FinishRedirect(HTTPGetOp*); //2d6d90
    void FollowRedirect(); //2d6f80
    void DoneRecvHeader(); //2d7140
    static std::string mStaticProxyHost; //565fb0
public:
    time_t GetLastModifiedTime(); //2d7870, native lazy file timestamp cache
    void SetSocket(BlockingSocket*); //2d77f0, native owning assignment member78
    BlockingSocket* GetSocket(); //2d7850, returns NULL unless socket is connected
    void DoFinish(); //2d4370
    void SetProxy(const std::string&); //2d4390/325; genuine SDK complete API
    bool CheckStatus(WONStatus); //2d4550, support-only status forwarding
    bool ExtractStatus(const char*); //2d4570
    bool ExtractRedirect(const char*); //2d45f0

    HTTPGetOp(const std::string& inURL); //2d3f90,actual15ccaller3ddd81
    void Init();
    HTTPGetOp(const std::string& inHost,const std::string& inPath); //2d3b10 call-only untilnewctorcohort
    void SetRecvChunkCompletion(CompletionBase<AsyncOpPtr>*,DWORD); //2d44e0 actual107B ownershipsetter call-only
    static void ReadProxyHostFile(bool inForce=true); //2d3830
    static bool WriteProxyHostFile(const std::string&);
    static const std::string& GetStaticProxyHost();
    static void SetProxyHostFileName(std::string&);
    void SetProxy(const std::string&,unsigned short);
    virtual void Virtual04(); //2d7330
    virtual void Virtual08(); //2d7770
    virtual bool CallbackHook(AsyncOp* inOp,int inParam); //2d6a50
    // Implicit destructor: native2d3d50 has no own-vptr store; exact567B member/base lifetime below.
    const std::string& GetLocalPath() { return mLocalPath; }
    const std::string& GetHost() {return mHost;}
    const std::string& GetRemotePath() {return mRemotePath;}
    const std::string& GetEndingHost() {return mRedirectHost.empty()?mHost:mRedirectHost;}
    const std::string& GetEndingPath() {return mRedirectPath.empty()?mRemotePath:mRedirectPath;}
    int GetHTTPStatus() {return mHTTPStatus;}
    void SetHTTPCache(HTTPCache* inCache) { mHTTPCache=inCache; }
    void SetLocalPath(const std::string& thePath) { mLocalPath=thePath; }
    void SetBinaryFile(bool isBinary) { mBinaryFile=isBinary; }
    void SetOnlySend() { mOnlySend=true; mOnlyRecv=false; }
    void SetOnlyRecv() { mOnlyRecv=true; mOnlySend=false; }
    void SetSendAndRecv() { mOnlySend=false; mOnlyRecv=false; }
    bool GetOnlySend() { return mOnlySend; }
    bool GetOnlyRecv() { return mOnlyRecv; }
    void SetDoRangeResume(bool doRangeResume) { mDoRangeResume=doRangeResume; }
    void SetCloseConnection(bool closeConnection) { mCloseConnection=closeConnection; }
    bool ContentIsNew() { return mContentIsNew; }
    SmartPtr<BlockingSocket> mSocket; //78
    SmartPtr<HTTPCache> mHTTPCache; //7c
    SmartPtr<HTTPCacheEntry> mHTTPCacheEntry; //80
    std::string mHost; //84
    std::string mRemotePath; //94
    std::string mLocalPath; //a4
    bool mIfRange; //b4
    bool mContentIsChunked; //b5
    bool mIsFirstChunk; //b6
    bool mBinaryFile; //b7
    bool mCloseConnection; //b8
    bool mDoClose; //b9
    bool mDoRangeResume; //ba,naturalpadbb
    std::string mProxyHost; //bc
    std::string mRedirectHost; //cc
    std::string mRedirectPath; //dc
    time_t mServerTime; //ec
    time_t mExpireTime; //f0
    time_t mLastModified; //f4
    time_t mLocalFileModifyTime; //f8
    size_t mLocalFileSize; //fc
    std::string mEntityTag; //100
    std::string mContentType; //110
    DWORD mContentLength; //120
    DWORD mChunkLength; //124
    ByteBufferPtr mContent; //128
    WriteBuffer mContentAccumulator; //12c..144
    int mHTTPStatus; //144
    bool mContentIsNew; //148,naturalpad149..14b
    OpCompletionBasePtr mRecvChunkCompletion; //14c
    DWORD mRecvChunkSize; //150
    bool mOnlySend; //154
    bool mOnlyRecv; //155
    bool mAutoRedirect; //156,naturalpad157
    int mNumRedirects; //158..15c
};

// Full DNS operation7c; actual explicit destructor and borrowed address getter.
// Complete target ThreadBase14 / Thread1c. Event8 supplied by AsyncOpContainer contract.
// Native base ctor2a6c10 and base dtor2a6c80; DNSThread allocation20 at2d980a.
enum ThreadPriority { ThreadPriority_Highest,ThreadPriority_High,ThreadPriority_Normal,ThreadPriority_Low,ThreadPriority_Lowest };
class ThreadBase {
protected:
    virtual void StartHook(); //slot00 2a6c50
    virtual void WaitForStopHook(); //slot04 empty actual326b60
    virtual bool InThisThreadHook(); //slot08 false actual30a7b0
    virtual void SetPriorityHook(); //slot0c empty actual326b60
    virtual void ThreadFunc()=0; //slot10 pure
public:
    ThreadBase(); //2a6c10, stopped1/running0/autodelete0/priority2/Event auto reset
    virtual ~ThreadBase(); //slot14 2a6c80 ordinary
    virtual void Signal(); //slot18 2a6cf0
    void Start(); //2a6ca0
    void Stop(bool waitForStop=true); //2a6cc0
    bool InThisThread(); //2a6d00
    void WaitForStop(); //2a6d10
    void SetPriority(ThreadPriority thePriority); //genuine SDK state-gated priority update
protected:
    bool mStopped; //04
    bool mRunning; //05
    bool mAutoDelete; //06,naturalpadding07
    Event mSignalEvent; //08..10
    ThreadPriority mPriority; //10..14
};
// Genuine SDK static-only diagnostics API; no instance fields, bases, or virtual slots.
class WONDebug {
public:
    static void SetStackFileName(const std::string &theName);
    static const std::string& GetStackFileName();
    static int DumpStackToFile(void *theInfo = NULL);
};
class Thread : public ThreadBase {
  private:
    static unsigned __stdcall StaticThreadFunc(void*); //2a85e0
  protected:
    virtual void StartHook(); //slot00 2a8670
    virtual void WaitForStopHook(); //slot04 2a86c0
    virtual bool InThisThreadHook(); //slot08 2a86d0
    virtual void SetPriorityHook(); //slot0c 2a86f0
    HANDLE mThreadHandle; //14
    DWORD mThreadId; //18, native StartHook writes stack threadid rather than member in this revision
public:
    void SetAutoDelete(bool inAutoDelete) { mAutoDelete=inAutoDelete; }
};
class DNSTestOp : public AsyncOp {
    friend class DNSTestThread;
public:
    DNSTestOp(const std::string& inHost);    //2d95a0
    // Real nominal accessor in pinned DNSTestOp.h; target LAN3e28d0 family
    // takes receiver+54 directly. No helper marker or forced instantiation.
    const IPAddr& GetHostAddr() { return mHostAddr; }
protected:
    virtual ~DNSTestOp();                   //2d9740 explicit/out of line
    virtual void Virtual04();               //2d97f0 RunHook,slot04
    std::string mHostString;                //44..54
    IPAddr mHostAddr;                       //54..7c
};
typedef SmartPtr<DNSTestOp> DNSTestOpPtr;
class DNSTestThread : public Thread {
public:
    DNSTestThread(DNSTestOp* inOp) : mOp(inOp) {} //actual inline constructor2d9826..985d
    virtual void ThreadFunc(); //slot10 2d9920
    DNSTestOpPtr mOp; //1c,owned; full20. Implicit dtor2d98b0 no ownvptr store.
};


// WONAPI's HTTP document request (sizeof 0xdc; implicit dtor 0x4cff46; ctor out of line in the base game,
// base exe 0x6cc4d0, members PROVISIONAL)
// All target enums are32-bit int under VC6; no narrowed bool getter argument.
enum HTTPDocType {
    HTTPDocType_MOTD=0,
    HTTPDocType_TOU=1,
    HTTPDocType_ADV=2,
    HTTPDocType_Count=3
};
enum HTTPDocOwner {
    HTTPDocOwner_Sys=0,
    HTTPDocOwner_Game=1,
    HTTPDocOwner_Count=2
};

// Fulldc. Implicit dtorcff46 stays naturally emitted by protected consumers;
// neither explicit empty dtor nor out-of-line-only declaration may replace it.
// Actual table44d378: delete2d0320,Run2d1070,Cleanup2a0290,Callback2d0810.
class GetHTTPDocumentOp : public AsyncOpWithTracker {
public:
    void SetDocType(HTTPDocType); //2d0340
    void DoFinish(); //2d06c0
    void OpFinished(HTTPGetOp*,int); //2d0700
    void TryHTTPOp(HTTPDocOwner,bool=false); //2d0840, original implementation remains carved
    static void CheckHostAndPath(HTTPDocType); //2cffb0, actual static native owner

    GetHTTPDocumentOp(const std::string& inProduct,HTTPDocType inType); //2d0170
    HTTPDocType GetDocType() { return mDocumentType; }
    void SetExtraConfig(const std::string& theExtraConfig) { mPath=theExtraConfig; }
    const std::string& GetDocumentPath(HTTPDocOwner theOwner) const { return mDocuments[theOwner]; }
    time_t GetModifiedTime(HTTPDocOwner theOwner) const { return mLastModified[theOwner]; }
    WONStatus GetDocumentStatus(HTTPDocOwner theOwner) { return mDocumentStatus[theOwner]; }
    void SetHTTPSession(HTTPSession* theSession) { mSession=theSession; }
    virtual void Virtual04();               //slot04,2d1070
    virtual bool CallbackHook(AsyncOp* inOp,int inParam); //slot0c,2d0810
    ByteBufferPtr GetDocument(HTTPDocOwner inOwner) const; //2d10e0
    HTTPDocType mDocumentType;              //78
    std::string mProduct;                   //7c
    std::string mPath;                      //8c
    HTTPSessionPtr mSession;                //9c
    bool mIsNew[2];                         //a0
    bool mRetried[2];                       //a2
    WONStatus mDocumentStatus[2];           //a4,ac
    int mHTTPStatus[2];                     //ac,b4
    time_t mLastModified[2];                //b4,bc; VC6 signed32-bit
    std::string mDocuments[2];              //bc,cc,dc; FILE PATHS
};


// the message of the day
class GetMOTDOp : public GetHTTPDocumentOp {
public:
    GetMOTDOp(const std::string& inProduct)
        : GetHTTPDocumentOp(inProduct, HTTPDocType_MOTD) {} //actual base4c965b pushes0
};

// WONAPI's CD key (dead in AoC; r25pre-won-2 from the base game's Login: ctor base exe 0x68ab90, an 0x1c-byte local;
// implicit dtor 0x4cff2a: the SmartPtr at +0x14, the string at +4; names PROVISIONAL)
// CDKey complete declaration moved to EEWonNet foundation before AuthContext mapped value.
// Exact storage, methods and inline virtual bodies retained; no duplicate class definition.


// Complete actual AoC11fc owner. Constructor/destructor/protocol remain original-owned.
// Actual RTTI GetCertOp >ServerRequestOp >ServerOp >AsyncOpWithTracker >AsyncOp >RefCount.
class AuthPubKeyBlock;
class AuthPeerData;
class GetCertOp : public ServerRequestOp {
public:
    struct COMMUNITY_STATUS {
        short mStatus; //00
        unsigned long mCommunityID; //04
        std::wstring mCommunityName; //08..18, native list node20
    };
    typedef std::vector<std::string> ERROR_LIST;
    typedef std::list<COMMUNITY_STATUS> COMMUNITY_STATUS_LIST;
private:
    SmartPtr<AuthContext> mAuthContext; //f4
    Blowfish mSessionKey; //f8..11ac, existing genuine10b4 owner
    ConstSmartPtr<AuthPubKeyBlock> mKeyBlock; //11ac
    time_t mAuthDelta; //11b0, native constructor leaves untouched
    ConstSmartPtr<AuthPeerData> mPeerData; //11b4
    RawBuffer mChallengeSeed; //11b8..11c8
    std::wstring mNewPassword; //11c8..11d8
    bool mCreateAccount; //11d8, native constructorfalse
    enum State { GETTING_PUB_KEYS=1,GETTING_CHALLENGE=2,GETTING_CERTIFICATE=3 } mState; //11dc, ctor untouched
    ERROR_LIST mErrorList; //11e0..11f0
    COMMUNITY_STATUS_LIST mCommunityStatusList; //11f0..11fc
    WONStatus HandleLoginReply(ReadBuffer&); //2e4880 actual RET4; full reply parser
    WONStatus HandleLoginChallenge(ReadBuffer&); //2e5800 actual RET4
    WONStatus SendPubKeyRequest(); //2e4270 actualRET0
    WONStatus SendLoginRequest(); //2e4380 actualRET0
    WONStatus SendLoginConfirm(); //2e46a0 actualRET0
protected:
    virtual ~GetCertOp(); //2e3d90 actual own-vptr destructor, native cleanup all real fields
    virtual void Virtual04(); //2e3f40 RunHook
    virtual void Virtual08(); //2e4050 CleanupHook
    // Actual inherited CallbackHook0c=2adb10, no added slot.
    virtual WONStatus Virtual10(); //2e4850 GetNextRequest
    virtual WONStatus Virtual14(); //2e5980 CheckResponse
    virtual void Virtual18(); //2e4130 Reset
public:
    GetCertOp(AuthContext*); //2e3c30 native319, external only
    const AuthPeerData* GetPeerData() const { return mPeerData; }
    void SetCreateAccount(bool value) { mCreateAccount=value; }
    void SetNewPassword(const std::wstring& password) { mNewPassword=password; }
    void SetAuthContext(AuthContext* context) { mAuthContext=context; }
    const Blowfish& GetSessionKey() const { return mSessionKey; }
    const ERROR_LIST& GetErrorList() { return mErrorList; }
    const COMMUNITY_STATUS_LIST& GetCommunityStatusList() { return mCommunityStatusList; }
};


class ProfileData : public RefCount {
public:
    ProfileData(); //2cd770, actual150B, initializes birth8/9/a=0, mapc, id18=0, data1c=NULL
    void ClearNewsletterSubs(); //2cdc20, actual mapclear
    void ClearNewsletterSub(unsigned short);
    void SetCustomAction(unsigned short,const ByteBuffer*);
    void ClearCustomAction();
    unsigned long GetBirthDate();
    void AddNewsletterSub(unsigned short inId, bool inSubscribe = false); //2cd9e0
    bool GetNewsletterSub(unsigned short inId); //2cdae0
    void PackRequestData(WriteBuffer&,bool); //2cdcc0
    void UnpackGetData(ReadBuffer&); //2cde20
    void SetBirthDate(unsigned char inMonth,unsigned char inDay,unsigned short inYear); //2ce070
protected:
    virtual ~ProfileData(); //2cd830, explicit ownVT store2cd852; native lifetime only
public:
    unsigned char mBirthMonth; //08
    unsigned char mBirthDay; //09
    unsigned short mBirthYear; //0a
    std::map<unsigned short,bool> mNewsletterSubs; //0c..18
    unsigned short mCustomActionId; //18, natural padding1a..1b
    ByteBufferPtr mCustomActionData; //1c..20
};
class DBProxyOp : public ServerRequestOp {
public:
    DBProxyOp(ServerContext* inContext); //2cca10,235B; dtor implicit224B2ccb20,noownVT
    DBProxyOp(const IPAddr&); //genuine same-SDK direct-address constructor
    virtual WONStatus Virtual10(); //2ccc00
    virtual WONStatus Virtual14(); //2ccd30
    virtual void Virtual18(); //4142a0
    short mMessageType; //f4
    short mMessageReplyType; //f6
    short mSubMessageType; //f8
    short mSubMessageReplyType; //fa
    ByteBufferPtr mRequestData; //fc
    WriteBuffer mReceivedData; //100..118
    ByteBufferPtr mReplyData; //118
    std::string mReplyErrorString; //11c..12c
};
// Complete native patch-server records. VersionData48/PatchDataa4,
// native constructors2d83a0/2d86f0 and owned allocations2d93a7/2d9079.
enum VersionState { VersionState_Invalid=0, VersionState_Inactive=1, VersionState_Active=2, VersionState_BadState=3 };
typedef std::list<std::string> VersionTypeList;
class VersionData : public RefCount {
protected:
    virtual ~VersionData(); //2d8510, explicit ownVT; clears list then owns members
private:
    std::string mConfigName; //08
    std::string mVersion; //18
    std::string mDescriptionURL; //28
    VersionState mState; //38
    VersionTypeList mVersionTypeList; //3c..48
public:
    VersionData(); //2d83a0
    void WriteToBuffer(WriteBuffer&,DWORD inFlags=0); //2d8660
    enum { WriteFlag_AppendState=1, WriteFlag_AppendDescripURL=2 };
    void AddVersionType(const std::string&);
    void ReadFromBuffer(ReadBuffer&);
    bool operator==(const VersionData&) const;
    bool operator!=(const VersionData&) const;
    void SetDescriptionURL	(const std::string& theDescriptionURL)	{ mDescriptionURL	= theDescriptionURL;}
    const std::string& GetDescriptionURL()		const { return mDescriptionURL;	 }
    const VersionState GetState()				const { return mState;			 }
    const VersionTypeList& GetVersionTypeList() const { return mVersionTypeList; }
    void SetConfigName(const std::string& inName) { mConfigName=inName; }
    void SetVersion(const std::string& inVersion) { mVersion=inVersion; }
    void SetState(VersionState inState) { mState=inState; }
    const std::string& GetConfigName() const { return mConfigName; }
    const std::string& GetVersion() const { return mVersion; }
};
class PatchData : public RefCount {
protected:
    virtual ~PatchData(); //2d8a50, explicit ownVT then9 actual string owners
private:
    std::string mConfigName; //08
    std::string mFromVersion; //18
    std::string mToVersion; //28
    std::string mDisplayName; //38
    std::string mNetAddress; //48
    unsigned long mCheckSum; //58
    unsigned long mPatchSize; //5c
    std::string mHostName; //60
    std::string mHostURL; //70
    std::string mHostText; //80
    std::string mHostImageURL; //90
    bool mMustVisitHost; //a0
    bool mIsActive; //a1, naturalpaddinga2..a3
public:
    PatchData(); //2d86f0
    void ReadFromBuffer(ReadBuffer&,bool inGetPatchList=false); //2d8bc0
    void WriteToBuffer(WriteBuffer&,bool inUpdateOnly=false);
    bool operator==(const PatchData&) const;
    bool operator!=(const PatchData&) const;
    void SetDisplayName	(const std::string& theDisplayName)	{ mDisplayName = theDisplayName;	}
    void SetNetAddress	(const std::string& theNetAddress)	{ mNetAddress = theNetAddress;		}
    void SetCheckSum	(unsigned long theCheckSum)			{ mCheckSum = theCheckSum;			}
    void SetPatchSize	(unsigned long thePatchSize)		{ mPatchSize = thePatchSize;		}
    void SetHostName	(const std::string& theHostName)	{ mHostName = theHostName;			}
    void SetHostURL		(const std::string& theHostURL)		{ mHostURL = theHostURL;			}
    void SetHostText	(const std::string& theHostText)	{ mHostText = theHostText;			}
    void SetHostImageURL(const std::string& theHostImageURL){ mHostImageURL = theHostImageURL;	}
    void SetMustVisitHost(bool theMustVisitHost)			{ mMustVisitHost = theMustVisitHost;}
    const std::string&  GetConfigName()		const	{ return mConfigName;	}
    const std::string&  GetFromVersion()	const	{ return mFromVersion;	}
    const std::string&  GetToVersion()		const	{ return mToVersion;	}
    const std::string&  GetDisplayName()	const	{ return mDisplayName;	}
    const std::string&  GetNetAddress()		const	{ return mNetAddress;	}
    const unsigned long GetCheckSum()		const	{ return mCheckSum;		}
    const unsigned long GetPatchSize()		const	{ return mPatchSize;	}
    const std::string&  GetHostName()		const	{ return mHostName;		}
    const std::string&  GetHostURL()		const	{ return mHostURL;		}
    const std::string&  GetHostText()		const	{ return mHostText;		}
    const std::string&  GetHostImageURL()	const	{ return mHostImageURL; }
    bool  GetMustVisitHost()				const	{ return mMustVisitHost;}
    bool  GetIsActive()						const	{ return mIsActive;		}
    void SetConfigName(const std::string& inName) { mConfigName=inName; }
    void SetFromVersion(const std::string& inVersion) { mFromVersion=inVersion; }
    void SetToVersion(const std::string& inVersion) { mToVersion=inVersion; }
    void SetIsActive(bool inActive) { mIsActive=inActive; }
};
typedef std::list<SmartPtr<PatchData> > PatchDataList;
class CheckValidVersionOp : public DBProxyOp {
    void Init(); //2d9380
protected:
    virtual ~CheckValidVersionOp(); //2d8d90/458 explicitownVTstore2d8db0
    virtual void Virtual04(); //2d9250
    virtual WONStatus Virtual14(); //2d8f60
private:
    std::string mProductName; //12c..13c
    SmartPtr<VersionData> mVersionData; //13c
    PatchDataList mPatchDataList; //140..14c
    bool mGetPatchList; //14c, naturalalignment14d..14f
    std::string mVersionTo; //150..160
    std::string mVersionToDescriptionURL; //160..170
    const unsigned short mMsgType; //170=1, naturalalignment172..173,full174
public:
    CheckValidVersionOp(const std::string& inProduct,ServerContext* inContext); //2d8c80
    void SetConfigName(const std::string& inName) { mVersionData->SetConfigName(inName); }
    void SetVersion(const std::string& inVersion) { mVersionData->SetVersion(inVersion); }
};
class DetectFirewallOp : public ServerRequestOp {
protected:
    void Init(); //2cf5c0, native constructor call2cf651
    virtual void Virtual04(); //2cf760
    virtual void Virtual08(); //2cf920
    virtual bool CallbackHook(AsyncOp*,int); //2cf8b0
    virtual WONStatus Virtual10(); //2cf970
    virtual WONStatus Virtual14(); //2cfad0
    virtual WONStatus Virtual1C(); //2cfba0 actual eighth SetupReceiveTracker slot
private:
    unsigned short mListenPort; //f4, naturalalignmentf6/f7
    unsigned long mMaxConnectWaitTime; //f8
    SmartPtr<BlockingSocket> mListenSocket; //fc
    bool mDoListen; //100
    bool mWaitForConnect; //101
    bool mUseUDP; //102,naturalalignment103
    SmartPtr<RecvBytesFromOp> mRecvBytesFromOp; //104; canonicalcomplete90B
    IPAddr mPublicAddr; //108..130,full130
public:
    DetectFirewallOp(ServerContext* inContext); //2cf600
    // Native208B dtor2cf690 has NO own-vptr store, remains implicit.
    void SetListenPort(unsigned short inPort) { mListenPort=inPort; }
    void SetDoListen(bool inDoListen) { mDoListen=inDoListen; }
    void SetWaitForConnect(bool inWait) { mWaitForConnect=inWait; }
    IPAddr& GetPublicAddr() { return mPublicAddr; }
};

class RequestPasswordEmailOp : public DBProxyOp {
public:
    RequestPasswordEmailOp(ServerContext* inContext); //2ce480,126B
    virtual void Virtual04(); //2ce670, actual RunHook
    virtual WONStatus Virtual14(); //2ce280,shared CheckResponse
protected:
    void Init(); //actual native163B, length84=4 then new completeProfile20
    virtual ~RequestPasswordEmailOp(); //2ce520,329B, explicit ownVT2ce540
public:
    void SetUserName(const std::wstring& inUser) { mUserName=inUser; }
    std::wstring mUserName; //12c..13c
    SmartPtr<ProfileData> mProfileData; //13c
    const unsigned short mMsgType; //140=8,naturalpadding142..143
};
class RequestAccountInfoEmailOp : public DBProxyOp {
public:
    RequestAccountInfoEmailOp(ServerContext* inContext); //2ce090,126B
    virtual void Virtual04(); //2ce2b0
    virtual WONStatus Virtual14(); //2ce280
protected:
    void Init(); //actual native163B, length84=4 then new completeProfile20
    virtual ~RequestAccountInfoEmailOp(); //2ce130,328B,explicit ownVT2ce150
public:
    void SetEmail(const std::string& inEmail) { mEmail=inEmail; }
    std::string mEmail; //12c..13c
    SmartPtr<ProfileData> mProfileData; //13c
    const unsigned short mMsgType; //140=14,naturalpadding142..143
};

// Complete native GetUserIdFromNameOp140; ctor2cd3a0/destructor2cd4d0,
// RTTI44d2b8 six inherited operation slots plus Reset18; no additional slots.
class GetUserIdFromNameOp : public DBProxyOp {
protected:
    virtual ~GetUserIdFromNameOp(); //2cd4d0 explicit ownVT44d2b8
    virtual void Virtual04(); //2cd600, serialized lookup request
    virtual WONStatus Virtual14(); //2cd700, vtable-derived response
public:
    GetUserIdFromNameOp(ServerContext*); //2cd3a0
    void SetUserName(const std::wstring& v) { mUserName=v; }
    unsigned long GetUserId() const { return mUserId; }
    std::wstring mUserName; //12c
    unsigned long mUserId; //13c, constructorzero
};

// Complete native account creator1398. Embedded ElGamal108..2b0 and Blowfish2b0..1364.
// Full owner/lifetime independently proved in auth/publickey packet and native2cc2e0.
class CreateAccountOp : public ServerRequestOp {
    SmartPtr<ProfileData> mProfileData; //f4
    std::string mErrorString; //f8
    ElGamal mPubKey; //108,8aligned
    Blowfish mSymKey; //2b0
    enum State { State_GettingPubKey,State_SettingSessionKey,State_CreatingAccount } mState; //1364
    std::wstring mUserName; //1368
    std::wstring mPassword; //1378
    std::string mEmail; //1388..1398
    void Init(); //2cbe70
protected:
    virtual WONStatus Virtual10(); //2cc4f0
    virtual WONStatus Virtual14(); //2cc8d0,vtable44d254 slot14
    virtual void Virtual18(); //2cc4e0,Reset support0
public:
    CreateAccountOp(ServerContext*); //2cbf20
    void SetProfileData(ProfileData* inProfile) {mProfileData=inProfile;}
    void SetUserName(const std::wstring& inUser) {mUserName=inUser;}
    void SetPassword(const std::wstring& inPassword) {mPassword=inPassword;}
    void SetEmail(const std::string& inEmail) {mEmail=inEmail;}
    const std::wstring& GetUserName() const {return mUserName;}
    const std::wstring& GetPassword() const {return mPassword;}
    const std::string& GetEmail() const {return mEmail;}
    const std::string& GetErrorString() const {return mErrorString;}
    ProfileData* GetProfileData() {return mProfileData.get();}
    // Implicit ordinary dtor2cc2e0 NO ownVT store; zeroquota, all real members destroy normally.
};

class GetAccountOp : public ServerRequestOp {
public:
    GetAccountOp(ServerContext* inContext,AuthContext* inAuth); //2ccfa0
    void Init(); //2ccef0, actual163B, length4 before newProfile20
    // Destructor remains implicit:2cd0a0 has no own-vptr store; releases114,
    // string104 then stringf4, calls actual ServerRequestOp::~2ad110.
    virtual WONStatus Virtual10();          //2cd190 GetNextRequest
    virtual WONStatus Virtual14();          //2cd2b0 CheckResponse
    virtual void Virtual18();               //4142a0 Reset, actual override
    std::string mEmail;                     //f4..104
    std::string mErrorString;               //104..114
    SmartPtr<ProfileData> mProfileData;      //114..118
};
class UpdateAccountOp : public ServerRequestOp {
public:
    UpdateAccountOp(ServerContext* inContext,AuthContext* inAuth); //2ce8f0
    // Destructor remains implicit:2cea10 has no own-vptr store; string118,
    // wstring108,stringf8,ProfilePtrf4, then ServerRequestOp::~2ad110.
    virtual WONStatus Virtual10();          //2ceb30 GetNextRequest
    virtual WONStatus Virtual14();          //2cec80 CheckResponse
    virtual void Virtual18();               //4142a0 Reset, actual override
    void SetEmail(const std::string& inEmail) { mEmail=inEmail; }
    void SetNewPassword(const std::wstring& inPassword) { mNewPassword=inPassword; }
    SmartPtr<ProfileData> mProfileData;      //f4
    std::string mErrorString;               //f8..108
    std::wstring mNewPassword;              //108..118
    std::string mEmail;                     //118..128
private:
    void Init(); //2ce840, actual163B, length4 before newProfile20
};

// The +20 EEWonNet owner is actual WONAPICore (WONAPICore.h); +24 remains separate window interface.
class WONLobbyWindow {
public:
    virtual void Virtual00();
    virtual void Virtual04();
    virtual void Virtual08();
    virtual void Virtual0C();
    virtual void Virtual10();
    virtual ~WONLobbyWindow();              // +0x14
    virtual void Virtual18();
    void Close(bool inFlag);                // base exe 0x6a2220

    bool mClosed;                           // +0x04 (the base dtor waits for it after Close)
};

// the connection to a routing server (sizeof 0x258; ctor 0x2bfb00)
// Model-owned declaration fragments. No implementation or allocation authority.
// Original connection_lifetime.txt / connection_op_rtti.txt / full_connection_header_original.txt.
// Pinned SDK headers provide nominal names only. All bases at0; exact target extents.
// Canonical BlockingSocketa0 and AuthContext/PeerAuthOp pointees must precede use.
// Socket virtual04/08/0c are void(), factory10 returns AsyncSocket* (LAN owner).

// Complete actual SocketThread38/SocketThreadSimple44, pinned native+SDK ownership proof.
// Requires actual Thread1c/Event8 from HTTP/Common packets before these declarations.
class SocketThread : public Thread {
public:
    SocketThread(); //2b60d0
    virtual ~SocketThread(); //2b6120, explicit native ownVT44cb10
    virtual void ThreadFunc(); //base slot10,2b6140
    virtual void AddSocketOp(SocketOp*) {} //newslot1c, target version-empty2fa500
    virtual void RemoveSocketOp(SocketOp*) {} //20, version-empty2fa500
    virtual void Pump(unsigned long) {} //24, version-empty2fa500
    virtual void PurgeOps() {} //28, version-empty4142a0
    virtual bool NeedThread() {return true;} //2c, true2f30d0
    void SetSingleThreaded(bool value) { mSingleThreaded=value; } //actual SDK inline bool34
    static bool DoOp(SocketOp*); //2b6160,cdecl actual ret/pops outside
    CriticalSection mDataCrit; //1c..34
    bool mSingleThreaded; //34; total38
};
class SocketThreadSimple : public SocketThread {
public:
    typedef std::set<SmartPtr<SocketOp>,SmartPtr<SocketOp>::Comp> OpSet;
    virtual ~SocketThreadSimple(); //2b61a0, explicit ownVT44cb44 and PurgeOps
    virtual void ThreadFunc(); //2b6890
    virtual void AddSocketOp(SocketOp*); //2b64f0
    virtual void RemoveSocketOp(SocketOp*); //2b6660
    virtual void Pump(unsigned long); //2b6760
    virtual void PurgeOps(); //2b6380
    OpSet mOpSet; //38 allocator,3chead,40size; total44; nodes18 retainSocketOp0c
};

class ServerConnection : public BlockingSocket {
public:
    void StartKeepAlive(); //2aaf10
    static void StaticConnectCallback(AsyncOpPtr,SmartPtr<RefCount>); //2ab160, support0
    static void StaticMsgCallback(AsyncOpPtr,SmartPtr<RefCount>); //2ab6a0
    static void StaticKeepAliveCallback(AsyncOpPtr,SmartPtr<RefCount>); //2ab7b0, support0
    void KeepAliveCallback(AsyncOp*); //2ab840
    void SendNoOp(); //2ab910
    ServerConnection(); //2aab10; implicit ordinary dtor2aabe0, no explicit body
    virtual void Virtual08(); //CloseHook2abe70
    virtual void Virtual0C(); //KillHook2abf50
    virtual void Virtual14(PeerAuthOp*); //ConnectCallback2ab1f0, ret4
    virtual void Virtual18(const ByteBuffer*); //MsgCallback2fa500,ret4
    void Fn2AB0A0(AuthContext*, AuthType); //SetAuth, real typed identity
    PeerAuthOp* ConnectAsync(const IPAddr&, unsigned long); //2abc40; raw borrowed member result
    WONStatus ConnectBlock(const IPAddr&,DWORD inTimeout=0xffffffff);
    void SetKeepAlivePeriod(DWORD);
    void SetConnectCompletion(CompletionBase<AsyncOpPtr>*); //2abdc0
    SmartPtr<AuthContext> mAuthContext; //a0; ctor2aab35zero, dtor owned
    AuthType mAuthType; //a4; scalar32, never decref
    SmartPtr<PeerAuthOp> mPeerAuthOp; //a8, owned
    OpCompletionBasePtr mConnectCompletion; //ac, owned
    AsyncOpPtr mKeepAliveOp; //b0, owned plainAsyncOp44
    unsigned long mKeepAlivePeriod; //b4=300, sizeb8
};

class ServerConnectionConnectOp : public AsyncOp {
public:
    static void StaticConnectCompletion(AsyncOpPtr,SmartPtr<RefCount>); //2ac240, support0
    void ConnectCompletion(AsyncOp*); //2ac2d0, support0
    ServerConnectionConnectOp(ServerConnection*, const IPAddr&); //2abfc0/ret8
    virtual void Virtual04(); //2ac2f0 RunHook
    virtual void Virtual08(); //2ac380 CleanupHook
    // Destructor stays implicit: ordinary2ac1a0 has NOown-vptr store.
    SmartPtr<ServerConnection> mConnection; //44
    IPAddr mAddr; //48..6f, sizeof70, full0x28nativeIPAddr alreadycanonical
};

class RoutingConnection : public ServerConnection {
public:
    void AddOp(RoutingOp*); //2ba630, actual AutoCrit/list ownership
    void CalcCompletion(RoutingOp*); //2ba740, type-index/default completion
    SmartPtr<RoutingOp> GetUnsolicitedOp(unsigned char,bool); //2ba830, sret/ret0c; call-only
    RoutingConnection(); //2ba050; allocation1c0 at3eddd4
    // 0x2bf1d0: subscribes to the data objects of a type (EEWonNet's room and game objects)
    void ReadDataObject(unsigned short inLinkId, const std::string& inDataType, unsigned long inFlag,
                        CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2bf2e0
    void Fn2BF2E0(unsigned short inLinkId, const std::string& inDataType, unsigned long inFlag,
                  CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2bcc30: registers the client
    void Register(const std::wstring& inName, const std::wstring& inPassword, unsigned long inArg3,
                  unsigned long inArg4, unsigned long inArg5, unsigned long inFlags,
                  CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2bd430: creates a game group
    void CreateGroup(unsigned short inParent, const std::wstring& inName, const std::wstring& inPassword,
                     unsigned short inMaxPlayers, unsigned long inArg5, unsigned char inFlag, unsigned long inArg7,
                     CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2becd0: creates a data object
    void CreateDataObject(unsigned char inFlag, unsigned short inLinkId, const std::string& inDataType,
                          const std::wstring& inName, const ByteBuffer* inData,
                          CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2bee30: replaces a data object
    void ReplaceDataObject(unsigned short inLinkId, const std::string& inDataType, const std::wstring& inName,
                           CompletionBase<AsyncOpPtr>* inCompletion);
    void GetGroupList(unsigned char inFlags, CompletionBase<AsyncOpPtr>* inCompletion);       // 0x2bcf90
    // 0x2c35a0: the client of a name
    // 0x2be540: chat to a group
    void SendChat(const std::wstring& inText, unsigned long inFlags, unsigned short inGroup,
                  CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2be6a0: chat to clients
    void SendChatTo(const std::wstring& inText, unsigned short inFlags, const std::list<unsigned short>& inClients,
                    CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2bef50: replaces a data object's data
    void ModifyDataObject(unsigned short inLinkId, const std::string& inDataType, const std::wstring& inName,
                          unsigned short inOffset, bool inFlag, const ByteBuffer* inData,
                          CompletionBase<AsyncOpPtr>* inCompletion);
    // 0x2bd580: joins a group
    void JoinGroup(unsigned short inId, const std::wstring& inPassword, const std::wstring& inComment, unsigned char inFlag4,
                   unsigned char inFlag5, CompletionBase<AsyncOpPtr>* inCompletion);
    void CancelJoinGroup(unsigned short inId, bool inFlag2, CompletionBase<AsyncOpPtr>* inCompletion);      // 0x2bd6b0
    void LeaveGroup(unsigned short inId, CompletionBase<AsyncOpPtr>* inCompletion);              // 0x2bd850
    // 0x2bf0c0: subscribes to a group's data objects
    void Fn2BF0C0(unsigned short inLinkId, const std::string& inDataType, unsigned long inFlag,
                  CompletionBase<AsyncOpPtr>* inCompletion);
    void GetClientList(CompletionBase<AsyncOpPtr>* inCompletion);  // 0x2bcd40
    void GetMembersOfGroup(unsigned short inId, CompletionBase<AsyncOpPtr>* inCompletion);    // 0x2bd1e0


// Insert INSIDE actual RoutingConnection1c0; model/root owns promotion and names.
// All returnvoid; native_call_contracts.txt retains actual wrapper/callee stacks,
// typed member writes and owned completion assignment. No operation allocation
// bodies, shadow receiver or overload solely for ABI adaptation is proposed.
void SetClientFlags(unsigned long clientMask, unsigned long clientFlags,
    unsigned long asyncMask, unsigned long asyncFlags,
    CompletionBase<AsyncOpPtr>* completion); //2be390 ret14
void SetGroupFlags(unsigned short groupId, unsigned long groupMask,
    unsigned long groupFlags, unsigned long asyncMask, unsigned long asyncFlags,
    CompletionBase<AsyncOpPtr>* completion); //2bdc10 ret18
void SendData(const ByteBuffer* data, unsigned char flags, unsigned short recipient,
    CompletionBase<AsyncOpPtr>* completion); //2be910 ret10; flags lowbyte in2bea70
void SendData(const ByteBuffer* data,unsigned char flags,const std::list<unsigned short>& recipients,
    CompletionBase<AsyncOpPtr>* completion); //native2bea70 ret10; genuine SDK overload
void BanClient(unsigned short groupId, bool isBanned, unsigned long seconds,
    const std::wstring& explanation, unsigned short clientId,
    CompletionBase<AsyncOpPtr>* completion); //2bf590 ret18; SDKBanClientByClientId
void AcceptClient(unsigned short groupId, unsigned short clientId, bool accepted,
    const std::wstring& explanation,
    CompletionBase<AsyncOpPtr>* completion); //2bd9f0 ret14
void InviteClient(unsigned short groupId, unsigned short clientId, bool invited,
    const std::wstring& explanation,
    CompletionBase<AsyncOpPtr>* completion); //2bdb00 ret14
void BanClientByCDKeyId(unsigned short groupId,bool on,unsigned long seconds,
    const std::wstring& explanation,unsigned long cdKeyId,
    CompletionBase<AsyncOpPtr>* completion); //genuine SDK Source-only dispatch
void BanClientByWONId(unsigned short groupId, bool on, unsigned long seconds,
    const std::wstring& explanation, unsigned long wonId,
    CompletionBase<AsyncOpPtr>* completion); //native2bf6b0 ret18
void MuteClientByWONId(unsigned short groupId, bool on, unsigned long seconds,
    const std::wstring& explanation, unsigned long wonId,
    CompletionBase<AsyncOpPtr>* completion); //native2bf8f0 ret18
void BecomeModerator(bool moderatorOn,
    CompletionBase<AsyncOpPtr>* completion); //2bf3f0 ret8
void MuteClient(unsigned short groupId, bool isMuted, unsigned long seconds,
    const std::wstring& explanation, unsigned short clientId,
    CompletionBase<AsyncOpPtr>* completion); //2bf7d0 ret18; SDKMuteClientByClientId
void SendServerAlert(const std::wstring& text,
    CompletionBase<AsyncOpPtr>* completion); //2bddd0 ret8
void StartServerShutdown(const std::wstring& text, unsigned long seconds,
    CompletionBase<AsyncOpPtr>* completion); //2be010 ret0c
void AbortServerShutdown(CompletionBase<AsyncOpPtr>* completion); //2be200 ret4

// Genuine same-SDK Source-only operation APIs; existing complete owners retained.
    void DisconnectClient(CompletionBase<AsyncOpPtr> *theCompletion);
    void GetClientCDKeyIdList(CompletionBase<AsyncOpPtr> *theCompletion, bool bDoSubscribe);
    void GetClientListEx(CompletionBase<AsyncOpPtr> *theCompletion);
    void SetGroupName(unsigned short theGroupId, const std::wstring &theName, CompletionBase<AsyncOpPtr> *theCompletion);
    void SetGroupMaxPlayers(unsigned short theGroupId, unsigned short theMaxPlayers, CompletionBase<AsyncOpPtr> *theCompletion);
    void RelinquishCaptaincy(unsigned short theGroupId, unsigned short theNewCaptainId, CompletionBase<AsyncOpPtr> *theCompletion);
    void SendServerAlertWithRecipientList(const std::wstring &theAlertText, std::list<unsigned short>& recipientList, CompletionBase<AsyncOpPtr> *theCompletion);
    void DetectFirewall(unsigned short theListenPort, unsigned long theMaxConnectSeconds, bool doListen, CompletionBase<AsyncOpPtr> *theCompletion);
    void MuteClientByCDKeyId(unsigned short theGroupId, bool isMuted, unsigned long theSeconds, 
	const std::wstring &theComment, unsigned long theCDKeyId, CompletionBase<AsyncOpPtr> *theCompletion);
    void GetBadUserList(RoutingBadUserListType theListType, unsigned short theGroupId, unsigned char theFlags,
									   CompletionBase<AsyncOpPtr> *theCompletion);


    virtual void Virtual08(); //CloseHook2bcb20
    virtual void Virtual0C(); //KillHook2bcbb0
    virtual void Virtual18(const ByteBuffer*); //MsgCallback2bc850, ret4
    virtual RoutingClientInfoPtr Virtual1C(); //GetNewClientInfo2ba100; owned sret ret4
    virtual RoutingMemberInfoPtr Virtual20(); //GetNewMemberInfo2ba230; owned sret ret4
    virtual RoutingGroupInfoPtr Virtual24(); //GetNewGroupInfo2ba300; owned sret ret4
    virtual SmartPtr<RoutingOp> Virtual28(unsigned char); //GetUnsolicitedDerivedOp2bc6c0, sret+arg ret8
    virtual void Virtual2C(RoutingOp*) {} //genuine SDK inline RoutingOpCompleteHook, shared30a800 ret4
    // 04/10/14 are inherited unchanged; no generic vtable padding or aliases.
    typedef std::list<SmartPtr<RoutingOp> > RoutingOpList;
    RoutingOpList mOpList; //b8 allocator/headbc/countc0; owns elements
    bool mAllDerivedOpsAreUnsolicited; //c4
    OpCompletionBasePtr mCompletions[61]; //c8..1b8, targetctor61-element vector
    OpCompletionBasePtr mDefaultCompletion; //1bc; extent1c0
protected:
    virtual ~RoutingConnection(); //ordinary2ba4b0; explicit own-vptr store
};

// TargetIgnoreClient2c3848/2c38a5 both call2a6b60, compare negative for tree order.

class SmartRoutingConnection : public RoutingConnection {
public:
    // Nominal enum declarations; target widths/int fields and map operations corroborate.
    enum MatchResult { MatchResult_NotFound, MatchResult_Ambiguous, MatchResult_Partial, MatchResult_Exact };
    enum InputError { InputError_None, InputError_InvalidCommand, InputError_ClientNotFound,
        InputError_ClientAmbiguous, InputError_NoChat, InputError_NotLoggedIn, InputError_BadTime };
    enum InputCommand { InputCommand_None, InputCommand_Broadcast, InputCommand_Whisper,
        InputCommand_Emote, InputCommand_Ignore, InputCommand_Away, InputCommand_BecomeModerator,
        InputCommand_ServerMute, InputCommand_ServerBan, InputCommand_Mute, InputCommand_Ban,
        InputCommand_Unmute, InputCommand_ServerUnmute, InputCommand_Invite, InputCommand_Uninvite };
    // Genuine SDK value-copy records; independent records, no enclosing-object fields.
struct ClientInfo
	{
		std::wstring mName;
		unsigned short mId;
		unsigned long mFlags;
		unsigned long mCDKeyId;

		ClientInfo() : mId(0), mFlags(0), mCDKeyId(0) {}
	};
struct MemberInfo
	{
		std::wstring mName;
		unsigned short mClientId;
		unsigned long mClientFlags;
		unsigned char mMemberFlags;

		MemberInfo() : mClientId(0), mClientFlags(0), mMemberFlags(0) {}
	};
struct GroupInfo
	{
		std::wstring mName;
		unsigned short mId;
		unsigned short mCaptainId;
		unsigned short mMaxPlayers;
		unsigned long mFlags;
		unsigned long mAsyncFlags;
		unsigned short mObserverCount; // only applicable if non-detailed observers is on

		GroupInfo() : mId(0), mCaptainId(0), mMaxPlayers(0), mFlags(0), mAsyncFlags(0), mObserverCount(0) {}
	};
    RoutingMemberInfoPtr GetMemberRef(unsigned short,int=-1);
    bool GetClient(unsigned short,ClientInfo&);
    bool GetGroup(unsigned short,GroupInfo&);
    bool GetMember(unsigned short,unsigned short,MemberInfo&);
    void StartClientItr();
    bool GetNextClient(ClientInfo&);
    void StartGroupItr();
    bool GetNextGroup(GroupInfo&);
    bool StartMemberItr(unsigned short);
    bool GetNextMember(MemberInfo&);
    bool GetLastTargetMember(MemberInfo&);
    bool GetLastTargetClient(ClientInfo&);
    SmartRoutingConnection(); //2bfb00; historical allocation258 belongs THIS class
    // Destructor stays implicit: ordinary2c0130 lacks own-vptr store, natural member cleanup.
    virtual void Virtual08(); //CloseHook2c4120
    virtual RoutingClientInfoPtr Virtual1C(); //2bfe60 SAME base smartptr result, not covariantSP
    virtual void Virtual2C(RoutingOp*); //2c0be0, actualop type calls slot14
    void InitHandlerVariables(RoutingClientInfo*); //native2c0a10
    void InitHandlerVariables(RoutingGroupInfo*,RoutingMemberInfo* =NULL); //native2c0ac0
    void HandleClientFlagsChanged(RoutingClientFlagsChangedOp*); //native2c24f0
    void HandleGroupCaptainChanged(RoutingGroupCaptainChangedOp*); //native2c2860
    void HandleGroupMaxPlayersChanged(RoutingGroupMaxPlayersChangedOp*); //native2c2900
    void HandleGroupFlagsChanged(RoutingGroupFlagsChangedOp*); //native2c2bb0
    void HandleGroupNameChanged(RoutingGroupNameChangedOp*); //native2c2c50
    void HandleGroupObserverCount(RoutingGroupObserverCountOp*); //native2c2d00
    void HandleMemberFlagsChanged(RoutingMemberFlagsChangedOp*); //native2c2da0
    RoutingClientInfoPtr GetClient(unsigned short); //2c0330, owned BY VALUE
    RoutingGroupInfoPtr GetGroup(unsigned short); //2c0450, owned BY VALUE
    void AddCommandString(const std::wstring&, InputCommand); //native2c3970
    void AddDefaultUserCommandStrings(); //native2c3a50
    void AddDefaultModeratorCommandStrings(); //native2c3d90
    void SetAllowPartialNameMatch(bool); //native2c40a0 locked actual enum2/3
    void SetServerWideCommands(bool); //native2c40f0
    void EraseClient(const RoutingClientMap::iterator&); //2c0570
    void EraseClient(unsigned short); //2c0600
    void EraseGroup(const RoutingGroupMap::iterator&); //2c0670
    void EraseMember(RoutingMemberMap&,const RoutingMemberMap::iterator&); //2c0740
    void SetClientName(RoutingClientInfo*,const std::wstring&); //2c07d0
    SmartRoutingClientInfo* EnsureInClientMap(unsigned short); //2c0860, borrowedraw
    void HandleGetClientList(RoutingGetClientListOp*); //2c1130
    void SyncMemberMapWithClientMap(RoutingMemberMap&,const RoutingMemberMap&); //2c1bd0
    void SyncMemberMapWithClientMap(RoutingMemberMap&); //2c1f20
    void HandleGetGroupList(RoutingGetGroupListOp*); //2c2070
    void SyncClientMapWithIgnoreList(); //2c2740
    void HandleRegisterClient(RoutingRegisterClientOp *theOp);
    void HandleSetClientFlags(RoutingSetClientFlagsOp *theOp);
    void HandleReadDataObject(RoutingReadDataObjectOp *theOp);
    void HandleDataObjectCreated(RoutingDataObjectCreatedOp *theOp);
    void HandleDataObjectModified(RoutingDataObjectModifiedOp *theOp);
    void SetClientCDKeyIdFromDataObject(unsigned short theClientID, ByteBufferPtr theBuf);
    void HandleClientJoinedGroup(RoutingClientJoinedGroupOp *theOp);
    void HandleClientLeftGroup(RoutingClientLeftGroupOp *theOp);
    void HandleGetMembersOfGroup(RoutingGetMembersOfGroupOp *theOp);
    void HandleJoinGroup(RoutingJoinGroupOp *theOp);
    void HandleClientJoinedServer(RoutingClientJoinedServerOp *theOp);
    void HandleClientLeftServer(RoutingClientLeftServerOp *theOp);
    void HandleGroupCreated(RoutingGroupCreatedOp *theOp);
    void HandleGroupDeleted(RoutingGroupDeletedOp *theOp);
    void CheckIgnore(RoutingOp *theOp, unsigned short theSenderId);
    void HandlePeerChat(RoutingPeerChatOp *theOp);
    void HandleGroupInvitation(RoutingGroupInvitationOp *theOp);
    void HandleGroupJoinAttempt(RoutingGroupJoinAttemptOp *theOp);
    void HandleLeaveGroup(RoutingLeaveGroupOp *);
    void HandleDataObjectDeleted(RoutingDataObjectDeletedOp	*theOp);
    MatchResult FindClient(const std::wstring&, RoutingClientInfoPtr&); //2c35a0
    MatchResult GetMemberByNameRef(RoutingGroupInfoPtr,const std::wstring&,RoutingMemberInfoPtr&);
    MatchResult GetMemberByNameRef(unsigned short,const std::wstring&,RoutingMemberInfoPtr&);
    bool HandleInput(const std::wstring&,int=-1);
    void ClearCommandStrings();
    void SetDoColonEmote(bool);
protected:
    bool HandleCommand(const std::wstring&,int);
public:
    void IgnoreClient(RoutingClientInfo*, bool); //2c37d0 RAW pointer, not SmartPtr&
    void LoadIgnoreSet(); // native2c3110
    void SaveIgnoreSet(); // native2c3460
    void SetProductName(const std::string&); //2c2fc0 actual IgnoreFilePath setter; old name provisional
    void ClearProductName() { SetProductName(""); } //existing protected helper body, actualignorepath setter
    const RoutingGroupMap& GetGroupMap() const { return mGroupMap; }
    unsigned short GetMyClientId() const { return mClientId; }   // inline (OnChat 0x4d040c: loaded into si first)
    RoutingGroupMap mGroupMap; //1c0..1cb
    RoutingClientMap mClientMap; //1cc..1d7
    RoutingClientMap::iterator mClientItr; //1d8
    RoutingGroupMap::iterator mGroupItr; //1dc
    RoutingMemberMap::iterator mMemberItr; //1e0
    RoutingGroupInfoPtr mMemberItrGroup; //1e4, CORRECT group ptr (not member ptr)
    unsigned short mDefaultGroupId; //1e8
    RoutingClientInfoPtr mMyClient; //1ec
    unsigned short mClientId; //1f0, nominal mMyClientId; keep old protected read name only
    unsigned long mAsyncFlags; //1f4
    typedef std::map<std::wstring,InputCommand> CommandStringMap;
    CommandStringMap mCommandStringMap; //1f8..203
    MatchResult mMatchTolerance; //204
    bool mServerWideCommands; //208
    bool mDoColonEmote; //209
    InputError mLastInputError; //20c
    InputCommand mLastInputCommand; //210
    RoutingMemberInfoPtr mLastTargetMember; //214
    RoutingClientInfoPtr mLastTargetClient; //218
    std::wstring mLastMatchName; //21c..22b
    RoutingClientInfoPtr mHandlerClient; //22c, OWNING
    RoutingGroupInfoPtr mHandlerGroup; //230, OWNING
    RoutingMemberInfoPtr mHandlerMember; //234, OWNING
    bool mReadClientCDKeyId; //238
    typedef std::set<std::wstring,WStringLessNoCase> IgnoreSet;
    IgnoreSet mIgnoreSet; //23c..247; comparator2a6b60
    std::string mIgnoreFilePath; //248..257
};

// No class here is a duplicate alias for old flattened RoutingConnection258.
// No zero-valued gap fields or placement/partial object lifetime is introduced.
// All callable names remain root-owned until original/candidate binding review.

typedef SmartPtr<RoutingConnection> RoutingConnectionPtr;
typedef SmartPtr<SmartRoutingConnection> SmartRoutingConnectionPtr;


}

#endif
