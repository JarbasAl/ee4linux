// HTML nested table-cell contexts and span-safe borrowed cell edits.
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
namespace WONAPI {
// FUNCTION: WON 0x0073b1d0
void HTMLDisplayGen::HandleTableData(HTMLTableDataTag* tag)
{
    if(tag->IsEnd())
    {
        if(mParsingTableData && !mInTable) HandleEndTableData();
        return;
    }
    if(mInTable)
    {
        TableElement::CellInfo* cell=mCurTable->NextColumn();
        MultiLineElement* element=cell->mContents.get();
        cell->mBGColor=tag->mBGColor;
        cell->mHAlign=(char)(tag->mHAlign-HTMLAlign_Left);
        cell->mVAlign=(char)(tag->mVAlign-HTMLAlign_Top);
        if(cell->mVAlign<0 || cell->mVAlign>2) cell->mVAlign=-1;
        if(cell->mHAlign<0 || cell->mHAlign>2) cell->mHAlign=-1;
        cell->mNoWrap=tag->mNoWrap;
        if(tag->mWidthSpec>=0) cell->mWidth=tag->mWidthSpec;
        else if(tag->mColSpan<=1) mCurTable->SetColumnWidth(tag->mWidthSpec);
        if(tag->mHeight>0) cell->mHeight=tag->mHeight;
        HTMLAlign align=(HTMLAlign)((mCurTable->GetCurRowAlign()-HorzAlign_Left)+HTMLAlign_Left);
        if(cell->mHAlign==1) align=HTMLAlign_Center;
        else if(cell->mHAlign==2) align=HTMLAlign_Right;
        // SetCellSpan may reallocate the cell vector; no cell pointer access follows.
        if(tag->mRowSpan>1 || tag->mColSpan>1)
            mCurTable->SetCellSpan(tag->mRowSpan,tag->mColSpan);
        HTMLDisplayGen generator;
        generator.GenerateTableData(*this,element,tag->GetType()==HTMLTagType_TableHeader,align);
    }
    else if(mParsingTableData) HandleEndTableData();
}
// FUNCTION: WON 0x0073b4a0
// (??1HTMLDisplayGen@WONAPI@@QAE@XZ)

// FUNCTION: WON 0x0073b660
void HTMLDisplayGen::HandleEndTableData()
{
    --mTagListItr;
    mDone=true;
}
// FUNCTION: WON 0x0073c250
void HTMLDisplayGen::GenerateTableData(HTMLDisplayGen& parent,MultiLineElement* display,bool header,HTMLAlign align)
{
    mDisplay=display;
    mWindow=parent.mWindow;
    mDefaultLinkColor=parent.mDefaultLinkColor;
    mDefaultALinkColor=parent.mDefaultALinkColor;
    mDefaultTextColor=parent.mDefaultTextColor;
    mDefaultFontFace=parent.mDefaultFontFace;
    mDefaultAlign=align;
    mBoldCount=mItalicCount=mUnderlineCount=0;
    mTagList=parent.mTagList;
    mTagListItr=parent.mTagListItr;
    mTagListEnd=parent.mTagListEnd;
    mFontSize=parent.mFontSize;
    mLeftMargin=mRightMargin=mTopMargin=mBottomMargin=0;
    if(header) mBoldCount++;
    mParsingTableData=true;
    PushFont(mDefaultFontFace.c_str(),3,-1);
    if(mDefaultAlign!=HTMLAlign_Left) PushAlign(mDefaultAlign,true);
    GenerateBase();
    parent.mTagListItr=mTagListItr;
}
}
