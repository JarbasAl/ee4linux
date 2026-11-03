// Actual editor UIEvent families. Nominal header placement provisional.
// All five are0c: original new sites19eec4 and19f439..19f489, RTTI/base chains.
#ifndef EE_EESEEDITOREVENTS_H
#define EE_EESEEDITOREVENTS_H
#include "lle/UIEvent.h"
class EEUIEOpenCivBuilder : public UIEvent {
public:
    virtual ~EEUIEOpenCivBuilder() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity); //19ef15
};
class EESEELoadScenario : public UIEvent {
public:
    virtual ~EESEELoadScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity); //1af59f
};
class EESEESaveScenario : public UIEvent {
public:
    virtual ~EESEESaveScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity); //1af74a
};
class EESEETestScenario : public UIEvent {
public:
    virtual ~EESEETestScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity); //1af77e
};
class EESEEExitScenario : public UIEvent {
public:
    virtual ~EESEEExitScenario() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity); //1af7e2
};
#endif
