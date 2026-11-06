// Native table row/column growth; cell construction remains original-owned.
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
// FUNCTION: WON 0x0070d9c0
void TableElement::EnsureNumCols(int numCols)
{
    int oldNumCols=mColumnVector.size();
    if(numCols<oldNumCols)
        return;
    mColumnVector.resize(numCols);
    int i;
    for(i=oldNumCols;i<numCols;++i)
        mColumnVector[i].mColumnNum=i;
    for(i=0;i<mRowVector.size();++i)
        EnsureNumCols(mRowVector[i],numCols);
}

// FUNCTION: WON 0x0070dd20
void TableElement::EnsureNumRows(int numRows)
{
    int numCols=mColumnVector.size();
    int oldNumRows=mRowVector.size();
    for(int i=oldNumRows;i<numRows;++i) {
        mRowVector.push_back(RowInfo(i));
        RowInfo& row=mRowVector.back();
        EnsureNumCols(row,numCols);
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071b490
// (public: unsigned int __thiscall std::vector<struct WONAPI::TableElement::CellInfo,class std::allocator<struct WONAPI::TableElement::CellInfo> >::size()
// TEMPLATE: WON 0x007206b0
// (public: class std::_Ptrit<struct WONAPI::TableElement::RowInfo,int,struct WONAPI::TableElement::RowInfo *,struct WONAPI::TableElement::RowInfo &,struc)
// TEMPLATE: WON 0x007206d0
// (public: class std::_Ptrit<struct WONAPI::TableElement::RowInfo,int,struct WONAPI::TableElement::RowInfo *,struct WONAPI::TableElement::RowInfo &,struc)
// TEMPLATE: WON 0x00723540
// (public: __thiscall std::vector<struct WONAPI::TableElement::CellInfo,class std::allocator<struct WONAPI::TableElement::CellInfo> >::vector<struct WONA)
// TEMPLATE: WON 0x007240e0
// (struct WONAPI::TableElement::ColumnInfo * __cdecl std::copy(class std::_Ptrit<struct WONAPI::TableElement::ColumnInfo,int,struct WONAPI::TableElement:)
// TEMPLATE: WON 0x00724120
// (public: unsigned int __thiscall std::vector<struct WONAPI::TableElement::CellInfo,class std::allocator<struct WONAPI::TableElement::CellInfo> >::capac)
// TEMPLATE: WON 0x00724150
// (protected: bool __thiscall std::vector<struct WONAPI::TableElement::CellInfo,class std::allocator<struct WONAPI::TableElement::CellInfo> >::_Buy(unsig)
// TEMPLATE: WON 0x00724500
// (protected: struct WONAPI::TableElement::ColumnInfo * __thiscall std::vector<struct WONAPI::TableElement::ColumnInfo,class std::allocator<struct WONAPI)
// TEMPLATE: WON 0x00724540
// (void __cdecl std::fill(class std::_Ptrit<struct WONAPI::TableElement::ColumnInfo,int,struct WONAPI::TableElement::ColumnInfo *,struct WONAPI::TableEle)
// TEMPLATE: WON 0x00724570
// (class std::_Ptrit<struct WONAPI::TableElement::ColumnInfo,int,struct WONAPI::TableElement::ColumnInfo *,struct WONAPI::TableElement::ColumnInfo &,stru)
// TEMPLATE: WON 0x00724d70
// (void __cdecl std::_Construct(struct WONAPI::TableElement::CellInfo *,struct WONAPI::TableElement::CellInfo const &)    [?_Construct@std@@YAXPAUCellInf)
// TEMPLATE: WON 0x00724dd0
// (void __cdecl std::_Construct(struct WONAPI::TableElement::ColumnInfo *,struct WONAPI::TableElement::ColumnInfo const &)    [?_Construct@std@@YAXPAUCol)
// TEMPLATE: WON 0x00724f60
// (public: struct WONAPI::TableElement::CellInfo & __thiscall WONAPI::TableElement::CellInfo::operator=(struct WONAPI::TableElement::CellInfo const &)   )
// TEMPLATE: WON 0x00725700
// (struct WONAPI::TableElement::CellInfo * __cdecl std::copy(class std::_Ptrit<struct WONAPI::TableElement::CellInfo,int,struct WONAPI::TableElement::Cel)
