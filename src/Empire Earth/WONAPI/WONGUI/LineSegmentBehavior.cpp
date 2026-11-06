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
// FUNCTION: WON 0x007098d0
int LineSegment::GetSelChar(int x,int y)
{
    if(x>=GetWidth()/2) return 1;
    else return 0;
}
// FUNCTION: WON 0x00709520
void LineSegment::Wrap(MultiLineBuilder* builder,bool needSegmentOnFirstLine)
{
    if(!needSegmentOnFirstLine) builder->AddLine();
    builder->AddSegmentToCurLine(this);
}
// FUNCTION: WON 0x00709550
void LineSegment::SetViewport(ScrollAreaViewport*,int x,int y)
{
    mAbsX=x;
    mAbsY=y;
}
}

// Genuine SDK HTML instruction and line-break selection callbacks.
#include "ee/wonapi/WONLobbyHTMLSegments.h"
#include "ee/wonapi/WONLobbyHTMLAnchorSegment.h"
namespace WONAPI {
// FUNCTION: WON 0x007099c0
bool AlignLineSegment::PerformInstructions(MultiLineBuilder* builder)
{
    builder->SetLineAlignment(mAlign);
    return false;
}
// FUNCTION: WON 0x007099d0
bool MarginLineSegment::PerformInstructions(MultiLineBuilder* builder)
{
    builder->SetIndentSize(mLeftMargin,mNextLeftMargin);
    return false;
}
// FUNCTION: WON 0x007099f0
bool AnchorLineSegment::PerformInstructions(MultiLineBuilder*)
{
    return true;
}
// FUNCTION: WON 0x00709a00
void AnchorLineSegment::SetViewport(ScrollAreaViewport* viewport,int x,int y)
{
    LineSegment::SetViewport(viewport,x,y);
    if(viewport!=NULL) {
        Table* table=(Table*)viewport->GetParent();
        table->AddAnchor(mName,x,y);
    }
}
// FUNCTION: WON 0x00709a40
void LineBreakSegment::GetSelText(GUIString& text,ElementSelectionInfo& info)
{
    if(mStartSelIndex<info.mStartSelSegment || mStartSelIndex>info.mEndSelSegment)
        return;
    if(mHeight>0) text.append("\r\n\r\n");
    else text.append("\r\n");
}
}
