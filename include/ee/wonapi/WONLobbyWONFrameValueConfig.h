#ifndef EE_WONAPI_WONLOBBYWONFRAMEVALUECONFIG_H
#define EE_WONAPI_WONLOBBYWONFRAMEVALUECONFIG_H
#include "ee/wonapi/WONLobbyWONListConfig.h"
#include "ee/wonapi/WONLobbyComboBoxConfig.h"
#include "ee/wonapi/WONLobbyTabCtrlConfig.h"
#include "ee/wonapi/WONLobbySkinTab.h"
#include "ee/wonapi/WONLobbySkinnedContainers.h" //genuine complete WONComboBoxbc for typed SDK inline binding
namespace WONAPI {
// Original-owned borrowed config prefixes; no allocation/lifetime/copy authority.
class WONComboBoxConfig : public ComboBoxConfig {
public:
    static ComponentConfig* CfgFactory() { return new WONComboBoxConfig; } //genuine SDK inline, native396a90/83 completea0
    virtual bool Virtual24(const std::string&); //397f70
    virtual void Virtual34(Component* component) { mComboBox=static_cast<WONComboBox*>(component); ComboBoxConfig::Virtual34(component); } //genuine SDK derived9c beforebase, native38e7d0/19
    WONComboBox* mComboBox; //9c
};
class WONTabCtrlConfig : public TabCtrlConfig {
public:
    static ComponentConfig* CfgFactory() { return new WONTabCtrlConfig; } //genuine SDK inline, native396af0/83 completea0
    virtual bool Virtual24(const std::string&); //3981f0
    virtual void Virtual34(Component* component) { TabCtrlConfig::Virtual34(component); mTabCtrl=static_cast<WONTabCtrl*>(component); } //genuine SDK basefirst thenderived9c, native38f100/25
    WONTabCtrl* mTabCtrl; //9c
};
}
#endif
