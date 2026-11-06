// Additional complete native HTTP document request behavior.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <time.h>
#include <list>
#include <map>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "LostDebug.h"
// FUNCTION: WON 0x006d0e70
static std::string& GetHost()
{
    static std::string host="www.sierra.com";
    return host;
}
// FUNCTION: WON 0x006d0f70
static std::string& GetPath()
{
    static std::string path="/motd/";
    return path;
}
namespace WONAPI {
// FUNCTION: WON 0x006cffb0
void GetHTTPDocumentOp::CheckHostAndPath(HTTPDocType inType)
{
    static bool checked=false;
    static CriticalSection crit;
    AutoCrit guard(crit);
    if (checked) return;
    checked=true;
    // The fopen results are discarded.
    if (inType==HTTPDocType_MOTD)
        fopen((WONAPICore::GetDefaultFileDirectory()+"_wonmotdpath.txt").c_str(),"r");
    else if (inType==HTTPDocType_TOU)
        fopen((WONAPICore::GetDefaultFileDirectory()+"_wontoupath.txt").c_str(),"r");
    else if (inType==HTTPDocType_ADV)
        fopen((WONAPICore::GetDefaultFileDirectory()+"_wonadvpath.txt").c_str(),"r");
    // LOST-DEBUG: none: exact only (the discarded fopen results suggest compiled-away code that read the files)
    EE_LOST_DEBUG(2);
}
// FUNCTION: WON 0x006d0810
bool GetHTTPDocumentOp::CallbackHook(AsyncOp* inOp,int inParam)
{
    OpFinished(static_cast<HTTPGetOp*>(inOp),inParam);
    if (mTracker.mMaps.mEntries.empty()) DoFinish();
    return true;
}
// FUNCTION: WON 0x006d0840
void GetHTTPDocumentOp::TryHTTPOp(HTTPDocOwner inOwner,bool inForceDefault)
{
    std::string document;
    std::string config=StringToLowerCase(mPath);
    if (config=="en") config.erase();
    if (config.empty() || inForceDefault) {
        mRetried[inOwner]=true;
        if (mDocumentType==HTTPDocType_MOTD) document="motd.txt";
        else if (mDocumentType==HTTPDocType_TOU) document="tou.txt";
        else if (mDocumentType==HTTPDocType_ADV) document="adv.txt";
    } else {
        if (mDocumentType==HTTPDocType_MOTD) document="motd_"+config+".txt";
        else if (mDocumentType==HTTPDocType_TOU) document="tou_"+config+".txt";
        else if (mDocumentType==HTTPDocType_ADV) document="adv_"+config+".txt";
    }
    std::string path;
    if (inOwner==HTTPDocOwner_Sys) path=GetPath()+"sys/";
    else if (inOwner==HTTPDocOwner_Game) path=GetPath()+StringToLowerCase(mProduct)+"/";
    SmartPtr<HTTPGetOp> op=new HTTPGetOp(GetHost(),path+document);
    op->SetDoRangeResume(false);
    op->SetLocalPath(mDocuments[inOwner]);
    if (mRunMode==1) {
        Track(op.get(),inOwner);
        if (mSession.get()!=NULL) mSession->AddOp(op.get());
        else op->Run(-1);
        return;
    }
    op->Fn29F500(2,TimeLeft());
    OpFinished(op.get(),inOwner);
}
}

// TEMPLATE: WON 0x006d0f20
// (GetHost static-local atexit destructor)
// TEMPLATE: WON 0x006d1020
// (GetPath static-local atexit destructor)
