// Native selection colors and foreground mode behavior; direct initialization/getter helpers are support0.
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
// Native3494a0/23 support-only field initialization.
SelectionColor::SelectionColor()
    :mBackgroundColor(0x80000007),mForegroundColor(0x80000008),mMode(ForegroundSelectionMode_UseForegroundColor) {}
// Native3494c0/25 support-only field initialization.
SelectionColor::SelectionColor(DWORD background,ForegroundSelectionMode mode)
    :mBackgroundColor(background),mForegroundColor(0xffffff),mMode(mode) {}
// Native3494e0/25 support-only field initialization.
SelectionColor::SelectionColor(DWORD background,DWORD foreground)
    :mBackgroundColor(background),mForegroundColor(foreground),mMode(ForegroundSelectionMode_UseForegroundColor) {}
// FUNCTION: WON 0x007494e0
// (??0SelectionColor@WONAPI@@QAE@KK@Z)

// Native349500/5 direct getter; Graphics argument genuinely unused, support0.
// FUNCTION: WON 0x00749500
unsigned long SelectionColor::GetBackgroundColor(Graphics&) const { return mBackgroundColor; }
// FUNCTION: WON 0x00749510
unsigned long SelectionColor::GetForegroundColor(Graphics&,unsigned long original) const
{
    switch(mMode)
    {
    case ForegroundSelectionMode_UseOriginalColor: return original;
    case ForegroundSelectionMode_InvertOriginalColor: return original^0xffffff;
    case ForegroundSelectionMode_UseForegroundColor: return mForegroundColor;
    default: return 0xffffff;
    }
}
}
