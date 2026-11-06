// Genuine SDK skin control constructor completion; support0.
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
#include "ee/wonapi/WONLobbySkinFrame.h"
#include "ee/wonapi/WONLobbySkinListControls.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
#include "ee/wonapi/WONLobbySkinTab.h"
namespace WONAPI {
// SUPPORT: EXE 0x007191a0; native115B, actual header button defaults after real WONButton base.
WONHeaderButton::WONHeaderButton()
{
    SetHorzAlign(HorzAlign_Left);
    mLeftTextPad=mRightTextPad=4;
    mTopTextPad=mBottomTextPad=4;
    SetComponentFlags(8,false);
}
// SUPPORT: EXE 0x00719540; native32B, true vertical argument forwarded to full Scrollbar base.
WONScrollbar::WONScrollbar(bool vertical) : Scrollbar(vertical)
{
    mIsTransparent=false;
}
// SUPPORT: EXE 0x0071a830; native181B, real TabCtrl, SkinSP, Frame and BackSP automatic construction.
WONTabCtrl::WONTabCtrl(WONSkin* skin)
{
    mSkin=skin;
    mIsTransparent=false;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007191a0
// (public: __thiscall WONAPI::WONHeaderButton::WONHeaderButton(void)    [??0WONHeaderButton@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00719540
// (public: __thiscall WONAPI::WONScrollbar::WONScrollbar(bool)    [??0WONScrollbar@WONAPI@@QAE@_N@Z])
// TEMPLATE: WON 0x0071a830
// (public: __thiscall WONAPI::WONTabCtrl::WONTabCtrl(class WONAPI::WONSkin *)    [??0WONTabCtrl@WONAPI@@QAE@PAVWONSkin@1@@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00719220
// (public: virtual void * __thiscall WONAPI::WONHeaderButton::`scalar deleting destructor'(unsigned int)    [??_GWONHeaderButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0071a8f0
// (public: virtual void * __thiscall WONAPI::WONTabCtrl::`scalar deleting destructor'(unsigned int)    [??_GWONTabCtrl@WONAPI@@UAEPAXI@Z])
