// RECONSTRUCTION: new native WONAPI class family; exact original filename/grouping unproved.
// Unmatched code/support remains original-carved; no runtime authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <stdio.h>
#include <ctype.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Original coherent HTTP document class; exact source filename/grouping remains provisional.
// Normal profile hypothesis -O2 -MD -GR -GX -DEE_DINKUM_SXL233; no runtime claim.

// FUNCTION: WON 0x006d0170
GetHTTPDocumentOp::GetHTTPDocumentOp(const std::string& inProduct, HTTPDocType inType)
{
    mProduct = inProduct;
    SetDocType(inType);
}

// FUNCTION: WON 0x006d0340
void GetHTTPDocumentOp::SetDocType(HTTPDocType inType)
{
    mDocumentType = inType;
    CheckHostAndPath(inType);
    if (mDocumentType == HTTPDocType_MOTD) {
        mDocuments[HTTPDocOwner_Sys] = WONAPICore::GetDefaultFileDirectory() + "_wonsysmotd.txt";
        mDocuments[HTTPDocOwner_Game] = WONAPICore::GetDefaultFileDirectory() + "_won" + mProduct + "motd.txt";
    } else if (mDocumentType == HTTPDocType_TOU) {
        mDocuments[HTTPDocOwner_Sys] = WONAPICore::GetDefaultFileDirectory() + "_wonsystou.txt";
        mDocuments[HTTPDocOwner_Game] = WONAPICore::GetDefaultFileDirectory() + "_won" + mProduct + "tou.txt";
    } else if (mDocumentType == HTTPDocType_ADV) {
        mDocuments[HTTPDocOwner_Sys] = WONAPICore::GetDefaultFileDirectory() + "_wonsysadv.txt";
        mDocuments[HTTPDocOwner_Game] = WONAPICore::GetDefaultFileDirectory() + "_won" + mProduct + "adv.txt";
    }
}

// FUNCTION: WON 0x006d06c0
void GetHTTPDocumentOp::DoFinish()
{
    if (mDocumentStatus[HTTPDocOwner_Sys] != 0 && mDocumentType != HTTPDocType_ADV)
        Fn29F220(100098);
    else if (mDocumentStatus[HTTPDocOwner_Game] != 0)
        Fn29F220(100099);
    else
        Fn29F220(0);
}

// FUNCTION: WON 0x006d0700
void GetHTTPDocumentOp::OpFinished(HTTPGetOp* inOp, int inOwner)
{
    if (!mRetried[inOwner] && (inOp->mStatus == 100094 || inOp->mHTTPStatus == 404)) {
        TryHTTPOp(static_cast<HTTPDocOwner>(inOwner),true);
        return;
    }
    mDocumentStatus[inOwner] = inOp->mStatus;
    mHTTPStatus[inOwner] = inOp->mHTTPStatus;
    mLastModified[inOwner] = inOp->GetLastModifiedTime();
    if (inOp->mStatus != 0) return;
    mIsNew[inOwner] = inOp->mContentIsNew;
    if (mDocumentType != HTTPDocType_MOTD) return;
    FILE* file = fopen(mDocuments[inOwner].c_str(),"r");
    if (file != NULL) {
        int first = fgetc(file);
        if (first == EOF) {
            mIsNew[inOwner] = false;
            fclose(file);
            return;
        }
        if (toupper(first & 0xff) != '1') mIsNew[inOwner] = true;
    }
    // The native tail closes even a NULL fopen result; preserve that failure path.
    fclose(file);
}

// FUNCTION: WON 0x006d1070
void GetHTTPDocumentOp::Virtual04()
{
    for (int i=0; i<HTTPDocOwner_Count; ++i) {
        mIsNew[i] = false;
        mDocumentStatus[i] = 100004;
        mHTTPStatus[i] = 0;
        mLastModified[i] = 0;
        mRetried[i] = false;
    }
    if (mDocumentType == HTTPDocType_ADV) TryHTTPOp(HTTPDocOwner_Game,false);
    else for (int owner=0; owner<HTTPDocOwner_Count; ++owner)
        TryHTTPOp(static_cast<HTTPDocOwner>(owner),false);
    if (mRunMode != 1) DoFinish();
}

// FUNCTION: WON 0x006d10e0
ByteBufferPtr GetHTTPDocumentOp::GetDocument(HTTPDocOwner inOwner) const
{
    FILE* file = NULL;
    if (!mDocuments[inOwner].empty()) file = fopen(mDocuments[inOwner].c_str(), "r");
    if (file == NULL) return new ByteBuffer("");
    WriteBuffer buffer(0,true);
    if (mDocumentType == HTTPDocType_MOTD) {
        int first = fgetc(file);
        if (first == '<') ungetc(first,file);
    }
    char bytes[1024];
    while (!feof(file)) {
        int count = fread(bytes,1,1024,file);
        if (count>0) buffer.AppendBytes(bytes,count);
    }
    buffer.AppendByte(0);
    fclose(file);
    return buffer.ToByteBuffer(true);
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006cfe30
// (public: virtual void * __thiscall WONAPI::AsyncOpWithTracker::`scalar deleting destructor'(unsigned int)    [??_GAsyncOpWithTracker@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006d0320
// (public: virtual void * __thiscall WONAPI::GetHTTPDocumentOp::`scalar deleting destructor'(unsigned int)    [??_GGetHTTPDocumentOp@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006d9d20
// (public: __thiscall WONAPI::AsyncOpTrackerMaps::AsyncOpTrackerMaps(bool,bool))
// TEMPLATE: WON 0x00769650
// (public: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,struct std::pair<class WONAPI::SmartPtr<class WONAPI::AsyncOp> const ,class std)
