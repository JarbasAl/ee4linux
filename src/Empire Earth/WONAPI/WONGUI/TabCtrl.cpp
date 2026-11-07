// Original-backed WONGUI TabBar/TabCtrl cohesive source family; original .cpp grouping unproved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// Native308160..3081cf111B. Base/vector setup and defaults, conservative support0.
TabBar::TabBar(TabCtrl* inCtrl) : mCurTab(NULL),mTabCtrl(inCtrl) { mOverlap=4; }
// Native308280..30829218B. Borrowed null-guarded getter, support0.
// FUNCTION: WON 0x00708280
int TabBar::Fn308280() { return mCurTab==NULL ? -1 : mCurTab->mControlId; }
// Native3082a0..3082f7,87B.
// FUNCTION: WON 0x007082a0
void TabBar::PositionTab(Button* inTab)
{
    int x=0;
    int id=inTab->mControlId;
    if(id>0 && id<=mTabVector.size()) {
        Button* previous=mTabVector[id-1].get();
        x=previous->Right()-mOverlap;
    }
    inTab->Fn2F4B70(x,0);
}
// Native308300..3083f9,249B. Owning conversion temporary ends before PositionTab.
// FUNCTION: WON 0x00708300
void TabBar::AddTab(Button* inTab)
{
    inTab->Fn2F3370(0x40,true);
    inTab->mControlId=mTabVector.size();
    mTabVector.push_back(inTab);
    PositionTab(inTab);
    Fn2F4B90(inTab->Left()+inTab->Width(),Height());
    AddChildEx(inTab,0);
}
// Native308400..3084fc,252B. Re-read current member after SetCheck callback.
// FUNCTION: WON 0x00708400
void TabBar::SetCurTab(int inId,bool inRequestFocus)
{
    if(inId<0 || inId>=mTabVector.size()) return;
    int oldId=Fn308280();
    if(oldId==inId) {
        if(inRequestFocus && mCurTab!=NULL) mCurTab->Fn2F5890();
        return;
    }
    Button* tab=mTabVector[inId].get();
    if(tab->IsDisabled()) return;
    if(mCurTab!=NULL) {
        mCurTab->SetCheck(false,false);
        if(mCurTab->mParent!=this) {
            AddChildEx(mCurTab,int(mTabVector.size())-oldId-1);
            PositionTab(mCurTab);
        }
    }
    mCurTab=tab;
    tab->SetCheck(true,false);
    mTabCtrl->mUserHasInteracted=true;
    mTabCtrl->NotifyTabChange(inRequestFocus);
}
// Native308500..308557,87B. Current vector/count reload each iteration.
// FUNCTION: WON 0x00708500
void TabBar::SetCurTabByName(const GUIString& inName,bool inRequestFocus)
{
    for(int i=0;i<mTabVector.size();++i) {
        if(mTabVector[i]->GetText()==inName) {
            SetCurTab(i,inRequestFocus);
            return;
        }
    }
}
// Native308560..3085c7,103B. No new bounds recheck after nested SetCurTab.
// FUNCTION: WON 0x00708560
void TabBar::EnableTab(int inId,bool inEnabled)
{
    if(inId<0 || inId>=mTabVector.size()) return;
    int oldId=Fn308280();
    if(oldId==inId && !inEnabled)
        SetCurTab(oldId>0 ? oldId-1 : oldId+1,true);
    Button* tab=mTabVector[inId].get();
    if(tab!=NULL) tab->SetEnabled(inEnabled);
}
// Native3085d0..30860a,58B. Invalid or null tabs are disabled.
// FUNCTION: WON 0x007085d0
bool TabBar::Disabled(int inId)
{
    if(inId>=0 && inId<mTabVector.size())
    {
        Button* tab=mTabVector[inId].get();
        if(tab!=NULL)
            return tab->IsDisabled()?true:false;
    }
    return true;
}
// Native308610..30866a,90B. Selected sender must equal current indexed Button.
// FUNCTION: WON 0x00708610
void TabBar::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==15) {
        int id=inEvent->GetControlId();
        if(id>=0 && id<mTabVector.size() && inEvent->mComponent==mTabVector[id].get()) {
            SetCurTab(id,true);
            return;
        }
    }
    Component::HandleComponentEvent(inEvent);
}

