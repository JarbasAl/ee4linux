// Table geometry, spanning, painting, selection and search on original-owned records.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextSegment.h"
#include "ee/wonapi/WONLobbyTableElement.h"
namespace WONAPI {

// Support0 genuine SDK helper; no separate authored quota.
// FUNCTION: WON 0x0070dec0
// FUNCTION: WON 0x0070df10
TableElement::CellInfo* TableElement::GetCellInfoAt(RowInfo &theInfo, int theCol)
{
	if(theCol<0 || theCol>=theInfo.mRow.size())
		return NULL;

	return &(theInfo.mRow[theCol]);
}

// Support0 genuine SDK helper; no separate authored quota.
TableElement::CellInfo* TableElement::GetCellInfoAt(int theRow, int theCol)
{
	if(theRow<0 || theRow>=mRowVector.size() || theCol<0 || theCol>=mColumnVector.size())
		return NULL;
	
	RowInfo &anInfo = mRowVector[theRow];
	if(theCol>=anInfo.mRow.size())
		return NULL;

	return &(anInfo.mRow[theCol]);
}

// Support0 genuine SDK helper; no separate authored quota.
// FUNCTION: WON 0x0070dfc0
MultiLineElement* TableElement::GetCellAt(int theRow, int theCol)
{
	CellInfo *anInfo = GetCellInfoAt(theRow, theCol);
	if(anInfo==NULL)
		return NULL;

	return anInfo->mContents.get();
}

// Support0 genuine SDK helper; no separate authored quota.
MultiLineElement* TableElement::GetCellAt(RowInfo &theInfo, int theCol)
{
	CellInfo *anInfo = GetCellInfoAt(theInfo,theCol);
	if(anInfo==NULL)
		return NULL;
	else
		return anInfo->mContents.get();
}

// Support0 genuine SDK helper; no separate authored quota.
// FUNCTION: WON 0x0070e320
int TableElement::FindRowAt(int theYPos)
{
	mSearchRow.my = theYPos;
	RowVector::iterator anItr = std::lower_bound(mRowVector.begin(), mRowVector.end(), mSearchRow, RowSearchPred());
	if(anItr==mRowVector.end() || anItr->my>theYPos)
	{
		if(anItr!=mRowVector.begin())
			--anItr;
	}

	return anItr - mRowVector.begin();
}

// Support0 genuine SDK helper; no separate authored quota.
// FUNCTION: WON 0x0070eca0
int TableElement::GetMinWidth() 
{ 
	if(mMinWidth==0)
		CalcMinMaxWidths();

	if(mWidthSpec>0)
		return mWidthSpec;
	else 
		return mMinWidth; 
}

// Support0 genuine SDK helper; no separate authored quota.
// FUNCTION: WON 0x0070ecc0
int TableElement::GetMaxWidth() 
{ 
	if(mMaxWidth==0)
		CalcMinMaxWidths();

	if(mWidthSpec>0)
		return mWidthSpec;
	else
		return mMaxWidth; 
}

// Support0 genuine SDK helper; no separate authored quota.
static void DrawShadedRect(Graphics &g, int x, int y, int width, int height, int size, bool up)
{
	g.Translate(x,y);
	width--;
	height--;
	int light = 0xd6d6d6;
	int dark = 0x848484;
	for(int i=0; i<size; i++)
	{
		g.SetColor(up?light:dark);
		g.DrawLine(i,i,width-i,i);
		g.DrawLine(i,i,i,height-i);
		g.SetColor(up?dark:light);
		g.DrawLine(width-i,i,width-i,height);
		g.DrawLine(i,height-i,width,height-i);
	}
	g.Translate(-x,-y);
}

// FUNCTION: WON 0x0070E8C0
void TableElement::CalcMinMaxWidths()
{
	mMinWidth = mMaxWidth = 0;
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	if(aNumRows==0 || aNumColumns==0)
		return;	

	int aRow, aCol;
	int aPad = mCellPadding*2 + (mBorder?4:0);
	for(aCol=0; aCol<aNumColumns; aCol++)
		mColumnVector[aCol].mMinWidth = mColumnVector[aCol].mMaxWidth = 0;

	// first pass
	for(aCol=0; aCol<aNumColumns; aCol++) 
	{
		ColumnInfo &aColumnInfo = mColumnVector[aCol];
		int aMaxMinWidth = aColumnInfo.mMinWidth;
		int aMaxMaxWidth = aColumnInfo.mMaxWidth;
		for(aRow=0; aRow<aNumRows; aRow++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRow,aCol);
			if(aCellInfo!=NULL && aCellInfo->mColumnSpan==1)
			{
				MultiLineElement *aCell = aCellInfo->mContents.get();
				if(aCell!=NULL)
				{
					int aMinWidth = aCell->GetMinWidth();
					int aMaxWidth = aCell->GetMaxWidth();
					if(aCellInfo->mNoWrap)
						aMinWidth = aMaxWidth; 

					if(aCellInfo->mWidth > 0)
					{
						if(aMinWidth < aCellInfo->mWidth)
							aMaxWidth = aCellInfo->mWidth;
						else
							aMaxWidth = aMinWidth;
					}

					if(aMinWidth > aMaxMinWidth)
						aMaxMinWidth = aMinWidth;

					if(aMaxWidth > aMaxMaxWidth)
						aMaxMaxWidth = aMaxWidth;					
				}
			}
		}

		aColumnInfo.mMinWidth = aMaxMinWidth + aPad;
		aColumnInfo.mMaxWidth = aMaxMaxWidth + aPad;
		mMinWidth += aColumnInfo.mMinWidth;
		mMaxWidth += aColumnInfo.mMaxWidth;
	}

