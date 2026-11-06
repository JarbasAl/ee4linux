// Native MS control configuration instruction handlers on original-owned controls.
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
#include "ee/wonapi/WONLobbyMSConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x0078f3e0
bool MSButtonConfig::Virtual24(const std::string& key)
{
    if(key=="FONT") {
        mMSButton->SetFont(ReadFont());
    } else if(key=="TEXTCOLOR") {
        int color=Fn385510();
        mMSButton->SetTextColor(color);
    } else if(key=="VERTALIGN") {
        VertAlign align=ReadVertAlign();
        mMSButton->SetVertAlign(align);
    } else if(key=="HORZALIGN") {
        HorzAlign align=ReadHorzAlign();
        mMSButton->SetHorzAlign(align);
    } else if(key=="TRANSPARENT") {
        mMSButton->SetTransparent(Fn3837F0());
    } else if(key=="ADJUSTHEIGHTTOFITTEXT") {
        mMSButton->SetAdjustHeightToFitText(Fn3837F0());
    } else if(key=="WRAP") {
        mMSButton->SetWrap(Fn3837F0());
    } else if(key=="INCREMENTHORZTEXTPAD") {
        int left=ReadInt(); EnsureComma();
        int right=ReadInt();
        mMSButton->IncrementHorzTextPad(left,right);
    } else if(key=="INCREMENTVERTEXTPAD") {
        int top=ReadInt(); EnsureComma();
        int bottom=ReadInt();
        mMSButton->IncrementVertTextPad(top,bottom);
    } else return ButtonConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078f690
bool MSArrowButtonConfig::Virtual24(const std::string& key)
{
    if(key=="DIRECTION") {
        GUIString direction=Fn3842F0(false);
        if(direction.CompareNoCase("Up")==0) mMSArrowButton->SetDirection(Direction_Up);
        else if(direction.CompareNoCase("Down")==0) mMSArrowButton->SetDirection(Direction_Down);
        else if(direction.CompareNoCase("Left")==0) mMSArrowButton->SetDirection(Direction_Left);
        else if(direction.CompareNoCase("Right")==0) mMSArrowButton->SetDirection(Direction_Right);
    } else return ButtonConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078f7d0
bool MSTabButtonConfig::Virtual24(const std::string& key)
{
    if(key=="LOWERAMOUNT")
        mMSTabButton->SetLowerAmount(ReadInt());
    else return MSButtonConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x0078f8f0
bool MS3DFrameConfig::Virtual24(const std::string& key)
{
    if(key=="FRAMETYPE") {
        int type=ReadInt();
        mMS3DFrame->SetFrameType(type);
    } else if(key=="UP") {
        bool up=Fn3837F0();
        mMS3DFrame->SetIsUp(up);
    } else return ComponentConfig::Virtual24(key);
    return true;
}
}

namespace WONAPI {
// FUNCTION: WON 0x0078f3c0
void MSArrowButtonConfig::Virtual34(Component* component)
{
    ButtonConfig::Virtual34(component);
    mMSArrowButton=static_cast<MSArrowButton*>(component);
}
}

namespace WONAPI {
// SUPPORT: EXE 0x0078f8d0; genuine SDK MS3DFrameConfig SetComponent, zero primary.
void MS3DFrameConfig::Virtual34(Component* component)
{
    ComponentConfig::Virtual34(component);
    mMS3DFrame=static_cast<MS3DFrame*>(component);
}
}

namespace WONAPI {
// SUPPORT: EXE 0x0078f3a0; genuine SDK MSButtonConfig, zero primary.
MSButtonConfig::MSButtonConfig()
{
}
}

namespace WONAPI {
// (0x0078f3c0: ICF-folded with MSArrowButtonConfig::Virtual34 above)
void MSButtonConfig::Virtual34(Component* component)
{
    ButtonConfig::Virtual34(component);
    mMSButton=static_cast<MSButtonBase*>(component);
}
// FUNCTION: WON 0x0078f7b0
void MSTabButtonConfig::Virtual34(Component* component)
{
    MSButtonConfig::Virtual34(component);
    mMSTabButton=static_cast<MSTabButton*>(component);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0078f3a0
// (public: __thiscall WONAPI::MSButtonConfig::MSButtonConfig(void)    [??0MSButtonConfig@WONAPI@@QAE@XZ])
