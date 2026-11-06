// Native borrowed MultiList configuration instruction dispatch; no owner allocation/lifetime bodies.
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
#include "ee/wonapi/WONLobbyResourceConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00790160
bool MultiListAreaConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SORTCOLUMN") {
        int column=ReadInt(); EnsureComma();
        ListSortType type=ReadSortType();
        mMultiListArea->SetSortColumn(column,type);
    } else if(instruction=="COLUMNWIDTH") {
        int column=ReadInt(); EnsureComma();
        int width=ReadInt();
        mMultiListArea->SetColumnWidth(column,width,false);
    } else if(instruction=="NUMCOLUMNS") mMultiListArea->SetNumColumns(ReadInt());
    else if(instruction=="DYNAMICSIZECOLUMN") mMultiListArea->mDynamicSizeColumn=ReadInt();
    else if(instruction=="SORTCOLUMNFUNC") {
        int column=ReadInt(); EnsureComma();
        GUIString name=Fn3842F0(false);
        ListArea::SortFunc function=NULL;
        if(name.CompareNoCase("NUMERIC")==0) function=ListArea::NumberSortFunc;
        else throw ConfigObjectException("Unknown sort func: "+(std::string)name);
        mMultiListArea->SetSortColumnFunc(column,function);
    } else if(instruction=="AUTOGROWCOLUMNS") mMultiListArea->mAutoGrowColumns=Fn3837F0();
    else if(instruction=="COLUMNHALIGN") {
        int column=ReadInt(); EnsureComma();
        HorzAlign align=ReadHorzAlign();
        mMultiListArea->SetColumnHAlign(column,align);
    } else return ListAreaConfig::Virtual24(instruction);
    return true;
}
// FUNCTION: WON 0x00790490
bool MultiListHeaderConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SETCOLUMN") {
        int column=ReadInt(); EnsureComma();
        Component* component=ReadComponent();
        mHeader->SetColumnItem(column,component);
    } else if(instruction=="MULTILISTAREA") {
        mHeader->SetMultiListArea(SafeConfigGetComponent<MultiListArea*>(ReadComponent(),"MultiListArea"));
    } else return ContainerConfig::Virtual24(instruction);
    return true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a20d0
// (class WONAPI::MultiListArea * __cdecl WONAPI::SafeConfigGetComponent(class WONAPI::Component *,char const *)    [?SafeConfigGetComponent@WONAPI@@YAPAV)
