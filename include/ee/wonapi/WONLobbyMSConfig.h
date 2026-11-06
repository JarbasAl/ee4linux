#ifndef EE_WONAPI_WONLOBBYMSCONFIG_H
#define EE_WONAPI_WONLOBBYMSCONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyResourceConfig.h"
#include "ee/wonapi/WONLobbyMSRemainingPainters.h" //complete true MSHeaderButton60/MSCloseButton44 for genuine SDK inline callbacks
#include "ee/wonapi/WONLobbyMSBorderPainters.h" //complete true MSEtchedFrame30 for SDK inline callback
#include "ee/wonapi/WONLobbyRoomServer.h" //complete genuine MSSeperator30 for SDK inline callback
namespace WONAPI {
class ResourceConfigTable;
class MSHeaderButton;
void MSComponentConfig_Init(ResourceConfigTable*); //38dcb0 genuine SDK registration, support0
class MSButtonBase;
class MSArrowButton;
class MSTabButton;
class MS3DFrame;
// Borrowed actual config prefixes only, no ctor/dtor/sizeof/new authority.
class MSButtonConfig : public ButtonConfig {
public:
    MSButtonConfig(); //38f3a0 genuine SDK explicit empty constructor, complete80
    static ComponentConfig* CfgFactory(); //38e600 genuine external SDK static factory; call-only
    static Component* ButtonFactory(); //38e480 genuine external SDK static factory; call-only
    static Component* CheckboxFactory(); //38e4e0 genuine external SDK static factory; call-only
    static Component* RadioFactory(); //38e540 genuine external SDK static factory; call-only
    static Component* LabelFactory(); //38e5a0 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //native SetComponent override external
    MSButtonBase* mMSButton; //7c borrowed
};
class MSArrowButtonConfig : public ButtonConfig {
public:
    static ComponentConfig* CfgFactory(); //38e850 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38e7f0 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //native SetComponent override external
    MSArrowButton* mMSArrowButton; //7c borrowed
};
class MSTabButtonConfig : public MSButtonConfig {
public:
    static ComponentConfig* CfgFactory() { return new MSTabButtonConfig; } //genuine SDK inline, native38ea40/83 complete84
    static Component* CompFactory(); //38e960 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //native SetComponent override external
    MSTabButton* mMSTabButton; //80 borrowed
};
class MS3DFrameConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //38ee70 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38ee10 genuine external SDK static factory; call-only
    virtual bool Virtual24(const std::string&);
    virtual void Virtual34(Component*); //native SetComponent override external
    MS3DFrame* mMS3DFrame; //78 borrowed
};
// Genuine missing SDK config subclasses, external call-only API contracts.
// No instance/new/sizeof/copy/lifetime/own-vtable source authority is granted.
// Static factories remain original external providers, never local allocators.
class MSInputBoxConfig : public InputBoxConfig {
public:
    static ComponentConfig* CfgFactory(); //38e6c0 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38e660 genuine external SDK static factory; call-only
};
class MSCloseButtonConfig : public ButtonConfig {
public:
    static ComponentConfig* CfgFactory(); //38e910 genuine external SDK static factory; call-only
    static Component* CompFactory() { return new MSCloseButton; } //genuine SDK inline allocation callback, native38e8b0/87; support0
};
class MSHeaderButtonConfig : public MSButtonConfig {
protected:
    MSHeaderButton* mHeaderButton; //80 actual SDK pointer member; borrowed only
public:
    static ComponentConfig* CfgFactory() { return new MSHeaderButtonConfig; } //genuine SDK inline, native38eb80/83 complete84
    static Component* CompFactory(); //38eaa0 genuine external SDK static factory; call-only
    virtual void Virtual34(Component* component) { mHeaderButton=static_cast<MSHeaderButton*>(component); MSButtonConfig::Virtual34(component); } //genuine SDK own borrowed80 store before qualifiedbase, native38ebe0/19
};
class MSSeperatorConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //38ed20 genuine external SDK static factory; call-only
    static Component* CompFactory() { return new MSSeperator; } //genuine SDK inline allocation callback, native38ecc0/89; support0
};
class MSEtchedFrameConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //38ef20 genuine external SDK static factory; call-only
    static Component* CompFactory() { return new MSEtchedFrame; } //genuine SDK inline allocation callback, native38eec0/87; support0
};
class MSDlgBackConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //38edc0 genuine external SDK static factory; call-only
    static Component* CompFactory(); //38ed70 genuine external SDK static factory; call-only
};
}
#endif
