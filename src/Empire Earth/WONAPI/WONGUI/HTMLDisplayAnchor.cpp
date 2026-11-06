// HTML named anchors and retained hyperlink font/link-state transitions.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONLobbyHTMLAnchorSegment.h"
namespace WONAPI {
// FUNCTION: WON 0x00739f00
void HTMLDisplayGen::HandleAnchor(HTMLAnchorTag* tag)
{
    if(tag->IsStart())
    {
        if(!tag->mName.empty())
            mDisplay->AddSegment(new AnchorLineSegment(tag->mName));
        if(tag->mHRef.length()!=0)
        {
            mInAnchor=true;
            ++mUnderlineCount;
            PushFont(NULL,-1,mDefaultLinkColor,GetIsFontMonospace());

            mCurLinkParam=new TextLinkParam(tag->mHRef,tag->mLaunchBrowser);
        }
    }
    else if(mInAnchor)
    {
        mInAnchor=false;
        --mUnderlineCount;
        PopFont();
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x0073a170
// (??1TextLinkParam@WONAPI@@UAE@XZ)
}

// FUNCTION: WON 0x0073a0c0
// (?GetVertAlign@AnchorLineSegment@WONAPI@@UAEDXZ)

// FUNCTION: WON 0x0073a0d0
// (??_GAnchorLineSegment@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0073a0f0
// (??1AnchorLineSegment@WONAPI@@UAE@XZ)

// FUNCTION: WON 0x0073a150
// (??_GTextLinkParam@WONAPI@@UAEPAXI@Z)
