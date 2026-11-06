// Actual MSMessageBox custom construction and layout policy, native3016f0.
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
#include "ee/wonapi/WONLobbyMSMessageBox.h"
namespace WONAPI {
// FUNCTION: WON 0x007016f0
MSMessageBox::MSMessageBox(const GUIString& title,const GUIString& message,
    const GUIString& button1,const GUIString& button2,const GUIString& button3)
{
    mTitle=title;
    GUIString text=message;
    if(text.empty()) text=title;
    mLabel=new MSLabel(text);
    mLabel->SetHorzAlign(HorzAlign_Center);
    mLabel->SetAdjustHeightToFitText(true);
    mLabel->SetWrap(true);

    text=button1;
    if(text.empty()) text=GUIString("Ok");
    mButton1=new MSButton(text);
    mButton2=new MSButton(button2);
    mButton3=new MSButton(button3);
    bool doButton2=button2.length()!=0;
    bool doButton3=doButton2 && button3.length()!=0;
    SmartPtr<SameSizeLayout> sameSize=(new SameSizeLayout(mButton1.get(),true,true))->Add(mButton2.get())->Add(mButton3.get());
    sameSize->Execute(this);

    int width1=mLabel->mText->GetWidth();
    if(width1<140) width1=140;
    if(width1>400) width1=400;
    int buttonWidth=mButton1->Width();
    if(buttonWidth<80) {
        buttonWidth=80;
        mButton1->Fn2F4B90(buttonWidth,mButton1->Height());
    }
    int width2=buttonWidth;
    if(doButton2) width2+=20+buttonWidth;
    if(doButton3) width2+=20+buttonWidth;
    if(width2>width1) width1=width2;

    SetDefaultButton(mButton1.get());
    mButton1->Fn2F3370(0x100,true);
    mButton2->Fn2F3370(0x100,true);
    mButton3->Fn2F3370(0x100,true);
    mButton1->mControlId=1;
    mButton2->mControlId=2;
    mButton3->mControlId=3;
    if(doButton3) SetEndOnEscape(3);
    else if(doButton2) SetEndOnEscape(2);
    else SetEndOnEscape(1);

    Fn2F4B90(width1+20,Height());
    mLabel->Fn2F4B70(10,10);
    Fn2F6140(mLabel.get(),1,this,0,0,-20,0,0);
    SmartPtr<CenterLayout> center=new CenterLayout(this,mButton1.get(),true,false);
    Fn2F6140(mButton1.get(),0x200,mLabel.get(),0,20,0,0,0);
    if(doButton2) {
        Fn2F6140(mButton2.get(),0x4403,mButton1.get(),10,0,0,0,0);
        center->Add(mButton2.get());
    }
    if(doButton3) {
        Fn2F6140(mButton3.get(),0x4403,mButton2.get(),10,0,0,0,0);
        center->Add(mButton3.get());
    }
    AddChildLayout(center.get());
    Fn2F4B90(Width(),mButton1->Top()+mButton1->Height()+10);
    Fn2F6A60(mLabel.get());
    Fn2F6A60(mButton1.get());
    if(doButton2) Fn2F6A60(mButton2.get());
    if(doButton3) Fn2F6A60(mButton3.get());
}
}

// Native original composition helpers; actual MSWindow c8 owner, no substitute window.
namespace WONAPI {
// Original301fd0/184B allocation and DoDialog forwarder, support0.
// FUNCTION: WON 0x00701fd0
int MSMessageBox::Msg(Window* parent,const GUIString& title,const GUIString& message,
    const GUIString& button1,const GUIString& button2,const GUIString& button3)
{
    SmartPtr<MSMessageBox> box=new MSMessageBox(title,message,button1,button2,button3);
    return box->DoDialog(parent);
}
// Original302090/273B actual parent/title/create/component/focus composition.
// FUNCTION: WON 0x00702090
SmartPtr<Window> MSMessageBox::PrepareWindow(Window* parent)
{
    SmartPtr<MSWindow> window=new MSWindow;
    window->SetParent(parent);
    window->SetTitle(mTitle);
    window->mStyle=0x59;
    window->Create(WONRectangle(0,0,Width(),Height()));
    window->AddComponent(this);
    mButton1->Fn2F5890();
    return window.get();
}
// Original3021b0/134B modal forwarding and close, support0.
// FUNCTION: WON 0x007021b0
int MSMessageBox::DoDialog(Window* parent)
{
    SmartPtr<Window> window=PrepareWindow(parent);
    int result=window->RunDialog(this);
    window->Close();
    return result;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00700630
// (public: virtual __thiscall WONAPI::MSDialog::~MSDialog(void)    [??1MSDialog@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x00701d20
// (public: __thiscall WONAPI::ChildLayoutInstructionBase::ChildLayoutInstructionBase(void)    [??0ChildLayoutInstructionBase@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00701d60
// (public: virtual __thiscall WONAPI::CenterLayout::~CenterLayout(void)    [??1CenterLayout@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x00701ea0
// (public: virtual __thiscall WONAPI::MSMessageBox::~MSMessageBox(void)    [??1MSMessageBox@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x0071b310
// (public: void __thiscall std::list<class WONAPI::Component *,class std::allocator<class WONAPI::Component *> >::push_back(class WONAPI::Component * con)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00701d40
// (public: virtual void * __thiscall WONAPI::CenterLayout::`scalar deleting destructor'(unsigned int)    [??_GCenterLayout@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00701de0
// (public: virtual void * __thiscall WONAPI::SameSizeLayout::`scalar deleting destructor'(unsigned int)    [??_GSameSizeLayout@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00701e80
// (public: virtual void * __thiscall WONAPI::MSMessageBox::`scalar deleting destructor'(unsigned int)    [??_GMSMessageBox@WONAPI@@UAEPAXI@Z])
