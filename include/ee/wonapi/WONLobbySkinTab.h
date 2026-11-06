#ifndef EE_WONAPI_WONLOBBYSKINTAB_H
#define EE_WONAPI_WONLOBBYSKINTAB_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyMSSkinImage.h"
namespace WONAPI {
// Actualf8 borrowed receiver: nativector31a830 and fullnormal31a910.
// No WONTabCtrl allocation/constructor/destructor/copy implementation here.
class WONTabCtrl : public TabCtrl {
public:
    WONTabCtrl(WONSkin* =NULL); //31a830 actual RET4, native skin caller passes NULL; bounded F allocation
    virtual ~WONTabCtrl(); //31a910 true original external normal; deleting31a8f0
    void SetFrame(const WONFrame&); //31aaa0
    WONTabCtrl* Duplicate(); //31ad20 actual raw result, original external
    void SetTransparent(bool); //31ac50 support0 conditional forwarding
    void AddStandardTab(const GUIString&,Container*); //31ac70
    SmartPtr<WONSkin> mSkin; //d8
    WONFrame mFrame; //dc..ef, includes retained ImageSPec
    SmartPtr<Component> mBack; //f0
    bool mIsTransparent; //f4 +natural padding throughf7
};
}
#endif
