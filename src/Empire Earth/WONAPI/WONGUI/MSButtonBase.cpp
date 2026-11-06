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

// FUNCTION: WON 0x006fe850
const GUIString& MSButtonBase::GetText() { return mText->mText; }

// FUNCTION: WON 0x006fe880
void MSButtonBase::VirtualB0(const GUIString& text) {
    mText->SetText(text);
    AdjustHeightToFitText();
}

// FUNCTION: WON 0x006fe8a0
void MSButtonBase::SetTransparent(bool transparent) {
    mIsTransparent=transparent;
    if(transparent) SetComponentFlags(0x100,true);
}

// FUNCTION: WON 0x006fe8c0
void MSButtonBase::Virtual0C(int& width,int& height) {
    width=mText->GetWidth()+mLeftTextPad+mRightTextPad;
    height=mText->GetHeight()+mTopTextPad+mBottomTextPad;
}

// FUNCTION: WON 0x006fe980
void MSButtonBase::AdjustHeightToFitText() {
    if(!mAdjustHeightToFitText) return;
    if(Height()-mTopTextPad-mBottomTextPad!=mText->GetHeight())
        Fn2F4B90(Width(),mText->GetHeight()+mTopTextPad+mBottomTextPad);
}

// FUNCTION: WON 0x006fe9f0
void MSButtonBase::SetAdjustHeightToFitText(bool adjust) {
    if(adjust==mAdjustHeightToFitText) return;
    mAdjustHeightToFitText=adjust;
    AdjustHeightToFitText();
}

// FUNCTION: WON 0x006fea10
void MSButtonBase::Virtual08() {
    Component::Virtual08();
    mText->SetMaxWidth(Width()-mLeftTextPad-mRightTextPad);
    AdjustHeightToFitText();
}

// FUNCTION: WON 0x006fea40
void MSButtonBase::DrawButtonText(Graphics& g,int xoff,int yoff) {
    int textWidth=mText->GetWidth();
    int textHeight=mText->GetHeight();
    int width=Width()-mLeftTextPad-mRightTextPad;
    int height=Height()-mTopTextPad-mBottomTextPad;
    int x,y;
    switch(mHAlign) {
    case HorzAlign_Center: x=(width-textWidth)/2;break;
    case HorzAlign_Right: x=width-textWidth;break;
    default:x=0;break;
    }
    switch(mVAlign) {
    case VertAlign_Center:y=(height-textHeight)/2;break;
    case VertAlign_Bottom:y=height-textHeight;break;
    default:y=0;break;
    }
    x+=xoff+mLeftTextPad;
    y+=yoff+mTopTextPad;
    if(IsDisabled()) {
        g.SetSysColor(StandardColor_3DHilight);
        mText->Paint(g,x+1,y+1);
        g.SetSysColor(StandardColor_GrayText);
    } else if(mTextColor<0) g.SetSysColor(mStandardTextColor);
    else g.SetColor(mTextColor);
    mText->Paint(g,x,y);
    if(HasFocus()) {
        Container* parent=mParent;
        if(parent!=NULL && parent->mHasUsedDialogKeys) {
            if(mTextColor<0) g.SetSysColor(mStandardTextColor);
            else g.SetColor(mTextColor);
            switch(mFocusStyle) {
            case FocusStyle_AroundWindow:g.DrawDottedRect(4,3,Width()-8,Height()-7);break;
            case FocusStyle_AroundText:g.DrawDottedRect(x-1,y-1,textWidth+2,textHeight+2);break;
            }
        }
    }
}

// Actual complete MSButtonBase5c lifetime, F contract authority required.
// FUNCTION: WON 0x006fe6e0
MSButtonBase::MSButtonBase() {
    mText=new WrappedText;
    mText->SetWrap(false);
    mText->SetFont(GetNamedFont("MSButton"));
    mStandardTextColor=StandardColor_ButtonText;
    mTextColor=-1;
    mHAlign=HorzAlign_Center;
    mVAlign=VertAlign_Center;
    mFocusStyle=FocusStyle_AroundWindow;
    mTopTextPad=mBottomTextPad=mLeftTextPad=mRightTextPad=2;
    mIsTransparent=false;
    mAdjustHeightToFitText=false;
}
// Native original owner cleanup; actual automatic WrappedTextSP44 thenButton base, support0.
// FUNCTION: WON 0x006fe910
MSButtonBase::~MSButtonBase() {}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006fe860
// (public: virtual void * __thiscall WONAPI::MSButtonBase::`scalar deleting destructor'(unsigned int)    [??_GMSButtonBase@WONAPI@@UAEPAXI@Z])
