// Browser document/image download completion and asynchronous image scheduling.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONURL.h"
#include "ee/wonapi/WONLobbyGUICompletion.h"
namespace WONAPI {

// Native3fb2c0/92 status assignment and event forwarding, support0.
// FUNCTION: WON 0x007fb2c0
void BrowserComponent::SetStatus(const GUIString& status)
{
    mCurStatus=status;
    Fn2F5380(ComponentEvent_BrowserStatusChange,0);
}

// FUNCTION: WON 0x007fb320
void BrowserComponent::DownloadDocumentCompletion(AsyncOpPtr op,SmartPtr<RefCount> that)
{
    BrowserComponent* browser=(BrowserComponent*)that.get();
    HTTPGetOp* request=(HTTPGetOp*)op.get();
    if(request->mB39)
        return;
    char status[512];
    sprintf(status,"Got %s%s - %s %d",request->GetHost().c_str(),
        request->GetRemotePath().c_str(),WONStatusToString(request->GetStatus()),request->GetHTTPStatus());
    browser->SetStatus(status);
    --browser->mNumPendingOps;
    if(request->GetStatus()!=0) {
        browser->Fn2F5380(ComponentEvent_BrowserButtonChange,0);
        browser->SetStatus(status);
        return;
    }
    browser->mHTMLDocumentPath=request->GetLocalPath();
    browser->mCurPath="http://"+request->GetEndingHost()+request->GetEndingPath();
    browser->ParseDocument();
}

// FUNCTION: WON 0x007fb6a0
void BrowserComponent::FinishImages()
{
    mHTMLDocument->DecodeImages(false);
    SmartPtr<MultiLineElement> element=new MultiLineElement;
    HTMLDisplayGen generator;
    generator.SetFontSize(mFontSize);
    generator.SetNoBackground(mHaveBackground);
    generator.Generate(mHTMLDocument.get(),element.get(),mHTMLView.get());
    mHTMLView->SetContents(element.get());
    SetStatus("");
    mHTTPSession->KillAll();
    mNumPendingOps=0;
    Fn2F5380(ComponentEvent_BrowserButtonChange,0);
}

// FUNCTION: WON 0x007fb930
void BrowserComponent::DownloadImageCompletion(AsyncOpPtr op,ImageParamPtr param)
{
    HTTPGetOp* request=(HTTPGetOp*)op.get();
    char status[1024];
    if(request->mB39) {
        sprintf(status,"Killed %s%s.",request->GetHost().c_str(),request->GetRemotePath().c_str());
        param->mBrowser->SetStatus(status);
        return;
    }
    int httpStatus=request->GetHTTPStatus();
    HTMLDocument::LoadedImageStruct* image=param->mImageStruct;
    if(request->Succeeded()) {
        if(image->mImage.get()!=NULL && httpStatus>=200 && httpStatus<300)
            image->mImage=NULL;
        if(image->mImage.get()==NULL)
            param->mBrowser->mHTMLDocument->DoDecodeImage(param->mImageStruct,
                param->mBrowser->GetRoot(),request->GetLocalPath());
    }
    sprintf(status,"Got %s%s - %s %d",request->GetHost().c_str(),request->GetRemotePath().c_str(),
        WONStatusToString(request->GetStatus()),httpStatus);
    param->mBrowser->SetStatus(status);
    int pending=--param->mBrowser->mNumPendingOps;
    if(pending<=0)
        param->mBrowser->FinishImages();
}

// FUNCTION: WON 0x007fbc10
bool BrowserComponent::DownloadImages()
{
    Window* window=GetRoot();
    HTMLDocument::LoadedImageMap& images=mHTMLDocument->GetLoadedImageMap();
    HTMLDocument::LoadedImageMap::iterator i=images.begin();
    while(i!=images.end()) {
        HTMLDocument::LoadedImageStruct& image=i->second;
        URL path(i->first);
        URL base(mCurPath);
        path.CalcRelativeTo(base);
        if(path.GetType()==URLType_File)
            mHTMLDocument->DoDecodeImage(&image,window,path.GetPathAsLocalFilePath());
        else if(path.GetType()==URLType_HTTP) {
            SmartPtr<HTTPGetOp> request=new HTTPGetOp(path.GetAddress(),path.GetPath());
            SmartPtr<HTTPCacheEntry> entry=mHTTPCache->GetEntry(path.GetAddress()+path.GetPath());
            if(entry->IsValid())
                mHTMLDocument->DoDecodeImage(&image,window,entry->GetLocalPath());
            request->SetHTTPCache(mHTTPCache.get());
            request->mCompletion=new OpGUICompletion(new ParamCompletion<AsyncOpPtr,ImageParamPtr>(
                DownloadImageCompletion,new ImageParam(this,&image)));
            mHTTPSession->AddOp(request.get());
            ++mNumPendingOps;
        }
        ++i;
    }
    if(mNumPendingOps<=0) {
        FinishImages();
        return true;
    }
    return false;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007fd6f0
// (public: __thiscall WONAPI::ParamCompletion<class WONAPI::SmartPtr<class WONAPI::AsyncOp>,class WONAPI::SmartPtr<struct WONAPI::BrowserComponent::Image)
// TEMPLATE: WON 0x007fd9c0
// (protected: static struct std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::HTMLDocument::LoadedImageSt)
// TEMPLATE: WON 0x007fd750
// TEMPLATE: WON 0x007fd820
// TEMPLATE: WON 0x007fd8a0
// TEMPLATE: WON 0x007fd9a0
// TEMPLATE: WON 0x007fd9e0
