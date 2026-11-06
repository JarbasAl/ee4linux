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
// FUNCTION: WON 0x006b5970
WONStatus RecvCRMsgOp::Virtual0C()
{
    mBuffer.Reset();
    mMessage=NULL;
    return Virtual10();
}
// FUNCTION: WON 0x006b59b0
WONStatus RecvCRMsgOp::Virtual10()
{
    for (;;) {
        mSocket->Fn2B1410(TimeLeft());
        unsigned char aByte;
        WONStatus aStatus=mSocket->Fn2B15E0(&aByte,1,NULL);
        if (aStatus==0) {
            mBuffer.AppendByte(aByte);
            if (aByte==10 && mBuffer.mLength>=2 && mBuffer.mData[mBuffer.mLength-2]==13) {
                mBuffer.mData[mBuffer.mLength-2]=0;
                mBuffer.SetSize(mBuffer.mLength-1);
                mMessage=mBuffer.ToByteBuffer(true);
                return 0;
            }
        } else if (aStatus==100001) {
            if (TimeLeft()==0) return 100001;
        } else return aStatus;
    }
}

}
