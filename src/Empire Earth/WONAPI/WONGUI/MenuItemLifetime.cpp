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
// FUNCTION: WON 0x006fcd30
MenuItem::MenuItem(const GUIString& text,ComponentListener* listener)
    :TextListItem(text),mListener(listener)
{
    mDisabled=false;
    mMenuId=0;
}
// Genuine SDK implicit destructor supplied by actual complete class/real constructor; no authored body.
// Genuine SDK implicit destructor supplied by actual complete class/real constructor; no authored body.
// FUNCTION: WON 0x006fd400
void PopupMenu::Reset()
{
    mCurSubMenu=(PopupMenu*)NULL;
    SetSelItem(-1,true);
}
// FUNCTION: WON 0x006fd440
// FUNCTION: WON 0x006fd460
MenuItem* PopupMenu::AddItem(const GUIString& text,int id)
{
    return AddItem(text,static_cast<ComponentListener*>(NULL),id);
}
// SUPPORT: EXE 0x006fd460; native25B, true SDK listener overload.
MenuItem* PopupMenu::AddItem(const GUIString& text,ComponentListener* listener,int id)
{
    return AddItem(text,static_cast<NativeImage*>(NULL),listener,id);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fcdc0
// (public: virtual void * __thiscall WONAPI::MenuItem::`scalar deleting destructor'(unsigned int)    [??_GMenuItem@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006fcde0
// (public: virtual __thiscall WONAPI::MenuItem::~MenuItem(void)    [??1MenuItem@WONAPI@@UAE@XZ])
