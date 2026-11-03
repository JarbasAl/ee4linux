// The Empire Earth editors sub-menu events (RECONSTRUCTION; header and file names inferred, from the class prefix
// EEUIEEEE = EEUIE + "Empire Earth Editors" + E; the UI's event factory in EEUIManager.cpp creates them, their ??_G
// sit at 0x27375f..0x27380b). ProcessEvent bodies: EEUIEEmpireEarthEditorsEvents.cpp. Requires lle/UIEvent.h.
#ifndef EE_EEUIEEMPIREEARTHEDITORSEVENTS_H
#define EE_EEUIEEMPIREEARTHEDITORSEVENTS_H

class EEUIEEEESubMenuOpen : public UIEvent {          // vtable 0x44b558
public:
    virtual ~EEUIEEEESubMenuOpen() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEEEEEditor : public UIEvent {               // vtable 0x44b580
public:
    virtual ~EEUIEEEEEditor() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEEEEMainMenu : public UIEvent {             // vtable 0x44b56c
public:
    virtual ~EEUIEEEEMainMenu() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEEEEScenarioEditor : public UIEvent {       // vtable 0x44b5d0
public:
    virtual ~EEUIEEEEScenarioEditor() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEEEECivBuilder : public UIEvent {           // vtable 0x44b594
public:
    virtual ~EEUIEEEECivBuilder() {}   // user-declared: its ??_G stores the own vptr first (EEUIManager.cpp)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

#endif
