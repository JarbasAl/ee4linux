// Original-backed PingLogic/PingGameOp family; full COFF/EH/data capture required.
// UDPManagerSendRecvOp and PingGameOp are complete before any allocation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyPing.h"
using namespace WONAPI;

// Source-local complete original global operation; admitted UDP88 prerequisite applies here.
class PingGameOp : public WONAPI::UDPManagerSendRecvOp {
public:
    PingGameOp(WONAPI::LobbyGame*, unsigned short, unsigned char); //3e5ed0

    virtual void Virtual04(); //3e6240
    virtual WONAPI::WONStatus Virtual0C(WONAPI::ReadBuffer&); //3e6360
    WONAPI::SmartPtr<WONAPI::LobbyGame> mGame; //88
    unsigned short mRequestId; //8c
    unsigned short mPing; //8e, initialized by RunHook, not ctor
    unsigned char mReplyFlags; //90
    unsigned short mRetries; //92, natural alignment before member
    WONAPI::ByteBufferPtr mReply; //94
};

// FUNCTION: WON 0x007e5ed0
PingGameOp::PingGameOp(LobbyGame* inGame, unsigned short inRequestId,
                       unsigned char inFlags)
{
    mGame = inGame;
    mRetries = 0;
    mRequestId = inRequestId;
    mReplyFlags = inFlags;
    mAddress = mGame->mAddress;
}

// FUNCTION: WON 0x007e6240
void PingGameOp::Virtual04()
{
    WriteBuffer aBuffer(0, true);
    aBuffer.AppendByte(7);
    aBuffer.AppendShort(mRequestId);
    aBuffer.AppendByte(mGame->GetGameType());
    aBuffer.AppendByte(mReplyFlags);
    mOutgoing = aBuffer.ToByteBuffer(true);
    mPing = 0xffff;
    UDPManagerSendRecvOp::Virtual04();
}

// FUNCTION: WON 0x007e6360
WONStatus PingGameOp::Virtual0C(ReadBuffer& inBuffer)
{
    if (inBuffer.ReadByte() != 8)
        return 0x186a4; // original response-not-for-this-op status
    if (inBuffer.ReadShort() != mRequestId)
        return 0x186a4; // signed ReadShort promoted against unsigned-short member
    mPing = GetTickCount() - mStartTime;
    mReplyFlags = 0;
    if (inBuffer.mL08 - inBuffer.mL00 > 0) {
        mReplyFlags = inBuffer.ReadByte();
        unsigned long aLength = inBuffer.mL08 - inBuffer.mL00;
        mReply = new ByteBuffer(inBuffer.ReadBytes(aLength), aLength, false);
    }
    return 0;
}

namespace WONAPI {
// FUNCTION: WON 0x007e6490
PingLogic::PingLogic()
{
    mNextRequestId = 0;
}

// FUNCTION: WON 0x007e6520
PingLogic::~PingLogic() {}

// FUNCTION: WON 0x007e6b40
int PingLogic::Fn3E6B40()
{
    return mPending.size() + mTracker.mMaps.mEntries.size();
}
}


