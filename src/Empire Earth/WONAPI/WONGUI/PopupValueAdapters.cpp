// Complete original-backed GUI values; source-only, no runtime substitution.
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
// FUNCTION: WON 0x00728540
PopupParams::PopupParams(Component* popup,int flags,Component* owner,int x,int y)
{
    mPopup=popup;
    mFlags=flags;
    mx=x;
    mParent=NULL;
    mInputComponent=NULL;
    mOwnerComponent=owner;
    my=y;
}
// FUNCTION: WON 0x0072b0a0
void Window::Fn32B0A0(int result,Component* component)
{
    mManager->Fn32DDB0(result,this,component);
}
// FUNCTION: WON 0x0072b0c0
void Window::OpenPopup(PopupParams& params)
{
    params.mParent=this;
    mManager->DoPopup(params);
}
// FUNCTION: WON 0x0072b0e0
void Window::ClosePopup(Component* component)
{
    mManager->EndPopup(component);
}
}
