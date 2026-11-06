#ifndef EE_WONAPI_WONLOBBYCOMPONENTLISTITEM_H
#define EE_WONAPI_WONLOBBYCOMPONENTLISTITEM_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Complete native20, VT44df70: ListItem18 +ownedComponent18 +sizable1c/padding.
class ComponentListItem : public ListItem {
protected:
    virtual ~ComponentListItem(); //2fdaa0 actualexplicit normal; originalexternal
public:
    ComponentListItem(Component* component,bool sizable=false,bool selectable=false)
        :mComponent(component),mIsSizable(sizable) { mB09=selectable; }
    void CheckInvalid(ListArea*); //2fa500 genuine SDK nonvirtual empty policy, nativeRET4
    virtual void Virtual04(Graphics&,ListArea*); //2fa510 actualPaint
    virtual void Virtual08(ListArea*); //2fa480 originalexternal dimensionsgetter
    virtual void Virtual0C(ListArea*,Container*); //2fa4a0 originalexternal viewport
    virtual void Virtual18(int,int); //2fa4c0 originalexternal position
    virtual bool Virtual1C(int,int); //2fa4e0 originalexternal contains
    virtual void Virtual20(ListArea*,int,int); //2fd970 originalexternal enter
    virtual void Virtual24(ListArea*); //2fd9a0 originalexternal exit
    virtual void Virtual28(ListArea*,int,int); //2fd9c0 originalexternal move
    virtual void Virtual2C(ListArea*,int,int); //2fd9f0 originalexternal drag
    virtual void Virtual30(ListArea*,int,int,int); //2fda20 originalexternal down
    virtual void Virtual34(ListArea*,int,int,int); //2fda50 originalexternal up
    SmartPtr<Component> mComponent; //18
    bool mIsSizable; //1c, natural1d..1f padding
};
}
#endif
