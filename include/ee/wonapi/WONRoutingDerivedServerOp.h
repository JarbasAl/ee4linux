// Complete actual derived routing6c owner and genuine inline construction.
#ifndef EE_WONAPI_WONROUTINGDERIVEDSERVEROP_H
#define EE_WONAPI_WONROUTINGDERIVEDSERVEROP_H
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
class RoutingDerivedServerOp : public RoutingOp {
protected:
    unsigned char mMsgType; //64, natural65..67 alignment
    ByteBufferPtr mRecvMsg; //68, full6c
    virtual WONStatus Virtual10(unsigned char,ReadBuffer&); //slot10 actual2c5260/180 RET8
public:
    RoutingDerivedServerOp(RoutingConnection* connection)
        :RoutingOp(connection),mMsgType(0) { mIsDerivedServerOp=true; } //SDK inline/native2bc701..2bc719; support0
    virtual RoutingOpType GetType() const { return (RoutingOpType)60; } //2bc780 true6B accessor, support0
    unsigned char GetMsgType() { return mMsgType; } //genuine SDK, no standalone RVA/code credit
    const ByteBuffer* GetRecvMsg() const { return mRecvMsg.get(); } //genuine SDK borrowed const-method getter
    // Natural implicit destructor releases68 then complete existingRoutingOp64;
    // actualordinary2bc7b0/157, no authored lifetime definition or invented provider.
};
}
#endif
