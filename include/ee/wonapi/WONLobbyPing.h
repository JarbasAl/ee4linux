#ifndef EE_WONAPI_WONLOBBYPING_H
#define EE_WONAPI_WONLOBBYPING_H
#include <list>
#include "ee/wonapi/WONLobbyLogic.h"
#include "ee/wonapi/WONLobbyModel.h"
// Global PingGameOp RTTI is not namespace WONAPI; its complete record is source-local.
// Complete target PingLogic50; requires admitted LobbyLogic40 and canonical model list.
// Global PingGameOp98 is source-local in PingLogic.cpp; no UDP inheritance dependency here.
namespace WONAPI {
class PingLogic : public LobbyLogic {
public:
    PingLogic(); //3e6490
    virtual ~PingLogic(); //3e6520 explicit own-vtable store
    virtual void InitComponent(ComponentConfig*); //3e65d0
    virtual void HandleComponentEvent(ComponentEvent*); //3e6680
    void Fn3E66A0(ReadBuffer&, const IPAddr&); //3e66a0
    void Fn3E6810(AsyncOp*); //3e6810, receive completion
    void Fn3E68B0(AsyncOp*); //3e68b0, PingGameOp completion
    int Fn3E6B40(); //3e6b40
    void Fn3E6B50(LobbyGame*, bool, bool); //3e6b50
    void Fn3E6DB0(int); //3e6db0 unused one argument
    unsigned short mNextRequestId; //40
    std::list<SmartPtr<LobbyGame> > mPending; //44, natural alignment before list
};
}

#endif
