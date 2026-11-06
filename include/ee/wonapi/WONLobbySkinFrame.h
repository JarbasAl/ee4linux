#ifndef EE_WONAPI_WONLOBBYSKINFRAME_H
#define EE_WONAPI_WONLOBBYSKINFRAME_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
class StretchImage;
// Complete WONFrame14 declaration is provided by the included WONLobbyGUI.h.
// Actual58 original-owned simple button, target vtable450614. No new/sizeof/
// ctor/copy/delete authority; retained images use genuine existing Image owner.
class WONSimpleButton : public Button {
public:
    WONSimpleButton(); //317990 external full58, bounded F allocation
    virtual ~WONSimpleButton(); //317a30 external normal/deleting317a10
    WONSimpleButton* Duplicate(); //317ae0 actual raw result, original external
    virtual void Virtual0C(int&,int&); //317a90 actual SDK GetDesiredSize slot0c
    virtual void Paint(Graphics&); //317bd0 actual18
    void SetImage(int state,Image* image) { mImages[state]=image; }
    void SetScaleImage(bool v) { mScaleImage=v; }
    void SetCheckMouseButtonDown(bool v) { mCheckMouseButtonDown=v; }
    SmartPtr<Image> mImages[4]; //44..53
    bool mScaleImage,mCheckMouseButtonDown; //54,55
};
// Actual fieldless WONSimpleButton58 descendant, VT45085c/47slots.
// Bounded F new authority solely original3194a0 constructor from reviewed skin composition.
class WONScrollThumb : public WONSimpleButton {
public:
    WONScrollThumb(); //3194a0 original external
    virtual ~WONScrollThumb(); //3194e0 actual normal cleanup, external only
};
}
#endif
