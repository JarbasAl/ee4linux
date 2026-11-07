// WONAPI staging game flow; original-backed StagingLogic family.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <time.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyStaging.h"
#include "ee/wonapi/WONLobbyDialogs.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/EELobbySound.h"
#include "LostDebug.h"
namespace WONAPI {
void LobbyFn361CB0(const ByteBuffer*);
void LobbyFn361CD0(const ByteBuffer*);
void LobbyFn361CF0(LobbyClient*, const ByteBuffer*);
void LobbyFn361D50(LobbyClient*, bool);
void LobbyFn361D70(bool);
void LobbyMessageBox(const GUIString&, const GUIString&);
bool LobbyFn3613A0();
class Sound;
void LobbyFn361D10(LobbyGame*);
void LobbyFn361D30(LobbyGame*);

// Actual original scope cleanup3f06d0, bool storage only; generated support, zero primary credit.
struct StagingLeaveOnError {
    bool mLeave;
    StagingLeaveOnError() : mLeave(true) {}
    ~StagingLeaveOnError() { if (mLeave) LobbyFn361310(false); }
};

// FUNCTION: WON 0x007ef160
StagingLogic::StagingLogic()
{
    mInStaging = false;
    mLastReadyTime = 0;
}

// FUNCTION: WON 0x007ef1b0
StagingLogic::~StagingLogic() {}

// FUNCTION: WON 0x007ef290
void StagingLogic::InitComponent(ComponentConfig* inConfig)
{
    SafeGetComponent(inConfig, mCreateGameDialog, "CreateGameDialog", __FILE__, 41);
}

// FUNCTION: WON 0x007ef2b0
void StagingLogic::Fn3EF2B0(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (inClient->Fn3A5580()) return;
    ConstSmartPtr<ByteBuffer> aReply = mGame->Fn3A7830(inBuffer, inClient);
    if (aReply.get() == NULL) return;
    Fn3EFDE0(inClient, aReply.get());
    LobbyPlayer* aPlayer = inClient->mPlayer.get();
    if (aPlayer != NULL) {
        WriteBuffer aBuffer(0, true);
        aBuffer.AppendByte(3);
        aBuffer.AppendShort(inClient->GetClientId());
        aPlayer->WriteToBuffer(aBuffer);
        Fn3EFDA0(aBuffer.ToByteBuffer(true).get());
        LobbyFn361260();
        return;
    }
}

// FUNCTION: WON 0x007ef410
void StagingLogic::Fn3EF410(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (inClient->Fn3A54E0(1)) {
        mJoinStatus = mGame->Fn3A78E0(inBuffer);
        if (mJoinOp.get() != NULL) mJoinOp->Fn29F220(0);
    }
}

// FUNCTION: WON 0x007ef450
void StagingLogic::Fn3EF450(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (!inClient->Fn3A54E0(1)) return;
    unsigned short aClientId = inBuffer.ReadShort();
    LobbyClient* aClient = mGame->FindClient(aClientId);
    if (aClient == NULL) return;
    if (!aClient->Fn3A5580()) {
        SmartPtr<LobbyPlayer> aPlayer = LobbyPlayer::CreatePlayer();
        if (!aPlayer->ReadFromBuffer(inBuffer)) return;
        aClient->Fn3A5520(aPlayer.get());
    } else if (!Fn3EFE10()) return;
    LobbyFn359100(new PlayerChangedEvent(aClient, 0), true);
    if (aClient->Fn3A55C0(mMyClient.get()))
        LobbyFn359100(new PlayerTeamChangedEvent(aClient, 0), true);
    mGame->Fn3A74D0(aClient);
    if (LobbyFn35E340()) LobbyFn35B9A0(::LobbyGlobal_PlayerJoined_Sound.get());
    if (mLastAllReady && !aClient->Fn3A5590()) mLastAllReady = false;
    Fn3F11F0();
}

// FUNCTION: WON 0x007ef620
void StagingLogic::Fn3EF620(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (Fn3EFE10() && inClient->Fn3A5580()) {
        bool aReady = inBuffer.ReadBool();
        if (inClient->Fn3A5590() != aReady) {
            WriteBuffer aBuffer(0, true);
            aBuffer.AppendByte(5);
            aBuffer.AppendShort(inClient->GetClientId());
            aBuffer.AppendBool(aReady);
            Fn3EFDA0(aBuffer.ToByteBuffer(true).get());
        }
    }
}

// FUNCTION: WON 0x007ef720
void StagingLogic::Fn3EF720(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (inClient->Fn3A54E0(1)) {
        unsigned short anId = inBuffer.ReadShort();
        bool aReady = inBuffer.ReadBool();
        LobbyClient* aClient = mGame->FindClient(anId);
        if (aClient != NULL && aClient->mPlayer.get() != NULL) {
            aClient->mPlayer->mBA = aReady;
            Fn3F1200();
            LobbyFn359100(new PlayerChangedEvent(aClient, 2), true);
        }
    }
}

// FUNCTION: WON 0x007ef7d0
void StagingLogic::Fn3EF7D0(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (Fn3EFE10()) {
        unsigned short aPing = inBuffer.ReadShort();
        if (inClient->mPlayer.get() != NULL) {
            WriteBuffer aBuffer(0, true);
            aBuffer.AppendByte(9);
            aBuffer.AppendShort(inClient->GetClientId());
            aBuffer.AppendShort(aPing);
            Fn3EFDA0(aBuffer.ToByteBuffer(true).get());
        }
    }
}

// FUNCTION: WON 0x007ef8c0
void StagingLogic::Fn3EF8C0(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (inClient->Fn3A54E0(1)) {
        unsigned short anId = inBuffer.ReadShort();
        unsigned short aPing = inBuffer.ReadShort();
        LobbyClient* aClient = mGame->FindClient(anId);
        if (aClient != NULL && aClient->mPlayer.get() != NULL) {
            aClient->mPlayer->mPlayerId = aPing;
            LobbyFn359100(new PlayerChangedEvent(aClient, 2), true);
        }
    }
}

// FUNCTION: WON 0x007ef970
void StagingLogic::Fn3EF970(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (inClient->Fn3A54E0(1) && !Fn3EFE10()) {
        Fn3F0C60(false);
        LobbyMessageBox(::RoutingLogic_GameDissolvedTitle_String, ::RoutingLogic_GameDissolvedDetails_String);
    }
}

// FUNCTION: WON 0x007ef9b0
void StagingLogic::Fn3EF9B0()
{
    if (mGame.get() == NULL) return;
    LobbyClientSetIterator anItr(mGame->mClients.get());
    while (anItr.HasMore()) {
        LobbyClient* aClient = NULL;
        LobbyPlayer* aPlayer = anItr.Next(&aClient);
        if (aPlayer != NULL && aClient != NULL) {
            aPlayer->mBA = false;
            LobbyFn359100(new PlayerChangedEvent(aClient, 2), true);
        }
    }
    Fn3F1200();
}

// FUNCTION: WON 0x007efa80
void StagingLogic::Fn3EFA80(bool inValue)
{
    if (mGame.get() != NULL && mGame->mB13 != inValue && Fn3EFE10()) {
        mGame->mB13 = inValue;
        LobbyFn361D70(inValue);
        LobbyFn361260();
    }
}

// FUNCTION: WON 0x007efac0
void StagingLogic::Fn3EFAC0(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (inClient->Fn3A54E0(1)) {
        if (Fn3EFE10()) {
            Fn3EFA80(true);
            LobbyFn3627F0(2);
        } else LobbyFn3627F0(1);
    }
}

// FUNCTION: WON 0x007efb00
void StagingLogic::Fn3EFB00(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (inClient->Fn3A54E0(1)) {
        unsigned short anId = inBuffer.ReadShort();
        bool aFlag = inBuffer.ReadBool();
        LobbyClient* aClient = mGame->FindClient(anId);
        if (aClient != NULL) Fn3F0E10(aClient, aFlag);
    }
}

// FUNCTION: WON 0x007efb50
void StagingLogic::Fn3EFB50(ReadBuffer& inBuffer, LobbyClient* inClient)
{
    if (mGame.get() == NULL || inClient == NULL) return;
    try {
        unsigned char aType = inBuffer.ReadByte();
        if (mJoinStatus == 1 && aType != 2) return;
        switch (aType) {
        case 1: Fn3EF2B0(inBuffer, inClient); break;
        case 2: Fn3EF410(inBuffer, inClient); break;
        case 3: Fn3EF450(inBuffer, inClient); break;
        case 4: Fn3EF620(inBuffer, inClient); break;
        case 5: Fn3EF720(inBuffer, inClient); break;
        case 6: Fn3EF970(inBuffer, inClient); break;
        case 7: Fn3EFAC0(inBuffer, inClient); break;
        case 8: Fn3EF7D0(inBuffer, inClient); break;
        case 9: Fn3EF8C0(inBuffer, inClient); break;
        case 10: Fn3EFB00(inBuffer, inClient); break;
        default: mGame->Fn3A79E0(aType, inBuffer, inClient); break;
        }
    } catch (ReadBufferException&) {}
}

// FUNCTION: WON 0x007efd20
void StagingLogic::Fn3EFD20(const ByteBuffer* inMessage, LobbyClient* inClient)
{
    if (inMessage != NULL) {
        ReadBuffer aBuffer(inMessage->data(), inMessage->length());
        Fn3EFB50(aBuffer, inClient);
    }
}

// FUNCTION: WON 0x007efd60
void StagingLogic::Fn3EFD60(const ByteBuffer* inMessage)
{
    if (mGame.get() != NULL) {
        if (Fn3EFE10()) Fn3EFD20(inMessage, mMyClient.get());
        else LobbyFn361CB0(inMessage);
    }
}

// FUNCTION: WON 0x007efda0
void StagingLogic::Fn3EFDA0(const ByteBuffer* inMessage)
{
    if (mGame.get() != NULL && Fn3EFE10()) {
        Fn3EFD20(inMessage, mMyClient.get());
        LobbyFn361CD0(inMessage);
    }
}

// FUNCTION: WON 0x007efde0
void StagingLogic::Fn3EFDE0(LobbyClient* inClient, const ByteBuffer* inMessage)
{
    if (mGame.get() != NULL && Fn3EFE10()) LobbyFn361CF0(inClient, inMessage);
}

// FUNCTION: WON 0x007efe10
bool StagingLogic::Fn3EFE10()
{
    return mGame.get() != NULL && mGame->mIsHost;
}

// FUNCTION: WON 0x007efe30
bool StagingLogic::Fn3EFE30()
{
    return mMyClient.get() != NULL && mMyClient->Fn3A5590();
}

// FUNCTION: WON 0x007efe50
void StagingLogic::HandleControlEvent(int inControlId)
{
    switch (inControlId) {
    case 0x30d4b: Fn3F0410(); return;
    case 0x30d4e: Fn3F0C60(true); return;
    case 0x30d4a: Fn3F0110(true); return;
    case 0x30d4f: Fn3F0390(); return;
    }
}

// FUNCTION: WON 0x007efeb0
void StagingLogic::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch (inEvent->mType) {
    case 0x186bb: Fn3F08A0(inEvent); break;
    case 0x186b1: Fn3EFFA0(inEvent); break;
    }
}

