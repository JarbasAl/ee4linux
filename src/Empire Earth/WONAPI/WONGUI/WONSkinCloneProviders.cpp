// Genuine SDK skin duplicate and frame-copy constructor providers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyMSSkinImage.h"
#include "ee/wonapi/WONLobbyWONInputBox.h"
#include "ee/wonapi/WONLobbySkinListControls.h"
namespace WONAPI {
// FUNCTION: WON 0x00718d40
WONButton* WONButton::Duplicate()
{
    WONButton* copy=new WONButton;
    copy->CopyAttributes(this);
    return copy;
}
// FUNCTION: WON 0x00719350
WONHeaderButton* WONHeaderButton::Duplicate()
{
    WONHeaderButton* button=new WONHeaderButton;
    button->CopyAttributes(this);
    return button;
}
// SUPPORT-ONLY: EXE 0x0071a740 (101B actual constructor; zero primary quota)
WONFrameComponent::WONFrameComponent(const WONFrame& frame)
{
    mFrame.Copy(frame);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0071a740
// (public: __thiscall WONAPI::WONFrameComponent::WONFrameComponent(class WONAPI::WONFrame const &)    [??0WONFrameComponent@WONAPI@@QAE@ABVWONFrame@1@@Z])
