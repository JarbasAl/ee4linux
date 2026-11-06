// Genuine complete native Table140 construction, lifecycle support only.
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
// SUPPORT-ONLY: EXE 0x0070f9d0
Table::Table(LineSegment* contents)
{
    mContents=contents;
    SetBorderPadding(0,0,0,0);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0070f9d0
// (public: __thiscall WONAPI::Table::Table(class WONAPI::LineSegment *)    [??0Table@WONAPI@@QAE@PAVLineSegment@1@@Z])
// TEMPLATE: WON 0x0070fbb0
// (public: virtual __thiscall WONAPI::Table::~Table(void)    [??1Table@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0070fad0
// (public: virtual void * __thiscall WONAPI::Table::`scalar deleting destructor'(unsigned int)    [??_GTable@WONAPI@@UAEPAXI@Z])