// FUNCTION: WON 0x007efee0
void StagingLogic::Fn3EFEE0(LobbyGame* inGame)
{
    if (inGame != NULL) {
        SmartPtr<GameOptionsPopup> aPopup = new GameOptionsPopup;
        aPopup->Init(inGame);
        LobbyFn358DF0(aPopup.get(), 0x23e7, -1, -1);
    }
}

// FUNCTION: WON 0x007effa0
void StagingLogic::Fn3EFFA0(ComponentEvent* inEvent)
{
    GameActionEvent* anEvent = static_cast<GameActionEvent*>(inEvent);
    LobbyGame* aGame = anEvent->mGame.get();
    if (aGame == NULL) return;
    switch (anEvent->mAction) {
    case 7: Fn3EFEE0(aGame); break;
    case 1: Fn3F08B0(aGame); break;
    case 2:
        LobbyFn35B900(aGame, true);
        LobbyFn359100(new GameChangedEvent(aGame, 2), true);
        break;
    case 3:
    case 4:
        LobbyFn35B920(aGame);
        aGame->mB16 = true;
        LobbyFn359100(new GameChangedEvent(aGame, 2), true);
        break;
    case 5:
        aGame->mB16 = false;
        LobbyFn359100(new GameChangedEvent(aGame, 2), true);
        break;
    case 6:
        if (aGame->GetGameType() == 3)
            LobbyFn359300(new ChangeDirectConnectEvent(1, aGame));
        break;
    }
}

