#ifndef EE_WONAPI_WONLOBBYTABCTRLCONFIG_H
#define EE_WONAPI_WONLOBBYTABCTRLCONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Complete SDK ContainerConfig98 plus borrowed TabCtrl pointer98, full9c.
// Explicit constructor395720 delegates to base and installs actual VT452710.
class TabCtrlConfig : public ContainerConfig {
public:
    TabCtrlConfig(); //395720 genuine SDK explicit constructor; full9c, borrowed98 untouched
    static ComponentConfig* CfgFactory(); //39add0 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39ad70 genuine SDK static API, original external provider only
    virtual bool Virtual24(const std::string&); //395740
    virtual bool Virtual28(const std::string&,ComponentConfig*&,Component*&); //3958e0
    virtual void Virtual34(Component*); //38f960 original external
    TabCtrl* mTabCtrl; //98 borrowed
};
}
#endif
