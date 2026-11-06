#ifndef EE_WONAPI_WONLOBBYMSMESSAGEBOX_H
#define EE_WONAPI_WONLOBBYMSMESSAGEBOX_H
#include "ee/wonapi/WONLobbyMSRemainingPainters.h"
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// Fieldless actual MSDialog98, native ctor300550, RTTI44ea4c.
// Original constructor remains external; implicit destruction follows real Dialog98 owner graph.
class MSDialog : public Dialog {
public:
    MSDialog(); //actual300550, no additional fields or virtual entries
};
// Complete actual MSMessageBoxb0: original Msg301fd0 allocb0 and invokes3016f0.
// All owners independently matched to ctor/dtor; scalar CreateFlagsac remains naturally uninitialized.
class MSMessageBox : public MSDialog {
protected:
    SmartPtr<MSButton> mButton1,mButton2,mButton3; //98,9c,a0, real complete60 children
    SmartPtr<MSLabel> mLabel; //a4, real complete fieldless5c child
    GUIString mTitle; //a8
    int mCreateFlags; //ac, exact primarySDK scalar, actualfullb0; unaccessed by3016f0
public:
    MSMessageBox(const GUIString&,const GUIString&,const GUIString&,const GUIString&,const GUIString&); //3016f0, RET14
    static int Msg(Window*,const GUIString&,const GUIString&,const GUIString&,const GUIString&,const GUIString&); //301fd0, cdecl plain RET, six arguments
    SmartPtr<Window> PrepareWindow(Window*); //302090, hidden SmartPtr return, RET8
    int DoDialog(Window*); //3021b0, RET4
    // Original ordinary lifetime301ea0 is implicit member/base cleanup, not a hand-written body.
};
}
#endif
