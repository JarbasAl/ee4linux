// Genuine complete ComponentListItem adapters and retained component cleanup; support0.
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
#include "ee/wonapi/WONLobbyComponentListItem.h"
namespace WONAPI {
// FUNCTION: WON 0x006fa480
void ComponentListItem::Virtual08(ListArea*)
{
    if(mComponent.get()!=NULL) {
        mL10=mComponent->Width();
        mHeight=mComponent->Height();
    }
}
// FUNCTION: WON 0x006fa4a0
void ComponentListItem::Virtual0C(ListArea*,Container* viewport)
{
    mComponent->mParent=viewport;
    if(viewport!=NULL) mComponent->Virtual6C();
}
// FUNCTION: WON 0x006fa4c0
void ComponentListItem::Virtual18(int x,int y)
{
    ListItem::Virtual18(x,y);
    mComponent->Fn2F4B70(x,y);
}
// FUNCTION: WON 0x006fa4e0
bool ComponentListItem::Virtual1C(int x,int y) { return mComponent->Contains(x,y); }
// FUNCTION: WON 0x006fa500
void ComponentListItem::CheckInvalid(ListArea*) {}
// FUNCTION: WON 0x006fd970
void ComponentListItem::Virtual20(ListArea* area,int x,int y)
{
    mComponent->Virtual24(x,y);
    CheckInvalid(area);
}
// FUNCTION: WON 0x006fd9a0
void ComponentListItem::Virtual24(ListArea* area)
{
    mComponent->Virtual28();
    CheckInvalid(area);
}
// FUNCTION: WON 0x006fd9c0
void ComponentListItem::Virtual28(ListArea* area,int x,int y)
{
    mComponent->Virtual34(x,y);
    CheckInvalid(area);
}
// FUNCTION: WON 0x006fd9f0
void ComponentListItem::Virtual2C(ListArea* area,int x,int y)
{
    mComponent->Virtual38(x,y);
    CheckInvalid(area);
}
// FUNCTION: WON 0x006fda20
void ComponentListItem::Virtual30(ListArea* area,int x,int y,int button)
{
    mComponent->Virtual2C(x,y,button);
    CheckInvalid(area);
}
// FUNCTION: WON 0x006fda50
void ComponentListItem::Virtual34(ListArea* area,int x,int y,int button)
{
    mComponent->Virtual30(x,y,button);
    CheckInvalid(area);
}
// SUPPORT: EXE 0x006fdaa0; native100B, actual retained component then ListItem/RefCount cleanup.
ComponentListItem::~ComponentListItem() {}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006fdaa0
// (protected: virtual __thiscall WONAPI::ComponentListItem::~ComponentListItem(void)    [??1ComponentListItem@WONAPI@@MAE@XZ])
