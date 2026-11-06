// Browser scroller attachment, anchor/history navigation and relative link context.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONURL.h"
using namespace std;
namespace WONAPI {

// FUNCTION: WON 0x007FB1A0
void BrowserComponent::SetScroller(Scroller *theScroller)
{
	mScroller = theScroller;
	if(theScroller->GetScrollArea()!=mHTMLView.get())
		theScroller->SetScrollArea(mHTMLView.get());

	Fn2F6140(theScroller,3,this,0,0,0,0,0); //actual CLI_SameSize=3
	Fn2F6A60(theScroller);
}

// FUNCTION: WON 0x007FC540
void BrowserComponent::GotoAnchor()
{
	if(mVertOffset>=0) // this was a back or forward traversal and we were somewhere else on the page
	{
		mHTMLView->SetOffsets(0,mVertOffset);
		return;
	}

	mHTMLView->SetOffsets(0,0);
	if(mCurAnchor.length()==0)
		return;

	mHTMLView->GotoAnchor(mCurAnchor);
}

// FUNCTION: WON 0x007FC7C0
void BrowserComponent::RememberVertOffset()
{
	if(mCurPage>=0 && mCurPage<mPageVector.size()) // remember the current vert offset in this page
		mPageVector[mCurPage].mVertOffset = mHTMLView->mVertOffset;
}

// FUNCTION: WON 0x007FCF60
void BrowserComponent::Back()
{
	if(!mPageVector.empty() && mCurPage>0)
	{
		RememberVertOffset();

		mCurPage--;
		PageLocation &aLoc = mPageVector[mCurPage];
		Go(aLoc.mLocation,false,true,aLoc.mVertOffset);
	}
}

// FUNCTION: WON 0x007FCFC0
void BrowserComponent::Forward()
{
	if(!mPageVector.empty() && mCurPage<mPageVector.size()-1)
	{
		RememberVertOffset();

		mCurPage++;
		PageLocation &aLoc = mPageVector[mCurPage];
		Go(aLoc.mLocation,false,true,aLoc.mVertOffset);
	}
}

// FUNCTION: WON 0x007FD030
void BrowserComponent::GetLinkPath(const std::string &theLink, std::string &thePath)
{
	URL aLink(theLink);
	string aStr = mCurPath;
	aLink.CalcRelativeTo(aStr);
	aLink.GetEntire(thePath);
}
}

namespace WONAPI {
// Native3fb230/27 conditional accessor, support0.
bool BrowserComponent::StopButtonEnabled()
{
    return mHTTPSession.get()!=NULL && mNumPendingOps>0;
}
// Native3fb250/45 and3fb280/63 history accessors, support0.
// FUNCTION: WON 0x007fb250
bool BrowserComponent::BackButtonEnabled()
{
    return !mPageVector.empty() && mCurPage>0;
}
// FUNCTION: WON 0x007fb280
bool BrowserComponent::ForwardButtonEnabled()
{
    return !mPageVector.empty() && mCurPage<mPageVector.size()-1;
}
// Native3fc480/81 clear/state reset, support0.
// FUNCTION: WON 0x007fc480
void BrowserComponent::Reset()
{
    mPageVector.clear();
    mCurPage=0;
}
// Native3fc4e0/45 conditional forwarding, support0.
// FUNCTION: WON 0x007fc4e0
void BrowserComponent::Refresh()
{
    if(!mPageVector.empty()) Go(mCurLocation,true,true);
}
// Native3fc510/37 kill/state/event orchestration, conservatively support0.
// FUNCTION: WON 0x007fc510
void BrowserComponent::Stop()
{
    mHTTPSession->KillAll();
    mNumPendingOps=0;
    Fn2F5380(ComponentEvent_BrowserButtonChange,0);
}
}

#include "ee/wonapi/WONLobbyGUICompletion.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONLobbyImageButton.h"