// FUNCTION: WON 0x007f0110
bool StagingLogic::Fn3F0110(bool inConfirm)
{
    if (!Fn3F0C60(inConfirm)) return false;
    mCreateGameDialog->Reset(true);
    LobbyFn361DD0();
    LobbyFn35B960(0);
    LobbyFn361020();
    LobbyFn35E660(false);
    LobbyFn35E680(false);
    ::LobbyGlobal_SierraLogon_Sound->Stop();
    return true;
}

// FUNCTION: WON 0x007f0170
void StagingLogic::Fn3F0170(bool inReady)
{
    WriteBuffer aBuffer(0, true);
    aBuffer.AppendByte(4);
    aBuffer.AppendBool(inReady);
    Fn3EFD60(aBuffer.ToByteBuffer(true).get());
}

// FUNCTION: WON 0x007f0230
void StagingLogic::Fn3F0230()
{
    if (Fn3EFE10()) {
        WriteBuffer aBuffer(0, true);
        aBuffer.AppendByte(6);
        Fn3EFDA0(aBuffer.ToByteBuffer(true).get());
    }
}

// FUNCTION: WON 0x007f02e0
void StagingLogic::Fn3F02E0()
{
    WriteBuffer aBuffer(0, true);
    aBuffer.AppendByte(7);
    Fn3EFDA0(aBuffer.ToByteBuffer(true).get());
}

