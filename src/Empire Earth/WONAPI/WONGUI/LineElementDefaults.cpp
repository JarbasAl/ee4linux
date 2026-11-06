// Genuine LineElement dimension and selection endpoint queries, support only.
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
// FUNCTION: WON 0x00711470
int LineElement::GetWidth() { return mWidth; }
// FUNCTION: WON 0x0071a670
int LineElement::GetHeight() { return mHeight; }
// FUNCTION: WON 0x0070b870
int LineElement::GetAscent() { return mBaseline; }
// FUNCTION: WON 0x0070baf0
int LineElement::GetEndSelIndex()
{
    if(!mSegmentList.empty()) return mSegmentList.back()->GetEndSelIndex();
    else return mStartSelIndex;
}
}
