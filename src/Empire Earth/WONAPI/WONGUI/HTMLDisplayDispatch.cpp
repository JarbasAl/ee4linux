// Original HTML tag-to-renderer dispatch; switch data and forwarding are support0.
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
// FUNCTION: WON 0x0073c020
void HTMLDisplayGen::HandleTag(HTMLTag* tag)
{
    switch(tag->GetType())
    {
    case HTMLTagType_Body: HandleBody(static_cast<HTMLBodyTag*>(tag)); break;
    case HTMLTagType_Anchor: HandleAnchor(static_cast<HTMLAnchorTag*>(tag)); break;
    case HTMLTagType_LineBreak: HandleLineBreak(static_cast<HTMLLineBreakTag*>(tag)); break;
    case HTMLTagType_HorizontalRule: HandleHorizontalRule(static_cast<HTMLHorizontalRuleTag*>(tag)); break;
    case HTMLTagType_Text: HandleTextTag(static_cast<HTMLTextTag*>(tag)); break;
    case HTMLTagType_Bold: HandleModifyFontStyle(tag,FontStyle_Bold); break;
    case HTMLTagType_Italic: HandleModifyFontStyle(tag,FontStyle_Italic); break;
    case HTMLTagType_Underline: HandleModifyFontStyle(tag,FontStyle_Underline); break;
    case HTMLTagType_Center: HandleCenter(tag); break;
    case HTMLTagType_Small: HandleModifyFont(tag,NULL,2,-1,GetIsFontMonospace()); break;
    case HTMLTagType_Big: HandleModifyFont(tag,NULL,4,-1,GetIsFontMonospace()); break;
    case HTMLTagType_Code: HandleModifyFont(tag,"Courier New",-1,-1,true); break;
    case HTMLTagType_Pre: HandlePre(tag); break;
    case HTMLTagType_Font:
    case HTMLTagType_BaseFont: HandleFont(static_cast<HTMLFontTag*>(tag)); break;
    case HTMLTagType_Div:
    case HTMLTagType_Paragraph: HandleBlock(static_cast<HTMLBlockTag*>(tag)); break;
    case HTMLTagType_Table: HandleTable(static_cast<HTMLTableTag*>(tag)); break;
    case HTMLTagType_TableRow: HandleTableRow(static_cast<HTMLTableRowTag*>(tag)); break;
    case HTMLTagType_TableData:
    case HTMLTagType_TableHeader: HandleTableData(static_cast<HTMLTableDataTag*>(tag)); break;
    case HTMLTagType_UnorderedList:
    case HTMLTagType_OrderedList: HandleList(static_cast<HTMLListTag*>(tag)); break;
    case HTMLTagType_ListItem: HandleListItem(static_cast<HTMLListItemTag*>(tag)); break;
    case HTMLTagType_Image: HandleImage(static_cast<HTMLImageTag*>(tag)); break;
    case HTMLTagType_Input:
    case HTMLTagType_Select: HandleInput(static_cast<HTMLInputTag*>(tag)); break;
    case HTMLTagType_Option: HandleOption(tag); break;
    case HTMLTagType_H1:
    case HTMLTagType_H2:
    case HTMLTagType_H3:
    case HTMLTagType_H4:
    case HTMLTagType_H5:
    case HTMLTagType_H6: HandleHeading(static_cast<HTMLHeadingTag*>(tag)); break;
    case HTMLTagType_GUIComponent: HandleGUIComponent(static_cast<HTMLGUIComponentTag*>(tag)); break;
    default: break;
    }
}
}
