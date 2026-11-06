// Genuine current LineSegment virtual API completion; source-only.
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
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x006c7b10
int LineSegment::GetWidth() { return 0; }
// SUPPORT-ONLY: EXE 0x0070a7a0
int LineSegment::GetMinWidth() { return GetWidth(); }
// SUPPORT-ONLY: EXE 0x0070a7a0
int LineSegment::GetMaxWidth() { return GetWidth(); }
// SUPPORT-ONLY: EXE 0x006c7b10
int LineSegment::GetHeight() { return 0; }
// FUNCTION: WON 0x0070b840
int LineSegment::GetAscent() { return GetHeight(); }
// FUNCTION: WON 0x0070a7a0
int LineSegment::GetFirstWrapWidth() { return GetWidth(); }
// SUPPORT-ONLY: EXE 0x0070a7b0
char LineSegment::GetVertAlign() { return 0; }
// FUNCTION: WON 0x0070a7c0
int LineSegment::GetStartSelIndex() { return mStartSelIndex; }
// FUNCTION: WON 0x0070a7c0
int LineSegment::GetEndSelIndex() { return mStartSelIndex; }
// SUPPORT-ONLY: EXE 0x006fa2a0
void LineSegment::MouseEnter(int,int) {}
// SUPPORT-ONLY: EXE 0x00726b60
void LineSegment::MouseExit() {}
// SUPPORT-ONLY: EXE 0x007a6880
void LineSegment::MouseDown(int,int,MouseButton) {}
// SUPPORT-ONLY: EXE 0x007a6880
void LineSegment::MouseUp(int,int,MouseButton) {}
// SUPPORT-ONLY: EXE 0x006fa2a0
void LineSegment::MouseMove(int,int) {}
// SUPPORT-ONLY: EXE 0x006fa2a0
void LineSegment::MouseDrag(int,int) {}
// SUPPORT-ONLY: EXE 0x00726b60
void LineSegment::LinkDown() {}
// SUPPORT-ONLY: EXE 0x00726b60
void LineSegment::LinkUp() {}
// SUPPORT-ONLY: EXE 0x006f30d0
bool LineSegment::CanStartSelection() { return true; }
// FUNCTION: WON 0x0070b850
bool LineSegment::GetSelCharPos(int&,int&,int,int) { return false; }
// SUPPORT-ONLY: EXE 0x00727440
void LineSegment::GetSelText(GUIString&,ElementSelectionInfo&) {}
// SUPPORT-ONLY: EXE 0x007a90d0
bool LineSegment::FindStr(ElementFindInfo&) { return false; }
// FUNCTION: WON 0x0070a7f0
LineSegment* LineSegment::GetSegmentAt(int&,int&,bool) { return this; }
// SUPPORT-ONLY: EXE 0x0070a7b0
bool LineSegment::IsInvalid() { return false; }
// SUPPORT-ONLY: EXE 0x0070a7b0
bool LineSegment::AllowModalInput() { return false; }
// FUNCTION: WON 0x00787ff0
bool LineSegment::PerformInstructions(MultiLineBuilder*) { return true; }
// FUNCTION: WON 0x0070b860
void LineSegment::Paint(Graphics&,int,int,ElementPaintInfo&) {}
// SUPPORT-ONLY: EXE 0x0070a800
void LineSegment::SetMaxWidth(int) {}
}
