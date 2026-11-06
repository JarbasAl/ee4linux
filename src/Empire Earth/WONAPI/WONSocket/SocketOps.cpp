// Original-backed send operation family; filename/grouping provisional.
// Source-only scratch, no runtime authority and no compiler lease.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <sys/utime.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {

// FUNCTION: WON 0x006b5e50
WONStatus SendBytesOp::Virtual0C()
{
    mReadBuffer.Rewind();
    return Virtual10();
}

// FUNCTION: WON 0x006b5e70
WONStatus SendBytesOp::Virtual10()
{
    while (mReadBuffer.mL00 < mReadBuffer.length()) {
        mSocket->Fn2B1360(TimeLeft());
        int aSent = 0;
        WONStatus aStatus = mSocket->Send(mReadBuffer.mData + mReadBuffer.mL00,
            mReadBuffer.mL08 - mReadBuffer.mL00, &aSent);
        if (aSent > 0)
            mReadBuffer.ReadBytes(aSent);
        if (aStatus != 100001)
            return aStatus;
        if (TimeLeft() == 0)
            return 100001;
    }
    return 0;
}

// FUNCTION: WON 0x006b5f10
WONStatus SendBytesToOp::Virtual10()
{
    mSocket->Fn2B1360(TimeLeft());
    if (mSocket->Fn2B08D0())
        return mSocket->Send(mBuffer->data(), mBuffer->length(), NULL);
    return mSocket->Fn2B1700(mBuffer->data(), mBuffer->length(), mAddress);
}

// FUNCTION: WON 0x006b5fa0
WONStatus SendMsgOp::Virtual0C()
{
    mBuffer = mMessage;
    WONStatus aStatus = mSocket->Fn2B07E0(mBuffer);
    if (aStatus != 0)
        return aStatus;
    mReadBuffer.SetData(mBuffer->data(), mBuffer->length());
    return Virtual10();
}

// FUNCTION: WON 0x006b6020
WONStatus SendMsgOp::Virtual10()
{
    WONStatus aStatus = SendBytesOp::Virtual10();
    if (aStatus == 0)
        NetStats::IncrementMessagesSent(1);
    return aStatus;
}
// Round3 additional original operation behavior; source-only.

// FUNCTION: WON 0x006b4de0
RecvBytesOp::RecvBytesOp(unsigned long inLength, AsyncSocket* inSocket)
    : SocketOp(inSocket), mWriteBuffer(0, true)
{
    mB58[0] = true;
    mLength = inLength;
    mFile = NULL;
    mAppend = false;
    mBinary = true;
    mB80 = false;
    mFileModifyTime = 0;
}

// FUNCTION: WON 0x006b5000
WONStatus RecvBytesOp::Fn2B5000()
{
    mBytesRead = 0;
    mBytesSinceProgress = 0;
    mOwnFile = false;
    if (mFile == NULL && !mPath.empty()) {
        if (mBinary)
            mFile = fopen(mPath.c_str(), mAppend ? "a+b" : "wb");
        else
            mFile = fopen(mPath.c_str(), mAppend ? "a+" : "w");
        if (mFile == NULL)
            return 100007;
        mOwnFile = true;
    }
    if (mLength > 0 && mFile == NULL && !mB80)
        mWriteBuffer.Reserve(mLength);
    return Fn2B50C0();
}