	// second pass
	for(aCol=0; aCol<aNumColumns; aCol++) 
	{
		for(aRow=0; aRow<aNumRows; aRow++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRow,aCol);
			if(aCellInfo!=NULL && aCellInfo->mColumnSpan>1)
			{
				MultiLineElement *aCell = aCellInfo->mContents.get();
				if(aCell!=NULL)
				{
					int aMinWidth = aCell->GetMinWidth();
					int aMaxWidth = aCell->GetMaxWidth();
					if(aCellInfo->mNoWrap)
						aMinWidth = aMaxWidth; 

					if(aCellInfo->mWidth > 0)
					{
						if(aMinWidth < aCellInfo->mWidth)
							aMaxWidth = aCellInfo->mWidth;
						else
							aMaxWidth = aMinWidth;
					}

					// Figure out how much MinWidth and MaxWidth is available from all the columns
					int i;
					int anAvailableMinWidth = 0;
					int aTotalMaxWidth = 0;
					for(i=0; i<aCellInfo->mColumnSpan; i++)
					{
						anAvailableMinWidth += mColumnVector[aCol+i].mMinWidth;
						aTotalMaxWidth += mColumnVector[aCol+i].mMaxWidth;
					}
					anAvailableMinWidth += (aCellInfo->mColumnSpan-1)*mCellSpacing - aPad;

					// Need to distribute more MinWidth to the columns
					int aNeededExtra = aMinWidth - anAvailableMinWidth;
					if(aNeededExtra>0)
					{
						int aPool = aNeededExtra;
						mMinWidth += aNeededExtra;
						for(i=0; i<aCellInfo->mColumnSpan; i++)
						{						
							ColumnInfo &aColumnInfo = mColumnVector[aCol+i];
							int anAmount;
							if(i!=aCellInfo->mColumnSpan-1)
							{
								if(aTotalMaxWidth>0)
									anAmount = (aMinWidth - anAvailableMinWidth)*aColumnInfo.mMaxWidth/aTotalMaxWidth;
								else
									anAmount = (aMinWidth - anAvailableMinWidth)/aCellInfo->mColumnSpan;

								aColumnInfo.mMinWidth += anAmount;
								aPool -= anAmount;
							}
							else
								aColumnInfo.mMinWidth += aPool;
						}
					}

					// Need to distribute more MaxWidth to the columns
					aNeededExtra = aMaxWidth - aTotalMaxWidth;
					if(aNeededExtra > 0)
					{
						int aPool = aNeededExtra;
						mMaxWidth += aNeededExtra;
						for(i=0; i<aCellInfo->mColumnSpan; i++)
						{
							ColumnInfo &aColumnInfo = mColumnVector[aCol+i];
							int anAmount;
							if(i!=aCellInfo->mColumnSpan-1)
							{
								if(aTotalMaxWidth>0)
									anAmount = (aMaxWidth - aTotalMaxWidth)*aColumnInfo.mMaxWidth/aTotalMaxWidth;
								else
									anAmount = (aMaxWidth - aTotalMaxWidth)/aCellInfo->mColumnSpan;

								aPool -= anAmount;
							}
							else
								anAmount = aPool;

							aColumnInfo.mMaxWidth += anAmount;
						}
					}

				}
			}
		}
	}


	mMinWidth += mCellSpacing*(aNumColumns+1) + 2*mBorder;
	mMaxWidth += mCellSpacing*(aNumColumns+1) + 2*mBorder;
	if(mWidthSpec>0)
	{
		if(mWidthSpec<mMinWidth)
			mWidthSpec = mMinWidth;
	}
}

