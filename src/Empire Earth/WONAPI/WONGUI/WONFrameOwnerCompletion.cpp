// Native predefined child configuration dispatch; no runtime authority.
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
#include "ee/wonapi/WONLobbyWONListConfig.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x007175f0 (145B real uniform frame owner constructor)
WONFrame::WONFrame(Image* image,int size)
{
    mImage=image;
    mLeft=mTop=mRight=mBottom=size;
}
// SUPPORT-ONLY: EXE 0x00717690 (157B real frame owner constructor)
WONFrame::WONFrame(Image* image,int left,int top,int right,int bottom)
{
    mImage=image;
    mLeft=left;
    mTop=top;
    mRight=right;
    mBottom=bottom;
}
// SUPPORT-ONLY: EXE 0x007177a0 (19B genuine SDK deep-copy assignment forwarding)
WONFrame& WONFrame::operator=(const WONFrame& frame)
{
    Copy(frame);
    return *this;
}
// SUPPORT-ONLY: EXE 0x0071a5b0 (117B complete44 component/frame owner construction)
WONFrameComponent::WONFrameComponent(Image* image,int left,int top,int right,int bottom)
{
    VirtualAC(image,left,top,right,bottom);
}
// Actual SDK destructor is implicit; preserve natural compiler member/base cleanup.
}

#include "ee/wonapi/WONLobbyStretchImage.h"
namespace WONAPI {
// SUPPORT: EXE 0x00717550; genuine SDK retained StretchImage frame composition,155B.
WONFrame::WONFrame(StretchImage* image)
{
    mImage=image;
    mLeft=image->GetLeft();
    mTop=image->GetTop();
    mRight=image->GetRight();
    mBottom=image->GetBottom();
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00717550
// (public: __thiscall WONAPI::WONFrame::WONFrame(class WONAPI::StretchImage *)    [??0WONFrame@WONAPI@@QAE@PAVStretchImage@1@@Z])
// TEMPLATE: WON 0x007175f0
// (public: __thiscall WONAPI::WONFrame::WONFrame(class WONAPI::Image *,int)    [??0WONFrame@WONAPI@@QAE@PAVImage@1@H@Z])
// TEMPLATE: WON 0x00717690
// (public: __thiscall WONAPI::WONFrame::WONFrame(class WONAPI::Image *,int,int,int,int)    [??0WONFrame@WONAPI@@QAE@PAVImage@1@HHHH@Z])
// TEMPLATE: WON 0x007177a0
// (public: class WONAPI::WONFrame & __thiscall WONAPI::WONFrame::operator=(class WONAPI::WONFrame const &)    [??4WONFrame@WONAPI@@QAEAAV01@ABV01@@Z])
// TEMPLATE: WON 0x0071a5b0
// (public: __thiscall WONAPI::WONFrameComponent::WONFrameComponent(class WONAPI::Image *,int,int,int,int)    [??0WONFrameComponent@WONAPI@@QAE@PAVImage@1)
// TEMPLATE: WON 0x0071a6b0
// (public: virtual void * __thiscall WONAPI::WONFrameComponent::`scalar deleting destructor'(unsigned int)    [??_GWONFrameComponent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0071a6d0
// (public: virtual __thiscall WONAPI::WONFrameComponent::~WONFrameComponent(void)    [??1WONFrameComponent@WONAPI@@UAE@XZ])