namespace WONAPI {
// Canonical UDP contracts pending model placement. These are actual call-only APIs.
UDPManager* LobbyFn35B8F0();
unsigned short LobbyFn35E3A0(); // max simultaneous ping count, actual caller masks16
void LobbyFn361EB0(LobbyGame*); // update game UI/status

// FUNCTION: WON 0x007e65d0
void PingLogic::InitComponent(ComponentConfig*)
{
    UDPManager* aManager = LobbyFn35B8F0();
    if (aManager != NULL)
        aManager->mDefaultCompletion = new LogicCompletion<PingLogic>(&PingLogic::Fn3E6810, this);
}

// FUNCTION: WON 0x007e66a0
void PingLogic::Fn3E66A0(ReadBuffer& inBuffer, const IPAddr& inAddress)
{
    LobbyGame* aGame = GetMyLobbyGame();
    if (aGame == NULL || !aGame->mIsHost)
        return;
    unsigned short anId = inBuffer.ReadShort();
    unsigned char aKind = inBuffer.ReadByte();
    if (aGame->GetGameType() != aKind)
        return;
    if (aGame->mB13) {
        LobbyConfig* aConfig = GetLobbyConfig();
        if (aConfig != NULL && aConfig->mNoPingInProgress)
            return;
    }
    WriteBuffer aBuffer(3, true);
    aBuffer.AppendByte(8);
    aBuffer.AppendShort(anId);
    unsigned char aFlags = inBuffer.ReadByte() & 3;
    aBuffer.AppendByte(aFlags);
    if (aFlags & 1)
        aGame->WriteSummary(aBuffer);
    if (aFlags & 2)
        aGame->Fn3A7240(aBuffer);
    UDPManager* aManager = LobbyFn35B8F0();
    if (aManager != NULL)
        aManager->Send(aBuffer.ToByteBuffer(true).get(), inAddress);
}

// FUNCTION: WON 0x007e6810
void PingLogic::Fn3E6810(AsyncOp* inOp)
{
    UDPManagerRecvOp* anOp = static_cast<UDPManagerRecvOp*>(inOp);
    if (anOp->GetStatus() != 0 || anOp->mB39)
        return;
    ByteBuffer* aBuffer = anOp->mReceivedData.get();
    ReadBuffer aReadBuffer(aBuffer != NULL ? aBuffer->data() : NULL,
                          aBuffer != NULL ? aBuffer->length() : 0);
    try {
        switch ((char)aReadBuffer.ReadByte()) {
        case 7: Fn3E66A0(aReadBuffer, anOp->mAddress); break;
        }
    }
    catch (ReadBufferException&) {}
}

// FUNCTION: WON 0x007e6b50
void PingLogic::Fn3E6B50(LobbyGame* inGame, bool inDetails, bool inForce)
{
    if (inGame == NULL)
        return;
    if (inGame->mB13) {
        LobbyConfig* aConfig = GetLobbyConfig();
        if (aConfig != NULL && aConfig->mNoPingInProgress)
            return;
    }
    if (inDetails) {
        if (inGame->Fn3A6D90())
            return;
    }
    else if (inGame->Fn3A6D70())
        return;
    if (!inForce && !inDetails) {
        unsigned short aMaximum = LobbyFn35E3A0();
        if (static_cast<long>(mTracker.mMaps.mEntries.size()) >= aMaximum && aMaximum > 0) {
            inGame->mL64 = 0xfffd;
            LobbyFn361EB0(inGame);
            mPending.push_back(inGame);
            return;
        }
    }
    UDPManager* aManager = LobbyFn35B8F0();
    if (aManager == NULL)
        return;
    inGame->mL64 = inDetails ? 0xfffc : 0xfffd;
    LobbyFn361EB0(inGame);
    unsigned char aFlags = inGame->GetGameType() == 3 ? 1 : 0;
    if (inDetails)
        aFlags |= 2;
    SmartPtr<PingGameOp> anOp = new PingGameOp(inGame, mNextRequestId++, aFlags);
    anOp->mManager = aManager;
    LobbyConfig* aConfig = GetLobbyConfig();
    unsigned long aTimeout = aConfig != NULL ? aConfig->mPingCeiling : 5000;
    RunTrackedOp(anOp.get(), new LogicCompletion<PingLogic>(&PingLogic::Fn3E68B0, this), aTimeout);
}

// FUNCTION: WON 0x007e6db0
void PingLogic::Fn3E6DB0(int)
{
    std::list<SmartPtr<LobbyGame> >::iterator aGameItr = mPending.begin();
    for (; aGameItr != mPending.end(); ++aGameItr)
        (*aGameItr)->mL64 = 0xfffe;
    AsyncOpTrackerMaps::EntryMap::iterator anItr = mTracker.mMaps.mEntries.begin();
    for (; anItr != mTracker.mMaps.mEntries.end(); ++anItr) {
        PingGameOp* anOp = dynamic_cast<PingGameOp*>((*anItr).second->mOp.get());
        if (anOp != NULL)
            anOp->mGame->mL64 = 0xfffe;
    }
    mPending.clear();
    mTracker.KillAll();
}
}

