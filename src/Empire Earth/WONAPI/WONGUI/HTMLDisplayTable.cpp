// HTML table start/end and row style transitions on real TableElement owners.
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
#include "ee/wonapi/WONLobbyHTMLSegments.h"
namespace WONAPI {
// FUNCTION: WON 0x0073afc0
void HTMLDisplayGen::HandleTable(HTMLTableTag* tag)
{
    if(tag->IsEnd())
    {
        if(mInTable)
        {
            mInTable=false;
            int flags=LineSegmentFlag_PreLineBreak;
            if(mCurTableAlign==HTMLAlign_Left) flags|=LineSegmentFlag_FloatLeft;
            else if(mCurTableAlign==HTMLAlign_Right) flags|=LineSegmentFlag_FloatRight;
            else if(mCurTableAlign==HTMLAlign_Center) PushAlign(HTMLAlign_Center);
            AddDisplaySegment(mCurTable.get(),flags);
            if(mCurTableAlign==HTMLAlign_Center) PopAlign();
            else if(mCurTableAlign==HTMLAlign_None) StartFreshLine();
        }
        else if(mParsingTableData) HandleEndTableData();
    }
    else
    {
        ClearAccumulatedSegment();
        mInTable=true;
        mCurTable=new TableElement;
        mCurTable->mCellSpacing=tag->mCellSpacing;
        mCurTable->mCellPadding=tag->mCellPadding;
        mCurTable->mBorder=tag->mBorder;
        mCurTable->mBGColor=tag->mBGColor;
        mCurTable->mWidthSpec=tag->mWidthSpec;
        mCurTableAlign=tag->mAlign;
        mCurTable->StartTraversal();
    }
}
// FUNCTION: WON 0x0073b160
void HTMLDisplayGen::HandleTableRow(HTMLTableRowTag* tag)
{
    if(tag->IsEnd())
    {
        if(mParsingTableData && !mInTable) HandleEndTableData();
        return;
    }
    if(mInTable)
    {
        mCurTable->NextRow();
        mCurTable->SetRowBGColor(tag->mBGColor);
        int horizontal=tag->mHAlign-HTMLAlign_Left;
        int vertical=tag->mVAlign-HTMLAlign_Top;
        mCurTable->SetRowAlign(horizontal,vertical);
    }
    else if(mParsingTableData) HandleEndTableData();
}
}
