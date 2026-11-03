// C:\EEX\Empire Earth\Scenario Editor\EESETSETCAMGModifyObject.h (RECONSTRUCTION; file name inferred). The list-box
// event of the "create AI military group" effect type (RTTI EESETSETCAMGModifyObject > UIEvent > UReferenceCounter;
// vtable 0x443e14; sizeof 0x18: new 0x18 in EESETSETCreateAIMilitaryGroup::Virtual14 0x183d81, which passes itself
// and the controls' mListBoxes[0]/[1] (+0xb0/+0xb4)). The inline ctor stores the three
// members before the vptr: an init list. ProcessEvent 0x18370d calls AddObject when the clicked list's id (+0x80)
// is 0x9bf, else RemoveObject (EESETSETCAMGModifyObject.cpp 0x183d81..0x1843c7). ??_G 0x184371. Names are provisional.
// Requires lle/UIEvent.h, ee/EESETSETAI.h.
#ifndef EE_EESETSETCAMGMODIFYOBJECT_H
#define EE_EESETSETCAMGMODIFYOBJECT_H

class EESETSETCreateAIMilitaryGroup;
class UILBString;

class EESETSETCAMGModifyObject : public UIEvent {
public:
    EESETSETCAMGModifyObject(EESETSETCreateAIMilitaryGroup* inType, UILBString* inFirst, UILBString* inSecond)
        : mType(inType), mFirst(inFirst), mSecond(inSecond) {}
    virtual ~EESETSETCAMGModifyObject() {}                      // (??_G: 0x2b bytes, three vptr stores: a user-declared empty dtor)
    virtual long ProcessEvent(UIForm* inForm, UIEntity* inEntity);   // +0x04 0x18370d

    EESETSETCreateAIMilitaryGroup* mType;   // +0x0c
    UILBString* mFirst;                     // +0x10
    UILBString* mSecond;                    // +0x14
};

#endif
