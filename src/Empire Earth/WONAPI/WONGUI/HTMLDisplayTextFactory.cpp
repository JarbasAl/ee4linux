// Original plain/linked text-segment creation; complete factory support0.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <ctype.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONLobbyTextLinkSegment.h"
namespace WONAPI {
// FUNCTION: WON 0x00739540
TextLineSegment* HTMLDisplayGen::GetTextLineSegment(const GUIString& text,int color,Font* font)
{
    if(mInAnchor)
        return new TextLinkLineSegment(mCurLinkParam.get(),text,color,mDefaultALinkColor,font);
    return new TextLineSegment(text,color,font);
}
}

// FUNCTION: WON 0x00739740
// (??_GTextLineSegment@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00739760
// (??1TextLineSegment@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x007397f0
// (??_GTextLinkLineSegment@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00739810
// (??1TextLinkLineSegment@WONAPI@@UAE@XZ)
