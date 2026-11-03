// C:\EEX\Empire Earth\UI Events\EEUIEHotKeyEvents.h (RECONSTRUCTION; header and file names inferred, from the class
// prefix EEUIEHK). The game's hot-key events: saved groups, the player event,
// the unit-type selection keys and the closest-aircraft keys. ProcessEvents: EEUIEHotKeyEvents.cpp
// (0x1e93df..0x1e9f40). The events are created by the hot-key setup (EEUIEHKSelectGroup's ctor is out of line,
// 0x1e955c, called from 0x2632ad..); only SelectGroup's ??_G is emitted here. Requires lle/UIEvent.h.
#ifndef EE_EEUIEHOTKEYEVENTS_H
#define EE_EEUIEHOTKEYEVENTS_H

class EEUIEHKAddSelectionToGroup : public UIEvent {     // vtable 0x44a9fc
public:
    EEUIEHKAddSelectionToGroup(long inGroup) : mGroup(inGroup) {}
    virtual ~EEUIEHKAddSelectionToGroup() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mGroup;                            // +0x0c (sent as a byte)
};

class EEUIEHKCreateGroup : public UIEvent {             // vtable 0x44a9e8
public:
    EEUIEHKCreateGroup(long inGroup) : mGroup(inGroup) {}
    virtual ~EEUIEHKCreateGroup() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mGroup;                            // +0x0c (also: the group's button is control 0xd4 + mGroup)
};

class EEUIEHKPlayerEvent : public UIEvent {             // vtable 0x44a8d0
public:
    virtual ~EEUIEHKPlayerEvent() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

class EEUIEHKSelectGroup : public UIEvent {             // vtable 0x446af0
public:
    EEUIEHKSelectGroup(long inGroup, bool inFlag);      // 0x1e955c (out of line)
    virtual ~EEUIEHKSelectGroup();                      // 0x1e959c (out of line: the ??_G calls it)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mGroup;                            // +0x0c
    bool mFlag;                             // +0x10 (provisional name)
};

class EEUIEHKUnitSelection : public UIEvent {           // vtable 0x442b28
public:
    // inline (the main game form's ctor 0x262e73 stores +0x0c..+0x12, then the vptr: init list)
    EEUIEHKUnitSelection(long inUnitType, bool inIdleWorkers, bool inIdleMilitary, bool inSelectAll)
        : mUnitType(inUnitType), mIdleWorkers(inIdleWorkers), mIdleMilitary(inIdleMilitary), mSelectAll(inSelectAll) {}
    virtual ~EEUIEHKUnitSelection() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mUnitType;                         // +0x0c (0x23 and 0x29, 0x34 and 0x2ba are special)
    bool mIdleWorkers;                      // +0x10 provisional name (categories 6/0x16 with command 5000/4026)
    bool mIdleMilitary;                     // +0x11 (provisional name; other categories, command 5000, stats +0xac > 0)
    bool mSelectAll;                        // +0x12 provisional name (all of them, no camera jump)
};

class EEUIESelectClosestAircraft : public UIEvent {     // vtable 0x44a9c0
public:
    EEUIESelectClosestAircraft(long inKind) { this->mKind = inKind; }  // inline (0x262e73)
    virtual ~EEUIESelectClosestAircraft() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    long mKind;                             // +0x0c 0..5 (the aircraft kind; provisional name)
};

#endif
