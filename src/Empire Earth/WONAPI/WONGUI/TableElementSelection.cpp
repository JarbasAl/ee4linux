// Actual table-element selection indexing over existing typed rows and cells.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTableElement.h"
namespace WONAPI {
// FUNCTION: WON 0x0070f500
void TableElement::SetSelIndex()
{
	LineSegment::SetSelIndex();
	int aNumColumns = mColumnVector.size();
	int aNumRows = mRowVector.size();
	int aRow, aCol;
	
	for(aRow=0; aRow<aNumRows; aRow++)
	{
		RowInfo &aRowInfo = mRowVector[aRow];
		for(aCol=0; aCol<aNumColumns; aCol++)
		{
			MultiLineElement *aCell = GetCellAt(aRowInfo,aCol);
			if(aCell!=NULL)
				aCell->SetSelIndex();
		}
	}
	mEndSelIndex = mCounter-1;
}

}
