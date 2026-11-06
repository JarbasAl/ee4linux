#ifndef EE_WONAPI_WONLOBBYWONBUTTONCONFIG_H
#define EE_WONAPI_WONLOBBYWONBUTTONCONFIG_H
#include "ee/wonapi/WONLobbyResourceConfig.h"
#include "ee/wonapi/WONLobbyMSSkinImage.h"
namespace WONAPI {
// Actual80 borrowed descendants: ButtonConfig7c +raw own-control pointer7c.
// No config/control new, constructor/copy/lifetime/vtable allocation authority.
class WONSimpleButtonConfig : public ButtonConfig {
public:
    static ComponentConfig* CfgFactory(); //3969c0 genuine SDK external API; call-only
    virtual bool Virtual24(const std::string&); //3974e0 actual24
    virtual void Virtual34(Component*); //396a20 sharedoriginal SetComponent
protected:
    WONSimpleButton* mButton; //7c
    void HandleButtonState(); //3971c0
    void HandleButtonStates(); //3973f0
};
class WONButtonConfig : public ButtonConfig {
public:
    static ComponentConfig* CfgFactory(); //396a30 genuine SDK external API; call-only
    virtual bool Virtual24(const std::string&); //397aa0 actual24
    virtual void Virtual34(Component*); //396a20 genuine sharedoriginal
protected:
    WONButton* mButton; //7c
    void HandleSetButtonState(); //397740
};
}
// Genuine original SDK file-static owner566af0; reference declaration only.
// No replacement definition, initialization or new source-owned state.
extern WONAPI::SmartPtr<WONAPI::WONSkin> gSkin;
#endif
