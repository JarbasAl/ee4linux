// Genuine borrowed-font metrics and current table cursor forwarding, support only.
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
#include "ee/wonapi/WONLobbyTextSegment.h"
namespace WONAPI {
int LineSegment::mCounter;
Table* LineSegment::mTableContext;
// FUNCTION: WON 0x00739720
int TextLineSegment::GetHeight() { return mFont->GetHeight(); }
// FUNCTION: WON 0x00739730
int TextLineSegment::GetAscent() { return mFont->GetAscent(); }
// FUNCTION: WON 0x0070b310
void TextLineSegment::MouseEnter(int,int)
{
    mTableContext->Virtual78(Cursor::GetStandardCursor(StandardCursor_IBeam));
}
}
