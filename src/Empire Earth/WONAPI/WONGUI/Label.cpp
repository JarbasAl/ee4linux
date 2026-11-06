// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;

// FUNCTION: WON 0x006f9d80
Label::Label() {
    mColor=(int)0x80000005;
    mAlign38=HorzAlign_Left;
    mAlign3C=VertAlign_Top;
    mWrappedText=new WrappedText;
    mWrappedText->SetFont(GetNamedFont("Label"));
    mWrappedText->SetWrap(false);
    mLeftPad=mRightPad=mTopPad=mBottomPad=0;
    mAdjustHeightToFitText=false;
    SetComponentFlags(0x2000,true);
    mBackground.SetColor(-1);
}

// FUNCTION: WON 0x006f9f20
void Label::SetMaxWidth() {
    int width=Width()-mLeftPad-mRightPad;
    if(width<0) width=5;
    mWrappedText->SetMaxWidth(width);
    AdjustHeightToFitText();
}

// FUNCTION: WON 0x006f9f50
Label::~Label() {}

// FUNCTION: WON 0x006fa020
void Label::Virtual08() {
    Component::Virtual08();
    SetMaxWidth();
}

// FUNCTION: WON 0x006fa040
void Label::SetHorzAlign(HorzAlign align) {
    mAlign38=align;
    mWrappedText->SetCentered(align==HorzAlign_Center);
}

// FUNCTION: WON 0x006fa060
void Label::SetText(const GUIString& text,bool invalidate) {
    mWrappedText->SetText(text);
    AdjustHeightToFitText();
    if(invalidate) Invalidate();
}

// FUNCTION: WON 0x006fa090
void Label::AdjustHeightToFitText() {
    if(!mAdjustHeightToFitText) return;
    if(Height()-mTopPad-mBottomPad!=mWrappedText->GetHeight())
        Fn2F4B90(Width(),mWrappedText->GetHeight()+mTopPad+mBottomPad);
}

// FUNCTION: WON 0x006fa100
void Label::SetAdjustHeightToFitText(bool adjust) {
    if(adjust==mAdjustHeightToFitText) return;
    mAdjustHeightToFitText=adjust;
    AdjustHeightToFitText();
}

// FUNCTION: WON 0x006fa120
void Label::SetWrap(bool wrap) {
    mWrappedText->SetWrap(wrap);
    AdjustHeightToFitText();
}

// FUNCTION: WON 0x006fa140
void Label::Virtual0C(int& width,int& height) {
    width=mWrappedText->GetWidth()+mLeftPad+mRightPad;
    height=mWrappedText->GetHeight()+mTopPad+mBottomPad;
}

// FUNCTION: WON 0x006fa1a0
void Label::Paint(Graphics& g) {
    Component::Paint(g);
    mBackground.Paint(g,0,0,Width(),Height());
    int textWidth=mWrappedText->GetWidth();
    int textHeight=mWrappedText->GetHeight();
    int width=Width()-mLeftPad-mRightPad;
    int height=Height()-mTopPad-mBottomPad;
    int x=0,y=0;
    switch(mAlign38) {
    case HorzAlign_Center:x=(width-textWidth)/2;break;
    case HorzAlign_Right:x=width-textWidth;break;
    }
    switch(mAlign3C) {
    case VertAlign_Center:y=(height-textHeight)/2;break;
    case VertAlign_Bottom:y=height-textHeight;break;
    }
    x+=mLeftPad;
    y+=mTopPad;
    g.SetColor(mColor);
    mWrappedText->Paint(g,x,y);
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f9f00
// (public: virtual void * __thiscall WONAPI::Label::`scalar deleting destructor'(unsigned int)    [??_GLabel@WONAPI@@UAEPAXI@Z])
