// Dialog dragging, focus and input behavior on complete original-owned Dialog98 receivers.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Original2f7990/20, direct state setter support0.
// FUNCTION: WON 0x006f7990
void Dialog::SetEndOnEscape(int id)
{
    mEndOnEscape=true;
    mEscapeId=id;
}
// Original2f79b0/8, direct field setter support0.
// FUNCTION: WON 0x006f79b0
void Dialog::SetDontEndOnEscape()
{
    mEndOnEscape=false;
}
// Original2f79c0/91, owned assignment helper support0.
// FUNCTION: WON 0x006f79c0
void Dialog::SetDefaultButton(Button* button)
{
    mC94=button;
}
// Original2f7a20/27, default-focus/base forwarding support0.
// FUNCTION: WON 0x006f7a20
void Dialog::Virtual48()
{
    Button::SetDialogDefaultButton(mC94.get());
    Container::Virtual48();
}
// Original2f7a40/27, default-focus/base forwarding support0.
// FUNCTION: WON 0x006f7a40
void Dialog::Virtual4C()
{
    Button::UnSetDialogDefaultButton(mC94.get());
    Container::Virtual4C();
}
// Original2f7a60, state reset/base forwarding support0.
// FUNCTION: WON 0x006f7a60
void Dialog::Virtual08()
{
    mB8C=false;
    Container::Virtual08();
}
// FUNCTION: WON 0x006f7a70
void Dialog::Virtual2C(int x,int y,int button)
{
    Container::Virtual2C(x,y,button);
    if(button!=0) return;
    Fn2F5890();
    if(mAllowDrag && mMouseChild!=NULL && mMouseChild->ComponentFlagSet(0x20)) {
        if(GetNextSibling(true,false)!=NULL) mParent->BringToTop(this);
        mB8C=true;
        mDragX=x;
        mDragY=y;
    }
}
// Original2f7af0/42, base forwarding and direct reset support0.
// FUNCTION: WON 0x006f7af0
void Dialog::Virtual30(int x,int y,int button)
{
    Container::Virtual30(x,y,button);
    if(button==0) mB8C=false;
}
// Original2f7b20/17, base forwarding and direct reset support0.
// FUNCTION: WON 0x006f7b20
void Dialog::Virtual28()
{
    Container::Virtual28();
    mB8C=false;
}
// FUNCTION: WON 0x006f7b40
void Dialog::Virtual38(int x,int y)
{
    if(!mB8C) {
        Container::Virtual38(x,y);
        return;
    }
    // Native target queries the actual RootContainer, unlike the newer SDK's GetParent spelling.
    Container* root=Fn2F58F0();
    int newX=Left()+(x-mDragX);
    int newY=Top()+(y-mDragY);
    if(mKeepInRoot) {
        if(newX+Width()>root->Width()) newX=root->Width()-Width();
        if(newX<0) newX=0;
        if(newY+Height()>root->Height()) newY=root->Height()-Height();
        if(newY<0) newY=0;
    }
    if(newX==Left() && newY==Top()) return;
    WONRectangle r(0,0,Width(),Height());
    mParent->VirtualC0(this);
    Virtual60(r);
    Fn2F4B70(newX,newY);
    Virtual60(r);
    Invalidate();
}
// FUNCTION: WON 0x006f7c30
bool Dialog::Virtual54(int key)
{
    if(Container::Virtual54(key)) return true;
    if(key==27 && mEndOnEscape) {
        GetRoot()->Fn32B0A0(mEscapeId,this);
        return true;
    } else if(key==13) return ActivateDefaultButton();
    return false;
}
// FUNCTION: WON 0x006f7c90
bool Dialog::Virtual5C()
{
    Window* window=GetRoot();
    if(!Container::Virtual5C()) return false;
    if(window->mManager->mModalComponent==this) {
        if(mEndOnEscape) window->Fn32B0A0(mEscapeId,NULL);
        else return false;
    }
    return true;
}
// Original2f7ce0/51, guarded defaultbutton forwarding support0.
// FUNCTION: WON 0x006f7ce0
bool Dialog::ActivateDefaultButton()
{
    if(mC94.get()!=NULL && !mC94->IsDisabled() && mC94->IsDefaultButton()) {
        mC94->Fn2F3440();
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006f7d20
void Dialog::HandleComponentEvent(ComponentEvent* event)
{
    if(event->mType==20) ActivateDefaultButton();
    else if(event->mType==15 && (static_cast<Button*>(event->mComponent)->mButtonFlags&0x100))
        GetRoot()->Fn32B0A0(event->mComponent->mControlId,NULL);
    else Container::HandleComponentEvent(event);
}
}
