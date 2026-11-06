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
// FUNCTION: WON 0x0072e7c0
void WindowManager::GetNextEvent() { Sleep(20); }
// FUNCTION: WON 0x0072c380
bool WindowManager::SetScreenResolutionHook(int,int) { return false; }
// SUPPORT-ONLY: EXE 0x00727440
void WindowManager::GetScreenResolutionHook(int&,int&) {}
// SUPPORT-ONLY: EXE 0x00726b60
void WindowManager::Signal() {}
// SUPPORT-ONLY: EXE 0x0070a800
void WindowManager::GetScreenRect(WONRectangle&) {}
// SUPPORT-ONLY: EXE 0x00726b60
void WindowManager::GetSystemColorScheme() {}
}