// FUNCTION: WON 0x007f0390
void StagingLogic::Fn3F0390()
{
    if (mMyClient.get() == NULL || mGame.get() == NULL) return;
    if (Fn3EFE10()) {
        if (mGame->Fn3A7490() && mGame->Fn3A74C0()) Fn3F02E0();
    } else {
        bool aReady = !mMyClient->Fn3A5590();
        if (aReady && !mGame->Fn3A74B0()) return;
        mLastReady = aReady;
        Fn3F0170(aReady);
    }
}

// FUNCTION: WON 0x007f0410
void StagingLogic::Fn3F0410()
{
    if (!Fn3F0C60(true)) return;
    int aGameType = LobbyFn361190();
    if (aGameType == 0) return;
    if (aGameType != 1 && LobbyFn35E360()) {
        int aResult = 0x186a1;
        if (!LobbyFn361B40()) aResult = LobbyFn358EF0(::GameStagingLogic_ConnectWarningTitle1_String, ::GameStagingLogic_ConnectWarningDetails1_String, true);
        else if (LobbyFn361B20()) aResult = LobbyFn358EF0(::GameStagingLogic_ConnectWarningTitle2_String, ::GameStagingLogic_ConnectWarningDetails2_String, true);
        if (aResult != 0x186a1) return;
    }
    mCreateGameDialog->SetGameType(aGameType);
    mCreateGameDialog->Reset(false);
    if (LobbyDoDialog(mCreateGameDialog.get()) != 0x186a1) return;
    SmartPtr<LobbyGame> aGame = mCreateGameDialog->GetGame();
    aGame->SetAddress(LobbyFn3617A0(aGameType));
    mGame = aGame;
    mJoinStatus = 0;
    StagingLeaveOnError aCleanup;
    if (!aGame->Fn3A6DC0()) return;
    if (!LobbyFn361C30(aGame.get())) return;
    mMyClient = LobbyFn361C90();
    if (mMyClient.get() == NULL) return;
    aGame->SetIsHost(true);
    aGame->SetCaptainId(mMyClient->GetClientId());
    mMyClient->Fn3A5500(true, true);
    if (!aGame->Fn3A6DD0()) return;
    aGame->mClients->Fn3A5CC0(mMyClient.get());
    aGame->Fn3A7360(mMyClient.get());
    aCleanup.mLeave = false;
    mInStaging = true;
    mLastReadyTime = 0;
    mLastAllReady = aGame->Fn3A7400();
    mLastReady = LobbyFn3613A0();
    LobbyFn361260();
    LobbyFn359120(0x186bd, true);
}

// FUNCTION: WON 0x007f06e0
bool StagingLogic::Fn3F06E0(LobbyGame* inGame)
{
    if (inGame->GetGameType() == 2 && !inGame->mB18) {
        LobbyConfig* aConfig = GetLobbyConfig();
        if (aConfig != NULL && !aConfig->mAllowJoinGameWithoutPing) {
            LobbyFn35B900(inGame, true);
            LobbyFn359100(new GameChangedEvent(inGame, 2), true);
            while (inGame->Fn3A6D70()) {
                SmartPtr<AsyncOp> anOp = new AsyncOp;
                LobbyFn358E70(anOp.get(), ::GameStagingLogic_VerifyingPingTitle_String, ::GameStagingLogic_VerifyingPingDetails_String, 2, 500);
                if (anOp->mB39) return false;
            }
            if (!inGame->mB18) {
                LobbyMessageBox(::GameStagingLogic_JoinGameErrorTitle_String, ::GameStagingLogic_NoPingReplyError_String);
                return false;
            }
        } else if (LobbyFn35E360()) {
            return LobbyFn358EF0(::GameStagingLogic_ConnectWarningTitle3_String, ::GameStagingLogic_ConnectWarningDetails3_String, true) == 0x186a1;
        }
    }
    return true;
}

