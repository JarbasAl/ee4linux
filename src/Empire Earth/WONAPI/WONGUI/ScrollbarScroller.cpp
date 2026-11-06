// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;
// FUNCTION: WON 0x00705760
void ScrollbarScroller::SetScrollbar(Scrollbar* scrollbar) {
    if(scrollbar->mVertical) mVertScrollbar=scrollbar;
    else mHorzScrollbar=scrollbar;
    scrollbar->AddListener(new ComponentListenerComp(this));
}
// FUNCTION: WON 0x00705840
void ScrollbarScroller::Virtual6C() {
    if(mVertScrollbar->mParent!=this) AddControls();
    Container::Virtual6C();
}
// FUNCTION: WON 0x00705860
void ScrollbarScroller::AddControls() {
    mScrollArea->SetScroller(this);
    Fn2F6A60(mScrollArea.get());
    Fn2F6A60(mVertScrollbar.get());
    Fn2F6A60(mHorzScrollbar.get());
    if(mScrollArea->mL0A4>0 || mScrollArea->mTotalHeight>0) NotifyScrollChange(false);
}
// FUNCTION: WON 0x007058d0
bool ScrollbarScroller::EnableScrollbar(Scrollbar* scrollbar,ScrollbarCondition condition) {
    if(scrollbar==NULL || mScrollArea.get()==NULL) return false;
    bool visible=scrollbar->IsVisible();
    if(mSettingScrollbarConditions && condition==ScrollbarCondition_Conditional && visible) return false;
    switch(condition) {
    case ScrollbarCondition_Disable:scrollbar->SetVisible(false);break;
    case ScrollbarCondition_Enable:scrollbar->SetVisible(true);break;
    case ScrollbarCondition_Conditional:
        if(scrollbar->mVertical) scrollbar->SetVisible(mScrollArea->mTotalHeight>mScrollArea->mScrollArea.height);
        else scrollbar->SetVisible(mScrollArea->mL0A4>mScrollArea->mScrollArea.width);
        break;
    }
    return visible!=scrollbar->IsVisible();
}
// FUNCTION: WON 0x00705990
void ScrollbarScroller::SetScrollbarConditions(ScrollbarCondition horizontal,ScrollbarCondition vertical) {
    mHorzCondition=horizontal;
    mVertCondition=vertical;
    bool changed=EnableScrollbar(mHorzScrollbar.get(),mHorzCondition)?true:false;
    changed=EnableScrollbar(mVertScrollbar.get(),mVertCondition) || changed;
    mSettingScrollbarConditions=true;
    if(changed) {
        Invalidate();
        Virtual08();
    }
    mSettingScrollbarConditions=false;
}
// FUNCTION: WON 0x00705a00
void ScrollbarScroller::HandleComponentEvent(ComponentEvent* event) {
    if(event->mType==16) {
        Scrollbar* scrollbar=(Scrollbar*)event->mComponent;
        int position=scrollbar->mPosition;
        if(scrollbar->mVertical) {
            if(scrollbar==mVertScrollbar.get()) { SetVertOffset(position);return; }
        } else if(scrollbar==mHorzScrollbar.get()) { SetHorzOffset(position);return; }
    }
    Component::HandleComponentEvent(event);
}
// FUNCTION: WON 0x00705a50
void ScrollbarScroller::NotifyScrollChange(bool becauseOfScroller) {
    if(mVertScrollbar.get()!=NULL) {
        mVertScrollbar->mTotalSize=mScrollArea->mTotalHeight+mScrollArea->mExtraHeight;
        mVertScrollbar->mPageSize=mScrollArea->mScrollArea.height;
        mVertScrollbar->mPageScrollSize=mScrollArea->mScrollArea.height;
        mVertScrollbar->mLineUpScrollSize=mScrollArea->mUpScrollAmount;
        mVertScrollbar->mLineDownScrollSize=mScrollArea->mDownScrollAmount;
        if(!becauseOfScroller) mVertScrollbar->mPosition=mScrollArea->mVertOffset;
        mVertScrollbar->LayoutThumb(!becauseOfScroller);
        mVertScrollbar->SetEnabled(mScrollArea->mTotalHeight+mScrollArea->mExtraHeight>mScrollArea->mScrollArea.height);
        mVertScrollbar->Invalidate();
    }
    if(mHorzScrollbar.get()!=NULL) {
        mHorzScrollbar->mTotalSize=mScrollArea->mL0A4;
        mHorzScrollbar->mPageSize=mScrollArea->mScrollArea.width;
        mHorzScrollbar->mPageScrollSize=mScrollArea->mScrollArea.width;
        if(!becauseOfScroller) mHorzScrollbar->mPosition=mScrollArea->mHorzOffset;
        mHorzScrollbar->LayoutThumb(!becauseOfScroller);
        mHorzScrollbar->SetEnabled(mScrollArea->mL0A4>mScrollArea->mScrollArea.width);
        mHorzScrollbar->Invalidate();
    }
    SetScrollbarConditions(mHorzCondition,mVertCondition);
}