namespace WONAPI {
// FUNCTION: WON 0x007e6680
void PingLogic::HandleComponentEvent(ComponentEvent* inEvent)
{
    if (inEvent->mType == 0x186c5) {
        PingGameEvent* anEvent = static_cast<PingGameEvent*>(inEvent);
        Fn3E6B50(anEvent->mGame.get(), anEvent->mFlag, false);
    }
}

// FUNCTION: WON 0x007e68b0
void PingLogic::Fn3E68B0(AsyncOp* inOp)
{
    PingGameOp* anOp = static_cast<PingGameOp*>(inOp);
    if (anOp->mB39)
        return;
    LobbyGame* aGame = anOp->mGame.get();
    if (anOp->mPing != 0xffff)
        aGame->mB18 = true;
    else if (anOp->mRetries < 3) {
        ++anOp->mRetries;
        LobbyConfig* aConfig = GetLobbyConfig();
        unsigned long aTimeout = aConfig != NULL ? aConfig->mPingCeiling : 5000;
        RunTrackedOp(anOp, new LogicCompletion<PingLogic>(&PingLogic::Fn3E68B0, this), aTimeout);
        return;
    }
    aGame->mL64 = anOp->mPing;
    if (anOp->mReply.get() != NULL) {
        ReadBuffer aBuffer(anOp->mReply->data(), anOp->mReply->length());
        unsigned char aFlags = anOp->mReplyFlags;
        bool aReadOk = true;
        if (aFlags & 1)
            aReadOk = aGame->ReadSummary(aBuffer);
        if ((aFlags & 2) && aReadOk)
            aGame->ReadDetails(aBuffer);
    }
    LobbyFn359100(new GameChangedEvent(aGame, 2), true);
    LobbyFn361EB0(aGame);
    unsigned short aMaximum = LobbyFn35E3A0();
    while (!mPending.empty()) {
        if (static_cast<long>(mTracker.mMaps.mEntries.size()) >= aMaximum && aMaximum != 0)
            break;
        SmartPtr<LobbyGame> aPendingGame = mPending.front();
        mPending.pop_front();
        if (!aPendingGame->Fn3A6D70())
            continue;
        if (aPendingGame->Fn3A6D90())
            continue;
        aPendingGame->mL64 = 0xfffe;
        Fn3E6B50(aPendingGame.get(), false, true);
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007e6110
// (public: virtual __thiscall PingGameOp::~PingGameOp(void)    [??1PingGameOp@@UAE@XZ])
// TEMPLATE: WON 0x007e6500
// (public: virtual void * __thiscall WONAPI::PingLogic::`scalar deleting destructor'(unsigned int)    [??_GPingLogic@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007e60f0
// (public: virtual void * __thiscall PingGameOp::`scalar deleting destructor'(unsigned int)    [??_GPingGameOp@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1470
// (public: virtual void * __thiscall WONAPI::LogicCompletion<class WONAPI::PingLogic>::`scalar deleting destructor'(unsigned int)    [??_G?$LogicCompleti)
// TEMPLATE: WON 0x007f15d0
// (public: virtual __thiscall WONAPI::LogicCompletion<class WONAPI::PingLogic>::~LogicCompletion<class WONAPI::PingLogic>(void)    [??1?$LogicCompletion@)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1b00
// (public: void __thiscall std::list<class WONAPI::SmartPtr<class WONAPI::LobbyGame>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::LobbyGame>)
// TEMPLATE: WON 0x007f26a0
// (protected: void __thiscall std::list<class WONAPI::SmartPtr<class WONAPI::LobbyGame>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::LobbyGa)
