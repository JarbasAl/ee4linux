// Scratch original-backed LoginHelp whole consumer. Browser reference contract pending GUI.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {
// Constructor3bfc20/32B initializes only members; implicitnormal3bfc60/106B noownVP,0quota.
LoginHelpCtrl::LoginHelpCtrl() : mHaveLoadedHelp(false) {}
// Original3bfea0..3bfecb,43B.
// FUNCTION: WON 0x007bfea0
void LoginHelpCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) SafeGetComponent(static_cast<ComponentConfigInit&>(inInit).mConfig,mBrowser,"Browser",__FILE__,40);
}
// Original3bfcd0..3bfe9f,463B. Both std::string values die before flags/ContainerPaint.
// FUNCTION: WON 0x007bfcd0
void LoginHelpCtrl::Paint(Graphics& inGraphics)
{
    if(!mHaveLoadedHelp) {
        mBrowser->Go(ComponentConfig::GetResourceFile("help/loginhelp.html"),false,0,-1);
        mBrowser->SetLaunchBrowserOnLink(true);
        mHaveLoadedHelp=true;
    }
    Container::Paint(inGraphics);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bfc40
// (public: virtual void * __thiscall WONAPI::LoginHelpCtrl::`scalar deleting destructor'(unsigned int)    [??_GLoginHelpCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bfc60
// (public: virtual __thiscall WONAPI::LoginHelpCtrl::~LoginHelpCtrl(void)    [??1LoginHelpCtrl@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bfc20
// (public: __thiscall WONAPI::LoginHelpCtrl::LoginHelpCtrl(void)    [??0LoginHelpCtrl@WONAPI@@QAE@XZ])
