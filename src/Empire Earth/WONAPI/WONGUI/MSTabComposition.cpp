// Actual tab-control frame/background/arrow composition, native3010e0.
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
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyMSPainters.h"
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// FUNCTION: WON 0x007010e0
MSTabCtrl::MSTabCtrl()
{
    mIsTransparent=false;
    SmartPtr<MS3DFrame> frame=new MS3DFrame(1,true);
    SmartPtr<MSDlgBack> back=new MSDlgBack;
    Fn2F6140(back.get(),0x5003,mContainerLayout.get(),0,0,0,0,0);
    Fn2F6140(frame.get(),0x5003,mContainerLayout.get(),0,0,0,0,0);
    Fn2F6A60(back.get());
    Fn2F6A60(frame.get());
    SmartPtr<Button> left=new MSArrowButton(Direction_Left);
    SmartPtr<Button> right=new MSArrowButton(Direction_Right);
    left->Fn2F4B90(15,15);
    right->Fn2F4B90(15,15);
    SetTabArrows(left.get(),right.get());
}
// Original301660/13, primitive setter support0.
// FUNCTION: WON 0x00701660
void MSTabCtrl::SetTransparent(bool transparent)
{
    mIsTransparent=transparent;
}
// Original301670/5, genuine AddedToParent forwarding support0.
// FUNCTION: WON 0x00701670
void MSTabCtrl::Virtual6C()
{
    TabCtrl::Virtual6C();
}
// Original301680/110, allocation/forwarding support0.
// FUNCTION: WON 0x00701680
void MSTabCtrl::AddStandardTab(const GUIString& text,Container* container)
{
    AddTab(new MSTabButton(text),container);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fd3e0
// (public: virtual void * __thiscall WONAPI::MSDlgBack::`scalar deleting destructor'(unsigned int)    [??_GMSDlgBack@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00701320
// (public: virtual void * __thiscall WONAPI::MSTabCtrl::`scalar deleting destructor'(unsigned int)    [??_GMSTabCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00706860
// (public: virtual __thiscall WONAPI::MSDlgBack::~MSDlgBack(void)    [??1MSDlgBack@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00701340
// (public: virtual __thiscall WONAPI::TabCtrl::~TabCtrl(void))
// TEMPLATE: WON 0x007014d0
// (public: virtual __thiscall WONAPI::MSTabCtrl::~MSTabCtrl(void))
