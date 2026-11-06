// Scratch whole LobbyContainer; common ScreenContainer external base now closed88.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
// Own LoginScreen9c declaration will be canonical before any source promotion/compile.
namespace WONAPI {
// Native default constructor38 performs only member/base setup, zero primary quota.
LobbyContainer::LobbyContainer() {}
// Original3bdd60..3bde1e,190B. Own-vptr3bdd7f precedes three owned releases.
// FUNCTION: WON 0x007bdd60
LobbyContainer::~LobbyContainer() {}
// Original3bde20..3bdeca,170B. Actual SafeGet line37/38/39 and screen-add order.
// FUNCTION: WON 0x007bde20
void LobbyContainer::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
    HTMLDisplayGen::SetGlobalDefaultFontFace(Fn2F5B20()->mDescriptor.mFace);
    SafeGetComponent(config,mLoginScreen,"LoginScreen",__FILE__,37);
    SafeGetComponent(config,mInternetScreen,"InternetScreen",__FILE__,38);
    SafeGetComponent(config,mLanScreen,"LanScreen",__FILE__,39);
    AddScreen(mLoginScreen.get());
    AddScreen(mInternetScreen.get());
    AddScreen(mLanScreen.get());
    ShowScreen(mLoginScreen.get(),true);
}
// Original3bded0..3bdedf,15B.
// FUNCTION: WON 0x007bded0
void LobbyContainer::ShowInternetScreen() { ShowScreen(mInternetScreen.get(),true); }
// Original3bdee0..3bdeef,15B.
// FUNCTION: WON 0x007bdee0
void LobbyContainer::ShowLanScreen() { ShowScreen(mLanScreen.get(),true); }
// Original3bdef0..3bdf0e,30B. Reload LoginScreen receiver after ShowScreen callback.
// FUNCTION: WON 0x007bdef0
void LobbyContainer::ShowLoginScreen()
{
    ShowScreen(mLoginScreen.get(),true);
    mLoginScreen->ShowMOTD();
}
// Original3bdf10..3bdf28,24B.
// FUNCTION: WON 0x007bdf10
bool LobbyContainer::OnInternetScreen() { return mCurScreen==mInternetScreen.get(); }
// Original3bdf30..3bdf48,24B.
// FUNCTION: WON 0x007bdf30
bool LobbyContainer::OnLanScreen() { return mCurScreen==mLanScreen.get(); }
// Original3bdf50..3bdf68,24B.
// FUNCTION: WON 0x007bdf50
bool LobbyContainer::OnLoginScreen() { return mCurScreen==mLoginScreen.get(); }
// Original3bdf70..3bdfbe,78B. Preserve distinct mode predicates and rawDWORD result.
// FUNCTION: WON 0x007bdf70
int LobbyContainer::GetGameScreenType()
{
    if(OnInternetScreen()) return 2;
    if(OnLanScreen()) {
        if(mLanScreen->OnLanTab()) return 1;
        if(mLanScreen->OnDirectConnectTab()) return 3;
    }
    return 0;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007bdd10
// (public: __thiscall WONAPI::LobbyContainer::LobbyContainer(void)    [??0LobbyContainer@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007bdd40
// (public: virtual void * __thiscall WONAPI::LobbyContainer::`scalar deleting destructor'(unsigned int)    [??_GLobbyContainer@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007cdea0
// TEMPLATE: WON 0x007ce020
