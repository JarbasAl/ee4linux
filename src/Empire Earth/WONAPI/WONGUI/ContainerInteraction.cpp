// Native container paint, invalidation propagation and keyboard/tab traversal.
// Baseline Container.cpp is untouched; no private object allocation.
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
// FUNCTION: WON 0x006f63b0
void Container::Paint(Graphics& graphics)
{
    ColorScheme* scheme=graphics.mColorScheme;
    Component::Paint(graphics);
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin();i!=mChildList.end();++i) {
        Component* child=i->get();
        if(child->mInvalidState!=0 && child->IsVisible()) {
            graphics.Translate(child->Left(),child->Top());
            if(child->ComponentFlagSet(0x2000)) graphics.PushClipRect(0,0,child->Width(),child->Height());
            if(child->GetOwnColorScheme()!=NULL) graphics.mColorScheme=child->GetOwnColorScheme();
            else graphics.mColorScheme=scheme;
            child->Paint(graphics);
            if(child->ComponentFlagSet(0x2000)) graphics.PopClipRect();
            graphics.Translate(-child->Left(),-child->Top());
        }
    }
}
// FUNCTION: WON 0x006f68c0
bool Container::VirtualC0(Component* changed)
{
    if(mInvalidState==2) return false;
    mInvalidState=1;
    bool higherInvalid=false;
    std::list<SmartPtr<Component> >::iterator i=mChildList.end();
    while(i!=mChildList.begin()) {
        --i;
        Component* child=i->get();
        if(child==changed) break;
        if(child->Intersects(changed) && child->IsVisible()) {
            child->Invalidate();
            higherInvalid=true;
        }
    }
    if(mParent!=NULL) mParent->VirtualC0(this);
    return higherInvalid;
}
// FUNCTION: WON 0x006f6940
void Container::Virtual68()
{
    // Target differs from laterSDK: exact state2 skips ALL work.
    if(mInvalidState==2) return;
    Component::Virtual68();
    mInvalidState=2;
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin();i!=mChildList.end();++i)
        (*i)->Virtual68();
}
// FUNCTION: WON 0x006f69a0
void Container::Virtual70()
{
    Component::Virtual70();
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin();i!=mChildList.end();++i)
        (*i)->Virtual70();
}
// FUNCTION: WON 0x006f75a0
bool Container::Virtual88(bool forward,bool init)
{
    if(!WantFocus()) return false;
    if(init) {
        if(mIsolateTabFocus) return false;
        if(ComponentFlagSet(0x4000)) {
            mHasUsedDialogKeys=true;
            Fn2F5890();
            return true;
        }
    }
    if(mChildList.empty()) return false;
    bool wrap=mIsolateTabFocus;
    Component* child=init?NULL:mFocusChild;
    if(child==NULL) { SmartPtr<Component> &first=forward?mChildList.front():mChildList.back(); child=first.get(); }
    else child=child->GetNextSibling(forward,wrap);
    unsigned long count=0;
    while(child!=NULL) {
        if(child->Virtual88(forward,true)) {
            mHasUsedDialogKeys=true;
            return true;
        }
        child=child->GetNextSibling(forward,wrap);
        if(++count>=mChildList.size()) break;
    }
    return false;
}
// FUNCTION: WON 0x006f7690
bool Container::Virtual54(int key)
{
    if(mFocusChild!=NULL && mFocusChild->Virtual54(key)) return true;
    if(key!=9) return false;
    if(mChildList.empty()) return false;
    bool forward=(GetKeyMod()&1)==0;
    return Virtual88(forward,false);
}
// FUNCTION: WON 0x006f7730
bool Container::Virtual5C()
{
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin();i!=mChildList.end();++i)
        if(!(*i)->Virtual5C()) return false;
    return true;
}
// FUNCTION: WON 0x006f7400
void Container::Virtual48() { Component::Virtual48(); if(mFocusChild!=NULL) mFocusChild->Virtual48(); }
// FUNCTION: WON 0x006f7420
void Container::Virtual4C() { Component::Virtual4C(); if(mFocusChild!=NULL) mFocusChild->Virtual4C(); }
// FUNCTION: WON 0x006f7710
bool Container::Virtual50(int key) { return mFocusChild!=NULL?mFocusChild->Virtual50(key):false; }
// FUNCTION: WON 0x006f76f0
bool Container::Virtual58(int key)
{
    if(mFocusChild!=NULL) {
        mFocusChild->Virtual58(key);
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006f6990
void Container::Virtual6C() { Component::Virtual6C(); mHasUsedDialogKeys=false; }
// FUNCTION: WON 0x006f6980
void Container::Invalidate() { Component::Invalidate(); }
// FUNCTION: WON 0x006f67c0
bool Container::Virtual3C(int amount)
{
    Component* child=mMouseChild;
    if(mMouseWheelGoesToFocusChild) child=mFocusChild;
    if(child!=NULL) return child->Virtual3C(amount);
    return false;
}
// FUNCTION: WON 0x006f67f0
void Container::Virtual40(unsigned long elapsed) { Component::Virtual40(elapsed); if(mMouseChild!=NULL) mMouseChild->Virtual40(elapsed); }
}
