// Genuine current table traversal state and row/column policy, support only.
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
#include "ee/wonapi/WONLobbyTableElement.h"
namespace WONAPI {
// FUNCTION: WON 0x0070dfe0
void TableElement::StartTraversal() { mCurRow=mCurCol=-1; mCurCell=NULL; }
// FUNCTION: WON 0x0070e070
HorzAlign TableElement::GetCurRowAlign() { return (HorzAlign)mRowVector[mCurRow].mHAlign; }
// FUNCTION: WON 0x0070e240
void TableElement::SetRowBGColor(int color)
{
    if(mCurRow<0 || mCurRow>=mRowVector.size()) return;
    mRowVector[mCurRow].mBGColor=color;
}
// FUNCTION: WON 0x0070e280
void TableElement::SetRowAlign(int horizontal,int vertical)
{
    if(mCurRow<0 || mCurRow>=mRowVector.size()) return;
    RowInfo& row=mRowVector[mCurRow];
    row.mHAlign=horizontal;
    row.mVAlign=vertical;
}
// FUNCTION: WON 0x0070e2d0
void TableElement::SetColumnWidth(int width)
{
    if(mCurCol<0 || mCurCol>=mColumnVector.size()) return;
    ColumnInfo& column=mColumnVector[mCurCol];
    if(width<column.mWeight) column.mWeight=width;
}
}
