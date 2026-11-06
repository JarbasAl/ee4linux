// Complete genuine TextSegmentBase queries and defaults on the current target contract.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x006be2e0
int TextSegmentBase::GetLength() { return 1; }
// FUNCTION: WON 0x00726b60
void TextSegmentBase::CalcDimensions() {}
// SUPPORT-ONLY: EXE 0x0070a800
void TextSegmentBase::PerformInstructions(TextArea*) {}
// SUPPORT-ONLY: EXE 0x006fa2a0
void TextSegmentBase::Paint(Graphics&,TextArea*) {}
// FUNCTION: WON 0x00710a60
int TextSegmentBase::CharAt(int x)
{
    if(x<mx+mWidth/2) return mAbsStartChar;
    else return mAbsEndChar;
}
// SUPPORT-ONLY: EXE 0x0070a7b0
bool TextSegmentBase::IsLink() { return false; }
// FUNCTION: WON 0x00726b60
void TextSegmentBase::Pressed() {}
// SUPPORT-ONLY: EXE 0x0070a7b0
bool TextSegmentBase::Released() { return false; }
// FUNCTION: WON 0x00710a90
const GUIString& TextSegmentBase::GetLinkParam() { return gEmptyGUIString; }
// SUPPORT-ONLY: EXE 0x007d02f0
TextSegmentBase* TextSegmentBase::GetActual() { return this; }
}