// FUNCTION: WON 0x0070ECE0
void TableElement::CalcRowHeights()
{
	int aRow, aCol;
	int aNumRows = mRowVector.size(), aNumColumns = mColumnVector.size();

	// First pass
	mHeight = mBorder + mCellSpacing;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		aRowInfo.my = mHeight;
		int aMaxHeight = 0;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRowInfo,aCol);
			MultiLineElement *aCell = aCellInfo->mContents.get();
			if(aCell!=NULL)
			{
				ColumnInfo &aColumnInfo = mColumnVector[aCol];
				int aColumnWidth = aColumnInfo.mWidth;
				if(aCellInfo->mColumnSpan>1)
				{
					ColumnInfo &anEndColumn = mColumnVector[aCol + aCellInfo->mColumnSpan - 1];
					aColumnWidth = anEndColumn.mx + anEndColumn.mWidth - aColumnInfo.mx;
				}

				aCell->SetMaxWidth(aColumnWidth - mCellPadding*2);
				if(aCellInfo->mRowSpan==1)
				{
					int aHeight = aCell->GetHeight();
					if(aCellInfo->mHeight>aHeight)
						aHeight = aCellInfo->mHeight;
					if(aHeight>aMaxHeight)
						aMaxHeight = aHeight;
				}
			}
		}
		aRowInfo.mHeight = aMaxHeight+mCellPadding*2 + (mBorder?4:0);
		mHeight += aRowInfo.mHeight + mCellSpacing;
	}
	mHeight+=mBorder;

	// Second pass
	mHeight = mBorder + mCellSpacing;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		aRowInfo.my = mHeight;
		int aMaxHeight = aRowInfo.mHeight-mCellPadding*2;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			CellInfo *aCellInfo = GetCellInfoAt(aRowInfo,aCol);
			MultiLineElement *aCell = aCellInfo->mContents.get();
			if(aCell!=NULL && aCellInfo->mRowSpan>1)
			{
				int aHeight = aCell->GetHeight();
				if(aCellInfo->mHeight>aHeight)
					aHeight = aCellInfo->mHeight;

				int i = 0;
				int anAvailableHeight = 0;
				for(i=0; i<aCellInfo->mRowSpan; i++)
					anAvailableHeight += mRowVector[aRow+i].mHeight;

				anAvailableHeight += mCellSpacing*(aCellInfo->mRowSpan-1);
				anAvailableHeight -= mCellPadding*2;
				
				if(aHeight > anAvailableHeight)
					aRowInfo.mHeight += aHeight - anAvailableHeight;
			}
		}
		mHeight += aRowInfo.mHeight + mCellSpacing;
	}

	mHeight+=mBorder;
/**/
}

