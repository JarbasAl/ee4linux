#ifndef EE_WONAPI_WONLOBBYMSBORDERPAINTERS_H
#define EE_WONAPI_WONLOBBYMSBORDERPAINTERS_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Actual fieldless Component descendant, original VT44ec7c; borrowed paint only.
// No constructor, allocation, sizeof, copy or lifetime implementation authority.
class MSEtchedFrame : public Component {
public:
    MSEtchedFrame(); //300840 genuine SDK explicit constructor, fieldless30
    virtual void Paint(Graphics&); //300860, genuine18slot, full98B
};
// Complete actual fieldless MSInputBoxec derived owner, native allocation33bc8e.
// Genuine implicit ctor calls original InputBox2f7fd0 and sets ownVT451858.
// Original external dtor33bec0 retains all cleanup; no new lifetime body/copy.
// Historical external label only; source contract has no explicit destructor. Natural full InputBox/member cleanup remains automatic.
class MSInputBox : public InputBox {
public:
    // virtual ~MSInputBox(); //original-owned lifetime only -- provisional external declaration withdrawn: genuine SDK destructor is implicit.
    virtual void Paint(Graphics&); //2ffad0, genuine18slot, full42B
    virtual void Virtual0C(int&,int&); //original forwarding desired-size provider
};
}
#endif
