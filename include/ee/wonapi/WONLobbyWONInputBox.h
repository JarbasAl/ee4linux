#ifndef EE_WONAPI_WONLOBBYWONINPUTBOX_H
#define EE_WONAPI_WONLOBBYWONINPUTBOX_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbySkinFrame.h"
namespace WONAPI {
// Complete actual100 owner: InputBoxec plus genuine WONFrame14, no other tail.
// Native319580/new316c8f and full43-slot RTTI4509ec prove this actual type.
class WONInputBox : public InputBox {
protected:
    WONFrame mFrame; //ec..ff: four border ints, ImageSmartPtrfc
public:
    WONInputBox(); //319580 genuine emptySDKbody, real base/member lifecycle
    // Keep actual implicit destructor:319600 releasesFrame thenInputBox; no own-vptr store.
    virtual void Paint(Graphics&); //3197e0 true18 override
    WONInputBox* Duplicate(); //319770 returns new defaultinput with themeframe only
    void SetFrame(const WONFrame& frame) {mFrame=frame;} //genuine SDK inline deep-copy assignment
};
}
#endif
