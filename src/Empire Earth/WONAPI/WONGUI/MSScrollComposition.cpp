// Native MS scrollbar and scroll-container construction/composition policy.
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
namespace WONAPI {
// FUNCTION: WON 0x006ff820
MSScrollbar::MSScrollbar(bool vertical) : Scrollbar(vertical)
{
    mIsTransparent=false;
    if(vertical) {
        mUpArrow=ArrowButtonFactory(Direction_Up);
        mDownArrow=ArrowButtonFactory(Direction_Down);
    } else {
        mUpArrow=ArrowButtonFactory(Direction_Left);
        mDownArrow=ArrowButtonFactory(Direction_Right);
    }
    if(mUpArrow->Width()==0 || mUpArrow->Height()==0)
        mUpArrow->Fn2F4BC0(15,15);
    if(mDownArrow->Width()==0 || mDownArrow->Height()==0)
        mDownArrow->Fn2F4BC0(15,15);
    mThumb=ThumbFactory(mVertical);
    mScrollRect=new Button;
    Fn2F4B90(mUpArrow->Width(),mUpArrow->Height());
}
// FUNCTION: WON 0x006ffeb0
MSScrollContainer::MSScrollContainer(ScrollArea* scrollArea)
{
    mVertScrollbar=new MSScrollbar(true);
    mHorzScrollbar=new MSScrollbar(false);
    mScrollArea=scrollArea;
    mHeaderHeight=0;
    mDraw3DFrame=true;
    mDrawFrame=true;
    mTransparent=false;
}
// Original2ffa60/13, primitive setter support0.
// FUNCTION: WON 0x006ffa60
void MSScrollbar::SetTransparent(bool transparent)
{
    mIsTransparent=transparent;
}
// Original300250/28, guarded setter/callback support0.
// FUNCTION: WON 0x00700250
void MSScrollContainer::SetDraw3DFrame(bool on)
{
    if(mDraw3DFrame==on) return;
    mDraw3DFrame=on;
    Virtual08();
}
// Original300270/28, guarded setter/callback support0.
// FUNCTION: WON 0x00700270
void MSScrollContainer::SetDrawFrame(bool on)
{
    if(mDrawFrame==on) return;
    mDrawFrame=on;
    Virtual08();
}
}

namespace WONAPI {
// SUPPORT: EXE 0x006fdc40; genuine SDK default arrow allocation policy, native131B.
SmartPtr<Button> DefaultMSArrowButtonFactory(Direction direction)
{
    return new MSArrowButton(direction);
}
// Genuine original mutable SDK factory cells; data only, no code-byte quota.
MSArrowButtonFactory MSScrollbar::ArrowButtonFactory=DefaultMSArrowButtonFactory;
MSScrollbar::ThumbFactoryFunc MSScrollbar::ThumbFactory=MSScrollbar::DefaultThumbFactory;
MSArrowButtonFactory MSComboBox::ArrowButtonFactory=DefaultMSArrowButtonFactory;
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006fccb0
// (public: static class WONAPI::SmartPtr<class WONAPI::Component> __cdecl WONAPI::MSScrollbar::DefaultThumbFactory(bool)    [?DefaultThumbFactory@MSScrol)
// TEMPLATE: WON 0x006fdc40
// (class WONAPI::SmartPtr<class WONAPI::Button> __cdecl WONAPI::DefaultMSArrowButtonFactory(enum WONAPI::Direction)    [?DefaultMSArrowButtonFactory@WONA)
// TEMPLATE: WON 0x006ffa50
// (public: virtual __thiscall WONAPI::MSScrollbar::~MSScrollbar(void)    [??1MSScrollbar@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007001e0
// (public: virtual __thiscall WONAPI::Scroller::~Scroller(void)    [??1Scroller@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00700040
// (public: virtual void * __thiscall WONAPI::MSScrollContainer::`scalar deleting destructor'(unsigned int)    [??_GMSScrollContainer@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00700060
// (public: virtual __thiscall WONAPI::ScrollbarScroller::~ScrollbarScroller(void)    [??1ScrollbarScroller@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x00719560
// (public: virtual void * __thiscall WONAPI::MSScrollbar::`scalar deleting destructor'(unsigned int)    [??_GMSScrollbar@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00700120
// (public: virtual __thiscall WONAPI::MSScrollContainer::~MSScrollContainer(void)    [??1MSScrollContainer@WONAPI@@UAE@XZ])
