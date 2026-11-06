// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
namespace WONAPI {
// Native300a60..300aac76B, simple dimensions support0.
// FUNCTION: WON 0x00700a60
void MSEtchedFrameGroup::Virtual0C(int& width,int& height) {
    if(mComponent.get()!=NULL) {
        width=mComponent->Width()+20;
        height=mComponent->Height()+mLabel->Height()+20;
    } else width=height=20;
}
// Native300ab0..300b63179B.
// FUNCTION: WON 0x00700ab0
void MSEtchedFrameGroup::Virtual6C() {
    if(mLabel->mParent!=this) {
        Fn2F6A60(mLabel.get());
        Fn2F6140(mLabel.get(),0x1000,this,5,0,0,0,0);
        if(mComponent.get()!=NULL) {
            Fn2F6A60(mComponent.get());
            Fn2F6140(mComponent.get(),0x1001,this,10,0,-20,0,0);
            Fn2F6140(mComponent.get(),0x200,mLabel.get(),0,10,0,0,0);
            Fn2F6140(mComponent.get(),0x80000,this,0,0,0,-10,0);
        }
    }
    Container::Virtual6C();
}
// Native300b70..300b9840B, thin provider support0.
// FUNCTION: WON 0x00700b70
void MSEtchedFrameGroup::SetLabelText(const GUIString& text) {
    mLabel->SetText(text);
    mLabel->Fn2F4E90(10,0);
}
// Native300ba0..300bfb91B, smart pointer assignment support0.
// FUNCTION: WON 0x00700ba0
void MSEtchedFrameGroup::AddContainedComponent(Component* c) { mComponent=c; }
// Native300c00..300d0d269B.
// FUNCTION: WON 0x00700c00
void MSEtchedFrameGroup::Paint(Graphics& g) {
    Container::Paint(g);
    int dy=mLabel->Height()/2;
    g.Translate(0,dy);
    int right=Width()-1;
    int bottom=Height()-1-dy;
    int labelLeft=mLabel->Left();
    int labelRight=mLabel->Right();
    g.SetSysColor(2);
    g.DrawLine(0,1,labelLeft,1);
    g.DrawLine(labelRight,1,right,1);
    g.DrawLine(right,0,right,bottom);
    g.DrawLine(0,bottom,right,bottom);
    g.DrawLine(1,1,1,bottom);
    g.SetSysColor(3);
    g.DrawLine(0,0,labelLeft,0);
    g.DrawLine(labelRight,0,right-1,0);
    g.DrawLine(right-1,0,right-1,bottom-1);
    g.DrawLine(0,bottom-1,right-1,bottom-1);
    g.DrawLine(0,0,0,bottom-1);
    g.Translate(0,-dy);
}
}