// FUNCTION: WON 0x0070EF50
void TableElement::CalcColumnWidths(int theWidth)
{
	int aNumColumns = mColumnVector.size();
	if(theWidth<mMinWidth)
		theWidth = mMinWidth;

	int aCol;
	int anExtraSpace = theWidth-mMinWidth, aColumnWeight = 0;
	int anOriginalExtraSpace = anExtraSpace;
	int anAvailableWidth = theWidth - 2*mBorder - (aNumColumns+1)*mCellSpacing;
	int aWidthForPercent = 0;
	int aTotalPercent = 0;
	int aNonPercentMaxWidth = mMaxWidth, aNonPercentMinWidth = mMinWidth;
	bool allColumnsPercent = true;
	for(aCol=0; aCol<aNumColumns; aCol++)
	{
		ColumnInfo &anInfo = mColumnVector[aCol];
		if(anInfo.mWeight < 0)
		{
			int aPercent = -anInfo.mWeight;
			anInfo.mWidth = anAvailableWidth*aPercent/100;
			if(anInfo.mWidth < anInfo.mMinWidth)
				anInfo.mWidth = anInfo.mMinWidth;
//			else
				aTotalPercent += aPercent;

			aWidthForPercent += anInfo.mWidth - anInfo.mMinWidth;
			aNonPercentMinWidth -= anInfo.mMinWidth - mCellSpacing;
			aNonPercentMaxWidth -= anInfo.mMaxWidth - mCellSpacing;
		}
		else
			allColumnsPercent = false;
	}

	if((aWidthForPercent > anExtraSpace) || (allColumnsPercent && aTotalPercent!=100))
	{
		int aLeftOver = anExtraSpace;
		ColumnInfo *aLeftOverColumn = NULL;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			ColumnInfo &anInfo = mColumnVector[aCol];
			if(anInfo.mWeight<0 /*&& anInfo.mWidth>anInfo.mMinWidth*/)
			{
				int anAmount = anExtraSpace*-anInfo.mWeight/aTotalPercent;
				anInfo.mWidth = anInfo.mMinWidth + anAmount;
				aLeftOver -= anAmount;
				aLeftOverColumn = &anInfo;
			}
		}
		if(aLeftOverColumn!=NULL && aLeftOver>0)
			aLeftOverColumn->mWidth += aLeftOver;
		
		anExtraSpace = 0;
	}
	else 
		anExtraSpace -= aWidthForPercent;


	if(anExtraSpace>=aNonPercentMaxWidth-aNonPercentMinWidth)
	{
		if(mWidthSpec!=0)
			anExtraSpace -= aNonPercentMaxWidth-aNonPercentMinWidth;
		else
			anExtraSpace = 0;

		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			ColumnInfo &anInfo = mColumnVector[aCol];
			if(anInfo.mWeight>=0)
			{
				anInfo.SetWidthToMax();
				anInfo.mWeight = anInfo.mMaxWidth;
				aColumnWeight += anInfo.mWeight;
			}
		}
	}
	else 
	{
	//	anExtraSpace = theWidth-aNonPercentMinWidth;
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			ColumnInfo &anInfo = mColumnVector[aCol];
			if(anInfo.mWeight>=0)
			{
				anInfo.SetWidthToMin();
				anInfo.mWeight = anInfo.mMaxWidth - anInfo.mMinWidth;
				aColumnWeight += anInfo.mWeight;
			}
		}
	}

	int aLeftOver = anExtraSpace;
	mWidth = mBorder + mCellSpacing;
	for(aCol=0; aCol<aNumColumns; aCol++)
	{
		ColumnInfo &aColInfo = mColumnVector[aCol];
		if(aColumnWeight>0 && aColInfo.mWeight>=0)
		{
			int anAmount = anExtraSpace*aColInfo.mWeight/aColumnWeight;
			aColInfo.mWidth += anAmount;
			aLeftOver -= anAmount;
		}

		aColInfo.mx = mWidth;
		mWidth += aColInfo.mWidth + mCellSpacing;
	}
	mWidth+=mBorder;
	if(aLeftOver>0 && aLeftOver!=anOriginalExtraSpace)
	{
		ColumnInfo &aColInfo = mColumnVector.back();
		aColInfo.mWidth += aLeftOver;
		mWidth += aLeftOver;
	}
}

