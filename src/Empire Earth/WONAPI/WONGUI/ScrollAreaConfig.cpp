// WONGUI ScrollArea configuration, actual borrowed control owner.
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
#include "ee/wonapi/WONLobbyResourceConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00794360
bool ScrollAreaConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="BACKGROUND") mScrollArea->mBackground=ReadBackground();
    else if(instruction=="BORDERPADDING")
    {
        int left=ReadInt();EnsureComma();
        int top=ReadInt();EnsureComma();
        int right=ReadInt();EnsureComma();
        int bottom=ReadInt();
        mScrollArea->SetBorderPadding(left,top,right,bottom);
    }
    else return ContainerConfig::Virtual24(instruction);
    return true;
}
// FUNCTION: WON 0x00794540
bool ScrollerConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SCROLLAREA")
    {
        ScrollArea* area=dynamic_cast<ScrollArea*>(ReadComponent());
        if(area==NULL) throw ConfigObjectException("Expecting ScrollArea");
        mScroller->SetScrollArea(area);
        return true;
    }
    return ContainerConfig::Virtual24(instruction);
}

// FUNCTION: WON 0x007946e0
ScrollbarCondition ScrollbarScrollerConfig::GetScrollbarCondition()
{
    GUIString condition=Fn3842F0(false);
    if(condition.CompareNoCase("DISABLE")==0) return ScrollbarCondition_Disable;
    if(condition.CompareNoCase("ENABLE")==0) return ScrollbarCondition_Enable;
    if(condition.CompareNoCase("CONDITIONAL")==0) return ScrollbarCondition_Conditional;
    throw ConfigObjectException("Invalid scrollbar condition: "+(std::string)condition);
}

// FUNCTION: WON 0x007948a0
bool ScrollbarScrollerConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SCROLLBARCONDITIONS")
    {
        ScrollbarCondition horz=GetScrollbarCondition();EnsureComma();
        ScrollbarCondition vert=GetScrollbarCondition();
        mScrollbarScroller->SetScrollbarConditions(horz,vert);
        return true;
    }
    return ScrollerConfig::Virtual24(instruction);
}

}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK button-scroller configuration policy.
ScrollButtonCondition ButtonScrollerConfig::GetScrollButtonCondition()
{
	GUIString aStr = Fn3842F0(false);
	if(aStr.CompareNoCase("DISABLE")==0)
		return ScrollButtonCondition_Disable;
	else if(aStr.CompareNoCase("ENABLE")==0)
		return ScrollButtonCondition_Enable;
	else if(aStr.CompareNoCase("CONDITIONAL")==0)
		return ScrollButtonCondition_Conditional;
	else if(aStr.CompareNoCase("PARTIALCONDITIONAL")==0)
		return ScrollButtonCondition_PartialConditional;
	else
		throw ConfigObjectException("Invalid scrollbar condition: " + (std::string)aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ButtonScrollerConfig::Virtual24(const std::string &theInstruction)
{
	if(theInstruction=="SCROLLBARCONDITIONS")
	{
		ScrollButtonCondition horz, vert;
		horz = GetScrollButtonCondition(); EnsureComma();
		vert = GetScrollButtonCondition();
		mButtonScroller->SetScrollButtonConditions(horz,vert);
	}
	else
		return ScrollerConfig::Virtual24(theInstruction);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////

}
