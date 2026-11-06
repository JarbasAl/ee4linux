// Existing CenterLayout20 borrowed geometry algorithm; no allocation or owner changes.
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
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// FUNCTION: WON 0x007375a0
void CenterLayout::Execute(Component* parent)
{
    if(mComponentList.empty())
        return;
    std::list<Component*>::iterator it;
    WONRectangle rectangle;
    for(it=mComponentList.begin();it!=mComponentList.end();++it)
    {
        Component* child=*it;
        rectangle.DoUnion(child->GetBounds());

    }
    int dx=0,dy=0;
    Component* center=parent;
    if(mCenterBox!=NULL && mCenterBox!=parent)
    {
        center=mCenterBox;
        if(mHorz) dx=center->Left();
        if(mVert) dy=center->Top();
    }
    if(mHorz) dx+=(center->Width()-rectangle.width)/2-rectangle.x;
    if(mVert) dy+=(center->Height()-rectangle.height)/2-rectangle.y;
    if(dx==0 && dy==0)
        return;
    for(it=mComponentList.begin();it!=mComponentList.end();++it)
    {
        Component* child=*it;
        child->Fn2F4B70(child->Left()+dx,child->Top()+dy);
    }
}
}