// FUNCTION: WON 0x0070F230
void TableElement::SetMaxWidth(int theWidth)
{
	mWidth = 0;
	mHeight = 0;
		
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	if(aNumRows==0 || aNumColumns==0)
		return;

	if(mMinWidth==0)
		CalcMinMaxWidths();

	if(mWidthSpec>0)
		theWidth = mWidthSpec;
	else if(mWidthSpec<0) // percentage
		theWidth = (theWidth*-mWidthSpec)/100;

	CalcColumnWidths(theWidth);
	CalcRowHeights();
}

// FUNCTION: WON 0x0070F2E0
void TableElement::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			CellInfoEx aCellInfoEx;
			if(GetCellInfoEx(aCellInfoEx,aRow,aCol,aRowInfo))
			{
				MultiLineElement *aCell = aCellInfoEx.mCellInfo->mContents.get();
				aCell->SetViewport(theViewport,x+aCellInfoEx.mCellX,y+aCellInfoEx.mCellY);
			}
		}
	}
}

// FUNCTION: WON 0x0070F3E0
LineSegment* TableElement::GetSegmentAt(int &x, int &y, bool lenient)
{

	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();

	int aRow = FindRowAt(y);
	if(aRow>=aNumRows)
		return this;

	RowInfo &aRowInfo = mRowVector[aRow];
	if(!lenient && (aRowInfo.my>y || aRowInfo.my+aRowInfo.mHeight<y))
		return this;

	CellInfoEx aCellInfoEx;
	for(int aCol=0; aCol<aNumColumns; aCol++)
	{
		ColumnInfo &aColumnInfo = mColumnVector[aCol];
		if(aColumnInfo.mx + aColumnInfo.mWidth > x || (lenient && aCol==aNumColumns-1))
		{
			if(GetCellInfoEx(aCellInfoEx,aRow,aCol,aRowInfo,true,true))
			{
				x-=aCellInfoEx.mCellX;
				y-=aCellInfoEx.mCellY;
				return aCellInfoEx.mCellInfo->mContents->GetSegmentAt(x,y,lenient);
			}
			else
				return this;
		}
	}
			
	return this;
}

// FUNCTION: WON 0x0070F5D0
void TableElement::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{

	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	bool done = false;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL && aCell->GetEndSelIndex()>=theInfo.mStartSelSegment)
			{
				done = true; 
				break;
			}
		}

		if(done)
			break;
	}

	done = false;
	for(; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(; aCol<aNumColumns && !done; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL)
			{
				if(aCell->mStartSelIndex>=theInfo.mEndSelSegment)
				{
					done = true;
					break;
				}
				else
				{
					aCell->GetSelText(theText,theInfo);
					theText.append(" ");
				}
			}
		}
		aCol = 0;
		if(!done)
			theText.append("\r\n");
		else
			break;
	}
}

// FUNCTION: WON 0x0070F880
bool TableElement::FindStr(ElementFindInfo &theInfo)
{
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	bool done = false;
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL && aCell->GetEndSelIndex()>=theInfo.mStartSelSegment)
			{
				done = true; 
				break;
			}
		}

		if(done)
			break;
	}

	done = false;
	for(; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(; aCol<aNumColumns && !done; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL)
			{
				theInfo.mFoundStartSegment = -1;
				if(aCell->mStartSelIndex>=theInfo.mEndSelSegment)
					return false;
				else if(aCell->FindStr(theInfo))
						return true;
			}
		}
		aCol = 0;
	}

	theInfo.mFoundStartSegment = -1;
	return false;
}

