// Original-backed LAN logic family; ordinary full COFF/EH/data capture required.
// Source-only near bodies and generated support retain original-carved execution limits.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <time.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyDialogs.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/WONLobbyLan.h"

namespace WONAPI {
// Canonical names to reconcile with account/model before compilation.
AuthContext* GetLobbyAuthContext(); // actual cdecl auth-context getter

// FUNCTION: WON 0x007dfc00
LanLogic::LanLogic()
{
    mNextClientId = 0;
    mLastServerPing = 0;
    mIsCaptain = false;
    mB74 = false;
    mClients = new LobbyClientList;
    mLanGames = new LobbyGameList(1);
    mDirectGames = new LobbyGameList(3);
}

// FUNCTION: WON 0x007dfef0
LanLogic::~LanLogic() {}

// Original filename/line association proves the four typed resources.

// FUNCTION: WON 0x007e00a0



























































void LanLogic::InitComponent(ComponentConfig* inConfig)
{
    SafeGetComponent(inConfig, mDirectConnectDialog, "DirectConnectDialog", __FILE__, 105);
    SafeGetComponent(inConfig, mLanNameDialog, "LanNameDialog", __FILE__, 106);
    SafeGetComponent(inConfig, mLanPortDialog, "LanPortDialog", __FILE__, 107);
    SafeGetComponent(inConfig, mPasswordDialog, "PasswordDialog", __FILE__, 108);
}

// FUNCTION: WON 0x007e0110
unsigned short LanLogic::Fn3E0110()
{
    if (mNextClientId == 0xffff || mNextClientId == 0)
        ++mNextClientId;
    return mNextClientId++;
}

// FUNCTION: WON 0x007e0140
bool LanLogic::Fn3E0140()
{
    return mListenSocket.get() != NULL || mServerSocket.get() != NULL;
}

// FUNCTION: WON 0x007e0160
bool LanLogic::Fn3E0160() { return mIsCaptain; }

// FUNCTION: WON 0x007e0170
LobbyClientList* LanLogic::Fn3E0170() { return mClients.get(); }

// FUNCTION: WON 0x007e0180
LobbyClient* LanLogic::Virtual20() { return mMyClient.get(); }

// Typed reference calls only: no socket object construction or guessed fields.
// FUNCTION: WON 0x007e0190
void LanLogic::Fn3E0190()
{
    AuthContext* anAuthContext = GetLobbyAuthContext();
    if (anAuthContext != NULL)
        anAuthContext->Fn2DEB30(false);
    mNextClientId = 0;
    mIsCaptain = false;
    mB74 = false;
    if (mListenSocket.get() != NULL) {
        mListenSocket->Close();
        mListenSocket = NULL;
    }
    if (mServerSocket.get() != NULL) {
        mServerSocket->QueueClose(-1);
        mServerSocket = NULL;
    }
    if (mNetworkTimer.get() != NULL) {
        mNetworkTimer->Kill(true);
        mNetworkTimer = NULL;
    }
    for (std::set<SmartPtr<LobbyLanSocket> >::iterator anItr = mPendingSockets.begin();
         anItr != mPendingSockets.end(); ++anItr)
        (*anItr)->QueueClose(-1);
    mMyClient = NULL;
    mPendingSockets.clear();
    mClients->Fn3A5770();
}

// New AsyncOp/LogicCompletion objects require model's complete canonical types.
// Their pointed-to sockets are never constructed here.
// FUNCTION: WON 0x007e0620
void LanLogic::Fn3E0620()
{
    if (mNetworkTimer.get() == NULL) {
        mNetworkTimer = new AsyncOp;
        mNetworkTimer->mCompletion = new LogicCompletion<LanLogic>(&LanLogic::Fn3E04E0, this);
    }
    mNetworkTimer->RunAsync(120000);
}

// FUNCTION: WON 0x007e53a0
void LanLogic::Fn3E53A0()
{
    if (mDiscoveryTimer.get() == NULL) {
        mDiscoveryTimer = new AsyncOp;
        mDiscoveryTimer->mCompletion = new LogicCompletion<LanLogic>(&LanLogic::Fn3E4BE0, this);
    }
    mDiscoveryTimer->RunAsync(15000);
}

// Local WriteBuffer construction awaits model's complete actual field contract.
// AppendBuffer second argument is an unsigned-byte length field size, not bool.
// FUNCTION: WON 0x007e1c30
void LanLogic::Virtual24(ByteBuffer* inBuffer)
{
    WriteBuffer aMessage(2, true);
    aMessage.AppendByte(8);
    aMessage.AppendBuffer(inBuffer, 0);
    Fn3E1980(aMessage.ToByteBuffer(true).get());
}

// FUNCTION: WON 0x007e1cf0
void LanLogic::Virtual28(ByteBuffer* inBuffer)
{
    WriteBuffer aMessage(2, true);
    aMessage.AppendByte(8);
    aMessage.AppendBuffer(inBuffer, 0);
    Fn3E1AD0(aMessage.ToByteBuffer(true).get(), false);
}

// FUNCTION: WON 0x007e1db0
void LanLogic::Virtual2C(LobbyClient* inClient, ByteBuffer* inBuffer)
{
    WriteBuffer aMessage(2, true);
    aMessage.AppendByte(8);
    aMessage.AppendBuffer(inBuffer, 0);
    Fn3E1840(inClient, aMessage.ToByteBuffer(true).get());
}

// FUNCTION: WON 0x007e4590
void LanLogic::Virtual38(LobbyClient* inClient, bool)
{
    if (Fn3E0160())
        static_cast<LobbyLanClient*>(inClient)->mSocket->QueueClose(1000);
}

// FUNCTION: WON 0x007e45b0
void LanLogic::Virtual40(LobbyGame*) { Fn3E0190(); }

// FUNCTION: WON 0x007e45c0
void LanLogic::Virtual30(LobbyGame* inGame)
{
    if (inGame->GetGameType() == 1)
        Fn3E4630(inGame, mDiscoveryAddress);
}

// FUNCTION: WON 0x007e45e0
void LanLogic::Virtual34(LobbyGame* inGame)
{
    if (inGame->GetGameType() == 1)
        Fn3E4A10(inGame);
}


void LanLogic::Virtual3C(bool) {}
void LanLogic::Virtual44() {}

// FUNCTION: WON 0x007e5900
void LanLogic::Virtual48() { LobbyFn361310(0); }

// FUNCTION: WON 0x007e5910
void LanLogic::Virtual4C()
{
    Fn3E5810();
    mDirectGames->Clear();
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e1840
void LanLogic::Fn3E1840(LobbyClient* inClient, ByteBuffer* inBuffer)
{
    if (!Fn3E0160() || !Fn3E0140())
        return;
    if (inClient == mMyClient.get()) {
        Fn3E2610(inBuffer, mMyClient.get());
        return;
    }
    LobbyLanClient* aClient = static_cast<LobbyLanClient*>(inClient);
    if (aClient->mSocket == NULL)
        return;
    aClient->mSocket->Fn2B4740(new SendMsgOp(inBuffer), -1);
}

// FUNCTION: WON 0x007e1980
void LanLogic::Fn3E1980(ByteBuffer* inBuffer)
{
    if (!Fn3E0140())
        return;
    if (Fn3E0160()) {
        Fn3E2610(inBuffer, mMyClient.get());
        return;
    }
    if (mServerSocket.get() != NULL) {
        mLastServerPing = time(NULL);
        mServerSocket->Fn2B4740(new SendMsgOp(inBuffer), -1);
    }
}

// FUNCTION: WON 0x007e4be0
void LanLogic::Fn3E4BE0(AsyncOp* inOp)
{
    if (inOp != mDiscoveryTimer.get())
        return;
    if (Fn3E0160()) {
        LobbyGame* aGame = GetMyLobbyGame();
        if (aGame != NULL)
            Fn3E4800(aGame);
    }
    if (mLanGames->RemoveStaleGames(30))
        LobbyFn359100(new SyncGameListEvent(mLanGames.get()), true);
    Fn3E53A0();
}

// FUNCTION: WON 0x007e5810
void LanLogic::Fn3E5810()
{
    if (!mLanGames->IsEmpty()) {
        mLanGames->Clear();
        LobbyFn359100(new SyncGameListEvent(mLanGames.get()), true);
    }
    if (mDiscoveryTimer.get() != NULL) {
        mDiscoveryTimer->Kill(true);
        mDiscoveryTimer = NULL;
    }
    if (mDiscoverySocket.get() != NULL) {
        mDiscoverySocket->Close();
        mDiscoverySocket = NULL;
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e4600
bool LanLogic::Fn3E4600(LobbyGame* inGame)
{
    if (!Fn3E0160() || mDiscoverySocket.get() == NULL || inGame == NULL)
        return false;
    return inGame->GetGameType() == 1 ? true : false;
}

// FUNCTION: WON 0x007e4630
void LanLogic::Fn3E4630(LobbyGame* inGame, const IPAddr& inAddress)
{
    if (!Fn3E4600(inGame))
        return;
    WriteBuffer aBuffer(0, true);
    aBuffer.AppendByte(1);
    aBuffer.AppendShort(inGame->mAddress.GetPort());
    inGame->WriteSummary(aBuffer);
    mDiscoverySocket->Fn2B4740(new SendBytesToOp(aBuffer.ToByteBuffer(true).get(), inAddress), -1);
}

// FUNCTION: WON 0x007e4800
void LanLogic::Fn3E4800(LobbyGame* inGame)
{
    if (!Fn3E4600(inGame))
        return;
    long aTime = time(NULL);
    if (mLastBroadcastTime > 0 && aTime - mLastBroadcastTime >= 30)
        Fn3E4630(inGame, mDiscoveryAddress);
    else {
        WriteBuffer aBuffer(0, true);
        aBuffer.AppendByte(2);
        aBuffer.AppendShort(inGame->mAddress.GetPort());
        mDiscoverySocket->Fn2B4740(new SendBytesToOp(aBuffer.ToByteBuffer(true).get(), mDiscoveryAddress), -1);
    }
    mLastBroadcastTime = aTime;
}

// FUNCTION: WON 0x007e4a10
void LanLogic::Fn3E4A10(LobbyGame* inGame)
{
    if (!Fn3E4600(inGame))
        return;
    WriteBuffer aBuffer(0, true);
    aBuffer.AppendByte(3);
    aBuffer.AppendShort(inGame->mAddress.GetPort());
    mDiscoverySocket->Fn2B4740(new SendBytesToOp(aBuffer.ToByteBuffer(true).get(), mDiscoveryAddress), -1);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e1e70
void LanLogic::Fn3E1E70(ReadBuffer& inBuffer)
{
    unsigned short anId = inBuffer.ReadShort();
    std::wstring aName;
    inBuffer.ReadWString(aName, 2);
    SmartPtr<LobbyClient> aClient;
    if (Fn3E0160())
        aClient = mClients->FindClient(anId);
    else {
        SmartPtr<LobbyLanClient> aNewClient = new LobbyLanClient;
        aNewClient->mClientId = anId;
        aNewClient->mName = GUIString(new GUIStringData(aName));
        aClient = aNewClient.get();
        mClients->Fn3A5CC0(aClient.get());
    }
    if (aClient.get() != NULL) {
        int aRoomSpec = 8;
        LobbyFn359100(new ClientChangedEvent(aClient.get(), aRoomSpec, 0), true);
    }
}

// FUNCTION: WON 0x007e2080
void LanLogic::Fn3E2080(ReadBuffer& inBuffer)
{
    unsigned short aClientId = inBuffer.ReadShort();
    SmartPtr<LobbyClient> aClient = mClients->Fn3A5D90(aClientId);
    if (aClient.get() != NULL) {
        LobbyFn359100(new ClientChangedEvent(aClient.get(), 8, 1), true);
        LobbyFn361E70(aClient.get());
    }
}

// FUNCTION: WON 0x007e2160
void LanLogic::Fn3E2160(ReadBuffer& inBuffer, LobbyClient* inSender)
{
    if (!Fn3E0160())
        return;
    unsigned short aFlags = inBuffer.ReadShort();
    std::wstring aText;
    inBuffer.ReadWString(aText, 2);
    unsigned short aTargetId = inBuffer.ReadShort();
    WriteBuffer aBuffer(2, true);
    aBuffer.AppendByte(6);
    aBuffer.AppendShort(aFlags);
    aBuffer.AppendShort(inSender->GetClientId());
    aBuffer.AppendWString(aText, 2);
    aBuffer.AppendShort(aTargetId);
    ByteBufferPtr aMessage = aBuffer.ToByteBuffer(true);
    if (aFlags & 4) {
        LobbyClient* aTarget = mClients->FindClient(aTargetId);
        if (aTarget == NULL)
            return;
        Fn3E1840(aTarget, aMessage.get());
        if (inSender != aTarget)
            Fn3E1840(inSender, aMessage.get());
    }
    else if (aFlags & 0x100) {
        LobbyClientIterator anItr(mClients.get());
        while (anItr.HasMore()) {
            LobbyClient* aClient = anItr.Next();
            if (aClient->Fn3A55C0(inSender))
                Fn3E1840(aClient, aMessage.get());
        }
    }
    else
        Fn3E1AD0(aMessage.get(), true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e23a0
void LanLogic::Fn3E23A0(ReadBuffer& inBuffer)
{
    unsigned short aFlags = inBuffer.ReadShort();
    unsigned short aSenderId = inBuffer.ReadShort();
    std::wstring aText;
    inBuffer.ReadWString(aText, 2);
    unsigned short aTargetId = inBuffer.ReadShort();
    LobbyClient* aSender = mClients->FindClient(aSenderId);
    if (aSender == NULL || aSender->mB22)
        return;
    LobbyClient* aTarget = NULL;
    if (aFlags & 4) {
        aTarget = mClients->FindClient(aTargetId);
        aFlags = 4;
        if (aTarget == NULL)
            return;
        if (aSender != mMyClient.get()) {
            if (LobbyFn35E330())
                LobbyFn35B9A0(::LobbyGlobal_WhisperRecv_Sound.get());
            ChatCommandParser* aParser = LobbyFn35B710();
            if (aParser != NULL)
                aParser->mInput = aSender->mName;
        }
    }
    int aRoomSpec = 8;
    LobbyFn359100(new RecvChatEvent(aRoomSpec, aFlags, aSender,
        GUIString(new GUIStringData(aText)), aTarget), true);
}

// FUNCTION: WON 0x007e25f0
void LanLogic::Fn3E25F0(ReadBuffer& inBuffer, LobbyClient* inSender)
{
    LobbyFn361E30(inBuffer, inSender);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e2610
void LanLogic::Fn3E2610(ByteBuffer* inBuffer, LobbyClient* inClient)
{
    if (inBuffer == NULL || inClient == NULL)
        return;
    unsigned long anOffset = 0;
    if (Fn3E0160() && inClient == mMyClient.get())
        anOffset = 2;
    ReadBuffer aBuffer(inBuffer->data() + anOffset, inBuffer->length() - anOffset);
    try {
        switch (aBuffer.ReadByte()) {
        case 3: Fn3E1E70(aBuffer); break;
        case 4: Fn3E2080(aBuffer); break;
        case 5: Fn3E2160(aBuffer, inClient); break;
        case 6: Fn3E23A0(aBuffer); break;
        case 8: Fn3E25F0(aBuffer, inClient); break;
        }
    }
    catch (ReadBufferException&) {}
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e28d0
void LanLogic::Fn3E28D0(ChangeDirectConnectEvent* inEvent)
{
    if (inEvent->IsDelete()) {
        if (inEvent->mGame.get() == NULL)
            return;
        LobbyFn35D870(inEvent->mGame->mAddress);
        mDirectGames->RemoveGame(inEvent->mGame->mAddress);
        LobbyFn359100(new GameChangedEvent(inEvent->mGame.get(), 1), true);
        return;
    }
    if (!inEvent->IsAdd())
        return;
    mDirectConnectDialog->Reset();
    if (LobbyDoDialog(mDirectConnectDialog.get()) != 0x186a1)
        return;
    IPAddr anAddress;
    LobbyConfig* aConfig = GetLobbyConfig();
    unsigned short aDefaultPort = LobbyFn361400();
    if (aConfig != NULL)
        aDefaultPort = aConfig->mLobbyPort;
    anAddress.Fn2B2DF0(mDirectConnectDialog->GetAddress(), aDefaultPort);
    if (!anAddress.IsValid()) {
        SmartPtr<DNSTestOp> anOp = new DNSTestOp(mDirectConnectDialog->GetAddress());
        LobbyFn358E70(anOp.get(), ::LANLogic_CheckingAddressTitle_String, ::LANLogic_CheckingAddressDetails_String, 1, 0);
        if (!anOp->Succeeded())
            return;
        anAddress = anOp->GetHostAddr();
        if (anAddress.GetPort() == 0)
            anAddress.SetPort(aDefaultPort);
    }
    GUIString aName = mDirectConnectDialog->GetName();
    if (aName.length() == 0)
        aName = mDirectConnectDialog->GetAddress();
    bool aFlag = false;
    LobbyGame* aGame = mDirectGames->GetOrCreateGame(anAddress, &aFlag);
    aGame->mName = aName;
    LobbyFn35D7F0(anAddress, aName);
    LobbyFn359100(new GameChangedEvent(aGame, aFlag ? 2 : 0), true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e4c90
void LanLogic::Fn3E4C90(ReadBuffer&, const IPAddr& inAddress)
{
    LobbyGame* aGame = GetMyLobbyGame();
    if (aGame != NULL)
        Fn3E4630(aGame, inAddress);
}

// FUNCTION: WON 0x007e4cb0
void LanLogic::Fn3E4CB0(ReadBuffer& inBuffer, const IPAddr& inAddress)
{
    IPAddr anAddress = inAddress;
    anAddress.SetPort(inBuffer.ReadShort());
    bool aFlag = false;
    LobbyGame* aGame = mLanGames->GetOrCreateGame(anAddress, &aFlag);
    if (aGame->ReadSummary(inBuffer)) {
        aGame->mL1C = time(NULL);
        LobbyFn359100(new GameChangedEvent(aGame, aFlag ? 2 : 0), true);
    }
    else {
        SmartPtr<LobbyGame> aHeldGame = aGame;
        mLanGames->RemoveGame(anAddress);
        if (aFlag)
            LobbyFn359100(new GameChangedEvent(aGame, aFlag ? 2 : 1), true);
    }
}

// FUNCTION: WON 0x007e4ec0
void LanLogic::Fn3E4EC0(ReadBuffer& inBuffer, const IPAddr& inAddress)
{
    IPAddr anAddress = inAddress;
    anAddress.SetPort(inBuffer.ReadShort());
    if (inAddress.Fn2B2FB0() != anAddress.Fn2B2FB0())
        return;
    LobbyGame* aGame = mLanGames->GetGame(anAddress);
    if (aGame != NULL)
        aGame->mL1C = time(NULL);
}

// FUNCTION: WON 0x007e5050
void LanLogic::Fn3E5050(ReadBuffer& inBuffer, const IPAddr& inAddress)
{
    IPAddr anAddress = inAddress;
    anAddress.SetPort(inBuffer.ReadShort());
    if (inAddress.Fn2B2FB0() != anAddress.Fn2B2FB0())
        return;
    SmartPtr<LobbyGame> aGame = mLanGames->RemoveGame(anAddress);
    if (aGame.get() != NULL)
        LobbyFn359100(new GameChangedEvent(aGame.get(), 1), true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e2800
void LanLogic::Fn3E2800()
{
    mLanNameDialog->Reset();
    if (LobbyDoDialog(mLanNameDialog.get()) == 0x186a1 && mLanNameDialog->GetName().length() != 0) {
        LobbyFn35D6C0(mLanNameDialog->GetName());
        LobbyFn359120(0x186d2, true);
    }
}

// FUNCTION: WON 0x007e2850
void LanLogic::Fn3E2850()
{
    int aPort = LobbyFn35E3C0();
    mLanPortDialog->SetGamePort(LobbyFn361400());
    mLanPortDialog->SetPort(aPort);
    mLanPortDialog->Reset();
    if (LobbyDoDialog(mLanPortDialog.get()) == 0x186a1) {
        int aNewPort = mLanPortDialog->GetPort();
        if (aNewPort != aPort) {
            LobbyFn35E3F0(aNewPort);
            LobbyFn359120(0x186d2, true);
            Fn3E54E0();
        }
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x007dfb90
void LobbyLanSocket::Fn3DFB90(LobbyLanClient* inClient)
{
    if (mClient.get() != NULL)
        mClient->mSocket = NULL;
    mClient = inClient;
    if (mClient.get() != NULL)
        mClient->mSocket = this;
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e1ad0
void LanLogic::Fn3E1AD0(ByteBuffer* inBuffer, bool inDeliverLocal)
{
    if (!Fn3E0160())
        return;
    if (inDeliverLocal)
        Fn3E2610(inBuffer, mMyClient.get());
    for (std::set<SmartPtr<LobbyLanSocket> >::iterator anItr = mPendingSockets.begin();
         anItr != mPendingSockets.end(); ++anItr) {
        LobbyLanSocket* aSocket = anItr->get();
        if (aSocket->mClient.get() != NULL)
            aSocket->Fn2B4740(new SendMsgOp(inBuffer), -1);
    }
}

// FUNCTION: WON 0x007e0300
void LanLogic::Fn3E0300(AsyncOp* inOp)
{
    LobbyLanSocket* aSocket = static_cast<LobbyLanSocket*>(
        static_cast<SocketOp*>(inOp)->mSocket.get());
    if (aSocket == NULL)
        return;
    if (Fn3E0160()) {
        if (aSocket->mClient.get() != NULL) {
            WriteBuffer aBuffer(2, true);
            aBuffer.AppendByte(4);
            aBuffer.AppendShort(aSocket->mClient->GetClientId());
            aSocket->Fn3DFB90(NULL);
            Fn3E1AD0(aBuffer.ToByteBuffer(true).get(), true);
        }
        mPendingSockets.erase(aSocket);
    } else if (aSocket == mServerSocket.get()) {
        mServerSocket->Close();
        mServerSocket = NULL;
        LobbyFn361E50(true);
    }
}

// FUNCTION: WON 0x007e04e0
void LanLogic::Fn3E04E0(AsyncOp* inOp)
{
    if (inOp != mNetworkTimer.get())
        return;
    if (Fn3E0160()) {
        long aTime = time(NULL);
        for (std::set<SmartPtr<LobbyLanSocket> >::iterator anItr = mPendingSockets.begin();
             anItr != mPendingSockets.end(); ++anItr)
            if (aTime - (*anItr)->mLastMessageTime > 300)
                (*anItr)->QueueClose(1000);
    } else if (mServerSocket.get() != NULL && time(NULL) - mLastServerPing >= 120) {
        WriteBuffer aBuffer(2, true);
        aBuffer.AppendByte(7);
        Fn3E1980(aBuffer.ToByteBuffer(true).get());
    }
    Fn3E0620();
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e2760
void LanLogic::HandleControlEvent(int inControlId)
{
    switch (inControlId) {
    case 0x30d53: Fn3E2800(); return;
    case 0x30d54: Fn3E2850(); return;
    case 0x30d50: Fn3E3290(); return;
    case 0x30d49: Fn3E3480(); return;
    }
}

// FUNCTION: WON 0x007e27c0
void LanLogic::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch (inEvent->mType) {
    case 0x186a5: Fn3E2DC0(static_cast<ClientActionEvent*>(inEvent)); break;
    case 0x186a8: Fn3E28D0(static_cast<ChangeDirectConnectEvent*>(inEvent)); break;
    case 0x186ac: Fn3E3050(inEvent->mComponent); break;
    }
}

// FUNCTION: WON 0x007e2f40
void LanLogic::Fn3E2F40(const GUIString& inText, unsigned short inFlags,
                       unsigned short inTarget)
{
    WriteBuffer aBuffer(2, true);
    aBuffer.AppendByte(5);
    aBuffer.AppendShort(inFlags);
    aBuffer.AppendWString(inText, 2);
    aBuffer.AppendShort(inTarget);
    Fn3E1980(aBuffer.ToByteBuffer(true).get());
}

// FUNCTION: WON 0x007e3050
void LanLogic::Fn3E3050(Component* inTarget)
{
    if (!Fn3E0140())
        return;
    ChatCommandParser* aParser = LobbyFn35B710();
    if (aParser == NULL || !(aParser->mRoomFlags & 8))
        return;
    LobbyClient* aClient = aParser->mTarget.get();
    switch (aParser->mCommand) {
    case 1: case 3: {
        unsigned short aFlags = aParser->mCommand == 3 ? 2 : 0;
        if (aParser->mRoomFlags & 0x10)
            aFlags |= 0x100;
        Fn3E2F40(aParser->mText, aFlags, 0);
        return;
    }
    case 2:
        if (aClient != NULL)
            Fn3E2F40(aParser->mText, 4, aClient->mClientId);
        return;
    case 5:
        if (aClient != NULL)
            LobbyFn35B7B0(aClient, !aClient->mB22, 4);
        return;
    default:
        LobbyFn359260(new ChatCommandErrorEvent(::RoutingLogic_InvalidCommand_String), inTarget);
        break;
    }
}
}

namespace WONAPI {
// Actual RecvMsgOp callback; reference-only until canonical descendant migration.
// FUNCTION: WON 0x007e12a0
void LanLogic::Fn3E12A0(AsyncOp* inOp)
{
    if (inOp->mStatus != 0 || inOp->mB39)
        return;
    RecvMsgOp* anOp = static_cast<RecvMsgOp*>(inOp);
    LobbyLanSocket* aSocket = static_cast<LobbyLanSocket*>(anOp->mSocket.get());
    if (Fn3E0160()) {
        aSocket->mLastMessageTime = time(NULL);
        if (aSocket->mClient.get() == NULL) {
            Fn3E1210(anOp->GetMsg().get(), aSocket);
            return;
        }
    }
    Fn3E2610(anOp->GetMsg().get(), aSocket->mClient.get());
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e1210
void LanLogic::Fn3E1210(ByteBuffer* inBuffer, LobbyLanSocket* inSocket)
{
    if (inBuffer == NULL || inSocket == NULL || !Fn3E0160())
        return;
    ReadBuffer aBuffer(inBuffer->data(), inBuffer->length());
    try {
        switch (aBuffer.ReadByte()) {
        case 1: Fn3E0760(aBuffer, inSocket); break;
        }
    } catch (ReadBufferException&) {
    }
}
}

namespace WONAPI {
// Original catch landing/return continuation11f1..1209 is owned EH support,

// FUNCTION: WON 0x007e0f90
int LanLogic::Fn3E0F90(ByteBuffer* inBuffer, LobbyGame* inGame)
{
    if (inBuffer == NULL || mServerSocket.get() == NULL || inGame == NULL)
        return 1;
    ReadBuffer aBuffer(inBuffer->data(), inBuffer->length());
    try {
        if (aBuffer.ReadByte() != 2)
            return 1;
        int aStatus = aBuffer.ReadShort();
        if (aStatus != 0)
            return aStatus;
        unsigned short aCaptainId = aBuffer.ReadShort();
        unsigned short aMyId = aBuffer.ReadShort();
        inGame->mL60 = aCaptainId;
        unsigned short aNumClients = aBuffer.ReadShort();
        for (int i = 0; i < aNumClients; ++i) {
            unsigned short anId = aBuffer.ReadShort();
            std::wstring aName;
            aBuffer.ReadWString(aName, 2);
            SmartPtr<LobbyLanClient> aClient = new LobbyLanClient;
            aClient->mClientId = anId;
            aClient->mName = GUIString(new GUIStringData(aName));
            mClients->Fn3A5CC0(aClient.get());
            if (anId == aMyId)
                mMyClient = aClient;
            else if (anId == aCaptainId) {
                mServerSocket->Fn3DFB90(aClient.get());
                aClient->Fn3A5500(true, true);
            }
        }
        return 0;
    } catch (ReadBufferException&) {
    }
    return 1;
}
}

namespace WONAPI {
// IPAddr/Socket nominal APIs are original+SDK corroborated proposals awaiting E3.
// SendMsgOp is a fully proved op proposal, no socket object is allocated here.
// Password conversion's two original wstring temporaries require normal COFF audit.
// FUNCTION: WON 0x007e0760
void LanLogic::Fn3E0760(ReadBuffer& inBuffer, LobbyLanSocket* inSocket)
{
    if (mMyClient.get() == NULL)
        return;
    LobbyGame* aGame = GetMyLobbyGame();
    if (aGame == NULL)
        return;
    std::wstring aName;
    std::wstring aPassword;
    inBuffer.ReadWString(aName, 2);
    inBuffer.ReadWString(aPassword, 2);
    WriteBuffer aReply(2, true);
    aReply.AppendByte(2);
    if (aGame->GetGameType() == 1) {
        IPAddr aLocalAddr = IPAddr::GetLocalAddr();
        const char* aPeerBytes = inSocket->GetDestAddr().GetSixByte();
        const char* aLocalBytes = aLocalAddr.GetSixByte();
        if (aPeerBytes[2] != aLocalBytes[2] || aPeerBytes[3] != aLocalBytes[3]) {
            aReply.AppendShort(3);
            inSocket->Fn2B4740(new SendMsgOp(aReply.ToByteBuffer(true).get()), -1);
            return;
        }
    }
    if (aPassword != std::wstring(aGame->mPassword)) {
        aReply.AppendShort(2);
        inSocket->Fn2B4740(new SendMsgOp(aReply.ToByteBuffer(true).get()), -1);
        return;
    }
    SmartPtr<LobbyLanClient> aClient = new LobbyLanClient;
    aClient->mName = GUIString(new GUIStringData(aName));
    aClient->mClientId = Fn3E0110();
    mClients->Fn3A5CC0(aClient.get());
    WriteBuffer aNotify(2, true);
    aNotify.AppendByte(3);
    aNotify.AppendShort(aClient->mClientId);
    aNotify.AppendWString(aClient->mName, 2);
    Fn3E1AD0(aNotify.ToByteBuffer(true).get(), true);
    inSocket->Fn3DFB90(aClient.get());
    aReply.AppendShort(0);
    aReply.AppendShort(mMyClient->GetClientId());
    aReply.AppendShort(aClient->mClientId);
    aReply.AppendShort(mClients->mClients.size());
    for (LobbyClientMap::iterator anItr = mClients->mClients.begin();
         anItr != mClients->mClients.end(); ++anItr) {
        LobbyClient* aListedClient = anItr->second.get();
        aReply.AppendShort(aListedClient->mClientId);
        aReply.AppendWString(aListedClient->mName, 2);
    }
    Fn3E1840(aClient.get(), aReply.ToByteBuffer(true).get());
}
}

namespace WONAPI {
// Full90 ClientOptionsPopup contract belongs Dialogs; promotion waits its header.
// FUNCTION: WON 0x007e2ce0
void LanLogic::Fn3E2CE0(LobbyClient* inClient, int inRoomSpec)
{
    if (mMyClient.get() == NULL || inClient == NULL)
        return;
    SmartPtr<ClientOptionsPopup> aPopup = new ClientOptionsPopup;
    aPopup->Init(inClient, mMyClient.get(), inRoomSpec);
    LobbyFn358DF0(aPopup.get(), 0x23e7, -1, -1);
}

// FUNCTION: WON 0x007e2dc0
void LanLogic::Fn3E2DC0(ClientActionEvent* inEvent)
{
    if (!Fn3E0140() || !(inEvent->mRoomSpec & 8))
        return;
    LobbyClient* aClient = inEvent->mClient.get();
    if (aClient == NULL)
        return;
    switch (inEvent->mAction) {
    case 5:
        LobbyFn35B7B0(aClient, true, 8);
        break;
    case 7:
        LobbyFn361BC0(aClient, false);
        break;
    case 12:
        LobbyFn35B7B0(aClient, false, 8);
        break;
    case 16:
        LobbyFn359100(new SetChatInputEvent(2, aClient->mName,
            inEvent->mRoomSpec, false), true);
        break;
    case 18:
        Fn3E2CE0(aClient, inEvent->mRoomSpec);
        break;
    }
}
}

namespace WONAPI {

// FUNCTION: WON 0x007e3290
void LanLogic::Fn3E3290()
{
    int aKind = LobbyFn361190();
    if (aKind == 1) {
        mLanGames->Clear();
        LobbyFn359100(new SyncGameListEvent(mLanGames.get()), true);
        if (mDiscoverySocket.get() == NULL)
            return;
        WriteBuffer aBuffer(0, true);
        aBuffer.AppendByte(4);
        mDiscoverySocket->Fn2B4740(new SendBytesToOp(
            aBuffer.ToByteBuffer(true).get(), mDiscoveryAddress), -1);
    } else if (aKind == 3) {
        mDirectGames->Rewind();
        while (mDirectGames->HasMore()) {
            LobbyGame* aGame = mDirectGames->Next();
            if (!aGame->GetIsFiltered())
                LobbyFn35B900(aGame, false);
        }
        LobbyFn359120(0x186da, true);
    }
}
}

namespace WONAPI {
// Requires model's complete RecvMsgOpB4/SocketOp5c proposal and canonical Queue API.
// This allocates an operation only; no socket object is constructed.
// FUNCTION: WON 0x007e13b0
void LanLogic::Fn3E13B0(LobbyLanSocket* inSocket)
{
    inSocket->SetCloseCompletion(new LogicCompletion<LanLogic>(&LanLogic::Fn3E0300, this));
    inSocket->Fn2B3BF0(new LogicCompletion<LanLogic>(&LanLogic::Fn3E12A0, this));
    inSocket->Fn2B4500(new RecvMsgOp);
    inSocket->mLastMessageTime = time(NULL);
    mPendingSockets.insert(inSocket);
}
}

namespace WONAPI {
// Canonical Core map alias/getter are queued; no persistence-object allocation/view.
// FUNCTION: WON 0x007e3480
void LanLogic::Fn3E3480()
{
    LobbyFn35CF90(NULL);
    mLanGames->Clear();
    mDirectGames->Clear();
    const LobbyDirectConnectMap& aSavedGames = LobbyFn35D8E0();
    LobbyDirectConnectMap::const_iterator anItr;
    for (anItr = aSavedGames.begin(); anItr != aSavedGames.end(); ++anItr) {
        LobbyGame* aGame = mDirectGames->GetOrCreateGame(anItr->first, NULL);
        aGame->mName = anItr->second;
    }
    LobbyFn359100(new SyncGameListEvent(mDirectGames.get()), true);
    Fn3E54E0();
    LobbyFn360F90();
}
}

namespace WONAPI {
// Complete source draft only. Genuine socket allocation remains uncompiled until
// root admits true full74/a0/a8 headers and reviews all ordinary constructor/EH data.
// FUNCTION: WON 0x007e37a0
bool LanLogic::Virtual18(LobbyGame* inGame)
{
    Fn3E0190();
    if (inGame->GetGameType() == 3 && LobbyFn35B540()) {
        mB74 = true;
        if (!Fn3E35A0())
            return false;
    }
    LobbyFn35B980();
    mIsCaptain = true;
    mMyClient = new LobbyLanClient;
    mMyClient->mClientId = Fn3E0110();
    mMyClient->mName = LobbyFn35D640();
    mMyClient->Fn3A5500(true, true);
    mClients->Fn3A5CC0(mMyClient.get());
    inGame->SetClients(mClients.get());
    mListenSocket = new LobbyLanSocket;
    WONStatus aStatus = mListenSocket->Fn2B0D90(LobbyFn361400(), 0);
    if (aStatus != 0) {
        LobbyFn358ED0(::LANLogic_SocketBindError_String, aStatus);
        return false;
    }
    aStatus = mListenSocket->Fn2B0FC0(1000);
    if (aStatus != 0) {
        LobbyFn358ED0(::LANLogic_SocketListenError_String, aStatus);
        return false;
    }
    mListenSocket->Fn2B3BF0(new LogicCompletion<LanLogic>(&LanLogic::Fn3E1700, this));
    mListenSocket->Fn2B4500(new AcceptOp(NULL));
    int aRoomSpec = 10;
    LobbyFn359100(new JoinedRoomEvent(GUIString(new GUIStringData("")),
        inGame->mName, aRoomSpec, mMyClient->mClientId, inGame->mClients.get()), true);
    mLastBroadcastTime = 0;
    Fn3E0620();
    return true;
}
}

namespace WONAPI {
// Uncompiled source draft; actual full QueueSocketa0 and op90/88 gates required.
// FUNCTION: WON 0x007e54e0
void LanLogic::Fn3E54E0()
{
    Fn3E5810();
    mDiscoveryAddress = IPAddr::GetBroadcastAddr(LobbyFn35E3C0());
    mDiscoverySocket = new QueueSocket(1);
    if (mDiscoverySocket->Fn2B0D90(LobbyFn35E3C0(), 0) != 0) {
        Fn3E5810();
        return;
    }
    mDiscoverySocket->Fn2B3BF0(new LogicCompletion<LanLogic>(&LanLogic::Fn3E5200, this));
    mDiscoverySocket->Fn2B4500(new RecvBytesFromOp(1024, NULL));
    Fn3E53A0();
    WriteBuffer aBuffer(0, true);
    aBuffer.AppendByte(4);
    mDiscoverySocket->Fn2B4740(new SendBytesToOp(
        aBuffer.ToByteBuffer(true).get(), mDiscoveryAddress), -1);
    if (Fn3E0160()) {
        LobbyGame* aGame = GetMyLobbyGame();
        if (aGame != NULL)
            Fn3E4630(aGame, mDiscoveryAddress);
    }
}
}

// E3 genuine nullable ByteBuffer API closes this complete discovery receive path.
namespace WONAPI {
// FUNCTION: WON 0x007e5200
void LanLogic::Fn3E5200(AsyncOp* inOp)
{
    RecvBytesFromOp* anOp = static_cast<RecvBytesFromOp*>(inOp);
    if (anOp->mSocket.get() != mDiscoverySocket.get() || !anOp->Succeeded())
        return;
    ReadBuffer aBuffer(anOp->GetBytes()->data(), anOp->GetBytes()->length());
    try {
        switch (aBuffer.ReadByte()) {
        case 1: Fn3E4CB0(aBuffer, anOp->mFromAddress); break;
        case 2: Fn3E4EC0(aBuffer, anOp->mFromAddress); break;
        case 3: Fn3E5050(aBuffer, anOp->mFromAddress); break;
        case 4: Fn3E4C90(aBuffer, anOp->mFromAddress); break;
        }
    }
    catch (ReadBufferException&) {}
}
}
/* Deferred original-carved bodies, zero handwritten source credit:
   Complete GetCert/PeerAuth allocation records and ancestor interfaces remain gated.
   Native declarations/calls above preserve original callees; no partial type is allocated.
*/

#include "ee/wonapi/EELobbyGameCode.h" //genuine LobbyMessageBox call declaration
extern unsigned short gEmptyWideString[]; // 0x8e60bc, an empty wide string (zero .bss; defined in EEWonNet.cpp)
// actual external GetCert11fc owner; only this auth initializer is newly closed.
namespace WONAPI {
// FUNCTION: WON 0x007e35a0
bool LanLogic::Fn3E35A0()
{
    AuthContext* auth=GetLobbyAuthContext();
    if(!auth) return false;
    if(!LobbyFn35B560()) {
        LobbyMessageBox(::AccountLogic_InvalidCDKeyTitle_String,::AccountLogic_InvalidCDKeyDetails_String);
        return false;
    }
    if(!auth->IsExpired(120)) {
        auth->Fn2DEB30(true);
        return true;
    }
    auth->SetUserName(std::wstring(gEmptyWideString));
    auth->SetPassword(std::wstring(gEmptyWideString));
    SmartPtr<GetCertOp> op=new GetCertOp(auth);
    LobbyFn358E70(op.get(),::LobbyContainer_LoginTitle_String,::LobbyContainer_LoginDetails_String,1,0);
    if(op->GetStatus()==0) {
        auth->Fn2DEB30(true);
        return true;
    }
    return false;
}
}

// private next-cohort draft: requires Root F actual PeerAuthOp2244 owner before canonical allocation.
namespace WONAPI {
// FUNCTION: WON 0x007e3d20
bool LanLogic::Virtual1C(LobbyGame* game)
{
    SmartPtr<AuthContext> auth;
    Fn3E0190();
    if(game->GetGameType()==3 && LobbyFn35B540()) {
        mB74=true;
        if(!Fn3E35A0()) return false;
        auth=GetLobbyAuthContext();
    }
    mServerSocket=new LobbyLanSocket;
    bool needPassword=game->mB10;
    bool needConnect=true;
    bool joined=false;
    while(!joined) {
        GUIString password(GetEmptyGUIStringData());
        if(needPassword) {
            mPasswordDialog->Reset();
            if(LobbyDoDialog(mPasswordDialog.get())!=100001) return false;
            password=mPasswordDialog->GetPassword();
        }
        if(needConnect) {
            SmartPtr<AsyncOp> connect;
            if(mB74) connect=new PeerAuthOp(game->mAddress,auth.get(),AUTH_TYPE_PERSISTENT,mServerSocket.get(),true);
            else connect=new ConnectOp(game->mAddress,mServerSocket.get());
            LobbyFn358E70(connect.get(),::LANLogic_ConnectToGameTitle_String,::LANLogic_ConnectToGameDetails_String,1,0);
            if(connect->GetStatus()!=0) return false;
            needConnect=false;
        }
        WriteBuffer request(2,true);
        request.AppendByte(1);
        request.AppendWString(static_cast<std::wstring>(LobbyFn35D640()),2);
        request.AppendWString(static_cast<std::wstring>(password),2);
        mServerSocket->Fn2B4740(new SendMsgOp(request.ToByteBuffer(true).get()),-1);
        mLastServerPing=time(NULL);
        SmartPtr<RecvMsgOp> reply=new RecvMsgOp(mServerSocket.get());
        LobbyFn358E70(reply.get(),::LANLogic_RegisterTitle_String,::LANLogic_RegisterDetails_String,1,0);
        if(reply->GetStatus()!=0) return false;
        int status=Fn3E0F90(reply->GetMsg().get(),game);
        switch(status) {
        case 0: joined=true;break;
        case 1: return false;
        case 2:
            LobbyMessageBox(::GameStagingLogic_JoinGameErrorTitle_String,::LobbyGameStatus_InvalidPassword_String);
            needPassword=true;
            break;
        case 3:
            LobbyMessageBox(::LANLogic_RegisterTitle_String,::LANLogic_InvalidSubnet_String);
            return false;
        }
    }
    mServerSocket->SetCloseCompletion(new LogicCompletion<LanLogic>(&LanLogic::Fn3E0300,this));
    mServerSocket->Fn2B3BF0(new LogicCompletion<LanLogic>(&LanLogic::Fn3E12A0,this));
    mServerSocket->Fn2B4500(new RecvMsgOp);
    Fn3E0620();
    game->SetClients(mClients.get());
    int roomSpec=10;
    LobbyFn359100(new JoinedRoomEvent(GUIString(new GUIStringData("")),game->mName,
        roomSpec,mMyClient->mClientId,game->mClients.get()),true);
    return true;
}
}

// Private reviewed-contract draft only: actual PeerAuthServerOp2240 allocation waits Root integration authority.
namespace WONAPI {
// FUNCTION: WON 0x007e1600
void LanLogic::Fn3E1600(AsyncOp* inOp)
{
    PeerAuthServerOp* op=static_cast<PeerAuthServerOp*>(inOp);
    LobbyLanSocket* socket=static_cast<LobbyLanSocket*>(op->GetSocket());
    if(op->GetStatus()!=0 || op->mB39) {
        socket->QueueClose(1000);
        return;
    }
    const AuthCertificateBase* certificate=op->mPeerAuthServer.GetClientCertificate();
    AuthContext* auth=GetLobbyAuthContext();
    if(!auth) return;
    DWORD community;
    {
        ConstSmartPtr<AuthPeerData> peer=auth->GetPeerData();
        community=peer->GetCertificate2()->GetFirstCommunity();
    }
    if(!certificate->VerifyPermissions(0,community,0)) {
        socket->QueueClose(1000);
        return;
    }
    Fn3E13B0(socket);
}
// FUNCTION: WON 0x007e1700
void LanLogic::Fn3E1700(AsyncOp* inOp)
{
    if(inOp->GetStatus()!=0 || inOp->mB39) return;
    LobbyLanSocket* socket=static_cast<LobbyLanSocket*>(static_cast<AcceptOp*>(inOp)->mAcceptedSocket.get());
    if(mB74) {
        AuthContext* auth=GetLobbyAuthContext();
        if(!auth) return;
        SmartPtr<PeerAuthServerOp> server=new PeerAuthServerOp(auth,socket);
        RunTrackedOp(server.get(),new LogicCompletion<LanLogic>(&LanLogic::Fn3E1600,this),30000);
    } else
        Fn3E13B0(socket);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007dfde0
// (public: virtual void * __thiscall WONAPI::LanLogic::`scalar deleting destructor'(unsigned int)    [??_GLanLogic@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007dfe00
// (public: virtual __thiscall WONAPI::NetLogic::~NetLogic(void)    [??1NetLogic@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007e0e80
// (public: __thiscall WONAPI::SendBytesOp::SendBytesOp(class WONAPI::ByteBuffer *,class WONAPI::AsyncSocket *)    [??0SendBytesOp@WONAPI@@QAE@PAVByteBuff)
// TEMPLATE: WON 0x007e0f80
// (public: virtual __thiscall WONAPI::LobbyLanClient::~LobbyLanClient(void)    [??1LobbyLanClient@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f20d0
// (public: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::LobbyLanSocket>,class WONAPI::SmartPtr<class WONAPI::LobbyLanSocket>,struct std::set<cla)
// TEMPLATE: WON 0x007f47e0
// (void __cdecl std::_Distance(class std::_Tree<class WONAPI::SmartPtr<class WONAPI::LobbyLanSocket>,class WONAPI::SmartPtr<class WONAPI::LobbyLanSocket>)
// TEMPLATE: WON 0x007f3850
// TEMPLATE: WON 0x007f39d0
// TEMPLATE: WON 0x007f3b50
// TEMPLATE: WON 0x007f3cd0

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007e3af0
// (public: virtual class WONAPI::AsyncSocket * __thiscall WONAPI::LobbyLanSocket::Duplicate(void)    [?Duplicate@LobbyLanSocket@WONAPI@@UAEPAVAsyncSocket)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007dfed0
// (public: virtual void * __thiscall WONAPI::NetLogic::`scalar deleting destructor'(unsigned int)    [??_GNetLogic@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007e0f60
// (public: virtual void * __thiscall WONAPI::LobbyLanClient::`scalar deleting destructor'(unsigned int)    [??_GLobbyLanClient@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007e31e0
// (public: virtual void * __thiscall WONAPI::ChatCommandErrorEvent::`scalar deleting destructor'(unsigned int)    [??_GChatCommandErrorEvent@WONAPI@@UAEP)
// TEMPLATE: WON 0x007e3200
// (public: virtual __thiscall WONAPI::ChatCommandErrorEvent::~ChatCommandErrorEvent(void)    [??1ChatCommandErrorEvent@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007e3b80
// (public: virtual void * __thiscall WONAPI::LobbyLanSocket::`scalar deleting destructor'(unsigned int)    [??_GLobbyLanSocket@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007e3ba0
// (public: virtual __thiscall WONAPI::LobbyLanSocket::~LobbyLanSocket(void)    [??1LobbyLanSocket@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1450
// (public: virtual void * __thiscall WONAPI::LogicCompletion<class WONAPI::LanLogic>::`scalar deleting destructor'(unsigned int)    [??_G?$LogicCompletio)
// TEMPLATE: WON 0x007f1570
// (public: virtual __thiscall WONAPI::LogicCompletion<class WONAPI::LanLogic>::~LogicCompletion<class WONAPI::LanLogic>(void)    [??1?$LogicCompletion@VL)
