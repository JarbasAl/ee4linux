// Existing typed Component defaults and adapters; all support0.
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
// SUPPORT: EXE 0x008142a0; actual Component slot04 folded empty PosChanged.
void Component::Virtual04() {}
// FUNCTION: WON 0x006f5050
bool Component::Virtual20(int,int)
{
    return IsVisible() && !ComponentFlagSet(0x800);
}
// SUPPORT: EXE 0x004ac9c8; actual slot38 empty MouseDrag.
void Component::Virtual38(int,int) {}
// SUPPORT: EXE 0x00620a18; actual TimerEvent always true.
bool Component::Virtual44(int) { return true; }
// SUPPORT: EXE 0x00418aa9; actual AllowWindowClose always true.
bool Component::Virtual5C() { return true; }
// SUPPORT: EXE 0x00556dd8; actual EnableHook empty bool hook.
void Component::Virtual74(bool) {}
// FUNCTION: WON 0x006f4c40
void Component::Virtual78(Cursor* cursor)
{
    if(mParent!=NULL) mParent->Virtual78(cursor);
}
// FUNCTION: WON 0x006f52c0
void Component::Virtual80(ComponentEvent* event)
{
    HandleComponentEvent(event);
}
// SUPPORT: EXE 0x005ad67e; genuine GetMouseChild null default.
Component* Component::Virtual98() { return NULL; }
// FUNCTION: WON 0x006f52a0
void Component::Fn2F52A0(ComponentEvent* event,bool sendNow)
{
    event->mBroadcast=true;
    Fn2F53F0(event,sendNow);
}
// FUNCTION: WON 0x006f4c20
void Component::EnsureOnTopOf(Component* sibling)
{
    if(mParent!=NULL) mParent->EnsureOnTopOf(sibling,this);
}
// FUNCTION: WON 0x006f4fb0
void Component::SetTranslucentColor(int color,int level)
{
    SetComponentFlags(0x200,true);
    if(!CheckGrabBG()) return;
    mExtraStuff->mGrabBG->SetTranslucentColor(color,level);
}

}
