// Game staging controls; full normal capture and independent peer review retained.
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
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyControls.h"
namespace WONAPI {
// Root-approved common call-only contract; actual original ready predicate.
bool LobbyFn3613A0();
//3bc740,54B.
// FUNCTION: WON 0x007bc740
GameStagingCtrl::GameStagingCtrl() : mInGame(false),mLastReady(false) { mRoomSpecFlags = 2; }
//3bc7a0..3bc833,147B;Start88 thenLeave84 thenContainer,2EHstates.
// Implicit destructor support is generated from the actual class/vtable.
//3bc840..3bc8a1,97B.
// FUNCTION: WON 0x007bc840
void GameStagingCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
        SafeGetComponent(config,mLeaveGameButton,"LeaveGameButton",__FILE__,33);
        SafeGetComponent(config,mStartGameButton,"StartGameButton",__FILE__,34);
        mStartGameButton->SetEnabled(false);
        Fn2F5360(1);
    }
}
//3bc8b0..3bc94c,156B.
// FUNCTION: WON 0x007bc8b0
void GameStagingCtrl::UpdateStartButton()
{
    if(mInGame) {
        LobbyGame* game=GetMyLobbyGame();
        if(game!=NULL) {
            bool enabled;
            if(game->mIsHost) enabled=game->Fn3A7490();
            else {
                if(mLastReady!=LobbyFn3613A0()) {
                    mLastReady=LobbyFn3613A0();
                    mStartGameButton->SetText(mLastReady?::GameStagingCtrl_NotReady_String : ::GameStagingCtrl_Ready_String,true);
                }
                if(!mLastReady) enabled=game->Fn3A74B0();
                else enabled=true;
            }
            mStartGameButton->SetEnabled(enabled);
        }
    }
}
//3bc950..3bc9f6,166B. Failed type predicate preserves existing mInGame state.
// FUNCTION: WON 0x007bc950
void GameStagingCtrl::JoinedGame()
{
    LobbyGame* game=GetMyLobbyGame();
    if(game!=NULL&&game->Fn3A6D40(mRoomSpecFlags)) {
        mInGame=true;
        if(game->mIsHost) {
            mLeaveGameButton->SetText(::GameStagingCtrl_DissolveGame_String,true);
            mStartGameButton->SetText(::GameStagingCtrl_StartGame_String,true);
        } else {
            mLeaveGameButton->SetText(::GameStagingCtrl_LeaveGame_String,true);
            mLastReady=LobbyFn3613A0();
            mStartGameButton->SetText(mLastReady?::GameStagingCtrl_NotReady_String : ::GameStagingCtrl_Ready_String,true);
        }
        UpdateStartButton();
    }
}
//3bca00..3bca08,8B.
// FUNCTION: WON 0x007bca00
void GameStagingCtrl::LeftGame() { mInGame=false; }
//3bca10..3bca8e,126B;2alignment+20dwordtable+33selectorbytes separately.
// FUNCTION: WON 0x007bca10
void GameStagingCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 100029:JoinedGame();break;
    case 100032:LeftGame();break;
    case 100054:mRoomSpecFlags|=static_cast<SetRoomSpecFlagsEvent*>(inEvent)->mFlags;break;
    case 100061:UpdateStartButton();break;
    }
    Component::HandleComponentEvent(inEvent);
}
}

/* Rejected empty explicit destructor candidates; zero handwritten credit.
GameStagingCtrl::~GameStagingCtrl() {}
Blocker: complete original normal destructors contain no own derived-vptr store.
Implicit member/base destruction is the source candidate; require actual normal
COFF functions/relocations/EH actions before admission.
*/

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bc780
// (public: virtual void * __thiscall WONAPI::GameStagingCtrl::`scalar deleting destructor'(unsigned int)    [??_GGameStagingCtrl@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007bc7a0
// (public: virtual __thiscall WONAPI::GameStagingCtrl::~GameStagingCtrl(void)    [??1GameStagingCtrl@WONAPI@@UAE@XZ])
