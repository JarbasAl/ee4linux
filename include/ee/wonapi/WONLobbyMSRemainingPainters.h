#ifndef EE_WONAPI_WONLOBBYMSREMAININGPAINTERS_H
#define EE_WONAPI_WONLOBBYMSREMAININGPAINTERS_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Fieldless original MSHeaderButton :MSButton; actual RTTI VT44e1f8.
// Complete fieldless60 MSHeaderButton; genuine explicit SDK constructor declared.
// External destructor remains unchanged; no copy or implicit lifetime body authored.
class MSHeaderButton : public MSButton {
public:
    MSHeaderButton(const GUIString& text=""); //2fef40, complete fieldless60 SDK owner
    virtual void Paint(Graphics&); //2ff000, genuine18slot /185nativebytes
};
// Fieldless original MSCloseButton :Button; actual RTTI VT44e378.
// Borrowed painting only. No construction, allocation, sizeof, copy or lifetime definitions.
class MSCloseButton : public Button {
public:
    MSCloseButton(); //2ff200 genuine explicit SDK constructor, fieldless44
    virtual void Paint(Graphics&); //2ff2a0, genuine18slot /111nativebytes
    virtual void Virtual0C(int& width,int& height) { width=16; height=14; } //genuine SDK inline GetDesiredSize, native2ff250/23
};
// Fieldless original MSLabel :MSButtonBase, VT44e668; borrowed paint only.
// No receiver allocation, constructor, sizeof, copy or lifetime definitions.
class MSLabel : public MSButtonBase {
public:
    MSLabel(); //2ff6c0 genuine explicit SDK default constructor, fieldless5c
    MSLabel(const GUIString&); //actual2ff740 textctor/RET4, full fieldless5c owner, external

    virtual void Paint(Graphics&); //2ff7d0, actual18slot /66B discovery-gap body
};
// Fieldless original MSScrollThumb :Component, VT44e438; borrowed paint only.
class MSScrollThumb : public Component {
public:
    MSScrollThumb(); //2ff310 genuine SDK explicit constructor, fieldless30
    virtual ~MSScrollThumb(); //original external lifetime
    virtual void Paint(Graphics&); //2ff360, actual18slot /70B discovery-gap body
};
}
#endif
