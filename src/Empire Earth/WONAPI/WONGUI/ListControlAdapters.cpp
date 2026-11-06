// Genuine SDK list sort/color/reset adapters; support0.
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
namespace WONAPI {
// FUNCTION: WON 0x006facd0
void ListArea::SetSortType(ListSortType type)
{
    if(mSortType==type) return;
    mSortType=type;
    Sort();
}
// FUNCTION: WON 0x006fb520
int ListArea::GetTextColor() { return mTextColor; }
// FUNCTION: WON 0x006faaf0
int ListArea::NumberSortFunc(ListArea*,const ListItem* left,const ListItem* right)
{
    return ((TextListItem*)left)->mText.mData->ToInt(0)-((TextListItem*)right)->mText.mData->ToInt(0);
}
// FUNCTION: WON 0x007035d0
void MultiListArea::NotifyMouseItemGone()
{
    ListArea::NotifyMouseItemGone();
    mMouseColItem=NULL;
}
}
