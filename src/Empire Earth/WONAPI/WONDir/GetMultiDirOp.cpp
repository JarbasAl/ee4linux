// Original-backed WONDir family; grouping provisional, no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {

// FUNCTION: WON 0x006f1520
void GetMultiDirOp::Init()
{
    mLengthFieldSize=4;
    mNumFailedRequests=0;
}
// FUNCTION: WON 0x6f1540
GetMultiDirOp::GetMultiDirOp(ServerContext* inContext)
    :ServerRequestOp(inContext),mCurRequest(NULL)
{
    Init();
}

// FUNCTION: WON 0x6f1790
void GetMultiDirOp::Virtual18()
{
    GetEntityRequestList::iterator i;
    i=mRequestList.begin();
    while(true) {
        if(i==mRequestList.end()) break;
        (*i)->Reset();
        ++i;
    }
}
// FUNCTION: WON 0x6f17c0
void GetMultiDirOp::AddRequest(GetEntityRequest* inRequest)
{
    mRequestList.insert(mRequestList.end(),inRequest);
}
// FUNCTION: WON 0x6f18f0
void GetMultiDirOp::AddPath(const std::wstring& inPath)
{
    GetEntityRequestPtr request=new GetEntityRequest;
    request->SetPath(inPath);
    request->SetFlags(mFlags);
    request->SetDataTypes(mDataTypes);
    AddRequest(request.get());
}
// FUNCTION: WON 0x6f1a30
WONStatus GetMultiDirOp::Virtual10()
{
    WriteBuffer message(mLengthFieldSize,true);
    message.AppendByte(5);
    message.AppendShort(2);
    message.AppendShort(113);
    message.AppendShort(0);
    message.AppendByte(static_cast<unsigned char>(mRequestList.size()));
    GetEntityRequestList::iterator i;
    i=mRequestList.begin();
    while(true) {
        if(!(i!=mRequestList.end())) break;
        (*i)->Pack(&message,GetEntityRequest::Pack_GetMultiDirOp);
        ++i;
    }
    mRequest=message.ToByteBuffer(true);
    mCurRequest=mRequestList.begin();
    return 100059;
}
// FUNCTION: WON 0x6f1b90
WONStatus GetMultiDirOp::Virtual14()
{
    if(mCurRequest==mRequestList.end()) {
        GetEntityRequestPtr request=new GetEntityRequest;
        return request->ParseMultiEntityReply(mResponse->data(),mResponse->length());
    }
    (*mCurRequest)->ParseMultiEntityReply(mResponse->data(),mResponse->length());
    GetEntityRequest* request=(*mCurRequest).get();
    if(request->GetStatus()!=100059) {
        if(request->GetStatus()!=0)
            ++mNumFailedRequests;
        ++mCurRequest;
    }
    if(mCurRequest==mRequestList.end()) {
        if(mNumFailedRequests==0)
            return 0;
        return mNumFailedRequests==static_cast<int>(mRequestList.size())?-1224:-1223;
    }
    return 100059;
}
} // WONAPI

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f1610
// (public: virtual void * __thiscall WONAPI::GetMultiDirOp::`scalar deleting destructor'(unsigned int)    [??_GGetMultiDirOp@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006f1630
// (public: virtual __thiscall WONAPI::GetMultiDirOp::~GetMultiDirOp(void)    [??1GetMultiDirOp@WONAPI@@UAE@XZ])
