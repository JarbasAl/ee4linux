// Source-only WONGUI ImageButton configuration.
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
#include "ee/wonapi/WONLobbyImageButton.h"
namespace WONAPI {
// FUNCTION: WON 0x0078ac90
ImageButtonConfig::ImageButtonConfig()
{
}
// FUNCTION: WON 0x0078acb0
bool ImageButtonConfig::Virtual24(const std::string& instruction)
{
    ImageButtonType type;
    if(instruction=="NORMALIMAGE") type=ImageButtonType_Normal;
    else if(instruction=="OVERIMAGE") type=ImageButtonType_Over;
    else if(instruction=="DOWNIMAGE") type=ImageButtonType_Down;
    else if(instruction=="DISABLEDIMAGE") type=ImageButtonType_Disabled;
    else if(instruction=="CHECKEDIMAGE") type=ImageButtonType_Checked;
    else return ComponentConfig::Virtual24(instruction);
    mImageButton->SetImage(type,ReadImage(false).get());
    return true;
}
// FUNCTION: WON 0x0078f8d0
void ImageButtonConfig::Virtual34(Component* component)
{
    ComponentConfig::Virtual34(component);
    mImageButton=static_cast<ImageButton*>(component);
}
}