// FUNCTION: WON 0x006b50c0
WONStatus RecvBytesOp::Fn2B50C0()
{
    for (;;) {
        mSocket->Fn2B1410(TimeLeft());
        int aRead = 0;
        WONStatus aStatus;
        if (mLength > 0 && mFile == NULL && !mB80) {
            aStatus = mSocket->Fn2B15E0(mWriteBuffer.data() + mWriteBuffer.length(),
                mWriteBuffer.capacity() - mWriteBuffer.length(), &aRead);
            if (aRead > 0)
                mWriteBuffer.Fn2A8260(aRead);
        }
        else {
            char aBuffer[1024];
            int aLength = 1024;
            if (mLength > 0) {
                aLength = mLength - mBytesRead;
                if (aLength > 1024) aLength = 1024;
                else if (aLength < 0) aLength = 0;
            }
            aStatus = mSocket->Fn2B15E0(aBuffer, aLength, &aRead);
            if (aRead > 0 && !mB80) {
                if (mFile != NULL) {
                    if (fwrite(aBuffer, 1, aRead, mFile) != aRead)
                        return Fn2B54A0(100008);
                }
                else
                    mWriteBuffer.AppendBytes(aBuffer, aRead);
            }
        }
        mBytesRead += aRead;
        if (aRead > 0 && mProgressCompletion.get() != NULL) {
            mBytesSinceProgress += aRead;
            if (mBytesSinceProgress > mProgressInterval) {
                SmartPtr<RecvChunkOp> anOp = new RecvChunkOp(this);
                anOp->mCompletion = mProgressCompletion;
                anOp->Fn29F500(mRunMode, 0);
                mBytesSinceProgress %= mProgressInterval;
            }
        }
        if ((mLength == 0 && aStatus == 100020) || (mLength != 0 && mBytesRead == mLength))
            return Fn2B54A0(0);
        if (aStatus == 100001) {
            if (TimeLeft() == 0)
                return 100001;
        }
        else if (aStatus != 0)
            return aStatus;
    }
}

// FUNCTION: WON 0x006b54a0
WONStatus RecvBytesOp::Fn2B54A0(WONStatus inStatus)
{
    if (mFile == NULL) {
        mReceivedData = mWriteBuffer.ToByteBuffer(true);
        return inStatus;
    }
    return Fn2B5410(inStatus);
}

// FUNCTION: WON 0x006b5b00
WONStatus RecvMsgOp::Virtual0C()
{
    mState = 1;
    mLength = mSocket->GetLengthFieldSize();
    if (mLength != 1 && mLength != 2 && mLength != 4)
        return 100023;
    WONStatus aStatus = Fn2B5000();
    return aStatus == 0 ? Fn2B5B50() : aStatus;
}

// FUNCTION: WON 0x006b5b50
WONStatus RecvMsgOp::Fn2B5B50()
{
    mState = 2;
    ByteBufferPtr aBuffer = GetBytes();
    ReadBuffer aMessage(aBuffer->data(), aBuffer->length());
    unsigned long aLength;
    switch (aMessage.length()) {
    case 1: aLength = aMessage.ReadByte(); break;
    case 2: aLength = (unsigned short)aMessage.ReadShort(); break;
    case 4: aLength = aMessage.ReadLong(); break;
    default: return 100024;
    }
    if (aLength <= aMessage.length() || aLength >= 256000)
        return 100024;
    if (aLength == 65535 && aMessage.length() == 2) {
        mLength = 4;
        mState = 1;
        WONStatus aStatus = Fn2B5000();
        return aStatus == 0 ? Fn2B5B50() : aStatus;
    }
    mState = 2;
    mLength = aLength - aMessage.length();
    WONStatus aStatus = Fn2B5000();
    return aStatus == 0 ? Fn2B5D20() : aStatus;
}

// FUNCTION: WON 0x006b5d20
WONStatus RecvMsgOp::Fn2B5D20()
{
    NetStats::IncrementMessagesReceived(1);
    mMessage = GetBytes();
    return mSocket->Fn2B0730(mMessage);
}

// FUNCTION: WON 0x006b5e20
WONStatus RecvMsgOp::Virtual10()
{
    WONStatus aStatus = Fn2B50C0();
    if (aStatus != 0)
        return aStatus;
    return mState == 1 ? Fn2B5B50() : Fn2B5D20();
}
// FUNCTION: WON 0x006b5410
WONStatus RecvBytesOp::Fn2B5410(WONStatus inStatus)
{
    if (mFile != NULL) {
        if (mOwnFile && fclose(mFile) != 0)
            inStatus = 100008;
        if (mFileModifyTime != 0) {
            struct _utimbuf aTime;
            aTime.actime = mFileModifyTime;
            aTime.modtime = mFileModifyTime;
            _utime(mPath.c_str(), &aTime);
        }
        mFile = NULL;
    }
    return inStatus;
}

