// Original lobby UDP binding and configured/random-port fallback.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONLobbyCore.h"

namespace WONAPI {
// FUNCTION: WON 0x0075ba00
void LobbyFn35BA00()
{
    LobbyConfig* aConfig=GetLobbyConfig();
    unsigned short aPort=aConfig?aConfig->mLobbyPort:8888;
    LobbyFn361420(aPort);
    UDPManager* aMgr=LobbyFn35B8F0();
    if(aMgr!=NULL) {
        WONStatus aStatus=aMgr->Fn2B79C0(aPort,true);
        if(aStatus!=0) {
            srand(time(NULL));
            aPort=rand()%10000+10000;
            aStatus=aMgr->Fn2B79C0(aPort,false);
            LobbyFn361420(aPort);
        }
        if(aStatus==0)
            aMgr->SetAutoRecv(true);
    }
}
}
