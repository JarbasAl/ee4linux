// Target local configuration classes; RTTI records have global namespace identities.
#ifndef EE_WONAPI_WONLAYOUTCONFIGS_H
#define EE_WONAPI_WONLAYOUTCONFIGS_H
#include "ee/wonapi/WONLobbyGUI.h"
// Borrowed complete ac view, native allocation38860b and ctor387fb0.
class PercentLayoutConfig : public WONAPI::ComponentConfig {
public:
    PercentLayoutConfig(); //387fb0 external; no own lifetime implementation
    virtual bool Virtual0C(WONAPI::ConfigParser*); //388000
    virtual bool Virtual10(WONAPI::ConfigParser*) { return true; } // genuine SDK inline PostParse, native387ff0/5
    virtual bool Virtual24(const std::string&); //3880c0
    WONAPI::Component* target; //78, borrowed
    WONAPI::Component* refs[4]; //7c..88, borrowed
    int params[8]; //8c..a8
};
// Target is80, SDKlater mSetGlobal80/SETGLOBAL/COPYSYSTEM do not survive.
class ColorSchemeConfig : public WONAPI::ComponentConfig {
public:
    ColorSchemeConfig(); //388260 external; ordinary implicit dtor388340
    virtual bool Virtual0C(WONAPI::ConfigParser*); //3883b0
    virtual bool Virtual10(WONAPI::ConfigParser*) { return true; } // genuine SDK inline PostParse, native387ff0/5
    virtual bool Virtual24(const std::string&); //388410
    WONAPI::Component* target; //78, borrowed and untouched by native constructor
    WONAPI::SmartPtr<WONAPI::ColorScheme> mColorScheme; //7c, true owned4B member
};
#endif
