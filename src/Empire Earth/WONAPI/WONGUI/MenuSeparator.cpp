// Actual separator rendering and complete owned component-list-item insertion.
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
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/WONLobbyComponentListItem.h"
namespace WONAPI {
// FUNCTION: WON 0x00700720
void MSSeperator::Paint(Graphics& g)
{
    Component::Paint(g);
    int y=Height()/2-1;
    if(y<0) y=0;
    g.SetSysColor(StandardColor_3DShadow);
    g.DrawLine(0,y,Width()-1,y);
    ++y;
    g.SetSysColor(StandardColor_3DHilight);
    g.DrawLine(0,y,Width()-1,y);
}

// FUNCTION: WON 0x006fa510
void ComponentListItem::Virtual04(Graphics& g,ListArea* area)
{
    if(mComponent.get()!=NULL)
    {
        if(mIsSizable)
            mComponent->Fn2F4B90(area->mL12C,mComponent->Height());
        mComponent->Virtual68();
        mComponent->Paint(g);
    }
}

}

// Compiler-generated and header-inline functions this unit emits:
