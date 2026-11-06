// Native owning child-timer registration/dispatch and cursor propagation support.
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
// FUNCTION: WON 0x006f70c0
void Container::VirtualB8(Component* child)
{
    if(child->mTimerFlags!=0) {
        mChildTimerSet.insert(child);
        bool requestParent=mTimerFlags==0;
        mTimerFlags|=1;
        if(requestParent && mParent!=NULL) mParent->VirtualB8(this);
    }
}
// FUNCTION: WON 0x006f7210
bool Container::Virtual44(int elapsed)
{
    bool childNeeds=false;
    ChildTimerSet::iterator i=mChildTimerSet.begin();
    while(i!=mChildTimerSet.end()) {
        Component* child=i->get();
        if(child->mTimerFlags!=0 && child->mParent==this && child->Virtual44(elapsed)) {
            childNeeds=true;
            ++i;
        } else {
            child->Fn2F57D0(false);
            mChildTimerSet.erase(i++);
        }
    }
    if(!childNeeds) mTimerFlags&=~1;
    return mTimerFlags!=0;
}
// FUNCTION: WON 0x006f7360
void Container::Virtual78(Cursor* cursor)
{
    if(mMouseChild!=NULL && mMouseChild->ComponentFlagSet(0x40)) {
        Cursor* fallback=Fn2F4B00();
        if(mCurrentCursor.get()!=fallback) mParent->Virtual78(fallback);
        return;
    }
    if(cursor==NULL) cursor=Fn2F4B00();
    if(cursor!=mCurrentCursor.get()) {
        mCurrentCursor=cursor;
        if(mParent!=NULL) mParent->Virtual78(cursor);
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071be00
// (protected: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::Component>,class WONAPI::SmartPtr<class WONAPI::Component>,struct std::_Setk<class WO)
