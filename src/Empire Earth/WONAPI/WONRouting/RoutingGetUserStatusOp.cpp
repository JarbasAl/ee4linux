// Native friend-status routing datagram operation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006c6630
RoutingGetUserStatusOp::RoutingGetUserStatusOp(const IPAddr& inAddress, UDPManager* inManager)
{
    SetDestAddr(inAddress);
    SetUDPManager(inManager);
    SetNumRetransmits(3);
    SetMsgTimeout(1000);
}
// FUNCTION: WON 0x006c68c0
void RoutingGetUserStatusOp::AddWONUserId(unsigned long inUserId)
{
    mFriendStatusMap.insert(FriendStatusMap::value_type(inUserId, 0));
}
// FUNCTION: WON 0x006c6970
void RoutingGetUserStatusOp::Virtual04()
{
    WriteBuffer message(0, true);
    message.AppendByte(3);
    message.AppendByte(7);
    message.AppendByte(72);
    message.AppendByte((unsigned char)mFriendStatusMap.size());
    for (FriendStatusMap::const_iterator i = mFriendStatusMap.begin(); i != GetFriendStatusMap().end(); ++i)
        message.AppendLong(i->first);
    mOutgoing = message.ToByteBuffer(true);
    UDPManagerSendRecvOp::Virtual04();
}
// FUNCTION: WON 0x006c6ab0
WONStatus RoutingGetUserStatusOp::Virtual0C(ReadBuffer& inMessage)
{
    try {
        if (inMessage.ReadByte() != 3) return (WONStatus)100004;
        if (inMessage.ReadByte() != 7) return (WONStatus)100004;
        if (inMessage.ReadByte() != 73) return (WONStatus)100004;
        unsigned char count = inMessage.ReadByte();
        if (count != mFriendStatusMap.size()) return (WONStatus)100006;
        FriendStatusMap::iterator i = mFriendStatusMap.begin();
        for (int j = 0; j < count; ++j, ++i)
            i->second = inMessage.ReadByte();
        return (WONStatus)0;
    } catch (ReadBufferException&) {
        return (WONStatus)100012;
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c6730
// (public: virtual void * __thiscall WONAPI::RoutingGetUserStatusOp::`scalar deleting destructor'(unsigned int)    [??_GRoutingGetUserStatusOp@WONAPI@@UA)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c6750
// (public: virtual __thiscall WONAPI::RoutingGetUserStatusOp::~RoutingGetUserStatusOp(void)    [??1RoutingGetUserStatusOp@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c9f90
// (protected: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned char>,struct std::_Mapk<unsigned long,unsigned char,struct st)
