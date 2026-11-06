// Complete original-backed LAN records; canonical BlockingSocket a0 prerequisite.
// Natural member lifetimes and generated support remain subject to ordinary COFF review.
#ifndef EE_WONAPI_WONLOBBYLAN_H
#define EE_WONAPI_WONLOBBYLAN_H
#include <set>
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyModel.h"
namespace WONAPI {
class DirectConnectDialog;
class LanNameDialog;
class LanPortDialog;
class PasswordDialog;
class ChangeDirectConnectEvent;
class ClientActionEvent;
class LobbyGameList;
class QueueSocket;
class LobbyLanSocket;

// RTTI4559d8; original new30 at3e0ada/3e37f6, LobbyClient base2c.
// Socket is a raw non-owning back pointer: dtor3e0f80 tailcalls LobbyClient only.
class LobbyLanClient : public LobbyClient {
public:
    LobbyLanClient() : mSocket(NULL) {}
    // Implicit destructor: original3e0f80 tailcalls base with no own-vptr store.
    LobbyLanSocket* mSocket; //+2c, non-owning; reciprocal socket owns client at+a0
};

class LobbyLanSocket : public BlockingSocket {
public:
    // Actual inline ctor in3e3af0/host/join: BlockingSocket(0),mClient(NULL),
    // then SetLengthFieldSize(2); timea4 deliberately uninitialized by ctor.
    LobbyLanSocket() : BlockingSocket(0) { SetLengthFieldSize(2); } //actual inline construction, no invented ctorRVA
    // Implicit dtor3e3ba0(381B), no own-vptr; inlines Queue fields then AsyncSocket dtor.
    virtual AsyncSocket* Duplicate() { return new LobbyLanSocket; } //genuine SDK inline fresh same-class factory,3e3af0
    void Fn3DFB90(LobbyLanClient* inClient); //3dfb90, reciprocal setter
    SmartPtr<LobbyLanClient> mClient; //a0, owning
    long mLastMessageTime; //a4: time(NULL) written3e12e9/3e14da
};

// Full b8 allocation proved362f7f. Base NetLogic has no data beyond LobbyLogic40.
// Referenced socket and dialog classes require their true inheritance contracts
// before this class's destructor/constructor can compile; no opaque offset fields.
class LanLogic : public NetLogic {
public:
    LanLogic(); //3dfc00
    virtual ~LanLogic(); //3dfef0
    virtual void InitComponent(ComponentConfig* inConfig); //3e00a0
    virtual void HandleControlEvent(int inControlId); //3e2760
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3e27c0
    virtual bool Virtual18(LobbyGame* inGame); //3e37a0
    virtual bool Virtual1C(LobbyGame* inGame); //3e3d20
    virtual LobbyClient* Virtual20(); //3e0180
    virtual void Virtual24(ByteBuffer* inBuffer); //3e1c30
    virtual void Virtual28(ByteBuffer* inBuffer); //3e1cf0
    virtual void Virtual2C(LobbyClient* inClient, ByteBuffer* inBuffer); //3e1db0
    virtual void Virtual30(LobbyGame* inGame); //3e45c0
    virtual void Virtual34(LobbyGame* inGame); //3e45e0
    virtual void Virtual38(LobbyClient* inClient, bool inFlag); //3e4590
    virtual void Virtual3C(bool inFlag); //ret4 ICF2fa500, no behavior
    virtual void Virtual40(LobbyGame* inGame); //3e45b0
    virtual void Virtual44(); //ret ICF4142a0, no behavior
    virtual void Virtual48(); //3e5900
    virtual void Virtual4C(); //3e5910
    unsigned short Fn3E0110();
    bool Fn3E0140();
    bool Fn3E0160();
    LobbyClientList* Fn3E0170();
    void Fn3E0190();
    void Fn3E0300(AsyncOp* inOp);
    void Fn3E04E0(AsyncOp* inOp);
    void Fn3E0620();
    void Fn3E1840(LobbyClient* inClient, ByteBuffer* inBuffer);
    void Fn3E1980(ByteBuffer* inBuffer);
    void Fn3E1AD0(ByteBuffer* inBuffer, bool inFlag);
    void Fn3E1E70(ReadBuffer& inBuffer);
    void Fn3E2080(ReadBuffer& inBuffer);
    void Fn3E2160(ReadBuffer& inBuffer, LobbyClient* inSender);
    void Fn3E23A0(ReadBuffer& inBuffer);
    void Fn3E25F0(ReadBuffer& inBuffer, LobbyClient* inSender);
    void Fn3E2610(ByteBuffer* inBuffer, LobbyClient* inClient);
    void Fn3E2800();
    void Fn3E2850();
    void Fn3E54E0();
    void Fn3E28D0(ChangeDirectConnectEvent* inEvent);
    bool Fn3E4600(LobbyGame* inGame);
    void Fn3E4630(LobbyGame* inGame, const IPAddr& inAddress);
    void Fn3E4800(LobbyGame* inGame);
    void Fn3E4A10(LobbyGame* inGame);
    void Fn3E4C90(ReadBuffer&, const IPAddr&);
    void Fn3E4CB0(ReadBuffer&, const IPAddr&);
    void Fn3E4EC0(ReadBuffer&, const IPAddr&);
    void Fn3E5050(ReadBuffer&, const IPAddr&);
    void Fn3E4BE0(AsyncOp* inOp);
    void Fn3E53A0();
    void Fn3E5810();

