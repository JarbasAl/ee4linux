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
#include "ee/wonapi/WONLobbySkinFrame.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x00717990 (113B genuine declared SDK simple button constructor)
WONSimpleButton::WONSimpleButton()
{
    mScaleImage=false;
    mCheckMouseButtonDown=false;
    Fn2F3370(1,true); //actual ButtonFlag_InvalidateOnMouseOver value1, genuine SetButtonFlags provider
}
// SUPPORT-ONLY: EXE 0x007194a0 (26B genuine declared SDK thumb constructor)
WONScrollThumb::WONScrollThumb()
{
    SetCheckMouseButtonDown(true);
    SetScaleImage(true);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00717990
// (public: __thiscall WONAPI::WONSimpleButton::WONSimpleButton(void)    [??0WONSimpleButton@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007194a0
// (public: __thiscall WONAPI::WONScrollThumb::WONScrollThumb(void)    [??0WONScrollThumb@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00717a10
// (public: virtual void * __thiscall WONAPI::WONSimpleButton::`scalar deleting destructor'(unsigned int)    [??_GWONSimpleButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007194c0
// (public: virtual void * __thiscall WONAPI::WONScrollThumb::`scalar deleting destructor'(unsigned int)    [??_GWONScrollThumb@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00717a30
// (public: virtual __thiscall WONAPI::WONSimpleButton::~WONSimpleButton(void))
// TEMPLATE: WON 0x007194e0
// (public: virtual __thiscall WONAPI::WONScrollThumb::~WONScrollThumb(void))
