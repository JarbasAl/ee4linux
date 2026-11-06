// Original-backed complete AuthBase1c owner, RTTI/native ctor2dcce0.
#ifndef EE_WONAPI_WONAUTHBASE_H
#define EE_WONAPI_WONAUTHBASE_H
#include <time.h>
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {
class ElGamal;
class AuthBase : public RefCount {
    ByteBufferPtr mRawBuf; //08
    unsigned short mDataLen; //0c
    unsigned short mFamily; //0e
    time_t mIssueTime; //10
    time_t mExpireTime; //14
    bool mIsValid; //18; complete extent1c
protected:
    virtual ~AuthBase(); //2dcdd0
    virtual bool UnpackHook(ReadBuffer&); //2dcf80
    bool Unpack(); //2dce40
public:
    AuthBase(const void* = NULL, unsigned long = 0); //2dcce0
    ByteBufferPtr GetRawBuf() const { return mRawBuf; } //genuine SDK owning return, actual native2e8060/51B sretRET4
    const void* GetRaw() const { return mRawBuf.get()!=NULL ? mRawBuf->data() : NULL; } //genuine SDK borrowed raw08; Start2e6730
    unsigned long GetRawLen() const { return mRawBuf.get()!=NULL ? mRawBuf->length() : 0; } //genuine SDK raw08 length; Start2e6730
    bool IsValid() const { return mIsValid; } //actual SDK inline, native18
    const void* GetData() const { return mRawBuf.get()!=NULL ? mRawBuf->data() : NULL; }
    unsigned long GetDataLen() const { return mRawBuf.get()!=NULL ? mDataLen : 0; }
    const void* GetSig() const { return mRawBuf.get()!=NULL ? mRawBuf->data()+mDataLen : NULL; }
    unsigned long GetSigLen() const { return mRawBuf.get()!=NULL ? mRawBuf->length()-mDataLen : 0; }
    time_t GetIssueTime() const { return mIssueTime; } //actual SDK borrowed timestamp10
    time_t GetExpireTime() const { return mExpireTime; } //actual SDK inline/native14
    bool IsExpired(time_t = 0) const; //2dcf40
    bool Verify(const ElGamal&) const; //2dced0 remains external; no ElGamal allocation/type claim
};
}
#endif
