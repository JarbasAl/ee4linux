// Source-only complete table cell growth; original-backed MultiLineElement construction.
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
// FUNCTION: WON 0x0070da90
void TableElement::EnsureNumCols(RowInfo& row,int numCols)
{
    int oldNumCols=row.mRow.size();
    if(numCols<oldNumCols) return;
    row.mRow.reserve(numCols);
    for(int i=oldNumCols;i<numCols;++i)
        row.mRow.push_back(CellInfo());
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007243c0
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::TableElement::CellInfo,int,struct WONAPI::TableElement::CellInfo *,struct WONAPI::TableElement)
// TEMPLATE: WON 0x00724450
// (class std::_Ptrit<struct WONAPI::TableElement::CellInfo,int,struct WONAPI::TableElement::CellInfo *,struct WONAPI::TableElement::CellInfo &,struct WON)