// Actual standalone path comparison: case-sensitive code units, slash directions equivalent.
// FUNCTION: WON 0x007fced0
static bool PathsAreEqual(const WONAPI::GUIString& first,const WONAPI::GUIString& second)
{
    if(first.length()!=second.length()) return false;
    for(int i=0;i<first.length();++i)
    {
        if(first[i]!=second[i])
        {
            int a=first[i],b=second[i];
            bool slashA=a=='\\' || a=='/';
            bool slashB=b=='\\' || b=='/';
            if(!slashA || !slashB) return false;
        }
    }
    return true;
}

namespace WONAPI {
// FUNCTION: WON 0x007fc590
void BrowserComponent::Go(HTMLDocument* document)
{
    if(document==NULL) return;
    Stop();
    mHTMLDocument=document;
    SmartPtr<MultiLineElement> element=new MultiLineElement;
    if(!DownloadImages())
    {
        HTMLDisplayGen generator;
        generator.SetFontSize(mFontSize);
        generator.SetNoBackground(mHaveBackground);
        generator.Generate(mHTMLDocument.get(),element.get(),mHTMLView.get());
        mHTMLView->SetContents(element.get());
        mHTMLView->SetOffsets(0,0);
    }
}
// FUNCTION: WON 0x007fc800
void BrowserComponent::Go(const GUIString& location,bool force,bool backTraversal,int vertOffset)
{
    Stop();
    int pound=location.find('#',0);
    GUIString path,anchor;
    if(pound>=0)
    {
        path=location.substr(0,(unsigned short)pound);
        anchor=location.substr((unsigned short)(pound+1));
    }
    else path=location;
    mCurAnchor=anchor;
    if(!backTraversal)
    {
        RememberVertOffset();
        if(!mPageVector.empty() && mCurPage<mPageVector.size()+1)
            mPageVector.erase(mPageVector.begin()+mCurPage+1,mPageVector.end());
        if(mPageVector.empty() || location!=mPageVector.back().mLocation)
        {
            mPageVector.push_back(PageLocation(location,0));
            if(mPageVector.size()>100)
                mPageVector.erase(mPageVector.begin(),mPageVector.begin()+mPageVector.size()-100);
        }
        mCurPage=mPageVector.size()-1;
    }
    mCurLocation=location;
    mVertOffset=vertOffset;
    Fn2F5380(ComponentEvent_BrowserLocationChange,0);
    Fn2F5380(ComponentEvent_BrowserButtonChange,0);
    if(PathsAreEqual(path,mCurPath) && !force)
    {
        GotoAnchor();
        return;
    }
    if(path.length()==0) return;
    if(mLaunchBrowserOnLink)
    {
        Browse(std::string(location));
        return;
    }
    URL pathURL(path);
    mCurPath=path;
    if(pathURL.GetType()==URLType_File)
    {
        mHTMLDocumentPath=pathURL.GetPathAsLocalFilePath();
        ParseDocument();
    }
    else if(pathURL.GetType()==URLType_HTTP)
    {
        SmartPtr<HTTPGetOp> op=new HTTPGetOp(pathURL.GetAddress(),pathURL.GetPath());
        op->SetHTTPCache(mHTTPCache.get());
        op->mCompletion=new OpGUICompletion(new ParamCompletion<AsyncOpPtr,SmartPtr<RefCount> >(DownloadDocumentCompletion,this));
        mHTTPSession->AddOp(op.get());
        ++mNumPendingOps;
        mHTMLView->SetContents(new MultiLineElement);
    }
}
}

#include "ee/wonapi/WONLobbyHTMLParser.h"