// FUNCTION: WON 0x007f08a0
void StagingLogic::Fn3F08A0(ComponentEvent* inEvent)
{
    Fn3F08B0(static_cast<JoinGameEvent*>(inEvent)->mGame.get());
}

// FUNCTION: WON 0x007f08b0
void StagingLogic::Fn3F08B0(LobbyGame* inGame)
{
    if (inGame == NULL) return;
    if (mGame.get() != NULL && inGame->Fn3A6D00(mGame.get())) {
        LobbyMessageBox(::RoutingLogic_AlreadyJoinedTitle_String, ::RoutingLogic_AlreadyInGame_String);
        return;
    }
    if (!Fn3F06E0(inGame)) return;
    short aStatus = inGame->GetJoinStatus();
    if (aStatus != 0) {
        if (aStatus != 1) LobbyMessageBox(::GameStagingLogic_JoinGameErrorTitle_String, inGame->Fn3A7A90(aStatus));
        return;
    }
    if (!Fn3F0C60(true)) return;
    StagingLeaveOnError aCleanup;
    SmartPtr<LobbyGame> aGame = LobbyGame::CreateGame(static_cast<LobbyGameType>(inGame->GetGameType()));
    aGame->CopyFrom(inGame);
    mGame = aGame;
    mJoinStatus = 1;
    if (!aGame->Fn3A6DA0()) return;
    if (!LobbyFn361C60(aGame.get())) return;
    mMyClient = LobbyFn361C90();
    if (mMyClient.get() == NULL) return;
    if (!aGame->Fn3A6DB0()) return;
    mJoinOp = new AsyncOp;
    ConstSmartPtr<ByteBuffer> aMessage = mGame->Fn3A72C0();
    Fn3EFD60(aMessage.get());
    LobbyFn358E70(mJoinOp.get(), ::GameStagingLogic_GettingGameInfoTitle_String, ::GameStagingLogic_GettingGameInfoDetails_String, 1, 0);
    mJoinOp = NULL;
    if (mJoinStatus != 0) {
        if (mJoinStatus != 1) LobbyMessageBox(::GameStagingLogic_JoinGameErrorTitle_String, inGame->Fn3A7A90(mJoinStatus));
        return;
    }
    aCleanup.mLeave = false;
    mInStaging = true;
    mLastReadyTime = 0;
    mLastAllReady = aGame->Fn3A7400();
    mLastReady = LobbyFn3613A0();
    // LOST-DEBUG: none: exact only
    EE_LOST_TRACE(mGame.get());
    LobbyFn359120(0x186bd, true);
}

// FUNCTION: WON 0x007f0c60
bool StagingLogic::Fn3F0C60(bool inConfirm)
{
    if (mGame.get() != NULL) {
        if (inConfirm && LobbyFn35E360()) {
            int aResult;
            if (Fn3EFE10()) aResult = LobbyFn358EF0(::GameStagingLogic_DissolveGame_String, ::GameStagingLogic_DissolveGameConfirm_String, true);
            else aResult = LobbyFn358EF0(::GameStagingLogic_LeaveGame_String, ::GameStagingLogic_LeaveGameConfirm_String, true);
            if (aResult != 0x186a1) return false;
        }
        if (mGame.get() != NULL) {
            if (Fn3EFE10()) {
                Fn3F0230();
                LobbyFn361D10(mGame.get());
            }
            if (mGame.get() != NULL) {
                if (mJoinOp.get() != NULL) {
                    mJoinOp->Kill(true);
                    mJoinOp = NULL;
                }
                SmartPtr<LobbyGame> aGame = mGame;
                mGame = NULL;
                aGame->Fn3A6C90();
                mMyClient = NULL;
                aGame->Fn3A6DE0();
                LobbyFn361D30(aGame.get());
                aGame->Fn3A6DF0();
                mLastReadyTime = 0;
                if (mInStaging) {
                    LobbyFn359120(0x186c0, true);
                    mInStaging = false;
                }
            }
        }
    }
    return true;
}

