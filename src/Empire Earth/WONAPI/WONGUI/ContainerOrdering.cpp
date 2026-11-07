// Native last-focus selection and retained child ordering, borrowed Container84.
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
#include "LostDebug.h"
namespace WONAPI {
// FUNCTION: WON 0x006f6030
void Container::SetFocusToLastFocusChild()
{
    unsigned char current=mNextFocusId;
    bool skipCurrent=false;
    if(mFocusChild!=NULL) {
        current=mFocusChild->mFocusId;
        mFocusChild->Virtual4C();
        mFocusChild=NULL;
        skipCurrent=true;
    }
    Component* last=NULL;
    unsigned char minimum=0;
    for(std::list<SmartPtr<Component> >::iterator i=mChildList.begin();i!=mChildList.end();++i) {
        Component* child=i->get();
        unsigned char id=child->mFocusId;
        if(id==0 || (id==current && skipCurrent)) continue;
        // Signed promoted difference is compared BEFORE truncating the stored BYTE.
        if(minimum==0 || int(current)-int(id)<int(minimum)) {
            minimum=(unsigned char)(current-id);
            last=child;
        }
    }
    if(last!=NULL) last->Fn2F5890();
}
// FUNCTION: WON 0x006f6f60
void Container::EnsureOnTopOf(Component* bottom,Component* top)
{
    if(bottom->mParent!=this || top->mParent!=this) return;
    std::list<SmartPtr<Component> >::iterator i=bottom->mChildListItr;
    while(i!=top->mChildListItr && i!=mChildList.end()) ++i;
    if(i!=mChildList.end()) return;
    SmartPtr<Component> keep=bottom;
    mChildList.erase(bottom->mChildListItr);
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(1);
    bottom->mChildListItr=mChildList.insert(top->mChildListItr,bottom);
}
// FUNCTION: WON 0x006f6010
unsigned char Container::GetNextFocusId()
{
    if(++mNextFocusId==0) ++mNextFocusId;
    return mNextFocusId;
}
}
