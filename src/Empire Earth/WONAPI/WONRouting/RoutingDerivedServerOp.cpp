// Actual derived routing payload holder and optional unsolicited-op factory.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRoutingDerivedServerOp.h"
namespace WONAPI {
// FUNCTION: WON 0x006c5260
WONStatus RoutingDerivedServerOp::Virtual10(unsigned char messageType,ReadBuffer& message)
{
    mMsgType=messageType;
    mRecvMsg=new ByteBuffer(message.mData+message.mL00,message.mL08-message.mL00,false);
    return 0;
}
// FUNCTION: WON 0x006bc6c0
SmartPtr<RoutingOp> RoutingConnection::Virtual28(unsigned char)
{
    if(mAllDerivedOpsAreUnsolicited)
        return new RoutingDerivedServerOp(this);
    return NULL;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006bc790
// (public: virtual void * __thiscall WONAPI::RoutingDerivedServerOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingDerivedServerOp@WONAPI@@UA)
// TEMPLATE: WON 0x006bc7b0
// (public: virtual __thiscall WONAPI::RoutingDerivedServerOp::~RoutingDerivedServerOp(void)    [??1RoutingDerivedServerOp@WONAPI@@UAE@XZ])