// FUNCTION: WON 0x0070E3B0
bool TableElement::GetCellInfoEx(CellInfoEx &theInfo, int theRow, int theCol, RowInfo &theRowInfo, bool seekUp, bool seekLeft)
{
	CellInfo *aCellInfo = GetCellInfoAt(theRowInfo,theCol);
	if(aCellInfo==NULL)
		return false;

	MultiLineElement *aCell = aCellInfo->mContents.get();
	if(aCell==NULL) 
	{
		// See if it's a cell which spans multiple rows or columns
		bool spanner = false;
		RowInfo *aRowInfo = &theRowInfo;
		if(seekUp && aCellInfo->mRowSpan<0)
		{
			theRow = theRow + aCellInfo->mRowSpan;
			aRowInfo = &mRowVector[theRow];
			spanner = true;
		}

		if(seekLeft && aCellInfo->mColumnSpan<0)
		{
			theCol = theCol + aCellInfo->mColumnSpan;
			spanner = true;
		}

		if(!spanner)
			return false;

		return GetCellInfoEx(theInfo,theRow,theCol,*aRowInfo);
	}

	ColumnInfo &aColumnInfo = mColumnVector[theCol];

	int cellx = aColumnInfo.mx;
	int celly = theRowInfo.my;
	int cellw = aColumnInfo.mWidth;
	int cellh = theRowInfo.mHeight;

	if(aCellInfo->mColumnSpan>1)
	{
		ColumnInfo &anEndColumn = mColumnVector[theCol + aCellInfo->mColumnSpan - 1];
		cellw = anEndColumn.mx + anEndColumn.mWidth - aColumnInfo.mx;
	}
	if(aCellInfo->mRowSpan>1)
	{
		RowInfo &anEndRow = mRowVector[theRow + aCellInfo->mRowSpan - 1];
		cellh = anEndRow.my + anEndRow.mHeight - theRowInfo.my;
	}

	char anHAlign = aCellInfo->mHAlign, aVAlign = aCellInfo->mVAlign;
	if(anHAlign==-1) anHAlign = theRowInfo.mHAlign;
	if(aVAlign==-1) aVAlign = theRowInfo.mVAlign;

	int dx, dy;
	int aPad = mCellPadding + (mBorder?2:0);
	switch(anHAlign)
	{
		case 1: dx = (cellw-aCell->GetWidth())/2; break;
		case 2: dx = cellw-aCell->GetWidth()-aPad; break;
		default: dx = aPad;
	}

	switch(aVAlign)
	{
		case 1: dy = (cellh-aCell->GetHeight())/2; break;
		case 2: dy = cellh-aCell->GetHeight()-aPad; break;
		default: dy = aPad;
	}


	if(dx<aPad)
		dx = aPad;
	if(dy<aPad)
		dy = aPad;

	theInfo.mCellInfo = aCellInfo;
	theInfo.mBorderX = cellx;
	theInfo.mBorderY = celly;
	theInfo.mBorderWidth = cellw;
	theInfo.mBorderHeight = cellh;
	theInfo.mCellX = cellx+dx;
	theInfo.mCellY = celly+dy;
	return true;
}

// FUNCTION: WON 0x0070E590
void TableElement::PaintCell(Graphics &g, int x, int y, int theRow, int theCol, RowInfo &theRowInfo, ElementPaintInfo &thePaintInfo, bool seekUp)
{
	CellInfoEx aCellInfoEx;
	if(!GetCellInfoEx(aCellInfoEx,theRow,theCol,theRowInfo,seekUp))
		return;

	CellInfo *aCellInfo = aCellInfoEx.mCellInfo;
	MultiLineElement *aCell = aCellInfo->mContents.get();

	int aBGColor = aCellInfo->mBGColor;
	if(aBGColor==-1) aBGColor = theRowInfo.mBGColor;
	if(aBGColor>=0)
	{
		g.SetColor(aBGColor);
		g.FillRect(x + aCellInfoEx.mBorderX,y + aCellInfoEx.mBorderY,aCellInfoEx.mBorderWidth,aCellInfoEx.mBorderHeight);
	}

	if(mBorder)
		DrawShadedRect(g,x + aCellInfoEx.mBorderX,y + aCellInfoEx.mBorderY,aCellInfoEx.mBorderWidth,aCellInfoEx.mBorderHeight,1,false);

	aCell->Paint(g,x + aCellInfoEx.mCellX,y + aCellInfoEx.mCellY,thePaintInfo);
}

