// Table traversal and span mutation over original-owned cells.
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
#include "ee/wonapi/WONLobbyTableElement.h"
namespace WONAPI {
// Support0 necessary original traversal helper.
// FUNCTION: WON 0x0070e000
void TableElement::NextRow()
{
	mCurRow++;
	mCurCol = -1;
	EnsureNumRows(mCurRow+1);
}

// FUNCTION: WON 0x0070E020
TableElement::CellInfo* TableElement::NextColumn()
{
	if(mCurRow<0)
		NextRow();

	RowInfo &aRowInfo = mRowVector[mCurRow];
	CellInfo *aCellInfo;
	do
	{
		mCurCol++;
		EnsureNumCols(mCurCol+1);
		aCellInfo = GetCellInfoAt(aRowInfo,mCurCol);
		mCurCell = aCellInfo->mContents.get();
	} while(mCurCell==NULL);

	return aCellInfo;
}

// FUNCTION: WON 0x0070E080
void TableElement::SetCellSpan(int theRowSpan, int theColumnSpan, bool truncate)
{
	if(mCurCell!=NULL)
	{
		if(truncate)
		{
			if(mCurRow + theRowSpan > mRowVector.size())
				theRowSpan = mRowVector.size()-mCurRow;

			if(mCurCol + theColumnSpan > mColumnVector.size())
				theColumnSpan = mColumnVector.size()-mCurCol;
		}
		else
		{
			EnsureNumCols(mCurCol + theColumnSpan);
			EnsureNumRows(mCurRow + theRowSpan);
		}

		for(int i=0; i<theRowSpan; i++)
		{
			for(int j=0; j<theColumnSpan; j++)
			{
				CellInfo* anInfo = GetCellInfoAt(mCurRow+i,mCurCol+j);
				if(i!=0 || j!=0)
				{
					anInfo->mContents = NULL;
					anInfo->mColumnSpan = -j;
					anInfo->mRowSpan = -i;
				}
			}
		}

		CellInfo *anInfo = GetCellInfoAt(mCurRow,mCurCol);
		anInfo->mColumnSpan = theColumnSpan;
		anInfo->mRowSpan = theRowSpan;
	}
}
}
