// C:\EEX\Empire Earth\Scenario Editor\EESEEvents.h (RECONSTRUCTION; header and file names inferred). The scenario
// editor's UI events (class prefix EESEE):
// their ProcessEvents are EESEEvents.cpp 0x16f892..0x16ff5f, in this order; each ??_G follows the event's first `new`
// (the trigger screens' ctors). EESEEConditionPlayerResource and EESEEChangeEffectAction (ProcessEvents 0x16fba7,
// 0x16fc3a) keep their own headers. Requires lle/UIEvent.h, lle/UWideString.h, <vector>.
#ifndef EE_EESEEVENTS_H
#define EE_EESEEVENTS_H

class EESETSEScenarioComboBox;
class UIControl;
class UITextLabel;


// vtable 0x443610; ??_G 0x17cb0c; ProcessEvent 0x16f892; sizeof 0xc; ctor inline; new at 0x17b39a
class EESEEShowTab : public UIEvent {
public:
    virtual ~EESEEShowTab() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4436c8; ??_G 0x17cb37; ProcessEvent 0x16f8b3; sizeof 0xc; ctor inline; new at 0x17b197
class EESEESelectEntry : public UIEvent {
public:
    virtual ~EESEESelectEntry() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4435fc; ??_G 0x17cb62; ProcessEvent 0x16f8ca; sizeof 0xc; ctor inline; new at 0x17b4ba
class EESEENewEntry : public UIEvent {
public:
    virtual ~EESEENewEntry() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4435e8; ??_G 0x17cb8d; ProcessEvent 0x16f8e1; sizeof 0xc; ctor inline; new at 0x17b5df
class EESEEDuplicateEntry : public UIEvent {
public:
    virtual ~EESEEDuplicateEntry() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4435d4; ??_G 0x17cbb8; ProcessEvent 0x16f8f8; sizeof 0xc; ctor inline; new at 0x17b704
class EESEEDeleteEntry : public UIEvent {
public:
    virtual ~EESEEDeleteEntry() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4435a4; ??_G 0x17cbe3; ProcessEvent 0x16f90f; sizeof 0xc; ctor inline; new at 0x17b93f
class EESEEIncPriority : public UIEvent {
public:
    virtual ~EESEEIncPriority() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x443590; ??_G 0x17cc0e; ProcessEvent 0x16f926; sizeof 0xc; ctor inline; new at 0x17ba41
class EESEEDecPriority : public UIEvent {
public:
    virtual ~EESEEDecPriority() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4435b8; ??_G 0x17cc39; ProcessEvent 0x16f93d; sizeof 0xc; ctor inline; new at 0x17b83d
class EESEEAlphaSort : public UIEvent {
public:
    virtual ~EESEEAlphaSort() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4434e4; ??_G 0x17cc64; ProcessEvent 0x16f954; sizeof 0xc; ctor inline; new at 0x17bb73
class EESEEShowDebugIDs : public UIEvent {
public:
    virtual ~EESEEShowDebugIDs() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4434cc; ??_G 0x17cc8f; ProcessEvent 0x16f97d; sizeof 0xc; ctor inline; new at 0x17bd40, 0x17f145
class EESEESelectName : public UIEvent {
public:
    virtual ~EESEESelectName() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4445f8; ??_G 0x19372b; ProcessEvent 0x16f994; sizeof 0xc; ctor inline; new at 0x18f16a, 0x18f2a3
class EESEEObjectDetail : public UIEvent {
public:
    virtual ~EESEEObjectDetail() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4443e4; ??_G 0x193756; ProcessEvent 0x16fa39; sizeof 0xc; ctor inline; new at 0x19038f, 0x19059f, 0x190916, 0x1910a9
class EESEEObjectCheck : public UIEvent {
public:
    virtual ~EESEEObjectCheck() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x444490; ??_G 0x193781; ProcessEvent 0x16fa57; sizeof 0xc; ctor inline; new at 0x190234
class EESEEObjectUnitSelect : public UIEvent {
public:
    virtual ~EESEEObjectUnitSelect() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4437f8; ??_G 0x17da52; ProcessEvent 0x16faf7; sizeof 0xc; ctor inline; new at 0x17cf0f
class EESEEAreaCheck : public UIEvent {
public:
    virtual ~EESEEAreaCheck() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x4437e4; ??_G 0x17da7d; ProcessEvent 0x16fb20; sizeof 0xc; ctor inline; new at 0x17cfda
class EESEECenterCameraOnArea : public UIEvent {
public:
    virtual ~EESEECenterCameraOnArea() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x443aac; ??_G 0x17ee15; ProcessEvent 0x16fb8d; sizeof 0xc; ctor inline; new at 0x17e66d
class EESEEChangeConditionType : public UIEvent {
public:
    virtual ~EESEEChangeConditionType() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x443c64; ??_G 0x182daf; ProcessEvent 0x16fc20; sizeof 0xc; ctor inline; new at 0x1827fb
class EESEEChangeEffectType : public UIEvent {
public:
    virtual ~EESEEChangeEffectType() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x444758; ??_G 0x196e12; ProcessEvent 0x16fc54; sizeof 0xc; ctor inline; new at 0x195bf0
class EESEETriggerOperator : public UIEvent {
public:
    virtual ~EESEETriggerOperator() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x44476c; ??_G 0x196e3d; ProcessEvent 0x16fc78; sizeof 0xc; ctor inline; new at 0x1958f1
class EESEETriggerConditionTimer : public UIEvent {
public:
    virtual ~EESEETriggerConditionTimer() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x444780; ??_G 0x196e68; ProcessEvent 0x16fcd6; sizeof 0xc; ctor inline; new at 0x19546b, 0x19554f, 0x195631, 0x19571b
class EESEETriggerChangeIndex : public UIEvent {
public:
    virtual ~EESEETriggerChangeIndex() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x444794; ??_G 0x196e93; ProcessEvent 0x16fd46; sizeof 0xc; ctor inline; new at 0x194fef, 0x195116, 0x19523d, 0x195370
class EESEETriggerRemove : public UIEvent {
public:
    virtual ~EESEETriggerRemove() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);
};

// vtable 0x443b58; ??_G 0x17fe51; ProcessEvent 0x16fd6a; sizeof 0x2c; ctor 0x17fdf1
class EESEESliderChange : public UIEvent {
public:
    // inline (an out-of-line copy 0x17fdf1: body assignments; the format is taken by value)
    EESEESliderChange(UITextLabel* inLabel, UWideString inFormat, float inScale, float inOffset)
    {
        this->mLabel = inLabel;
        this->mFormat = inFormat;
        this->mScale = inScale;
        this->mOffset = inOffset;
    }
    virtual ~EESEESliderChange() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    UITextLabel* mLabel;                    // +0x0c (SetText target)
    UWideString mFormat;                    // +0x10 (sprintf format of the value)
    float mScale;                           // +0x24 (value = slider * mScale + mOffset)
    float mOffset;                          // +0x28
};

// vtable 0x443b44; ??_G 0x17febd; ProcessEvent 0x16fdbf; sizeof 0x1c; ctor 0x17fe85 inline; new at 0x17f810, 0x17f987
class EESEECheckDisplay : public UIEvent {
public:
    EESEECheckDisplay() {}                  // inline (an out-of-line copy 0x17fe85: the vector's ctor)
    virtual ~EESEECheckDisplay() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    std::vector<UIControl*> mControls;      // +0x0c (shown/hidden with the check box)
};

// vtable 0x443e00; ??_G 0x18439c; ProcessEvent 0x16fe58; sizeof 0x18; ctor inline; new at 0x183bec, 0x187f05
class EESEEUseObjectArea : public UIEvent {
public:
    // init list: the members are stored before the vptr (0x187f26, 0x183bec)
    EESEEUseObjectArea(UITextLabel* inLabel, EESETSEScenarioComboBox* inComboBox, UIEntity* inAreaControl)
        : mLabel(inLabel), mComboBox(inComboBox), mAreaControl(inAreaControl) {}
    virtual ~EESEEUseObjectArea() {}
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);

    UITextLabel* mLabel;                    // +0x0c
    EESETSEScenarioComboBox* mComboBox;     // +0x10 (refilled with areas or objects)
    UIEntity* mAreaControl;                 // +0x14 (the control that selects areas)
};
#endif
