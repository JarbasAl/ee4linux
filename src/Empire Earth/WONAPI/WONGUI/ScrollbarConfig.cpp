// WONGUI Scrollbar configuration, no control allocation.
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
// FUNCTION: WON 0x00794920
bool ScrollbarConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="VERTICAL") mScrollbar->mVertical=Fn3837F0();
    else if(instruction=="RESIZETHUMB") mScrollbar->mResizeThumb=Fn3837F0();
    else if(instruction=="MINTHUMBSIZE") mScrollbar->mMinThumbSize=(unsigned char)ReadInt();
    else return ContainerConfig::Virtual24(instruction);
    return true;
}

// FUNCTION: WON 0x007949c0
bool ScrollbarConfig::Virtual28(const std::string& name,ComponentConfig*& config,Component*& component)
{
    if(name=="THUMB")
    {
        config=new ComponentConfig;
        component=mScrollbar->mThumb.get();
    }
    else if(name=="UPARROW")
    {
        config=new ButtonConfig;
        component=mScrollbar->mUpArrow.get();
    }
    else if(name=="DOWNARROW")
    {
        config=new ButtonConfig;
        component=mScrollbar->mDownArrow.get();
    }
    else if(name=="SCROLLRECT")
    {
        config=new ButtonConfig;
        component=mScrollbar->mScrollRect.get();
    }
    else return ContainerConfig::Virtual28(name,config,component);
    return true;
}
}
