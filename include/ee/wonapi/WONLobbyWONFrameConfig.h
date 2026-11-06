#ifndef EE_WONAPI_WONLOBBYWONFRAMECONFIG_H
#define EE_WONAPI_WONLOBBYWONFRAMECONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Reuses canonical WONFrameComponent borrowed facade in WONLobbyGUI.h.
class WONFrameComponentConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //396b50 genuine SDK external API; call-only
    virtual bool Virtual24(const std::string&); //398440
    virtual bool Virtual2C(const std::string&,int&); //398590
    virtual void Virtual34(Component*); //396ba0 original
    WONFrameComponent* mFrameComponent; //78 borrowed, prefix7c
};
}
#endif
