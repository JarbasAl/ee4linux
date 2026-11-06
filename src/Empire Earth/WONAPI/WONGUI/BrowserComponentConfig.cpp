// Genuine SDK BrowserComponentConfig unit: CfgFactory API body only; other real providers remain external.
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
#include "ee/wonapi/WONLobbyControlConfig.h"

namespace WONAPI {
// FUNCTION: WON 0x007facb0
ComponentConfig* BrowserComponentConfig::CfgFactory()
{
    return new BrowserComponentConfig;
}
}

#include "ee/wonapi/WONLobbyResourceConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x0078f960
void BrowserComponentConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mBrowserComponent=static_cast<BrowserComponent*>(component);
}
// FUNCTION: WON 0x007fa7e0
bool BrowserComponentConfig::Virtual28(const std::string& name,ComponentConfig*& config,Component*& component)
{
    if(name=="HTMLVIEW")
    {
        config=new ScrollAreaConfig;
        component=mBrowserComponent->GetHTMLView();
    }
    else return ContainerConfig::Virtual28(name,config,component);
    return true;
}
// FUNCTION: WON 0x007fa8a0
// Complete target instruction contract: transparent is an instruction without
// a boolean argument; target has no BACKGROUND branch.
bool BrowserComponentConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="TRANSPARENT")
        mBrowserComponent->SetTransparent(true);
    else if(instruction=="SCROLLER")
        mBrowserComponent->SetScroller(SafeConfigGetComponent<Scroller*>(ReadComponent(),"Scroller"));
    else if(instruction=="FONTSIZE")
    {
        HTMLFontSize size;
        GUIString value=Fn3842F0(false);
        if(value.CompareNoCase("Smallest")==0) size=HTMLFontSize_Smallest;
        else if(value.CompareNoCase("Small")==0) size=HTMLFontSize_Small;
        else if(value.CompareNoCase("Medium")==0) size=HTMLFontSize_Medium;
        else if(value.CompareNoCase("Large")==0) size=HTMLFontSize_Large;
        else if(value.CompareNoCase("Largest")==0) size=HTMLFontSize_Largest;
        else throw ConfigObjectException("Invalid font size: "+std::string(value));
        mBrowserComponent->SetFontSize(size);
    }
    else return ContainerConfig::Virtual24(instruction);
    return true;
}
// FUNCTION: WON 0x007fab70
void BrowserComponentConfig::Init(ResourceConfigTable* table)
{
    table->AddAllocator("BrowserComponent",CfgFactory,CompFactory);
}
// FUNCTION: WON 0x007fac50
Component* BrowserComponentConfig::CompFactory()
{
    return new BrowserComponent;
}
}
