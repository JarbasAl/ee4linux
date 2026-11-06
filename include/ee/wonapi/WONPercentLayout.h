// Actual PercentLayout50 owner used by native ContainerConfig3885d0.
#ifndef EE_WONAPI_WONPERCENTLAYOUT_H
#define EE_WONAPI_WONPERCENTLAYOUT_H
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
class PercentLayout : public ChildLayoutInstructionBase {
public:
    PercentLayout(Component* target,const PercentLayoutParam& param) : mTarget(target),mParam(param) {} //actual inline3886ea..70f full16DWORD copy
    PercentLayout(Component *target,Component *ref,int px,int py,int pw=-1,int ph=-1,int lp=-1,int tp=-1,int rp=-1,int bp=-1);
    PercentLayout(Component *target,Component *ref1,Component *ref2,int px,int py,int pw=-1,int ph=-1,int lp=-1,int tp=-1,int rp=-1,int bp=-1);
    PercentLayout(Component *target,Component *left,Component *top,Component *right,Component *bottom,int px,int py,int pw=-1,int ph=-1,int lp=-1,int tp=-1,int rp=-1,int bp=-1);
    virtual void Execute(Component*); //337580 true20B forwarding to Param::DoCalc; no authored body
    Component* mTarget; //0c borrowed
    PercentLayoutParam mParam; //10..4f, complete16DWORD POD, no owned pointers
};
}
#endif