// FUNCTION: WON 0x006b5580
void RecvBytesOp::Virtual08()
{
    SocketOp::Virtual08();
    Fn2B5410(mStatus);
}

// New original RecvBytesFromOp full ownership implementation; source-only.

// FUNCTION: WON 0x006b55a0
RecvBytesFromOp::RecvBytesFromOp(unsigned short inMaxLength, AsyncSocket* inSocket)
    : SocketOp(inSocket)
{
    mB58[0] = true;
    mMaxLength = inMaxLength;
    mRawBuffer = NULL;
}
// FUNCTION: WON 0x006b56f0
RecvBytesFromOp::~RecvBytesFromOp()
{
    delete mRawBuffer;
}
// FUNCTION: WON 0x006b57d0
void RecvBytesFromOp::Virtual04()
{
    if (mRawBuffer == NULL)
        mRawBuffer = new char[mMaxLength];
    mReceivedData = NULL;
    SocketOp::Virtual04();
}
// FUNCTION: WON 0x006b5820
WONStatus RecvBytesFromOp::Virtual10()
{
    mSocket->Fn2B1410(TimeLeft());
    int aReceived = 0;
    WONStatus aStatus;
    if (mSocket->Fn2B08D0()) {
        mFromAddress = mSocket->GetDestAddr();
        aStatus = mSocket->Fn2B15E0(mRawBuffer, mMaxLength, &aReceived);
    }
    else
        aStatus = mSocket->Fn2B1840(mRawBuffer, mMaxLength, &mFromAddress, &aReceived);
    if (aStatus == 0) {
        mReceivedData = new ByteBuffer(mRawBuffer, aReceived, true);
        mRawBuffer = NULL;
    }
    return aStatus;
}
}
namespace WONAPI {
// FUNCTION: WON 0x006b4f00
AsyncOp* RecvBytesOp::Duplicate()
{
    return new RecvBytesOp(mLength,mSocket.get());
}
// FUNCTION: WON 0x006ab3d0
AsyncOp* RecvMsgOp::Duplicate()
{
    return new RecvMsgOp(mSocket.get());
}
// FUNCTION: WON 0x006b5660
AsyncOp* RecvBytesFromOp::Duplicate()
{
    return new RecvBytesFromOp(mMaxLength,mSocket.get());
}
// FUNCTION: WON 0x006b4f90
void RecvBytesOp::Fn2B4F90(CompletionBase<AsyncOpPtr>* completion,long interval)
{
    mProgressCompletion=completion;
    mProgressInterval=interval;
}
// FUNCTION: WON 0x006ab770
ByteBufferPtr RecvMsgOp::GetMsg() const
{
    return mMessage;
}
// FUNCTION: WON 0x006aa920
ByteBufferPtr RecvBytesFromOp::GetBytes() const
{
    return mReceivedData;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b5380
// (public: virtual void * __thiscall WONAPI::RecvChunkOp::`scalar deleting destructor'(unsigned int)    [??_GRecvChunkOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006b53a0
// (public: virtual __thiscall WONAPI::RecvChunkOp::~RecvChunkOp(void)    [??1RecvChunkOp@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006b56d0
// (public: virtual void * __thiscall WONAPI::RecvBytesFromOp::`scalar deleting destructor'(unsigned int)    [??_GRecvBytesFromOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b5370
// (public: virtual void __thiscall WONAPI::RecvChunkOp::Virtual04(void)    [?Virtual04@RecvChunkOp@WONAPI@@UAEXXZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006ab450
// (public: virtual void * __thiscall WONAPI::RecvMsgOp::`scalar deleting destructor'(unsigned int)    [??_GRecvMsgOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b4f70
// (public: virtual void * __thiscall WONAPI::RecvBytesOp::`scalar deleting destructor'(unsigned int)    [??_GRecvBytesOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006b4b70
// (public: virtual void * __thiscall WONAPI::SocketOp::`scalar deleting destructor'(unsigned int)    [??_GSocketOp@WONAPI@@UAEPAXI@Z])
