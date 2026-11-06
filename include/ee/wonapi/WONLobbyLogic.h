// WONLobby logic interfaces, proved by original RTTI, vtables and caller ABI.
#ifndef EE_WONAPI_WONLOBBYLOGIC_H
#define EE_WONAPI_WONLOBBYLOGIC_H
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
class ComponentConfig;
class ComponentEvent;
class LobbyGame;
class LobbyClient;
class LogicCompletionBase;
// RTTI LobbyLogic>RefCount, VT455a04 six slots, derived members begin40.
// AsyncOpTracker size34; original ctor3e5930 and ordinary dtor3e5a40.
class LobbyLogic : public RefCount {
public:
    LobbyLogic(); //3e5930
    virtual ~LobbyLogic(); //3e5a40
    virtual void KillHook(); //+04 noarg, ICF4142a0 ret
    virtual void InitComponent(ComponentConfig* inConfig); //+08 2fa500 ret4
    virtual void Reset(); //+0c noarg ICF4142a0 ret
    virtual void HandleControlEvent(int inControlId); //+10 2fa500 ret4
    virtual void HandleComponentEvent(ComponentEvent* inEvent); //+14 2fa500 ret4
    void Kill(); //3e5b00
    void RunTrackedOp(AsyncOp* inOp, LogicCompletionBase* inCompletion, unsigned long inTimeout); //3e5b20
    void RunOp(AsyncOp* inOp, CompletionBase<AsyncOpPtr>* inCompletion, unsigned long inTimeout); //3e5ba0
    bool Fn3E5C00(AsyncOp* inOp); //3e5c00, untrack query used by LogicCompletion
    AsyncOpTracker mTracker; //+08 size34
    bool mKilled; //+3c
};
// Complete actual0c nonpolymorphic LobbyLogicMgr, single owning logic list.
class LobbyLogicMgr {
public:
    LobbyLogicMgr(); //original list-only provider remains external; no source quota
    void AddLogic(LobbyLogic*); //3e5c10 actual listpushback adapter, support0
    void Init(ComponentConfig*); //3e5d40
    void Kill(); //3e5d80
    void DispatchControlEvent(int); //3e5db0
    void DispatchComponentEvent(ComponentEvent*); //3e5df0
    void DispatchTimerEvent(); //3e5e30 invokes actual LobbyLogic slot0c
    void HandleComponentEvent(ComponentEvent*); //3e5e60
    typedef std::list<SmartPtr<LobbyLogic> > LogicList;
    LogicList mLogicList; //00allocator/04head/08count, full0c
};

// RTTI NetLogic>LobbyLogic>RefCount, VT455974 adds exactly14 pure slots.
// No data beyond LobbyLogic is accessed by any proved NetLogic lifetime body.
class NetLogic : public LobbyLogic {
public:
    // Implicit virtual dtor; original3dfe00 tail jump, deleting3dfed0.
    virtual bool Virtual18(LobbyGame* inGame) = 0;
    virtual bool Virtual1C(LobbyGame* inGame) = 0;
    virtual LobbyClient* Virtual20() = 0;
    virtual void Virtual24(ByteBuffer* inBuffer) = 0;
    virtual void Virtual28(ByteBuffer* inBuffer) = 0;
    virtual void Virtual2C(LobbyClient* inClient, ByteBuffer* inBuffer) = 0;
    virtual void Virtual30(LobbyGame* inGame) = 0;
    virtual void Virtual34(LobbyGame* inGame) = 0;
    virtual void Virtual38(LobbyClient* inClient, bool inFlag) = 0;
    virtual void Virtual3C(bool inFlag) = 0;
    virtual void Virtual40(LobbyGame* inGame) = 0;
    virtual void Virtual44() = 0;
    virtual void Virtual48() = 0;
    virtual void Virtual4C() = 0;
};
}
#endif
