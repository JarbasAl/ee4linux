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
// FUNCTION: WON 0x006f4e70
void Component::Virtual0C(int& width,int& height) { width=20; height=20; }
// FUNCTION: WON 0x006f4f20
void Component::SetDesiredHeight(int padding)
{
    int width=0,height=0;
    Virtual0C(width,height);
    Fn2F4B90(Width(),height+padding);
}
// FUNCTION: WON 0x006f5380
void Component::FireEvent(int type,bool sendNow)
{
    Fn2F53F0(new ComponentEvent(this,type),sendNow);
}
// FUNCTION: WON 0x006f5550
void Component::HandleComponentEvent(ComponentEvent* event) { Fn2F54E0(event); }
// FUNCTION: WON 0x006f57b0
bool Component::CheckDoubleClick(int button)
{
    Window* aWindow=GetRoot();
    return aWindow->CheckDoubleClick(static_cast<MouseButton>(button));
}
// FUNCTION: WON 0x0072b110
bool Window::CheckDoubleClick(MouseButton button) { return mButtonDoubleClick[button]; }
}
