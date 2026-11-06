// PROVISIONAL original WONGUI prerequisite family; original filename/object boundaries unproved.
// SOURCE ONLY: near/generated support remain original-carved; no runtime substitution authority.
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
namespace WONAPI {
// FUNCTION: WON 0x0072b530
ComponentEvent::ComponentEvent(Component* c, int type)
 : mComponentRef(c), mType(type), mComponent(c) { mBroadcast=false; }
// FUNCTION: WON 0x0072b580
// (??_GComponentEvent@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0072b5a0
void ComponentEvent::SetComponent(Component* c) { mComponent=c; mComponentRef=c; }
// FUNCTION: WON 0x0072b600
int ComponentEvent::GetControlId() const { return mComponent->mControlId; }
// FUNCTION: WON 0x0072b610
void ComponentEvent::Deliver() {
 if (mBroadcast) mWindow->mManager->Fn32F680(this);
 else mComponent->Fn2F54E0(this);
}
}

#include "ee/wonapi/WONLobbyPopupEvent.h"
namespace WONAPI {
// FUNCTION: WON 0x0072b520
void InputEvent::Deliver() { mWindow->HandleInputEvent(this); }
// FUNCTION: WON 0x0072b630
void MouseEvent::Deliver() { mWindow->HandleMouseEvent(this); }
// FUNCTION: WON 0x0072b640
void KeyEvent::Deliver() { mWindow->HandleKeyEvent(this); }
// FUNCTION: WON 0x0072b650
void DoPopupEvent::Deliver() { mWindow->OpenPopup(mParams); }
}