// Native308960..3089d3,115B. Padding and current selection deliberately not copied.
// FUNCTION: WON 0x00708960
void TabCtrl::CopyAttributes(TabCtrl* inFrom)
{
    mBottomTabLeftOffset=inFrom->mBottomTabLeftOffset;
    mBottomTabTopOffset=inFrom->mBottomTabTopOffset;
    mTabBarHeight=inFrom->mTabBarHeight;
    mArrowsOnLeft=inFrom->mArrowsOnLeft;
    mTabBarOnRight=inFrom->mTabBarOnRight;
    mTabBarOnBottom=inFrom->mTabBarOnBottom;
    mTabBarOverlap=inFrom->mTabBarOverlap;
    mBottomTabBar->mOverlap=inFrom->mBottomTabBar->mOverlap;
}
// Native3089e0..308a2a,74B.
// FUNCTION: WON 0x007089e0
void TabCtrl::Virtual0C(int& outWidth,int& outHeight)
{
    Container::Virtual0C(outWidth,outHeight);
    outWidth+=mRightPad+mLeftPad;
    outHeight+=mTabBarHeight+mBottomPad+mTopPad;
}
// Native308a30/13B and308a40/13B are simple field setters: support0.
// FUNCTION: WON 0x00708a30
void TabCtrl::SetTabBarOnRight(bool inFlag) { mTabBarOnRight=inFlag; }
// FUNCTION: WON 0x00708a40
void TabCtrl::SetArrowsOnLeft(bool inFlag) { mArrowsOnLeft=inFlag; }
// Native308a50..308b15,197B. Current arrow members reload after each call.
// FUNCTION: WON 0x00708a50
void TabCtrl::SetTabArrows(Button* inLeft,Button* inRight)
{
    mTabBarLeft=inLeft;
    mTabBarRight=inRight;
    mTabBarLeft->SetComponentFlags(0x1000,true);
    mTabBarRight->SetComponentFlags(0x1000,true);
    mTabBarLeft->Fn2F3370(2,true);
    mTabBarRight->Fn2F3370(2,true);
    Fn2F6A60(mTabBarLeft.get());
    Fn2F6A60(mTabBarRight.get());
}
// Native308b20..308bec,204B. New child snapshot survives old visibility callback.
// FUNCTION: WON 0x00708b20
void TabCtrl::NotifyTabChange(bool inRequestFocus)
{
    Button* button=mBottomTabBar->mCurTab;
    if(button==NULL) return;
    mTopTabBar->Fn2F6A60(button);
    int id=mBottomTabBar->Fn308280();
    if(id<0 || id>=mTabVector.size()) return;
    Container* selected=mTabVector[id];
    if(selected==mCurTab) return;
    if(mCurTab!=NULL) mCurTab->SetVisible(false);
    mCurTab=selected;
    PositionTabContainer(selected);
    mCurTab->SetVisible(true);
    BringToTop(mCurTab);
    if(inRequestFocus) mCurTab->Fn2F5890();
    KeepTabButtonInView();
    Invalidate();
    Fn2F5380(29,0);
}
// Native308bf0..308c39,73B. Native JS uses flags from left-position ADD,
// across intervening MOV/LEA, so first condition is left<0, not right<0.
// FUNCTION: WON 0x00708bf0
void TabCtrl::KeepTabButtonInView()
{
    Button* button=mBottomTabBar->mCurTab;
    if(button==NULL) return;
    int left=mBottomTabBar->Left()+button->Left();
    int width=button->Width();
    int right=left+width;
    if(left>=0) {
        int limit=mTopTabBarContainer->Width();
        if(width<=limit) {
            if(right>limit) AdjustTabBarScroll(limit-right);
            return;
        }
    }
    AdjustTabBarScroll(-left);
}
// Native308c40..308ca6,102B. Layout reference and parent fields reload after SetPos.
// FUNCTION: WON 0x00708c40
void TabCtrl::PositionTabContainer(Container* inContainer)
{
    if(inContainer==NULL) return;
    inContainer->Fn2F4B70(mLeftPad,mContainerLayout->Top()+mTopPad);
    inContainer->Fn2F4B90(Width()-mRightPad-mLeftPad,
        mContainerLayout->Height()-mBottomPad-mTopPad);
}
// Native308cb0..308cd6,38B. Initial selection precedes interaction flag reset.
// FUNCTION: WON 0x00708cb0
void TabCtrl::Virtual6C()
{
    Container::Virtual6C();
    if(mCurTab==NULL) Fn309350(0,false);
    mUserHasInteracted=false;
}
// Native308ce0..308d37,87B.
// FUNCTION: WON 0x00708ce0
void TabCtrl::CalcContainerLayout()
{
    int top=0;
    int bottom=Height();
    if(mTabBarOnBottom) bottom=mTopTabBarContainer->Top()+mTabBarOverlap;
    else top=mTopTabBarContainer->Bottom()-mTabBarOverlap;
    mContainerLayout->Fn2F4BF0(0,top,Width(),bottom-top);
}
// Native308d40..308dbf,127B. Fresh left arrow geometry for right positioning.
// FUNCTION: WON 0x00708d40
void TabCtrl::PositionArrows()
{
    Button* left=mTabBarLeft.get();
    if(left==NULL || mTabBarRight.get()==NULL) return;
    int x=0;
    if(!mArrowsOnLeft) x=Width()-mTabBarRight->Width()-left->Width();
    int y;
    if(mTabBarOnBottom) y=mContainerLayout->Bottom();
    else y=mContainerLayout->Top()-left->Height();
    left->Fn2F4B70(x,y);
    mTabBarRight->Fn2F4B70(mTabBarLeft->Right(),y);
}
// Native308dc0..308e76,182B.
// FUNCTION: WON 0x00708dc0
void TabCtrl::VertPositionTabBar()
{
    mBottomTabBar->Fn2F4B90(mBottomTabBar->Width(),
        mTabBarHeight-mTabBarOverlap-mBottomTabTopOffset);
    mBottomTabBarContainer->Fn2F4B90(mBottomTabBarContainer->Width(),mBottomTabBar->Height());
    mTopTabBar->Fn2F4B90(mTopTabBar->Width(),mTabBarHeight);
    mTopTabBarContainer->Fn2F4B90(mTopTabBarContainer->Width(),mTabBarHeight);
    int y=0;
    int offset=mBottomTabTopOffset;
    if(mTabBarOnBottom) { y=Height()-mBottomTabBarContainer->Height(); offset=-offset; }
    mTopTabBarContainer->Fn2F4B70(mTopTabBarContainer->Left(),y);
    mBottomTabBarContainer->Fn2F4B70(mBottomTabBarContainer->Left(),offset+y);
}
// Native308e80..308f4f,207B.
// FUNCTION: WON 0x00708e80
void TabCtrl::HorzPositionTabBar()
{
    int left=0;
    int right=Width();
    if(NeedScrollButtons()) {
        if(mArrowsOnLeft) {
            Button* arrow=mTabBarRight.get();
            if(arrow!=NULL && arrow->IsVisible()) left=arrow->Right();
        } else {
            Button* arrow=mTabBarLeft.get();
            if(arrow!=NULL && arrow->IsVisible()) right=arrow->Left()-1;
        }
    } else if(mTabBarOnRight) left=Width()-mBottomTabBar->Width()-mBottomTabLeftOffset;
    mTopTabBarContainer->Fn2F4B70(left,mTopTabBarContainer->Top());
    mTopTabBarContainer->Fn2F4B90(right-left,mTopTabBarContainer->Height());
    mBottomTabBarContainer->Fn2F4B70(mBottomTabLeftOffset+left,mBottomTabBarContainer->Top());
    mBottomTabBarContainer->Fn2F4B90(right-mBottomTabLeftOffset-left,mBottomTabBarContainer->Height());
}
// Native308f50..308fe3,147B. All resize/current-reference ordering preserved.
// FUNCTION: WON 0x00708f50
void TabCtrl::Virtual08()
{
    if(mTabBarLeft.get()!=NULL && mTabBarRight.get()!=NULL) {
        bool need=NeedScrollButtons();
        mTabBarLeft->SetVisible(need);
        mTabBarRight->SetVisible(need);
    }
    VertPositionTabBar();
    CalcContainerLayout();
    PositionArrows();
    HorzPositionTabBar();
    PositionTabContainer(mCurTab);
    AdjustTabBarScroll(0);
    if(!mUserHasInteracted) KeepTabButtonInView();
    Container::Virtual08();
}
// Native308ff0..309006,22B. Simple width predicate support0.
// FUNCTION: WON 0x00708ff0
bool TabCtrl::NeedScrollButtons() { return mBottomTabBar->Width()>Width(); }
// Native309010..3090dc,204B. Enable flags are untouched on the fit branch.
// FUNCTION: WON 0x00709010
void TabCtrl::AdjustTabBarScroll(int inAmount)
{
    int oldX=mBottomTabBar->Left();
    int x=oldX+inAmount;
    int width=mBottomTabBar->Width();
    int containerWidth=mBottomTabBarContainer->Width();
    if(containerWidth>=width) x=0;
    else {
        bool enableLeft=true,enableRight=true;
        if(x+width<containerWidth-5) { enableRight=false; x=containerWidth-width-5; }
        if(x>=0) { enableLeft=false; x=0; }
        if(mTabBarLeft.get()!=NULL && mTabBarRight.get()!=NULL) {
            mTabBarLeft->SetEnabled(enableLeft);
            mTabBarRight->SetEnabled(enableRight);
        }
    }
    if(x!=oldX) {
        mBottomTabBar->Fn2F4B70(x,mBottomTabBar->Top());
        mTopTabBar->Fn2F4B70(x,mTopTabBar->Top());
        mBottomTabBarContainer->Invalidate();
        mTopTabBarContainer->Invalidate();
    }
}
// Native3090e0..309136,86B.
// FUNCTION: WON 0x007090e0
void TabCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==15) {
        if(inEvent->mComponent==mTabBarLeft.get()) {
            mUserHasInteracted=true;
            AdjustTabBarScroll(10);
            return;
        }
        if(inEvent->mComponent==mTabBarRight.get()) {
            mUserHasInteracted=true;
            AdjustTabBarScroll(-10);
            return;
        }
    }
    Component::HandleComponentEvent(inEvent);
}
// Native309140..309237,247B. Two borrowed entry snapshots; current top reload after child focus.
// FUNCTION: WON 0x00709140
bool TabCtrl::Virtual88(bool inForward,bool inInit)
{
    Button* button=mBottomTabBar->mCurTab;
    if(button!=NULL) {
        Container* current=mCurTab;
        if(current!=NULL) {
            if(inInit) {
                if(!inForward) {
                    bool done=current->Virtual88(false,true);
                    if(done) return done;
                }
                bool result=button->Virtual88(inForward,true);
                if(result) mTopTabBar->SetHasUsedDialogKeys(true);
                return result;
            }
            if(button->HasFocus()) {
                if(inForward) return current->Virtual88(true,true);
                return false;
            }
            if(current->Virtual88(inForward,false)) return true;
            if(inForward) return false;
            if(button->Virtual88(false,true)) {
                mTopTabBar->SetHasUsedDialogKeys(true);
                return true;
            }
            return false;
        }
    }
    return Container::Virtual88(inForward,inInit);
}
// Native309240..309347,263B. Disable predicate is evaluated BEFORE loop bounds.
// FUNCTION: WON 0x00709240
bool TabCtrl::Virtual54(int inKey)
{
    Button* button=mBottomTabBar->mCurTab;
    if(button!=NULL && button->HasFocus()) {
        int step;
        if(inKey==39) step=1;
        else if(inKey==37) step=-1;
        else return Container::Virtual54(inKey);
        int old=mBottomTabBar->Fn308280();
        int next=old+step;
        while(mBottomTabBar->Disabled(next) && next>0 && next<mTabVector.size()-1) {
            if(inKey==39) { ++step; ++next; }
            else if(inKey==37) { --step; --next; }
        }
        mBottomTabBar->SetHasUsedDialogKeys(true);
        mTopTabBar->SetHasUsedDialogKeys(true);
        mUserHasInteracted=true;
        Fn309350(old+step,false);
        Button* current=mBottomTabBar->mCurTab;
        if(current!=NULL) current->Fn2F5890();
        return true;
    }
    return Container::Virtual54(inKey);
}
// Native309350/309370/3093d0 each24B: thin protected-call wrappers, support0.
// FUNCTION: WON 0x00709350
void TabCtrl::Fn309350(int inId,bool inFocus) { mBottomTabBar->SetCurTab(inId,inFocus); }
// FUNCTION: WON 0x00709370
void TabCtrl::Fn309370(const GUIString& inName,bool inFocus) { mBottomTabBar->SetCurTabByName(inName,inFocus); }
// FUNCTION: WON 0x007093d0
void TabCtrl::Fn3093D0(int inId,bool inEnable) { mBottomTabBar->EnableTab(inId,inEnable); }
// Native309390..3093c7,55B. Actual vector search, no pointer ownership added.
// FUNCTION: WON 0x00709390
int TabCtrl::Fn309390(Container* inContainer)
{
    for(int i=0;i<mTabVector.size();++i) if(mTabVector[i]==inContainer) return i;
    return -1;
}
// Native3093f0..3094c5,213B. Prior scroll requirement sampled before AddTab callback.
// FUNCTION: WON 0x007093f0
void TabCtrl::AddTab(Button* inTab,Container* inContainer)
{
    if(mTabBarHeight==0) {
        if(inTab->Height()>0) mTabBarHeight=inTab->Height();
        else mTabBarHeight=20;
    }
    inTab->Fn2F4B90(inTab->Width(),mTabBarHeight);
    inTab->SetComponentFlags(0x100,false);
    mTabVector.push_back(inContainer);
    bool oldNeed=NeedScrollButtons();
    mBottomTabBar->AddTab(inTab);
    if(inContainer!=NULL) {
        inContainer->SetVisible(false);
        inContainer->SetComponentFlags(0x2000,true);
        Fn2F6A60(inContainer);
    }
    if(NeedScrollButtons()!=oldNeed) Virtual08();
}
}
// Deferred native TabCtrl ctor308670/718: no body until complete standalone
// Container84 and LayoutPoint30 construction/lifetime contracts are closed.

