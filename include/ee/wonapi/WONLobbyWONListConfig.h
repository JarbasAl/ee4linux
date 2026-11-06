#ifndef EE_WONAPI_WONLOBBYWONLISTCONFIG_H
#define EE_WONAPI_WONLOBBYWONLISTCONFIG_H
#include "ee/wonapi/WONLobbyResourceConfig.h"
#include "ee/wonapi/WONLobbySkinListControls.h"
namespace WONAPI {
// Actual derived call-only control prefix; no allocation or tail access.
class WONScrollbar : public Scrollbar {
public:
    WONScrollbar(bool=true); //319540 original external RET4, bounded F newbc
    bool mIsTransparent; //b8; natural padding throughbb, ctor319553
};
// Actual configuration prefixes, original lifetimes external; no allocation/copy/sizeof.
class WONScrollbarConfig : public ScrollbarConfig {
public:
    static ComponentConfig* CfgFactory(); //396bc0 genuine SDK external API; call-only
    virtual bool Virtual24(const std::string&); //3986d0
    virtual void Virtual34(Component*); //38f100 original
    WONScrollbar* mScrollbar; //9c
};
class WONScrollContainerConfig : public ScrollbarScrollerConfig {
public:
    static ComponentConfig* CfgFactory(); //396c20 genuine SDK external API; call-only
    virtual bool Virtual24(const std::string&); //3987e0 original
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //3989f0 original
    virtual void Virtual34(Component* component) { ScrollbarScrollerConfig::Virtual34(component); mScrollContainer=(WONScrollContainer*)component; } //396c80
    WONScrollContainer* mScrollContainer; //a0
};
class WONHeaderControlConfig : public MultiListHeaderConfig {
public:
    virtual bool Virtual24(const std::string&); //398b20
    virtual void Virtual34(Component*); //398f70 original
    WONHeaderControl* mHeader; //9c
};
class WONListCtrlConfig : public WONScrollContainerConfig {
public:
    static ComponentConfig* CfgFactory(); //396ca0 genuine SDK external API; call-only
    virtual bool Virtual24(const std::string&); //398c00
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //398e30 original
    virtual void Virtual34(Component*); //396d00 original
    WONListCtrl* mListCtrl; //a4
};
}
#endif
