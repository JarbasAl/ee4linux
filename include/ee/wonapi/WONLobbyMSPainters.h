#ifndef EE_WONAPI_WONLOBBYMSPAINTERS_H
#define EE_WONAPI_WONLOBBYMSPAINTERS_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Actual fieldless Component descendants, SDK MSControls.h / native RTTI.
// MSPopup is complete fieldless30: native2fd222 new30/Componentctor2f49e0/VT44ddc0.
// MSDlgBack is complete fieldless30: native31ab5a
// allocates30, calls original Component constructor2f49e0, installs VT44e99c.
// Natural implicit ctor/dtor: deleting2fd3e0 ->306860 JMP2f4a70 base cleanup.
// No authored constructor/destructor or additional private owner state.
class MSPopup : public Component { public: virtual void Paint(Graphics&); }; //300450, VT44ddc0
class MSDlgBack : public Component { public: virtual void Paint(Graphics&); }; //300520, VT44e99c
}
#endif
