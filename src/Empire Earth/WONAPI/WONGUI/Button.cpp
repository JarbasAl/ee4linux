// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/EELobbySound.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Actual static owners: native966224/966228. Initialization/destruction support0.
SmartPtr<Button> Button::mDefaultButton;
SmartPtr<Button> Button::mDialogDefaultButton;
// FUNCTION: WON 0x006f3070
Button::Button()
{
    mButtonFlags=0;
    mKeyDown=false;
    mMouseDownState=0;
    SetComponentFlags(0x4000,true);
}
// Native normal109B member cleanup; compiler/lifetime support0.
Button::~Button() {}
// Native true hook3B, no quota.
// FUNCTION: WON 0x006f30d0
bool Button::VirtualAC() { return true; }
// Actual empty text hook folded156dd8; no quota.
void Button::VirtualB0(const GUIString&) {}
// FUNCTION: WON 0x006f3180
void Button::SetText(const GUIString& text,bool invalidate)
{
    VirtualB0(text);
    if(invalidate) Invalidate();
}
// FUNCTION: WON 0x006f31c0
void Button::SetDialogDefaultButton(Button* button)
{
    if(button==mDialogDefaultButton.get()) return;
    mDialogDefaultButton=button;
    SetDefaultButton(button);
}
// FUNCTION: WON 0x006f3230
void Button::UnSetDialogDefaultButton(Button* button)
{
    if(button!=mDialogDefaultButton.get()) return;
    mDialogDefaultButton=NULL;
    UnSetDefaultButton(button);
}
// FUNCTION: WON 0x006f3280
void Button::SetDefaultButton(Button* button)
{
    if(button==mDefaultButton.get()) return;
    if(mDefaultButton.get()!=NULL) mDefaultButton->Invalidate();
    mDefaultButton=button;
    if(button!=NULL) button->Invalidate();
}
// Native13B conditional forwarding, support0.
// FUNCTION: WON 0x006f3300
void Button::UnSetDefaultButton(Button* button)
{
    // the original ignores the argument and falls back to the dialog's default button (0x6f3300)
    SetDefaultButton(mDialogDefaultButton.get());
}
// FUNCTION: WON 0x006f3310
void Button::CopyAttributes(Button* from)
{
    mButtonFlags=from->mButtonFlags;
    mClickSound=from->mClickSound;
}
// Native9B flag getter, support0.
// FUNCTION: WON 0x006f3170
bool Button::IsMouseOver() { return mButtonFlags&0x10?true:false; }
// Native14B static identity getter, support0.
// FUNCTION: WON 0x006f31b0
bool Button::IsDefaultButton() { return mDefaultButton.get()==this; }
// FUNCTION: WON 0x006f3370
void Button::Fn2F3370(unsigned long flags,bool set)
{
    if(set) mButtonFlags|=flags;
    else mButtonFlags&=~flags;
    if(IsRadio()) SetComponentFlags(8,IsChecked());
}
// FUNCTION: WON 0x006f33c0
void Button::Virtual6C()
{
    Component::Virtual6C();
    Fn2F3370(0x30,false);
    if(IsRadio()) {
        SetCheck(IsChecked(),false);
        if(!WantFocus()) {
            Button* radio=GetEndRadio(false);
            while(radio!=NULL) {
                if(radio->WantFocus()) return;
                radio=radio->GetNextRadio(true);
            }
            SetComponentFlags(8,true);
        }
    }
}
// FUNCTION: WON 0x006f3440
void Button::Fn2F3440()
{
    if(IsDisabled() || IsNoActivate()) return;
    if(IsRadio()) SetCheck(true,false);
    else SetCheck(!IsChecked(),false);
    if(VirtualAC()) Fn2F5380(15,0);
}
// FUNCTION: WON 0x006f34a0
Button* Button::GetNextRadio(bool forward)
{
    Component* aSibling=GetNextSibling(forward,false);
    Button* button=dynamic_cast<Button*>(aSibling);
    return button!=NULL && button->IsRadio() ? button : NULL;
}
// FUNCTION: WON 0x006f34e0
Button* Button::GetEndRadio(bool forward)
{
    Button* last=this;
    while(true) {
        Button* next=last->GetNextRadio(forward);
        if(next==NULL) break;
        last=next;
    }
    return last;
}
// FUNCTION: WON 0x006f3500
void Button::SetCheck(bool checked,bool notify)
{
    if(IsRadio()) {
        SetComponentFlags(8,checked);
        if(checked) {
            Button* radio=GetEndRadio(false);
            while(radio!=NULL) {
                if(radio!=this) {
                    radio->SetComponentFlags(8,false);
                    if(radio->IsChecked()) { radio->Fn2F3370(4,false); radio->Invalidate(); }
                }
                radio=radio->GetNextRadio(true);
            }
            Fn2F5890();
        }
    }
    if(notify) Fn2F5380(15,0);
    if(IsChecked()!=checked) { Fn2F3370(4,checked); Invalidate(); }
}
// FUNCTION: WON 0x006f35b0
void Button::Virtual24(int x,int y)
{
    Component::Virtual24(x,y);
    Fn2F3370(0x10,true);
    if(mButtonFlags&1) Invalidate();
}
// FUNCTION: WON 0x006f35e0
void Button::Virtual2C(int,int,int button)
{
    if(button==0) { mMouseDownState=2; ButtonDown(); }
}
// FUNCTION: WON 0x006f3600
void Button::Virtual30(int,int,int button)
{
    if(button==0) { mMouseDownState=0; ButtonUp(true); }
}
// FUNCTION: WON 0x006f3620
void Button::Virtual28()
{
    Component::Virtual28();
    mMouseDownState=0;
    ButtonUp(false);
    if(IsMouseOver() && (mButtonFlags&1)) Invalidate();
    Fn2F3370(0x10,false);
}
// FUNCTION: WON 0x006f3660
void Button::ButtonDown()
{
    if(IsPressed()) return;
    if(mClickSound.get()!=NULL) mClickSound->Fn329BD0();
    Fn2F3370(0x20,true);
    Invalidate();
    if(IsRepeat()) { mDelayAcc=0; Fn2F57D0(true); Fn2F3440(); }
    if(IsActivateOnPress()) Fn2F3440();
    Fn2F5890();
}
// FUNCTION: WON 0x006f36d0
void Button::ButtonUp(bool activate)
{
    if(!IsPressed() || mMouseDownState==2 || mKeyDown) return;
    if(mMouseDownState==1) activate=false;
    Fn2F3370(0x20,false);
    Invalidate();
    if(IsRepeat()) { Fn2F57D0(false); return; }
    if(activate && !IsActivateOnPress()) Fn2F3440();
}
// FUNCTION: WON 0x006f3740
void Button::Virtual38(int x,int y)
{
    if(mMouseDownState==0) return;
    if(x>=0 && y>=0 && x<Width() && y<Height()) {
        mMouseDownState=2;
        if(!IsPressed()) { ButtonDown(); mDelayAcc=500; }
    } else { mMouseDownState=1; ButtonUp(false); }
}
// FUNCTION: WON 0x006f37a0
void Button::Virtual48()
{
    Component::Virtual48();
    if(IsDefaultOnFocus()) SetDefaultButton(this);
    Invalidate();
}
// FUNCTION: WON 0x006f37d0
void Button::Virtual4C()
{
    Component::Virtual4C();
    if(mKeyDown) { mKeyDown=false; ButtonUp(false); }
    UnSetDefaultButton(this);
    Invalidate();
}
// FUNCTION: WON 0x006f3800
bool Button::Virtual54(int key)
{
    if(key==13) {
        if(IsDefaultButton()) { Fn2F3440(); return true; }
    } else if(key==32 && !mKeyDown) {
        mKeyDown=true;
        if(mParent!=NULL) mParent->mHasUsedDialogKeys=true;
        ButtonDown();
        return true;
    }
    if(IsRadio()) {
        bool forward;
        if(key==38 || key==37) forward=false;
        else if(key==40 || key==39) forward=true;
        else return false;
        if(mParent!=NULL) mParent->mHasUsedDialogKeys=true;
        Button* next=GetNextRadio(forward);
        if(next!=NULL) { next->Fn2F3440(); next->Fn2F5890(); }
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006f38b0
bool Button::Virtual58(int key)
{
    if(key==32) {
        mKeyDown=false;
        ButtonUp(true);
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006f38d0
bool Button::Virtual44(int delta)
{
    mDelayAcc+=delta;
    if(mDelayAcc>500) { mDelayAcc-=100; Fn2F3440(); }
    return true;
}
// Native13B hover forwarding support0.
// FUNCTION: WON 0x006f3900
void Button::Virtual40(unsigned long delta) { Component::Virtual40(delta); }
// Native82B sound owner setter support0.
// FUNCTION: WON 0x006f3910
void Button::SetClickSound(Sound* sound) { mClickSound=sound; }
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006f30e0
// (public: virtual void * __thiscall WONAPI::Button::`scalar deleting destructor'(unsigned int)    [??_GButton@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006f3100
// (public: virtual __thiscall WONAPI::Button::~Button(void)    [??1Button@WONAPI@@UAE@XZ])
