// Complete original-backed Component source completion; all selected methods support0.
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
// SUPPORT: EXE 0x00705540; native28B, complete88 Container plus ScrollArea owner.
Scroller::Scroller() :mScrollArea(NULL) {}
// Genuine SDK implicit destructor supplied by actual complete class/real constructor; no authored body.
// SUPPORT: EXE 0x00705700; native55B, true default scrollbar state.
ScrollbarScroller::ScrollbarScroller()
{
    mVertCondition=mHorzCondition=ScrollbarCondition_Enable;
    mSettingScrollbarConditions=false;
}
// Genuine SDK implicit destructor supplied by actual complete class/real constructor; no authored body.
// SUPPORT: EXE 0x00705e10; native114B, completeb8 native defaults; drag coordinates untouched.
Scrollbar::Scrollbar(bool vertical)
{
    mPosition=0;
    mPageSize=0;
    mTotalSize=0;
    mLineUpScrollSize=mLineDownScrollSize=10;
    mPageScrollSize=50;
    mResizeThumb=true;
    mVertical=vertical;
    mMinThumbSize=10;
}
// SUPPORT: EXE 0x00705eb0; native227B, reverse thumb/rect/down/up owners then Container.
Scrollbar::~Scrollbar() {}
// Genuine SDK implicit destructor supplied by actual complete class/real constructor; no authored body.
// SUPPORT: EXE 0x00706a20; native28B, complete88 with borrowed current-screen pointer.
ScreenContainer::ScreenContainer() { mCurScreen=NULL; }
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00705540
// (public: __thiscall WONAPI::Scroller::Scroller(void)    [??0Scroller@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00705560
// (public: virtual void * __thiscall WONAPI::Scroller::`scalar deleting destructor'(unsigned int)    [??_GScroller@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00705700
// (public: __thiscall WONAPI::ScrollbarScroller::ScrollbarScroller(void)    [??0ScrollbarScroller@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00705740
// (public: virtual void * __thiscall WONAPI::ScrollbarScroller::`scalar deleting destructor'(unsigned int)    [??_GScrollbarScroller@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00705e10
// (public: __thiscall WONAPI::Scrollbar::Scrollbar(bool)    [??0Scrollbar@WONAPI@@QAE@_N@Z])
// TEMPLATE: WON 0x00705e90
// (public: virtual void * __thiscall WONAPI::Scrollbar::`scalar deleting destructor'(unsigned int)    [??_GScrollbar@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00705eb0
// (public: virtual __thiscall WONAPI::Scrollbar::~Scrollbar(void)    [??1Scrollbar@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x00706a20
// (public: __thiscall WONAPI::ScreenContainer::ScreenContainer(void)    [??0ScreenContainer@WONAPI@@QAE@XZ])
