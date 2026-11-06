// Native routing behavior; source-only reconstruction.
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
// FUNCTION: WON 0x6c5320
void RoutingGetBadUserListOp::Virtual0C()
{
    InitSendMsg(22);
    mSendMsg.AppendByte((unsigned char)mListType);
    mSendMsg.AppendShort(mGroupId);
    mSendMsg.AppendByte(mFlags);
    SendMsg();
    AddOp();
}

// FUNCTION: WON 0x6c5370
WONStatus RoutingGetBadUserListOp::Virtual10(unsigned char inMsgType,ReadBuffer& inMsg)
{
    if(inMsgType!=21) return (WONStatus)100104;
    mBadUserList.clear();
    WONStatus status=inMsg.ReadShort();
    if(status==0) {
        mListType=(RoutingBadUserListType)(signed char)inMsg.ReadByte();
        mGroupId=inMsg.ReadShort();
        mFlags=inMsg.ReadByte();
        int count=(unsigned short)inMsg.ReadShort();
        for(;count>0;--count) {
            unsigned short length=inMsg.ReadShort();
            long start=inMsg.pos();
            RoutingBadUser user; // real default: DWORD10/14/18=0, three wstring owners
            user.mWONId=inMsg.ReadLong();
            inMsg.ReadWString(user.mName,2);
            if(GetFlags()&1) user.mExpirationDiff=inMsg.ReadLong();
            if(GetFlags()&2) user.mModeratorWONId=inMsg.ReadLong();
            if(GetFlags()&4) inMsg.ReadWString(user.mModeratorName,2);
            if(GetFlags()&8) inMsg.ReadWString(user.mModeratorComment,2);
            mBadUserList.push_back(user);
            inMsg.ReadBytes(length-inMsg.pos()+start);
        }
    }
    return status;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c5600
// (public: __thiscall WONAPI::RoutingBadUser::~RoutingBadUser(void)    [??1RoutingBadUser@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ca450
// (protected: void __thiscall std::list<struct WONAPI::RoutingBadUser,class std::allocator<struct WONAPI::RoutingBadUser> >::_Incsize(unsigned int))
// TEMPLATE: WON 0x006cbb90
// (void __cdecl std::_Construct(struct WONAPI::RoutingBadUser *,struct WONAPI::RoutingBadUser const &))
// TEMPLATE: WON 0x006c9660
// (public: class std::list<struct WONAPI::RoutingBadUser,class std::allocator<struct WONAPI::RoutingBadUser> >::iterator __thiscall std::list<struct WONA)
