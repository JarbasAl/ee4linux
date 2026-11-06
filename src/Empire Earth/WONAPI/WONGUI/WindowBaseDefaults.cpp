// Complete original-backed GUI values; source-only, no runtime substitution.
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
// FUNCTION: WON 0x00729e60
SmartPtr<Font> Window::Virtual04(const FontDescriptor&) { return NULL; }
// SUPPORT-ONLY: EXE 0x008142a0
void Window::Virtual10() {}
// SUPPORT-ONLY: EXE 0x008142a0
void Window::Virtual14() {}
// SUPPORT-ONLY: EXE 0x006fa500
void Window::Virtual18(Cursor*) {}
// SUPPORT-ONLY: EXE 0x006fa500
void Window::Virtual1C(bool) {}
// SUPPORT-ONLY: EXE 0x006fa500
void Window::Virtual20(bool) {}
// SUPPORT-ONLY: EXE 0x006fa500
void Window::Virtual24(bool) {}
// SUPPORT-ONLY: EXE 0x006fa500
void Window::Virtual28(bool) {}
// FUNCTION: WON 0x007272f0
void Window::SetPosSize(int,int,int,int) {}
// FUNCTION: WON 0x00727440
void Window::GetRect(WONRectangle&,bool) {}
// SUPPORT-ONLY: EXE 0x008142a0
void Window::Virtual3C() {}
// SUPPORT-ONLY: EXE 0x006fa500
void Window::Virtual44(bool) {}
// FUNCTION: WON 0x0072b0f0
void Window::Virtual48(WindowEventBase* event) { event->mWindow=this; mManager->PostEvent(event); }
// FUNCTION: WON 0x00729e80
bool Window::Virtual4C() { return mIsShowing; }
}
