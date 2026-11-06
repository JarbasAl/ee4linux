// PROVISIONAL Container child-query family; original filename unproved.
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
namespace WONAPI {
// FUNCTION: WON 0x006f7770
Component* Container::VirtualC8(int inControlId)
{
    std::list<Component*> pending;
    pending.push_back(this);
    for (std::list<Component*>::iterator i=pending.begin(); i!=pending.end(); ++i) {
        Component* component=*i;
        if (component->mControlId==inControlId)
            return component;
        Container* container=dynamic_cast<Container*>(component);
        if (container!=NULL) {
            for (std::list<SmartPtr<Component> >::iterator child=container->mChildList.begin();
                 child!=container->mChildList.end(); ++child)
                pending.push_back(child->get());
        }
    }
    return NULL;
}

// FUNCTION: WON 0x006f62d0
void Container::Virtual0C(int& width, int& height)
{
    int right = 0, bottom = 0;
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin(); i!=mChildList.end(); ++i)
    {
        Component* child = i->get();
        if(child->Right() > right)
            right = child->Left() + child->Width();
        if(child->Bottom() > bottom)
            bottom = child->Top() + child->Height();
    }
    width = right;
    height = bottom;
}

// FUNCTION: WON 0x006f6480
void Container::Virtual8C(const ControlIdSet& ids, bool enabled)
{
    Component::Virtual8C(ids,enabled);
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin(); i!=mChildList.end(); ++i)
        (*i)->Virtual8C(ids,enabled);
}

// FUNCTION: WON 0x006f7320
void Container::Virtual80(ComponentEvent* event)
{
    Component::Virtual80(event);
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin();i!=mChildList.end();++i)
        (*i)->Virtual80(event);
}

// FUNCTION: WON 0x006f6650
void Container::Fn2F6650(int x,int y)
{
    Component* aChild=VirtualAC(x,y);
    if(aChild!=mMouseChild) {
        if(mMouseChild!=NULL) {
            mMouseChild->Virtual28();
            mMouseChild=aChild;
            Virtual78(Fn2F4B00());
        } else mMouseChild=aChild;
        if(aChild!=NULL) aChild->Virtual24(x-aChild->Left(),y-aChild->Top());
    }
}

// FUNCTION: WON 0x006f6780
void Container::Virtual34(int x,int y)
{
    Fn2F6650(x,y);
    Component* aChild=mMouseChild;
    if(aChild!=NULL) aChild->Virtual34(x-aChild->Left(),y-aChild->Top());
}

// FUNCTION: WON 0x006f6840
void Container::Virtual30(int x,int y,int button)
{
    Component* anOld=mMouseChild;
    Fn2F6650(x,y);
    Component* aChild=mMouseChild;
    if(aChild!=NULL && aChild==anOld) aChild->Virtual30(x-aChild->Left(),y-aChild->Top(),button);
}

// FUNCTION: WON 0x006f6880
void Container::Virtual2C(int x,int y,int button)
{
    Fn2F6650(x,y);
    Component* aChild=mMouseChild;
    if(aChild!=NULL) aChild->Virtual2C(x-aChild->Left(),y-aChild->Top(),button);
}

// SUPPORT ONLY: EXE 0x006f6810; coordinate-adapting direct dispatch37, zero quota
// FUNCTION: WON 0x006f6810
void Container::Virtual38(int x,int y)
{
    Component* aChild=mMouseChild;
    if(aChild!=NULL) aChild->Virtual38(x-aChild->Left(),y-aChild->Top());
}

// FUNCTION: WON 0x006f65b0
Component* Container::VirtualAC(int x,int y)
{
    if(!mChildList.empty()) {
        std::list<SmartPtr<Component> >::iterator i=mChildList.end();
        do {
            --i;
            Component* child=i->get();
            if(child->Contains(x,y) && child->Virtual20(x-child->Left(),y-child->Top())) {
                if(child->IsDisabled()) return NULL;
                if(mModalCount>0 && !child->ComponentFlagSet(0x8000)) return NULL;
                return child;
            }
        } while(i!=mChildList.begin());
    }
    return NULL;
}

