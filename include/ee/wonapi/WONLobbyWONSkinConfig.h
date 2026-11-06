#ifndef EE_WONAPI_WONLOBBYWONSKINCONFIG_H
#define EE_WONAPI_WONLOBBYWONSKINCONFIG_H
#include "ee/wonapi/WONLobbyWONListConfig.h"
#include "ee/wonapi/WONLobbySkinTab.h"
namespace WONAPI {
void WONControlsConfig_Init(ResourceConfigTable*); //395fa0 genuine SDK registrar, support0
// Complete actual14 cursor owner: native39a0ab new14 and326710 constructor.
// Exact4slot cursor, original constructor/destructor/virtual providers remain external.
class ComponentCursor : public Cursor {
protected:
    SmartPtr<Component> mComponent; //08
    int mMouseX,mMouseY; //0c,10
public:
    // Genuine SDK implicit destructor: native326770 releases ComponentSP then Cursor, with no own-vptr store.
    ComponentCursor(Component*,int=0,int=0); //326710 externalret0c
    virtual Component* GetComponent(int&,int&); //3267d0 actual08
    virtual bool IsNative(); //30a7b0 actual0c
};
// Original-owned fieldless78 configuration receiver. No new/sizeof/copy/lifetime.
class WONSkinConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //396d20 genuine SDK external API; call-only
    virtual bool Virtual0C(ConfigParser*); //399010 original PreParse, no own allocation body
    virtual bool Virtual24(const std::string&); //399210 actual dispatch
protected:
    bool ReadVert(); //398f90 support0
    void HandleButtonClickSound(); //3990e0
};
}
#endif
