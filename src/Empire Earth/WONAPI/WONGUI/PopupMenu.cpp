// Popup menu default configuration and real background ownership; support0.
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
#include "ee/wonapi/WONLobbyMSPainters.h"
namespace WONAPI {
// Native2fd150/458, initialization-only support0; mLastX/Y intentionally untouched.
PopupMenu::PopupMenu()
{
    mFont=GetNamedFont("Menu");
    Fn2FB600(0x806,true);
    mBackground=Background(new MSPopup);
    mTextColor=0x8000000c;
    mMouseUpCount=0;
    mTimerAcc=0;
    mKeyboardMode=false;
    SetBorderPadding(4,4,4,4);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fd320
// (public: virtual void * __thiscall WONAPI::PopupMenu::`scalar deleting destructor'(unsigned int)    [??_GPopupMenu@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006fd340
// (public: virtual __thiscall WONAPI::PopupMenu::~PopupMenu(void)    [??1PopupMenu@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fd150
// (public: __thiscall WONAPI::PopupMenu::PopupMenu(void)    [??0PopupMenu@WONAPI@@QAE@XZ])
