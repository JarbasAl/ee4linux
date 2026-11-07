// Native container child lifetime, modal and focus transitions.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include "LostDebug.h"
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
// FUNCTION: WON 0x006f6210
void Container::EnterModal()
{
    ++mModalCount;
    if(mMouseChild!=NULL && !mMouseChild->ComponentFlagSet(0x8000)) MouseAt();
    if(mFocusChild!=NULL && !mFocusChild->ComponentFlagSet(0x8000)) SetFocusToLastFocusChild();
}
// FUNCTION: WON 0x006f6260
void Container::LeaveModal()
{
    if(mModalCount>0) {
        mModalCount=0;
        MouseAt();
        if(mFocusChild==NULL) SetFocusToLastFocusChild();
    }
}
// FUNCTION: WON 0x006f6c60
void Container::VirtualC4(Component* child)
{
    if(child->mParent!=this) return;
    SmartPtr<Component> holder; // never assigned: its frame slot and EH state 0 are in the function
    if(!mRemovingAllChildren && child->ComponentFlagSet(0x100)) {
        child->Invalidate();
        RootContainer* root=Fn2F58F0();
        if(root!=NULL) root->AddBG(child->GetGrabBG());
    } else Invalidate();
    SmartPtr<Component> keep=child;
    mChildList.erase(child->mChildListItr);
    // LOST-DEBUG: none: exact only (keeps ~SmartPtr out of line inside erase).
    EE_LOST_DEBUG(1);
    if(!mRemovingAllChildren) mChildLayout.RemoveComponent(child);
    if(child==mFocusChild) SetFocusToLastFocusChild();
    if(child==mMouseChild) MouseAt();
    child->mParent=NULL;
}
// FUNCTION: WON 0x006f6d80
void Container::RemoveAllChildren()
{
    mRemovingAllChildren=true;
    while(!mChildList.empty()) VirtualC4(mChildList.front().get());
    mChildLayout.Clear();
    mRemovingAllChildren=false;
    Virtual78(NULL);
}
// FUNCTION: WON 0x006f6dd0
void Container::BringToTop(Component* child)
{
    if(child->mParent!=this) return;
    SmartPtr<Component> keep=child;
    BackgroundGrabber* grabber=child->GetGrabBG();
    RootContainer* root=Fn2F58F0();
    bool needInvalidate=true;
    if(grabber!=NULL) {
        child->Invalidate();
        root->AddBG(grabber);
        needInvalidate=false;
    }
    mChildList.erase(child->mChildListItr);
    // LOST-DEBUG: none: exact only (keeps the node dtor out of line inside erase).
    EE_LOST_DEBUG(1);
    child->mChildListItr=mChildList.insert(FindTop(),SmartPtr<Component>(child));
    if(needInvalidate) Invalidate();
}
// FUNCTION: WON 0x006f7440
void Container::VirtualB4(Component* child)
{
    if(!IsVisible() || IsDisabled()) return;
    if(mFocusChild==child && ComponentFlagSet(0x10)) return;
    if(mModalCount>0 && child!=NULL && !child->ComponentFlagSet(0x8000)) return;
    if(mFocusChild!=NULL && ComponentFlagSet(0x10)) mFocusChild->Virtual4C();
    mFocusChild=child;
    // Actual native write precedes its null test; no new null guard is introduced.
    child->mFocusId=GetNextFocusId();
    if(child!=NULL) {
        if(!ComponentFlagSet(0x10) && mParent!=NULL) mParent->VirtualB4(this);
        else {
            SetComponentFlags(0x10,true);
            if(mParent==NULL && Fn2F58F0()==this) {
                Window* window=GetRoot(); //existing physical2f5920 is genuine GetWindow
                if(window->mManager->mFocusWindow!=window) return;
            }
            child->Virtual48();
        }
    }
}
// FUNCTION: WON 0x006f7510
void Container::VirtualBC(Component* child)
{
    bool bad=child->IsDisabled() || !child->IsVisible();
    if(mFocusChild==child && bad) {
        mFocusChild->Virtual4C();
        mFocusChild=NULL;
    }
    if(Fn2F58F0()==NULL) return;
    int x,y;
    GetMousePos(x,y);
    if(child->Contains(x,y) || (mMouseChild==child && bad)) Fn2F6650(x,y);
}
}
