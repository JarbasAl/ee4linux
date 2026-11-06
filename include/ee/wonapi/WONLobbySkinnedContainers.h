#ifndef EE_WONAPI_WONLOBBYSKINNEDCONTAINERS_H
#define EE_WONAPI_WONLOBBYSKINNEDCONTAINERS_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbySkinFrame.h"
namespace WONAPI {
// Actualbc original-owned WONComboBox, parent actuala4. Borrowed-only receiver,
// no constructor/new/sizeof/copy/lifecycle source. Vtable450a9c51 real slots.
class WONComboBox : public ComboBox {
public:
    WONComboBox(); //319800 original external fullbc, bounded F allocation
    virtual ~WONComboBox(); //319970 original external normal/deleting319950
    WONComboBox* Duplicate(); //3199e0 actual raw result, original external
    virtual void Virtual08(); //319b00 actual SizeChanged08
    virtual void Virtual0C(int&,int&); //319a70 desired-size0c
    virtual void Paint(Graphics&); //319bf0 actual18
    void SetFrame(const WONFrame& f) { mFrame=f; } //genuineSDKinline, deep original assignment3177a0
    bool mDrawFrame,mButtonInFrame; //a4,a5; natural a6/a7 padding
    WONFrame mFrame; //a8..bb
};
// Actualbc original-owned WONScrollContainer, actual9c ScrollbarScroller base.
// Vtable450b6c52 real slots; original ctor/dtor owners remain external.
class WONScrollContainer : public ScrollbarScroller {
public:
    WONScrollContainer(); //319c50 original external fullbc, bounded F allocation
    void CopyAttributes(WONScrollContainer*); //319f70 original deep attribute composition
    void SetFrame(const WONFrame& f) { mFrame=f; } //genuineSDKinline
    void SetCorner(Image* i) { mCorner=i; } //genuineSDKinline retained imageb8
    virtual ~WONScrollContainer(); //319ce0 original external normal/deleting319cc0
    WONScrollContainer* Duplicate(); //319f00 actual raw result, original external
    virtual void Virtual08(); //319df0 actual08, direct Container base hook
    virtual void Paint(Graphics&); //319ff0 actual18
    int mHeaderHeight; //9c
    bool mTransparent; //a0; natural a1..a3 padding
    WONFrame mFrame; //a4..b7
    SmartPtr<Image> mCorner; //b8..bb
};
}
#endif
