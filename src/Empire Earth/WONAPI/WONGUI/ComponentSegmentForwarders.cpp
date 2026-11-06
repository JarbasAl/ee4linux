// Genuine ComponentLineSegment dimension, mouse forwarding and flag predicates.
// All methods are support-only; no primary byte/function quota.
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
#include "ee/wonapi/WONLobbyHTMLImageSegments.h"
namespace WONAPI {
// FUNCTION: WON 0x0073a990
int ComponentLineSegment::GetWidth() { return mComponent->Width(); }
// FUNCTION: WON 0x0073a9a0
int ComponentLineSegment::GetHeight() { return mComponent->Height(); }
// FUNCTION: WON 0x0073a900
void ComponentLineSegment::MouseEnter(int x,int y) { mComponent->Virtual24(x,y); }
// FUNCTION: WON 0x0073a920
void ComponentLineSegment::MouseExit() { mComponent->Virtual28(); }
// FUNCTION: WON 0x0073a930
void ComponentLineSegment::MouseDown(int x,int y,MouseButton button) { mComponent->Virtual2C(x,y,button); }
// FUNCTION: WON 0x0070a840
void ComponentLineSegment::MouseUp(int x,int y,MouseButton button) { mComponent->Virtual30(x,y,button); }
// FUNCTION: WON 0x0073a950
void ComponentLineSegment::MouseMove(int x,int y) { mComponent->Virtual34(x,y); }
// FUNCTION: WON 0x0070a880
void ComponentLineSegment::MouseDrag(int x,int y) { mComponent->Virtual38(x,y); }
// FUNCTION: WON 0x0073a970
bool ComponentLineSegment::IsInvalid() { return mComponent->mInvalidState!=0; }
// FUNCTION: WON 0x0070a7b0
bool ComponentLineSegment::CanStartSelection() { return false; }
// FUNCTION: WON 0x0073a980
bool ComponentLineSegment::AllowModalInput() { return mComponent->ComponentFlagSet(0x8000); }
}