    void Fn3E2DC0(ClientActionEvent*); //343B, Core361bc0 bool target closed by Staging
    void Fn3E2F40(const GUIString&,unsigned short,unsigned short); //268B
    void Fn3E3050(Component*); //374B
    void Fn3E3290(); //483B; model Game20 field/canonical Core wrappers pending
    void Fn3E3480(); //287B, closed Core saved-map alias/getter queued
    void Fn3E12A0(AsyncOp*); //262B, reference-only RecvMsgOp
    void Fn3E1210(ByteBuffer*,LobbyLanSocket*); //128B
    void Fn3E0760(ReadBuffer&,LobbyLanSocket*); //1815B original handler; socket/IP canonical API pending
    int Fn3E0F90(ByteBuffer*,LobbyGame*); //603B hot plus owned EH return continuation24B zero extra quota
    void Fn3E2CE0(LobbyClient*,int); //212B, Dialogs full ClientOptionsPopup90 proposal required
    void Fn3E13B0(LobbyLanSocket*); //579B, complete RecvMsgOpB4 proposal and Queue API/header pending
    bool Fn3E35A0(); //512B auth initialization remains originalcallee; no giant partial op allocation
    void Fn3E1600(AsyncOp*); //3e1600 accepted authentication completion, native255
    void Fn3E1700(AsyncOp*); //313B accept completion remains originalcallee pending full auth-op contracts
    void Fn3E5200(AsyncOp*); //382B receive callback remains originalcallee pending nullable ByteBuffer API

    SmartPtr<DirectConnectDialog> mDirectConnectDialog; //40
    SmartPtr<LanNameDialog> mLanNameDialog; //44
    SmartPtr<LanPortDialog> mLanPortDialog; //48
    SmartPtr<PasswordDialog> mPasswordDialog; //4c
    SmartPtr<LobbyGameList> mLanGames; //50, new2c/ctor3a7c50(kind1)
    SmartPtr<LobbyGameList> mDirectGames; //54, new2c/ctor3a7c50(kind3)
    std::set<SmartPtr<LobbyLanSocket> > mPendingSockets; //58, sentinel14
    SmartPtr<LobbyLanSocket> mListenSocket; //64, newa8/finalVT4559ec
    SmartPtr<LobbyLanSocket> mServerSocket; //68, newa8/finalVT4559ec
    long mLastServerPing; //6c, time(NULL) difference in callback3e04e0
    AsyncOpPtr mNetworkTimer; //70, new44/ctor29ef80, callback3e04e0
    bool mB74; //74, direct-connect/gamekind3 state; original meaning provisional
    SmartPtr<QueueSocket> mDiscoverySocket; //78, newa0/ctor2b3930(type1)
    AsyncOpPtr mDiscoveryTimer; //7c, new44/ctor29ef80, callback3e4be0
    IPAddr mDiscoveryAddress; //80
    long mLastBroadcastTime; //a8, time(NULL), signed comparison; ctor leaves uninitialized
    SmartPtr<LobbyClientList> mClients; //ac
    SmartPtr<LobbyLanClient> mMyClient; //b0, new30/finalVT4559d8
    unsigned short mNextClientId; //b4, two-byte allocation/wrap
    bool mIsCaptain; //b6
};
}
#endif