namespace WONAPI {
// Native308670..30893e: operational child construction/layout setup,718B.
// FUNCTION: WON 0x00708670
TabCtrl::TabCtrl()
{
    mLeftPad=mRightPad=mTopPad=mBottomPad=5;
    mBottomTabLeftOffset=2;
    mBottomTabTopOffset=3;
    mCurTab=NULL;
    mTabBarHeight=0;
    mTabBarOverlap=1;
    mTabBarOnBottom=mTabBarOnRight=mArrowsOnLeft=false;
    mBottomTabBarContainer=new Container;
    mTopTabBarContainer=new Container;
    mBottomTabBar=new TabBar(this);
    mTopTabBar=new Container;
    mTopTabBar->SetOnlyChildInput(true);
    mTopTabBarContainer->SetOnlyChildInput(true);
    mTopTabBarContainer->SetComponentFlags(0x100,true);
    mBottomTabBarContainer->SetComponentFlags(0x100,true);
    mBottomTabBarContainer->SetComponentFlags(8,false);
    mBottomTabBarContainer->SetComponentFlags(0x2000,true);
    mTopTabBarContainer->SetComponentFlags(0x3000,true);
    mBottomTabBarContainer->Fn2F6A60(mBottomTabBar.get());
    mTopTabBarContainer->Fn2F6A60(mTopTabBar.get());
    Fn2F6A60(mBottomTabBarContainer.get());
    mContainerLayout=new LayoutPoint;
    AddLayoutPoint(mContainerLayout.get());
    Fn2F6A60(mTopTabBarContainer.get());
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK tab removal/reindex/reselection algorithms.
void TabBar::RemoveTab(int theId)
{
	if(theId<0 || theId>=mTabVector.size())
		return;

	int aCurTab = Fn308280();

	VirtualC4(mTabVector[theId]);
	mTabVector.erase(mTabVector.begin() + theId);
	int i;
	for(i=0; i<mTabVector.size(); i++)
		mTabVector[i]->mControlId=i;

	mCurTab = NULL;
	for(i=0; i<mTabVector.size(); i++)
	{
		Button *aButton = mTabVector[i].get();
		if(aButton->GetParent()!=this)
			AddChildEx(aButton,mTabVector.size()-i-1);
			
		PositionTab(aButton);
	}
}

void TabCtrl::RemoveTab(int theId)
{
	if(theId<0 || theId>=mTabVector.size())
		return;

	int aCurTabId = mBottomTabBar->Fn308280();

	mTopTabBar->VirtualC4(mBottomTabBar->mCurTab);
	mCurTab = NULL;

	VirtualC4(mTabVector[theId]);
	mTabVector.erase(mTabVector.begin() + theId);
	mBottomTabBar->RemoveTab(theId);
	

	if(theId<aCurTabId)
		Fn309350(aCurTabId-1,true);
	else if(theId>aCurTabId)
		Fn309350(aCurTabId,true);
	else 
		Fn309350(0,true);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007081d0
// (public: virtual void * __thiscall WONAPI::TabBar::`scalar deleting destructor'(unsigned int)    [??_GTabBar@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00708940
// (public: virtual void * __thiscall WONAPI::TabCtrl::`scalar deleting destructor'(unsigned int)    [??_GTabCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071d270
// (protected: bool __thiscall std::vector<class WONAPI::Container *,class std::allocator<class WONAPI::Container *> >::_Buy(unsigned int)    [?_Buy@?$vec)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007081f0
// (public: virtual __thiscall WONAPI::TabBar::~TabBar(void))
// TEMPLATE: WON 0x00723cf0
// (protected: class WONAPI::SmartPtr<class WONAPI::Button> * __thiscall std::vector<class WONAPI::SmartPtr<class WONAPI::Button>,class std::allocator<cla)
// TEMPLATE: WON 0x00708160
// (public: __thiscall WONAPI::TabBar::TabBar(class WONAPI::TabCtrl *))
// TEMPLATE: WON 0x0071d1b0
// (public: class std::_Ptrit<class WONAPI::SmartPtr<class WONAPI::Button>,int,class WONAPI::SmartPtr<class WONAPI::Button> *,class WONAPI::SmartPtr<class)
// TEMPLATE: WON 0x0071d210
// (public: class std::_Ptrit<class WONAPI::Container *,int,class WONAPI::Container * *,class WONAPI::Container * &,class WONAPI::Container * *,class WONA)
// TEMPLATE: WON 0x00721820
// (public: void __thiscall std::vector<class WONAPI::SmartPtr<class WONAPI::Button>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::Button> > >)
// TEMPLATE: WON 0x00721c10
// (public: void __thiscall std::vector<class WONAPI::Container *,class std::allocator<class WONAPI::Container *> >::insert(class std::_Ptrit<class WONAPI)
// TEMPLATE: WON 0x00723900
// (protected: void __thiscall std::vector<class WONAPI::Container *,class std::allocator<class WONAPI::Container *> >::_Xlen(void)const )
