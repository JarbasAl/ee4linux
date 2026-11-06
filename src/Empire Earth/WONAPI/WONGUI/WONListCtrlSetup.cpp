// Genuine complete WONListCtrlc0 lifecycle, retained header linkage and original typed clone; support0.
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
#include "ee/wonapi/WONLobbySkinListControls.h"
namespace WONAPI {
// SUPPORT: EXE 0x0071a270; native32B, original real base/member construction then header height default.
WONListCtrl::WONListCtrl() { mHeaderHeight=0; }
// Actual SDK implicit normal31a2b0/304B is compiler generated from complete HeaderSP/base graph.
// FUNCTION: WON 0x0071a3e0
void WONListCtrl::SetHeader(WONHeaderControl* header)
{
    mHeader=header;
    Fn2F6A60(header);
}
// FUNCTION: WON 0x0071a520
WONListCtrl* WONListCtrl::Duplicate()
{
    WONListCtrl* result=new WONListCtrl;
    result->mHeaderHeight=mHeaderHeight;
    return result;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0071a270
// (public: __thiscall WONAPI::WONListCtrl::WONListCtrl(void)    [??0WONListCtrl@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x0071a290
// (public: virtual void * __thiscall WONAPI::WONListCtrl::`scalar deleting destructor'(unsigned int)    [??_GWONListCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0071a2b0
// (public: virtual __thiscall WONAPI::WONListCtrl::~WONListCtrl(void)    [??1WONListCtrl@WONAPI@@UAE@XZ])
