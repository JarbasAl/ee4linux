// AccountLogic and InitLogic: complete original member layouts (5c and90).
// Requires the canonical GUI declarations. Partial operation types remain reference-only;

#ifndef EE_WONAPI_WONLOBBYACCOUNTLOGIC_H
#define EE_WONAPI_WONLOBBYACCOUNTLOGIC_H
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
class ComponentEvent;
class HTTPSession;
class AccountLogic : public LobbyLogic {
public:
    AccountLogic(); //3db150
    virtual ~AccountLogic(); //3db1d0
    virtual void InitComponent(ComponentConfig* inConfig); //3db2a0
    virtual void HandleControlEvent(int inControlId); //3dc2e0
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3dc340
    void ShowCredits(); //3db2e0
    bool ShowTOU(); //3db310
    bool CheckLoginStatus(); //3db330
    void HandleCreateAccount(ComponentEvent* inEvent); //3db370, deferred allocations
    void HandleLostPassword(ComponentEvent* inEvent); //3db830, deferred allocations
    void HandleLostUsername(ComponentEvent* inEvent); //3db920, deferred allocations
    void HandleLogin(ComponentEvent* inEvent); //3dba10, deferred allocations
    void QueryAccount(); //3dbee0, deferred allocations
    void HandleUpdateAccount(ComponentEvent* inEvent); //3dc160, deferred allocations
    std::wstring mLastUserName; //+40
    bool mNeedTOU; //+50
    SmartPtr<CreditsDialog> mCreditsDialog; //+54
    SmartPtr<TOUDialog> mTOUDialog; //+58
};
class InitLogic : public LobbyLogic {
public:
    InitLogic(); //3dd100, deferred HTTPSession allocation
    virtual ~InitLogic(); //3dd200
    virtual void KillHook(); //3dfb00
    virtual void InitComponent(ComponentConfig* inConfig); //3dd340
    virtual void HandleControlEvent(int inControlId); //3dfb20
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //3dfb50
    void ChooseNetworkAdapter(); //3dd370
    bool SetError(int inError, const GUIString& inDescription); //3dd3b0
    void Start(bool inRetry); //3dd570
    void Init(); //3dd630
    void StartDNSTest(); //3def30, deferred full operation allocation
    void DNSCompletion(AsyncOp* inOp); //3dd810
    void StartHTTPDocuments(); //3df0c0
    void HTTPDocumentCompletion(AsyncOp* inOp); //3de280
    void ParseCrossPromotion(const ByteBuffer* inDocument); //3ddc00, deferred HTTPGetOp allocation
    void CrossPromotionImageCompletion(AsyncOp* inOp); //3dda10
    void StartDirectoryQuery(); //3df410
    void DirectoryCompletion(AsyncOp* inOp); //3de4c0, actual template callback
    void StartVersionCheck(); //3df6c0, original call-only continuation
    void StartFirewallTest(); //3df880, original call-only continuation
    void VersionCompletion(AsyncOp* inOp); //3de8f0
    void AcceptCompletion(AsyncOp* inOp); //3deb50, full450B ret4
    void FirewallCompletion(AsyncOp* inOp); //3ded20
    void HandleTOU(GetHTTPDocumentOp* inOp); //3de090
    void LoadTOUTimes(); //3ddef0
    void KillFirewallTest(); //3dfa80
    SmartPtr<HTTPSession> mHTTPSession; //+40, call-only partial type
    std::string mCrossPromotion; //+44, original3ddaf1 narrow-string assign
    SmartPtr<AcceptOp> mAcceptOp; //+54
    SmartPtr<AsyncSocket> mFirewallSocket; //+58
    IPAddr mFirewallAddress; //+5c
    SmartPtr<ChooseNetworkAdapterDialog> mNetworkAdapterDialog; //+84
    unsigned long mErrors; //+88
    unsigned long mIgnoredErrors; //+8c
};
}
#endif
