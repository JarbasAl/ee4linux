// Native patch-server version validation operation. Source-only reconstruction.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x006d8c80
CheckValidVersionOp::CheckValidVersionOp(const std::string& inProduct,ServerContext* inContext)
    : DBProxyOp(inContext),mProductName(inProduct),mMsgType(1)
{
    Init();
}
// FUNCTION: WON 0x006d8d90
CheckValidVersionOp::~CheckValidVersionOp()
{
}
// FUNCTION: WON 0x006d8f60
WONStatus CheckValidVersionOp::Virtual14()
{
    WONStatus result=DBProxyOp::Virtual14();
    if (result<-2816 || (result>-2814 && result!=0))
        return result;
    if (mSubMessageReplyType!=mMsgType+1) return InvalidReplyHeader();
    ReadBuffer reply(mReplyData->data(),mReplyData->length());
    // Constructed and destroyed without being read.
    std::string unusedVersion;
    std::string unusedURL;
    reply.ReadString(mVersionTo);
    reply.ReadString(mVersionToDescriptionURL);
    int count=(unsigned short)reply.ReadShort();
    for (int i=0;i<count;++i) {
        unsigned short length=reply.ReadShort();
        SmartPtr<PatchData> patch=new PatchData;
        patch->SetFromVersion(mVersionData->GetVersion());
        patch->SetToVersion(mVersionTo);
        patch->SetIsActive(true);
        patch->SetConfigName(mVersionData->GetConfigName());
        ReadBuffer data(reply.ReadBytes(length),length);
        patch->ReadFromBuffer(data,false);
        mPatchDataList.push_back(patch);
    }
    return result;
}
// FUNCTION: WON 0x006d9250
void CheckValidVersionOp::Virtual04()
{
    mMessageType=1004;
    mSubMessageType=mMsgType;
    WriteBuffer request(0,true);
    const std::string& aProductName=mProductName;
    request.AppendString(aProductName);
    mVersionData->WriteToBuffer(request,0);
    request.AppendBool(mGetPatchList);
    mRequestData=request.ToByteBuffer(true);
    DBProxyOp::Virtual04();
}
// FUNCTION: WON 0x006d9380
void CheckValidVersionOp::Init()
{
    Virtual18();
    mLengthFieldSize=4;
    mVersionData=new VersionData;
    mVersionData->SetState(VersionState_BadState);
    mVersionTo="";
    mVersionToDescriptionURL="";
    mGetPatchList=true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d8d70
// (protected: virtual void * __thiscall WONAPI::CheckValidVersionOp::`scalar deleting destructor'(unsigned int)    [??_GCheckValidVersionOp@WONAPI@@MAEPA)
// TEMPLATE: WON 0x006dc810
// (protected: void __thiscall std::list<class WONAPI::SmartPtr<class WONAPI::PatchData>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::PatchDa)
