// Genuine SDK control initialization, mouse refresh and frame/corner attributes; support0.
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
#include "ee/wonapi/WONLobbySkinnedContainers.h"
namespace WONAPI {
// FUNCTION: WON 0x006f7910
Dialog::Dialog()
{
    mDragX=mDragY=0;
    SetComponentFlags(0x118,true);
    mAllowDrag=true;
    mKeepInRoot=true;
    mEndOnEscape=false;
    mEscapeId=-1;
}
// FUNCTION: WON 0x006f66c0
void Container::MouseAt()
{
    if(GetRoot()->mMouseIsInClientArea) {
        int x,y;
        GetMousePos(x,y);
        Virtual34(x,y);
    }
}
// SUPPORT: EXE 0x00719c50; native97B, real ScrollbarScroller/Frame/corner automatic construction.
WONScrollContainer::WONScrollContainer()
{
    mHeaderHeight=0;
    mTransparent=false;
}
// FUNCTION: WON 0x00719f70
void WONScrollContainer::CopyAttributes(WONScrollContainer* from)
{
    mFrame=from->mFrame;
    SetCorner(from->mCorner.get());
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f7970
// (public: virtual void * __thiscall WONAPI::Dialog::`scalar deleting destructor'(unsigned int)    [??_GDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00719c50
// (public: __thiscall WONAPI::WONScrollContainer::WONScrollContainer(void)    [??0WONScrollContainer@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00719cc0
// (public: virtual void * __thiscall WONAPI::WONScrollContainer::`scalar deleting destructor'(unsigned int)    [??_GWONScrollContainer@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00719ce0
// (public: virtual __thiscall WONAPI::WONScrollContainer::~WONScrollContainer(void))
