// Table selected-text export and keyboard scrolling over original-owned receivers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextSegment.h"
namespace WONAPI {
// FUNCTION: WON 0x0070fce0
void Table::CopySelection()
{
    GUIString text;
    if(mContents.get()!=NULL)
    {
        ElementSelectionInfo selection;
        selection.mStartSelSegment=mStartSelSegment;
        selection.mStartSelSegmentPos=mStartSelSegmentPos;
        selection.mEndSelSegment=mEndSelSegment;
        selection.mEndSelSegmentPos=mEndSelSegmentPos;
        mContents->GetSelText(text,selection);
    }
    if(text.length()!=0) Clipboard::Set(text);
}
// FUNCTION: WON 0x0070fdc0
bool Table::Virtual50(int key)
{
    if(ScrollArea::Virtual50(key)) return true;
    if(key==3)
    {
        CopySelection();
        return true;
    }
    return false;
}
// FUNCTION: WON 0x0070fe00
bool Table::Virtual54(int key)
{
    if(ScrollArea::Virtual54(key)) return true;
    switch(key)
    {
    case VK_RIGHT: mHorzOffset+=10; break;
    case VK_LEFT: mHorzOffset-=10; break;
    case VK_DOWN: mVertOffset+=mDownScrollAmount; break;
    case VK_UP: mVertOffset-=mUpScrollAmount; break;
    case VK_NEXT: mVertOffset+=mScrollArea.height; break;
    case VK_PRIOR: mVertOffset-=mScrollArea.height; break;
    default: return false;
    }
    VirtualCC(false);
    return true;
}
}