// FUNCTION: WON 0x006f69d0
// FUNCTION: WON 0x006f6a40
std::list<SmartPtr<Component> >::iterator Container::FindTop(std::list<SmartPtr<Component> >::iterator i)
{
    if(i==mChildList.begin()) return mChildList.begin();
    do {
        --i;
        if(!(*i)->ComponentFlagSet(0x1000)) return ++i;
    } while(i!=mChildList.begin());
    return i;
}

// SUPPORT ONLY: EXE 0x006f6a40; forwarding overload27, zero quota
std::list<SmartPtr<Component> >::iterator Container::FindTop()
{
    return FindTop(mChildList.end());
}

// FUNCTION: WON 0x006f6a60
void Container::Fn2F6A60(Component* child)
{
    if(child->mParent!=NULL) child->mParent->VirtualC4(child);
    child->mParent=this;
    child->mChildListItr=mChildList.insert(FindTop(),SmartPtr<Component>(child));
    child->Virtual6C();
}

// FUNCTION: WON 0x006f6b50
void Container::AddChildEx(Component* child,int position)
{
    if(child->mParent!=NULL) child->mParent->VirtualC4(child);
    child->mParent=this;
    std::list<SmartPtr<Component> >::iterator i=mChildList.begin();
    for(int n=0;n<position && i!=mChildList.end();++n) ++i;
    child->mChildListItr=mChildList.insert(FindTop(i),SmartPtr<Component>(child));
    child->Virtual6C();
}

// FUNCTION: WON 0x006f6340
void Container::Virtual14(Graphics& g)
{
    if(!mChildList.empty()) {
        std::list<SmartPtr<Component> >::iterator i=mChildList.end();
        do {
            --i;
            Component* child=i->get();
            if(child->Virtual10()) {
                g.Translate(child->Left(),child->Top());
                child->Virtual14(g);
                g.Translate(-child->Left(),-child->Top());
            }
        } while(i!=mChildList.begin());
    }
    Component::Virtual14(g);
}

// FUNCTION: WON 0x006f64d0
void Container::Virtual84(bool visible)
{
    Component::Virtual84(visible);
    mHasUsedDialogKeys=false;
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin();i!=mChildList.end();++i) {
        Component* child=i->get();
        if(child->IsVisible()) child->Virtual84(visible);
    }
}

// FUNCTION: WON 0x006f6520
bool Container::Virtual20(int x,int y)
{
    if(!Component::Virtual20(x,y)) return false;
    if(mOnlyChildInput) {
        std::list<SmartPtr<Component> >::iterator i=mChildList.end();
        do {
            --i; // Native intentionally has no preliminary empty-child guard on this branch.
            Component* child=i->get();
            if(child->Contains(x,y) && child->Virtual20(x-child->Left(),y-child->Top())) return true;
        } while(i!=mChildList.begin());
        return false;
    }
    return true;
}
}

namespace WONAPI {
// Genuine SDK Container::SizeChanged, existing reduced virtual API.
// FUNCTION: WON 0x006f62b0
void Container::Virtual08()
{
    Component::Virtual08();
    mChildLayout.ExecuteInstructions(this);
}
// Genuine SDK Container::MouseEnter; base cursor policy precedes child transition.
// FUNCTION: WON 0x006f6700
void Container::Virtual24(int x,int y)
{
    Component::Virtual24(x,y);
    Fn2F6650(x,y);
}
// Genuine SDK Container::MouseExit; borrowed child dispatch then owned cursor clear.
// FUNCTION: WON 0x006f6730
void Container::Virtual28()
{
    Component::Virtual28();
    if(mMouseChild!=NULL)
    {
        mMouseChild->Virtual28();
        mMouseChild=NULL;
    }
    mCurrentCursor=NULL;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0071c2a0
// (public: void __thiscall std::list<class WONAPI::Component *,class std::allocator<class WONAPI::Component *> >::_Insert(class std::list<class WONAPI::C)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0071ca10
// (public: class std::list<class WONAPI::Component *,class std::allocator<class WONAPI::Component *> >::iterator __thiscall std::list<class WONAPI::Compo)
// TEMPLATE: WON 0x0071ebb0
// (protected: void __thiscall std::list<class WONAPI::Component *,class std::allocator<class WONAPI::Component *> >::_Incsize(unsigned int)    [?_Incsize)