// FUNCTION: WON 0x0070E750
void TableElement::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	if(mBGColor!=-1)
	{
		g.SetColor(mBGColor);
		g.FillRect(x,y,mWidth,mHeight);
	}
	DrawShadedRect(g,x,y,mWidth,mHeight,mBorder,true);

	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	int aBottom = (theInfo.mClipY+theInfo.mClipHeight) - y;

	
	int aTop = theInfo.mClipY - y;
	int aStartRow = 0;
	if(aTop > 0)
		aStartRow = FindRowAt(aTop);

	for(aRow=aStartRow; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		if(aRowInfo.my > aBottom)
			break;

		for(aCol=0; aCol<aNumColumns; aCol++)
			PaintCell(g,x,y,aRow,aCol,aRowInfo,theInfo,aRow==aStartRow);
	}
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; zero Native/primary/physical credit.

MultiLineElement* TableElement::NextCell()
{
	if(mCurRow >= mRowVector.size())
		return NULL;

	while(true)
	{
		if(mCurCol < mColumnVector.size()-1)
			mCurCol++;
		else
		{
			mCurRow++;
			mCurCol = 0;
		}
		if(mCurRow >= mRowVector.size())
			return NULL;

		mCurCell = GetCellAt(mCurRow, mCurCol);
		if(mCurCell!=NULL)
			return mCurCell;
	}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; zero Native/primary/physical credit.

void TableElement::SetCellBGColor(int theColor)
{
	if(mCurCell!=NULL)
	{
		CellInfo *anInfo = GetCellInfoAt(mCurRow,mCurCol);
		anInfo->mBGColor = theColor;
	}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; zero Native/primary/physical credit.

void TableElement::SetCellAlign(int theHAlign, int theVAlign)
{
	if(mCurCell!=NULL)
	{
		CellInfo *anInfo = GetCellInfoAt(mCurRow,mCurCol);
		anInfo->mHAlign = theHAlign;
		anInfo->mVAlign = theVAlign;
	}
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable API body, zero Native/primary/physical credit.
void TableElement::SetSize(int numRows, int numCols)
{
	mRowVector.clear();
	mColumnVector.clear();
	EnsureNumCols(numCols);
	EnsureNumRows(numRows);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable API body, zero Native/primary/physical credit.
void TableElement::Rewind()
{
	mCurRow = mCurCol = 0;
	mCurCell = GetCellAt(mCurRow, mCurCol);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable API body, zero Native/primary/physical credit.
void TableElement::AddSegment(LineSegment *theSegment, unsigned short theFlags)
{
	if(mCurCell!=NULL)
	{
		mCurCell->AddSegment(theSegment, theFlags);
	}
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0070e670
// (void __cdecl WONAPI::DrawShadedRect(class WONAPI::Graphics &,int,int,int,int,int,bool)    [?DrawShadedRect@WONAPI@@YAXAAVGraphics@1@HHHHH_N@Z])
// TEMPLATE: WON 0x00725f70
// (void __cdecl std::_Distance(class std::_Ptrit<struct WONAPI::TableElement::RowInfo,int,struct WONAPI::TableElement::RowInfo *,struct WONAPI::TableElem)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00725510
// (class std::_Ptrit<struct WONAPI::TableElement::RowInfo,int,struct WONAPI::TableElement::RowInfo *,struct WONAPI::TableElement::RowInfo &,struct WONAPI)
// TEMPLATE: WON 0x00725e20
// (void __cdecl std::_Distance(class std::_Ptrit<struct WONAPI::TableElement::RowInfo,int,struct WONAPI::TableElement::RowInfo *,struct WONAPI::TableElem)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00723c60
// (class std::_Ptrit<struct WONAPI::TableElement::RowInfo,int,struct WONAPI::TableElement::RowInfo *,struct WONAPI::TableElement::RowInfo &,struct WONAPI)
