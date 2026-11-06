// Genuine complete multiline dimension and selection endpoint queries, support only.
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
// FUNCTION: WON 0x0070c8f0
int MultiLineElement::GetWidth() { return mWidth; }
// FUNCTION: WON 0x0070c900
int MultiLineElement::GetHeight() { return mHeight; }
// FUNCTION: WON 0x0070cc30
int MultiLineElement::GetEndSelIndex()
{
    if(!mSegmentList.empty()) return mSegmentList.back().mSegment->GetEndSelIndex();
    else return mStartSelIndex;
}
}

namespace WONAPI {
// Genuine SDK explicit empty calculation hook, called by both real width queries.
// SUPPORT-CONTEXT: EXE 0x008142a0, actual shared RET1; no global alias or new-primary claim.
void MultiLineElement::CalcMinMaxWidths() {}
// FUNCTION: WON 0x0070d000
int MultiLineElement::GetMinWidth()
{
    if(mMinWidth==0) CalcMinMaxWidths();
    return mMinWidth;
}
// FUNCTION: WON 0x0070d020
int MultiLineElement::GetMaxWidth()
{
    if(mMaxWidth==0) CalcMinMaxWidths();
    return mMaxWidth;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; zero Native/primary/physical credit.

bool MultiLineElement::IsLastLineEmpty()
{
	if(mSegmentList.empty())
		return true;
	else
		return (mSegmentList.back().mFlags & 0x0008 /* SDK LineSegmentFlag_PostLineBreak */)?true:false;
}
}
