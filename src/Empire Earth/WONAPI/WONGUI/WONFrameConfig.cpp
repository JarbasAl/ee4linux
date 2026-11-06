// Original-owned WON frame component configuration instructions and named padding.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyWONFrameConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00798440
bool WONFrameComponentConfig::Virtual24(const std::string& key)
{
    if(key=="BORDERS") {
        mFrameComponent->VirtualB8(ReadInt()); EnsureComma();
        mFrameComponent->VirtualBC(ReadInt()); EnsureComma();
        mFrameComponent->VirtualC0(ReadInt()); EnsureComma();
        mFrameComponent->VirtualC4(ReadInt());
    } else if(key=="IMAGE") {
        mFrameComponent->VirtualB0(ReadImage(false).get());
    } else return ComponentConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x00798590
bool WONFrameComponentConfig::Virtual2C(const std::string& key,int& value)
{
    if(key=="LEFTPAD") value=mFrameComponent->VirtualC8();
    else if(key=="RIGHTPAD") value=mFrameComponent->VirtualD0();
    else if(key=="TOPPAD") value=mFrameComponent->VirtualCC();
    else if(key=="BOTTOMPAD") value=mFrameComponent->VirtualD4();
    else if(key=="HORZPAD") {
        WONFrameComponent* frame=mFrameComponent;
        int right=frame->VirtualD0(); value=right+frame->VirtualC8();
    } else if(key=="VERTPAD") {
        WONFrameComponent* frame=mFrameComponent;
        int bottom=frame->VirtualD4(); value=bottom+frame->VirtualCC();
    } else return ComponentConfig::Virtual2C(key,value);
    return true;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00796ba0
void WONFrameComponentConfig::Virtual34(Component* component) { ComponentConfig::Virtual34(component); mFrameComponent=static_cast<WONFrameComponent*>(component); }
}
