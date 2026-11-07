#ifndef EE_WONAPI_WONLOBBYMSEXTRACONFIG_H
#define EE_WONAPI_WONLOBBYMSEXTRACONFIG_H
#include "ee/wonapi/WONLobbyResourceConfig.h"
#include "ee/wonapi/WONLobbyComboBoxConfig.h"
#include "ee/wonapi/WONLobbyTabCtrlConfig.h"
#include "ee/wonapi/WONLobbyMSPainters.h" //existing complete fieldless MSDlgBack30 owner
#include "ee/wonapi/WONLobbyMSRemainingPainters.h" //genuine complete MSScrollThumb30 for SDK inline factory
namespace WONAPI {
// Actual control contracts; borrowed unless a complete owner is explicitly promoted below.
// MSComboBoxa8/MSScrollbarbc/MSScrollContainera4/MSTabCtrle0 full owner contracts
// authorize scoped composition source/allocation; shared base lifetimes stay original.
// Other controls remain borrowed; no allocation/lifetime/vtable source authority.
// MSComboBox is actual completea8: ComboBoxa4 + mDrawFramea4 + naturalpadding.
// Genuine own operational constructor source is now permitted; original destructor
// and shared base lifetime remain external. No new copy or destructor source bodies.
// Other controls below retain their separately stated borrowed contracts.
// Complete actual MSComboBoxa8; genuine operational constructor source now permitted.
// Original own destructor remains external; no copy or new lifetime definitions.
class MSComboBox : public ComboBox {
public:
    MSComboBox(); //2ffb20 actual661B operational composition constructor
    virtual void Virtual08(); //2ffe30 actual SizeChanged, same08slot
    virtual void Virtual0C(int&,int&); //2ffb00 actualdefaultsizegetter, support0
    virtual void Paint(Graphics&); //2ffdf0 actual18slot
    static SmartPtr<Button> (*ArrowButtonFactory)(Direction); //actual SDK MSArrowButtonFactory underlying type; retained original cell4d5e6c, undefined only
    bool mDrawFrame; //a4 actual38f83a, SDK inline SetDrawFrame
    void SetDrawFrame(bool v) { mDrawFrame=v; }
};
typedef SmartPtr<Button> (*MSArrowButtonFactory)(Direction); //actual SDK returnedSP/Direction ABI
SmartPtr<Button> DefaultMSArrowButtonFactory(Direction); //2fdc40 genuine SDK default allocation policy
// Complete actual MSScrollbarbc, inheritedb8 +boolb8/natural tailb9..bb.
class MSScrollbar : public Scrollbar {
public:
    MSScrollbar(bool=true); //2ff820 actual composition constructor559B
    static MSArrowButtonFactory ArrowButtonFactory; //retained mutable original cell4d5e64, no definition
    // one-byte class by value: the ctor 0x6ff820 byte copy into a temp + dword push, its inline ctor a site (name is ours)
    struct ThumbType { bool mVertical; ThumbType(bool v) : mVertical(v) {} };
    typedef SmartPtr<Component> (*ThumbFactoryFunc)(ThumbType);
    static SmartPtr<Component> DefaultThumbFactory(ThumbType) { return new MSScrollThumb; } //genuine SDK inline allocation policy2fccb0
    static ThumbFactoryFunc ThumbFactory; //retained mutable original cell4d5e68, no definition
    void SetTransparent(bool); //2ffa60 actual original13B provider
    virtual void Paint(Graphics&); //2ffa70 actual18slot, onlynon-dtoroverride
protected:
    bool mIsTransparent; //b8 exactnative2ffa64store/2ffa85read; remainingtail originalowned
};
class MSEtchedFrameGroup : public Container {
public:
    MSEtchedFrameGroup(); //3008d0 genuine explicit SDK complete8c owner
    virtual void Virtual0C(int&,int&); //300a60 actual desiredsize0c, support0
    virtual void Virtual6C(); //300ab0 actual AddedToParent6c
    virtual void Paint(Graphics&); //300c00 actual18slot
    void SetLabelText(const GUIString&); //300b70
    void AddContainedComponent(Component*); //300ba0
    Label* GetLabel() { return mLabel.get(); } //genuine SDK inline, borrowed ownedLabel84
    SmartPtr<Label> mLabel; //84 actual owning pointer, borrowed receiver prefix
    SmartPtr<Component> mComponent; //88 actual owning pointer, no receiver allocation authority
};
// Complete actual MSTabCtrle0: fullTabCtrld8 +boold8/naturalpadd9..db +MSDlgBackSPdc.
// Own composition constructor only; actual external destructor retains full member/base cleanup.
class MSTabCtrl : public TabCtrl {
public:
    MSTabCtrl(); //3010e0 actual575B composition
    virtual void Virtual6C(); //301670 actual5B AddedToParent JMP TabCtrl308cb0; real override
    void SetTransparent(bool); //301660 original provider
    void AddStandardTab(const GUIString&,Container*); //301680 original provider
    bool mIsTransparent; //d8 native constructor/setter, actual natural d9..db alignment
    SmartPtr<MSDlgBack> mBack; //dc owning, actual ctor defaultNULL/dtor3014ef releases first

};
// Complete actual MSScrollContainera4: true ScrollbarScroller9c base +int9c/boolsa0..a2/pada3.
class MSScrollContainer : public ScrollbarScroller {
public:
    MSScrollContainer(ScrollArea* =NULL); //2ffeb0 actual composition constructor387B/RET4
    virtual void Virtual08(); //300290 actualSizeChanged08slot
    virtual void Paint(Graphics&); //300390 actual18slot
    int mHeaderHeight; //9c actual390000 predecessor38fffa store
    bool mDraw3DFrame,mDrawFrame,mTransparent; //a0/a1/a2 actual300254/274 and38fdb2
    void SetDraw3DFrame(bool); //300250 original provider, preserves SizeChanged callback
    void SetDrawFrame(bool); //300270 original provider, preserves SizeChanged callback
    void SetTransparent(bool v) { mTransparent=v; } //actual SDK inline
};
class MSHeaderControl : public MultiListHeader {
public:
    MSHeaderControl(MultiListArea* =NULL); //300d10 completeb0 SDK owner
    void SetColumn(int,const GUIString&); //300dd0 genuine original provider
    int mTextLeftPad; //ac genuine SDK scalar tail; allocation300eed isb0

};
class MSListCtrl : public MSScrollContainer {
public:
    MSListCtrl(MultiListArea* =NULL); //300e80 completea8 SDK owner
    virtual void Virtual08(); //3010b0 genuine SDK SizeChanged override, existing actual08slot
    void SetHeaderHeight(int v) { mHeaderHeight=v; } //actual SDK inline/native38fffa
    MultiListArea* GetListArea() { return (MultiListArea*)GetScrollArea(); } //genuine SDK inline, borrowed84
    MSHeaderControl* GetHeader() { return mHeader.get(); } //genuine SDK inline, borroweda4
    SmartPtr<MSHeaderControl> mHeader; //actual sole retained taila4, native390062/300fe5; completea8
};
// Actual borrowed config subclasses, original14-slot surface retained. No local
// config allocation/lifetime or predefined mapper source in this cohort.
class MSComboBoxConfig : public ComboBoxConfig {
public:
    static ComponentConfig* CfgFactory() { return new MSComboBoxConfig; } //genuine SDK inline, native38e770/83 completea0
    static Component* CompFactory(); //38e710 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&); //38f810
    virtual void Virtual34(Component*); //38e7d0 original external
    MSComboBox* mMSComboBox; //9c borrowed, prefixa0
};
class MSScrollbarConfig : public ScrollbarConfig {
public:
    static ComponentConfig* CfgFactory(); //38ec60 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38ec00 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&); //38f880
    virtual void Virtual34(Component*); //38f860 original external
    MSScrollbar* mMSScrollbar; //9c borrowed, prefixa0
};
class MSEtchedFrameGroupConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory(); //38efe0 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38ef80 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&); //38fa40
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //38f980 original external
    virtual void Virtual34(Component*); //38f960 original external, ICF shared inherited shape
    MSEtchedFrameGroup* mFrame; //98 borrowed, prefix9c
};
class MSTabCtrlConfig : public TabCtrlConfig {
public:
    static ComponentConfig* CfgFactory() { return new MSTabCtrlConfig; } //genuine SDK inline, native38f0a0/83 completea0
    static Component* CompFactory(); //38f040 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&); //38fb30
    virtual void Virtual34(Component*); //38f100 original external
    MSTabCtrl* mMSTabCtrl; //9c borrowed, prefixa0
};
class MSScrollContainerConfig : public ScrollbarScrollerConfig {
public:
    static ComponentConfig* CfgFactory(); //38f180 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38f120 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&); //38fd30
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //38fdd0 original external
    virtual void Virtual34(Component*); //38fd10 original external, ICF shared inherited shape
    MSScrollContainer* mScrollContainer; //a0 borrowed, prefixa4
};
class MSHeaderControlConfig : public MultiListHeaderConfig {
public:
    static ComponentConfig* CfgFactory(); //38f240 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38f1e0 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&); //38ff00
    virtual void Virtual34(Component*); //38f2a0 original external
    MSHeaderControl* mMSHeader; //9c borrowed, prefixa0
};
class MSListCtrlConfig : public MSScrollContainerConfig {
public:
    static ComponentConfig* CfgFactory(); //38f320 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38f2c0 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&); //38ffd0
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //390020 original external
    virtual void Virtual34(Component*); //actual RTTI target pinned; original external
    MSListCtrl* mListCtrl; //a4 borrowed, prefixa8
};
}
#endif
