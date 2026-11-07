// Remaining component traversal and parent-state behavior; original-owned receivers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x006f5230
Component* Component::GetNextSibling(bool forward,bool wrap)
{
    if(mParent==NULL) return NULL;
    std::list<SmartPtr<Component> >::iterator i=mChildListItr;
    if(forward) {
        ++i;
        if(i==mParent->mChildList.end()) {
            if(!wrap) return NULL;
            return mParent->mChildList.front().get();
        }
    } else {
        if(i==mParent->mChildList.begin()) {
            if(!wrap) return NULL;
            return mParent->mChildList.back().get();
        }
        --i;
    }
    return (*i).get();
}

// FUNCTION: WON 0x006f5160
void Component::Virtual6C()
{
    if(HasFocus()) {
        if(!WantFocus()) Virtual4C();
        else mParent->VirtualB4(this);
    }
    if(mTimerFlags!=0) mParent->VirtualB8(this);
    if(mParent->Fn2F5600() && IsVisible()) Virtual84(true);
    mInvalidState=0;
    Invalidate();
}

// FUNCTION: WON 0x006f5720
void Component::RootToChild(int& x,int& y)
{
    x-=Left();
    y-=Top();
    if(mParent!=NULL) mParent->RootToChild(x,y);
}

// FUNCTION: WON 0x006f5810
bool Component::WantFocus()
{
    if(!ComponentFlagSet(8)) return false;
    if(!IsVisible() || IsDisabled()) return false;
    if(mParent!=NULL && mParent->mModalCount>0 && !ComponentFlagSet(0x8000)) return false;
    return true;
}
}

namespace WONAPI {
// Original2f5a00/39B actual font forwarding provider; support0.
// FUNCTION: WON 0x006f5a00
SmartPtr<Font> Component::GetFont(const FontDescriptor& descriptor)
{
    Window* root=GetRoot();
    return root->GetFont(descriptor);
}
// FUNCTION: WON 0x006f5a30
Font* Component::Fn2F5A30(Font* font,int style,int size)
{
    FontDescriptor descriptor=font->mDescriptor;
    descriptor.mStyle=style;
    if(size>0) descriptor.mSize=size;
    return GetFont(descriptor).get();
}
// FUNCTION: WON 0x006f54e0
void Component::Fn2F54E0(ComponentEvent* event)
{
    if(event->mBroadcast) return;
    if(mExtraStuff!=NULL && mExtraStuff->mListener.get()!=NULL)
        mExtraStuff->mListener->Virtual04(event);
    else if(mParent!=NULL)
        mParent->HandleComponentEvent(event);
    else {
        Window* window=GetRoot();
        if(window!=NULL) {
            window=window->mParent;
            if(window!=NULL && window->mRoot.get()!=NULL)
                window->mRoot->Fn2F54E0(event);
        }
    }
}
}

// Tool tip delay and display time, in milliseconds.
DWORD gComponentToolTipStartTime=500;
DWORD gComponentToolTipEndTime=5000;
namespace WONAPI {
// Original2f4cc0/19B defaultcursor forwarding support0.
// FUNCTION: WON 0x006f4cc0
void Component::Virtual24(int x,int y)
{
    if(mExtraStuff!=NULL) Virtual78(mExtraStuff->mDefaultCursor.get());
}
// Original2f4ce0/38B tooltip-state reset/close forwarding support0.
// FUNCTION: WON 0x006f4ce0
void Component::Virtual28()
{
    if(mExtraStuff!=NULL && mExtraStuff->mToolTip.get()!=NULL) {
        mExtraStuff->mDontDoTip=false;
        GetRoot()->ClosePopup(mExtraStuff->mToolTip.get());
    }
}
// FUNCTION: WON 0x006f4d10
void Component::Virtual40(unsigned long elapsed)
{
    if(mExtraStuff==NULL || mExtraStuff->mToolTip.get()==NULL) return;
    if(elapsed>=gComponentToolTipStartTime && !mExtraStuff->mDontDoTip) {
        Window* window=GetRoot();
        int x=window->mMouseX+15; int y=window->mMouseY+15;
        window->OpenPopup(PopupParams(mExtraStuff->mToolTip.get(),PopupFlag_StandardTip,this,x,y));
        mExtraStuff->mDontDoTip=true;
    } else if(elapsed>=gComponentToolTipStartTime+gComponentToolTipEndTime)
        GetRoot()->ClosePopup(mExtraStuff->mToolTip.get());
}
}

