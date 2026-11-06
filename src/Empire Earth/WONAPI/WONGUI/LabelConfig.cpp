// Source-only reconstructed WONGUI LabelConfig.
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
#include "ee/wonapi/WONLobbyBackground.h"
#include "ee/wonapi/WONLobbyResourceConfig.h"

namespace WONAPI {
// FUNCTION: WON 0x0078d2b0
bool LabelConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="TEXT") mLabel->SetText(Fn3842F0(false),true);
    else if(instruction=="FONT") mLabel->SetFont(ReadFont());
    else if(instruction=="TEXTCOLOR") mLabel->SetTextColor(Fn385510());
    else if(instruction=="BACKGROUND") mLabel->SetBackground(ReadBackground());
    else if(instruction=="VERTALIGN") mLabel->SetVertAlign(ReadVertAlign());
    else if(instruction=="HORZALIGN") mLabel->SetHorzAlign(ReadHorzAlign());
    else if(instruction=="WRAP") mLabel->SetWrap(Fn3837F0());
    else if(instruction=="ADJUSTHEIGHTTOFITTEXT") mLabel->SetAdjustHeightToFitText(Fn3837F0());
    else return ComponentConfig::Virtual24(instruction);
    return true;
}
}

namespace WONAPI {
// FUNCTION: WON 0x0078d290
LabelConfig::LabelConfig()
{
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0078ef70
// (public: virtual __thiscall WONAPI::LabelConfig::~LabelConfig(void)    [??1LabelConfig@WONAPI@@UAE@XZ])
