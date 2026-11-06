#ifndef EE_WONAPI_WONLOBBYMSTOOLCONTROLS_H
#define EE_WONAPI_WONLOBBYMSTOOLCONTROLS_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Complete genuine SDK classes with no additional data members; not opaque stand-ins.
// Native identities remain unlocated; no handwritten implicit lifetime definitions.
class MSScrollButtonContainer : public ButtonScroller {
public:
    MSScrollButtonContainer(ScrollArea*);
};
class MSToolTip : public Label {
public:
    MSToolTip(const GUIString&);
};
class MSToolButton : public MSButtonBase {
public:
    virtual void Paint(Graphics&);
    MSToolButton(const GUIString&);
};
}
#endif