// FUNCTION: WON 0x007f0de0
void StagingLogic::Fn3F0DE0(bool inShowError)
{
    if (mGame.get() != NULL) {
        Fn3F0C60(false);
        if (inShowError) LobbyMessageBox(::StagingLogic_DisconnectedTitle_String, ::StagingLogic_DisconnectedDetails_String);
    }
}

// FUNCTION: WON 0x007f0e10
void StagingLogic::Fn3F0E10(LobbyClient* inClient, bool inFlag)
{
    LobbyGame* aGame = mGame.get();
    if (inClient == NULL || aGame == NULL) return;
    int aKind = !inFlag;
    bool isMe = inClient == mMyClient.get();
    int aFlags = aGame->GetGameType() == 2 ? 6 : 10;
    LobbyFn359100(new ClientModeratedEvent(inClient, aFlags, mGame->mName,
        aKind, 0, isMe, gEmptyGUIString), true);
    if (isMe) {
        Fn3F0C60(false);
        if (inFlag) LobbyMessageBox(::StagingLogic_BannedTitle_String, ::StagingLogic_BannedDetails_String);
        else LobbyMessageBox(::StagingLogic_KickedTitle_String, ::StagingLogic_KickedDetails_String);
    }
}

// FUNCTION: WON 0x007f0f10
void StagingLogic::Fn3F0F10(LobbyClient* inClient)
{
    if (inClient->Fn3A5580() && mGame.get() != NULL) {
        LobbyFn359100(new PlayerChangedEvent(inClient, 1), true);
        if (inClient->Fn3A55C0(mMyClient.get()))
            LobbyFn359100(new PlayerTeamChangedEvent(inClient, 1), true);
        mGame->RemoveClient(inClient);
        if (Fn3EFE10()) LobbyFn361260();
        Fn3F11F0();
    }
}

// FUNCTION: WON 0x007f1000
void StagingLogic::Fn3F1000(LobbyGame* inGame)
{
    if (mGame.get() != NULL && !Fn3EFE10() && inGame->Fn3A6D00(mGame.get())) {
        WriteBuffer aBuffer(0, true);
        aBuffer.AppendByte(8);
        aBuffer.AppendShort(inGame->GetPing());
        Fn3EFD60(aBuffer.ToByteBuffer(true).get());
    }
}

// FUNCTION: WON 0x007f10f0
void StagingLogic::Fn3F10F0(LobbyClient* inClient, bool inFlag)
{
    if (inClient != NULL && mGame.get() != NULL && Fn3EFE10()) {
        WriteBuffer aBuffer(0, true);
        aBuffer.AppendByte(10);
        aBuffer.AppendShort(inClient->GetClientId());
        aBuffer.AppendBool(inFlag);
        Fn3EFDA0(aBuffer.ToByteBuffer(true).get());
        LobbyFn361D50(inClient, inFlag);
    }
}

// FUNCTION: WON 0x007f11f0
void StagingLogic::Fn3F11F0()
{
    LobbyFn359120(0x186dd, true);
}

// FUNCTION: WON 0x007f1200
void StagingLogic::Fn3F1200()
{
    Fn3F11F0();
    LobbyGame* aGame = mGame.get();
    if (aGame == NULL) return;
    bool isReady = aGame->Fn3A7400();
    if (isReady) {
        if (mLastAllReady != isReady) {
            long aTime = time(NULL);
            if (aTime - mLastReadyTime >= 15) {
                mLastReadyTime = aTime;
                if (LobbyFn35E340()) {
                    LobbyFn35B9A0(::LobbyGlobal_AllReady_Sound.get());
                }
            }
        }
    } else if (!aGame->mIsHost && mLastReady != LobbyFn3613A0()) {
        mLastReady = LobbyFn3613A0();
        if (!mLastReady && LobbyFn35E340()) LobbyFn35B9A0(::LobbyGlobal_AllNotReady_Sound.get());
    }
    mLastAllReady = isReady;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007ef190
// (public: virtual void * __thiscall WONAPI::StagingLogic::`scalar deleting destructor'(unsigned int)    [??_GStagingLogic@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f06d0
// (public: __thiscall WONAPI::StagingLeaveOnError::~StagingLeaveOnError(void)    [??1StagingLeaveOnError@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007f45d0

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f16f0
// (public: class WONAPI::LobbyGame const * __thiscall WONAPI::SmartPtrRep<class WONAPI::LobbyGame>::operator=(class WONAPI::LobbyGame const *))
