// Genuine SDK skin duplicate selection and arrow desired-size adapters; support0.
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
#include "ee/wonapi/WONLobbyMSSkinImage.h"
namespace WONAPI {
// FUNCTION: WON 0x00715530
WONButton* WONSkin::GetTabButton()
{
    return mTabButton->Duplicate();
}
// FUNCTION: WON 0x00715550
WONSimpleButton* WONSkin::GetArrowButton(Direction direction)
{
    WONSimpleButton* button=mArrowButton[direction]->Duplicate();
    button->Fn2F4E90(0,0);
    return button;
}
// FUNCTION: WON 0x00715660
WONSimpleButton* WONSkin::GetScrollThumb(bool vertical)
{
    return mScrollThumb[vertical?1:0]->Duplicate();
}
// FUNCTION: WON 0x00715680
WONSimpleButton* WONSkin::GetScrollGutter(bool vertical)
{
    return mScrollGutter[vertical?1:0]->Duplicate();
}
}