namespace WONAPI {
// FUNCTION: WON 0x007fc0d0
void BrowserComponent::ParseDocument()
{
    if(mHTMLDocumentPath.empty())
    {
        Stop();
        return;
    }
    HTMLParser parser;
    parser.SetDocument(new HTMLDocument(mCurPath));
    parser.ParseFromFile(mHTMLDocumentPath.c_str());
    mHTMLDocument=parser.GetDocument();
    SmartPtr<MultiLineElement> element=new MultiLineElement;
    if(!DownloadImages())
    {
        HTMLDisplayGen generator;
        generator.SetFontSize(mFontSize);
        mHTMLDocument->DecodeImages(true);
        generator.SetNoBackground(mHaveBackground);
        generator.Generate(mHTMLDocument.get(),element.get(),mHTMLView.get());
        mHTMLView->SetContents(element.get());
    }
    GotoAnchor();
    mCurLocation=mCurPath;
    Fn2F5380(ComponentEvent_BrowserLocationChange,0);
}
}

// Browser hyperlink activation/hover event handling; borrowed actual event only.
namespace WONAPI {
// FUNCTION: WON 0x007fd350
void BrowserComponent::HandleComponentEvent(ComponentEvent* event)
{
    switch(event->mType)
    {
    case ComponentEvent_HyperLinkActivated:
        {
            HyperLinkEvent* linkEvent=static_cast<HyperLinkEvent*>(event);
            const std::string& link=linkEvent->mLinkParam->mLinkParam;
            std::string path;
            GetLinkPath(link,path);
            int keyMod=GetKeyMod();
            if((keyMod & KEYMOD_SHIFT) ||
               (linkEvent->mLinkParam->mLaunchBrowser && !(keyMod & KEYMOD_ALT)))
                Browse(path);
            else Go(path);
        }
        return;
    case ComponentEvent_HyperLinkOver:
        {
            HyperLinkEvent* linkEvent=static_cast<HyperLinkEvent*>(event);
            if(linkEvent->mLinkParam.get()!=NULL)
            {
                const std::string& link=linkEvent->mLinkParam->mLinkParam;
                std::string path;
                GetLinkPath(link,path);
                SetStatus(path);
            }
            else SetStatus("");
        }
        return;
    }
}
}
namespace WONAPI {
// FUNCTION: WON 0x007fd1f0
bool BrowserComponent::FindStr(const GUIString& text,bool next,bool caseSensitive)
{
    return mHTMLView->FindStr(text,next,caseSensitive);
}
}

#include "ee/wonapi/WONLobbyMSExtraConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x007fb0e0
void BrowserComponent::Virtual6C()
{
    if(mScroller.get()==NULL)
    {
        SmartPtr<MSScrollContainer> scroller=new MSScrollContainer(mHTMLView.get());
        scroller->SetScrollbarConditions(ScrollbarCondition_Conditional,ScrollbarCondition_Conditional);
        SetScroller(scroller.get());
    }
    Container::Virtual6C();
}
}

namespace WONAPI {
// FUNCTION: WON 0x007fad10
BrowserComponent::BrowserComponent()
{
    mLaunchBrowserOnLink=false;
    mFontSize=HTMLFontSize_Medium;
    mHTTPSession=new HTTPSession;
    mHTTPCache=HTTPCache::GetGlobalCache();
    mCurPage=0;
    mNumPendingOps=0;
    mVertOffset=-1;
    mHTMLView=new Table;
    mHaveBackground=false;
}
// FUNCTION: WON 0x007faf30
BrowserComponent::~BrowserComponent()
{
}
// FUNCTION: WON 0x007fd210
// Target retains the older SDK SetTransparent contract. The cached newer
// SetBackground/RemoveBackground replacement is not the target API.
void BrowserComponent::SetTransparent(bool transparent)
{
    mHaveBackground=transparent;
    if(mHaveBackground)
        mHTMLView->mBackground=Background(-1);
    mHTMLView->SetComponentFlags(0x100,mHaveBackground);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007faf10
// (public: virtual void * __thiscall WONAPI::BrowserComponent::`scalar deleting destructor'(unsigned int)    [??_GBrowserComponent@WONAPI@@UAEPAXI@Z])
