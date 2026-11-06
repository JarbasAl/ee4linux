// Native namespace lobby initialization. All globals retain their original providers.
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
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONLobbyCore.h"
namespace WONAPI {
// The lobby singleton and its initialization failure state.
SmartPtr<WONLobbyPrv> gLobbyData;
GUIString gLobbyInitError5662FC;
SmartPtr<Dialog> gLobbyFailureDialog5662F4;
bool gLobbyInitFailed56633C;
void LobbyFn358F70(); //original cdecl initialization provider; no new body

// Actual target return is bool; legacy public void declaration requires Root correction.
// FUNCTION: WON 0x00761fb0
bool LobbyAddComponentLoader(ResourceConfigTable* table,void (*initFunc)(ComponentConfig*))
{
    gLobbyInitError5662FC=GetEmptyGUIStringData();
    gLobbyInitFailed56633C=false;
    gLobbyFailureDialog5662F4=NULL;
    gLobbyData=new WONLobbyPrv;
    if(!gLobbyData->Init(table,initFunc)) {
        if(WONAPI::WindowManager::GetDefaultWindowManager()!=NULL)
            WONAPI::WindowManager::GetDefaultWindowManager()->ClearSubscriptions();
        gLobbyInitError5662FC=gLobbyData->mInitError;
        gLobbyInitFailed56633C=true;
        gLobbyData=NULL;
        return false;
    }
    LobbyFn358F70();
    return true;
}
}

namespace WONAPI {
// Genuine SDK WONLobby::InitAuth; original26B. Legacy spelling retained, real string/pointer API restored.
void LobbySetProductName(const std::wstring& inLoginCommunity,const char* inHashFileListStr)
{
    if(gLobbyData.get()!=NULL) gLobbyData->InitAuth(inLoginCommunity,inHashFileListStr);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00762150
// (void __cdecl WONAPI::LobbySetProductName(class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned )

// Dynamic initializers and exit handlers of the globals above:
// TEMPLATE: WON 0x00761ef0
// TEMPLATE: WON 0x00761f00
// TEMPLATE: WON 0x00761f10
// TEMPLATE: WON 0x00761f20
// TEMPLATE: WON 0x00761f30
// TEMPLATE: WON 0x00761f40
// TEMPLATE: WON 0x00761f50
// TEMPLATE: WON 0x00761f60
// TEMPLATE: WON 0x00761f70
// TEMPLATE: WON 0x00761f80
// TEMPLATE: WON 0x00761f90
// TEMPLATE: WON 0x00761fa0
