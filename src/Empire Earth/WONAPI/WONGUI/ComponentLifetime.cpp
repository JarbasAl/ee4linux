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
// FUNCTION: WON 0x006f4890
ComponentExtra::ComponentExtra()
{
    mGrabBG=NULL;
    mDontDoTip=false;
}
// SUPPORT: EXE 0x006f48d0; native263B, delete BG before reverse SmartPtr member cleanup.
ComponentExtra::~ComponentExtra() { delete mGrabBG; }
// SUPPORT: EXE 0x006f4a70; native78B, unique Extra owner then RefCount base cleanup.
Component::~Component() { delete mExtraStuff; }
// FUNCTION: WON 0x006f4ac0
void Component::AllocateExtraStuff()
{
    if(mExtraStuff==NULL) mExtraStuff=new ComponentExtra;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006f48b0
// (public: virtual void * __thiscall WONAPI::ComponentExtra::`scalar deleting destructor'(unsigned int)    [??_GComponentExtra@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006f48d0
// (public: virtual __thiscall WONAPI::ComponentExtra::~ComponentExtra(void)    [??1ComponentExtra@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x006f4a70
// (public: virtual __thiscall WONAPI::Component::~Component(void)    [??1Component@WONAPI@@UAE@XZ])
