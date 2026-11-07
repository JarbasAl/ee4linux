// Original StagingLogic contract; 0x5c allocation, six inherited virtual slots.
#ifndef EE_WONAPI_WONLOBBYSTAGING_H
#define EE_WONAPI_WONLOBBYSTAGING_H
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyModel.h"
namespace WONAPI {
class CreateGameDialog;
// RTTI StagingLogic>LobbyLogic>RefCount. Vtable455ae0 six slots.
// Original ctor3ef160 starts fields40; final owned pointer58; no explicit padding.
class StagingLogic : public LobbyLogic {
public:
    StagingLogic();
    virtual ~StagingLogic();
    virtual void InitComponent(ComponentConfig* inConfig);
    virtual void HandleControlEvent(int inControlId);
    virtual void HandleComponentEvent(ComponentEvent* inEvent);
    void Fn3EF2B0(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF410(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF450(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF620(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF720(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF7D0(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF8C0(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF970(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EF9B0();
    void Fn3EFA80(bool inValue);
    void Fn3EFAC0(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EFB00(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EFB50(ReadBuffer& inBuffer, LobbyClient* inClient);
    void Fn3EFD20(const ByteBuffer* inMessage, LobbyClient* inClient);
    void Fn3EFD60(const ByteBuffer* inMessage);
    void Fn3EFDA0(const ByteBuffer* inMessage);
    void Fn3EFDE0(LobbyClient* inClient, const ByteBuffer* inMessage);
    bool Fn3EFE10();
    bool Fn3EFE30();
    void Fn3EFEE0(LobbyGame* inGame);
    void Fn3EFFA0(ComponentEvent* inEvent);
    bool Fn3F0110(bool inConfirm);
    void Fn3F0170(bool inReady);
    void Fn3F0230();
    void Fn3F02E0();
    void Fn3F0390();
    void Fn3F0410();
    bool Fn3F06E0(LobbyGame* inGame);
    void Fn3F08A0(ComponentEvent* inEvent);
    void Fn3F08B0(LobbyGame* inGame);
    bool Fn3F0C60(bool inConfirm);
    void Fn3F0DE0(bool inShowError);
    void Fn3F0E10(LobbyClient* inClient, bool inFlag);
    void Fn3F0F10(LobbyClient* inClient);
    void Fn3F1000(LobbyGame* inGame);
    void Fn3F10F0(LobbyClient* inClient, bool inFlag);
    void Fn3F11F0();
    void Fn3F1200();
    SmartPtr<LobbyGame> mGame; //40
    SmartPtr<LobbyClient> mMyClient; //44
    SmartPtr<AsyncOp> mJoinOp; //48
    short mJoinStatus; //4c
    bool mInStaging; //4e
    bool mLastReady; //4f
    bool mLastAllReady; //50
    long mLastReadyTime; //54
    SmartPtr<CreateGameDialog> mCreateGameDialog; //58
};
}
#endif
