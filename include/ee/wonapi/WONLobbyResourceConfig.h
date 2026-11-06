#ifndef EE_WONAPI_WONLOBBYRESOURCECONFIG_H
#define EE_WONAPI_WONLOBBYRESOURCECONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {

// Borrowed external-lifetime configuration owners. No constructor/destructor,
// sizeof/new/copy/by-value use, or emitted lifetime/vtable definitions authorized.
// Actual inheritance from pinned SDK headers; native readers use98 and9c.
class ScrollAreaConfig : public ContainerConfig {
public:
    virtual bool Virtual24(const std::string&); //394360 native only initially
    virtual void Virtual34(Component*); //native SetComponent override, no source
    ScrollArea* mScrollArea; //98
};
class ListAreaConfig : public ScrollAreaConfig {
public:
    static Component* CompFactory(); //39abf0 genuine SDK static API, original external provider only
    static ComponentConfig* CfgFactory(); //382150 actual surviving SDK factory, external original provider only
    virtual bool Virtual24(const std::string&); //38da70
    virtual void Virtual34(Component*); //native SetComponent override, no source
    ListSortType ReadSortType(); //38d5d0
    static void ReadListFlags(ComponentConfig*,ListArea*); //38d790
    ListArea* mListArea; //9c
};
// Borrowed actual MultiList config owners. Only instruction dispatch authored; no new/sizeof/copy/lifetime.
class MultiListAreaConfig : public ListAreaConfig {
public:
    static ComponentConfig* CfgFactory(); //39ae90 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39ae30 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&); //390160/815, native ends39048f
    virtual void Virtual34(Component*); //external38fd10 actual SetComponent override
    MultiListArea* mMultiListArea; //a0, borrowed prefixa4
};
class MultiListHeaderConfig : public ContainerConfig {
public:
    static ComponentConfig* CfgFactory(); //39af50 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39aef0 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&); //390490/144, native ends390520
    virtual void Virtual34(Component* component) { mHeader=(MultiListHeader*)component; ContainerConfig::Virtual34(component); } //39afb0
    MultiListHeader* mHeader; //98, borrowed prefix9c
};
// Borrowed actual InputBox configuration owner, full7c; no allocation/lifetime source.
class InputBoxConfig : public ComponentConfig {
public:
    static Component* CompFactory(); //39ab90 genuine SDK static API, original external provider only
    static ComponentConfig* CfgFactory(); //382100 actual surviving SDK factory, external original provider only
    virtual bool Virtual24(const std::string&); //38cd60
    virtual void Virtual34(Component*); //native external SetComponent override
    InputBox* mInputBox; //78
};
// Complete SDK LabelConfig7c: ComponentConfig78 plus borrowed Label*78.
// Explicit constructor38d290 delegates to base and installs actual VT452230.
class LabelConfig : public ComponentConfig {
public:
    LabelConfig(); //38d290 genuine SDK explicit constructor; full7c, borrowed78 untouched
    static Component* CompFactory(); //39afd0 genuine SDK static API, original external provider only
    static ComponentConfig* CfgFactory(); //38f9e0 genuine SDK factory, external original7c owner
    virtual bool Virtual24(const std::string&); //38d2b0
    virtual void Virtual34(Component*); //native external SetComponent
    Label* mLabel; //78
};
// Actual borrowed configuration prefixes; original constructors/destructors remain external.
class ScrollerConfig : public ContainerConfig {
public:
    virtual bool Virtual24(const std::string&); //394540
    virtual void Virtual34(Component*); //original external SetComponent
    Scroller* mScroller; //98, prefix9c
};
// SOURCE_ONLY_NO_NATIVE_ID: complete same-SDK configuration owner, no Native ABI identity assigned.
class ButtonScrollerConfig : public ScrollerConfig {
protected:
    ButtonScroller* mButtonScroller; //borrowed, source model9c after existing ScrollerConfig9c
    ScrollButtonCondition GetScrollButtonCondition();
public:
    virtual void Virtual34(Component* component) { ScrollerConfig::Virtual34(component); mButtonScroller=(ButtonScroller*)component; }
    virtual bool Virtual24(const std::string&);
};
class ScrollbarScrollerConfig : public ScrollerConfig {
public:
    virtual bool Virtual24(const std::string&); //3948a0
    virtual void Virtual34(Component*); //original external SetComponent
    ScrollbarCondition GetScrollbarCondition(); //3946e0
    ScrollbarScroller* mScrollbarScroller; //9c, prefixa0
};
class ScrollbarConfig : public ContainerConfig {
public:
    ScrollbarConfig(); //394900 genuine SDK constructor, original external full9c
    virtual bool Virtual24(const std::string&); //394920
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //3949c0
    virtual void Virtual34(Component*); //original external SetComponent
    Scrollbar* mScrollbar; //98, prefix9c
};
class TreeArea;
class TreeAreaConfig : public ListAreaConfig {
public:
    static ComponentConfig* CfgFactory(); //39b590 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39b530 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&); //395f10..395f52
    virtual void Virtual34(Component*); //original external SetComponent
    TreeArea* mTreeArea; //a0, prefixa4
};
}
#endif