namespace WONAPI {
// Original2f5590/18B root-visibility flag forwarding, support0.
// FUNCTION: WON 0x006f5590
void Component::Virtual84(bool visible)
{
    SetComponentFlags(0x40000,visible);
}
// Original2f5850/49B default tab-focus adapter, support0; actual RET8.
// FUNCTION: WON 0x006f5850
bool Component::Virtual88(bool forward,bool init)
{
    if(init && ComponentFlagSet(0x4000) && WantFocus()) {
        Fn2F5890();
        return true;
    }
    return false;
}
}

namespace WONAPI {
// FUNCTION: WON 0x006f57d0
void Component::Fn2F57D0(bool on)
{
    bool oldWantTimer=mTimerFlags!=0;
    if(on) mTimerFlags|=2;
    else mTimerFlags&=~2;
    bool wantTimer=mTimerFlags!=0;
    if(wantTimer!=oldWantTimer && mParent!=NULL) mParent->VirtualB8(this);
}
// FUNCTION: WON 0x006f5890
void Component::Fn2F5890()
{
    if(HasFocus() || !WantFocus()) return;
    if(mParent==NULL) Virtual48();
    else mParent->VirtualB4(this);
}
// Original2f58f0/40, root getter support0.
// FUNCTION: WON 0x006f58f0
RootContainer* Component::Fn2F58F0()
{
    Component* root=this;
    while(root->mParent!=NULL) root=root->mParent;
    return dynamic_cast<RootContainer*>(root);
}
// Original2f5920/21, window getter/default provider support0.
// FUNCTION: WON 0x006f5920
Window* Component::GetRoot()
{
    RootContainer* root=Fn2F58F0();
    if(root!=NULL) return root->mWindow;
    return Window::GetDefaultWindow();
}
// Original2f5940/19, own window getter support0; no default substitution.
// FUNCTION: WON 0x006f5940
Window* Component::Fn2F5940()
{
    RootContainer* root=Fn2F58F0();
    if(root!=NULL) return root->mWindow;
    return NULL;
}
// Original2f5600/9, root-visibility flag getter support0.
// FUNCTION: WON 0x006f5600
bool Component::Fn2F5600()
{
    return ComponentFlagSet(0x40000);
}
// Original2f58d0/10, focus flag setter support0.
// FUNCTION: WON 0x006f58d0
void Component::Virtual48()
{
    SetComponentFlags(0x10,true);
}
// Original2f58e0/10, focus flag setter support0.
// FUNCTION: WON 0x006f58e0
void Component::Virtual4C()
{
    SetComponentFlags(0x10,false);
}
}

namespace WONAPI {
// FUNCTION: WON 0x006f4db0
bool Component::CheckGrabBG()
{
    if(ComponentFlagSet(0x300)) {
        AllocateExtraStuff();
        if(mExtraStuff->mGrabBG==NULL)
            mExtraStuff->mGrabBG=new BackgroundGrabber;
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006f4f60
bool Component::Virtual10()
{
    if(mInvalidState==0) return false;
    if(IsVisible()) return true;
    return CheckGrabBG() && mExtraStuff->mGrabBG->NeedDrawBG();
}
// FUNCTION: WON 0x006f5110
void Component::Invalidate()
{
    if(mInvalidState==2) return;
    WONRectangle rect(0,0,Width(),Height());
    Virtual60(rect);
    if(mParent!=NULL) mParent->VirtualC0(this);
    Virtual68();
}
// FUNCTION: WON 0x006f51f0
void Component::Virtual68()
{
    mInvalidState=2;
    if(CheckGrabBG()) {
        mExtraStuff->mGrabBG->Invalidate(this);
        RootContainer* root=Fn2F58F0();
        if(root!=NULL) root->AddBG(mExtraStuff->mGrabBG);
    }
}
// Original2f4e50/26, forwarding support0.
// FUNCTION: WON 0x006f4e50
void Component::Virtual08()
{
    if(CheckGrabBG()) mExtraStuff->mGrabBG->SizeChanged(this);
}
// Original2f4ff0/33, forwarding support0.
// FUNCTION: WON 0x006f4ff0
void Component::Virtual14(Graphics& graphics)
{
    if(CheckGrabBG()) mExtraStuff->mGrabBG->PrePaint(this,graphics);
}
// Original2f5020/37, forwarding support0; invalidation reset precedes callback.
// FUNCTION: WON 0x006f5020
void Component::Paint(Graphics& graphics)
{
    mInvalidState=0;
    if(CheckGrabBG()) mExtraStuff->mGrabBG->Paint(this,graphics);
}
// Original2f51d0/25, forwarding support0.
// FUNCTION: WON 0x006f51d0
void Component::Virtual70()
{
    if(CheckGrabBG()) mExtraStuff->mGrabBG->Clear();
}
}
