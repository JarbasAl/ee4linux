#ifndef EE_WONAPI_WONLOBBYWONBUTTON_H
#define EE_WONAPI_WONLOBBYWONBUTTON_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// Complete actual100 WONButton, vtable4506d4. Reviewed skin allocations use
// its genuine original constructor; no new copy or lifetime source bodies.
class WONButton : public Button {
public:
    struct ButtonState {
        SmartPtr<WrappedText> mText;
        SmartPtr<Image> mImage;
        int mColor; //08
        ButtonState() : mColor(-1) {} //317dd0 genuine default construction before map
    };
    typedef std::map<Font*,SmartPtr<WrappedText> > FontTextMap;
    WONButton(); //317c70 external full100, bounded F allocation
    virtual ~WONButton(); //317f50 external normal/deleting317df0
    virtual void Virtual08(); //318100 actual SizeChanged08
    virtual void Virtual0C(int&,int&); //318640 actual GetDesiredSize0c
    virtual void Paint(Graphics&); //319080 actual18
    virtual void VirtualB0(const GUIString&); //318060 actual SetTextHookb0
    virtual const GUIString& GetText(); //3180e0 original external getterb8
    virtual void GetTextSize(int&,int&); //318580 actual first added slotbc
    void AdjustHeightToFitText(); //3181a0
    void SetAdjustHeightToFitText(bool); //318200 support0
    void SetWrapText(bool); //318220
    void SetTransparent(bool); //3182a0 support0
    void SetButtonState(int,Image*,int,Font*,bool=false); //3182c0
    void SetHorzAlign(HorzAlign); //3186e0
    void SetTextColor(int); //318790
    void SetVertAlign(VertAlign); //318780 actual external lowbyte store
    void SetScaleImage(bool v) { mScaleImage=v; }
    void SetHasCheck(bool v) { mHasCheck=v; } //genuineSDKinline, native3160e9 etc
    void SetPushOffsets(int x,int y) { mPushDX=x; mPushDY=y; } //genuineSDKinline
    void SetDefaultTextColor(int v) { mDefaultTextColor=v; }
    void SetDrawGrayText(bool v) { mDrawGrayText=v; }
    void SetFocusStyle(unsigned char v) { mFocusStyle=v; }
    void SetTextPadding(int l,int t,int r,int b) { mLeftTextPad=l; mTopTextPad=t; mRightTextPad=r; mBottomTextPad=b; }
    void SetFocusPadding(int l,int t,int r,int b) { mLeftFocusPad=l; mTopFocusPad=t; mRightFocusPad=r; mBottomFocusPad=b; }
    int GetRightTextPad() { return mRightTextPad; }
    void SetHorzTextPad(int l,int r) { mLeftTextPad=l; mRightTextPad=r; }
    void SetFont(Font*); //3187e0
    WONButton* Duplicate(); //318d40 genuine SDK raw duplicate provider
    void CopyAttributes(WONButton*); //318970, actual text/image cloning algorithm
    void DrawButtonText(Graphics&,WrappedText*,int); //318db0
    void GetButtonState(Graphics&,int*,WrappedText** =NULL,Image** =NULL); //318f90
    ButtonState mStates[5][2]; //44..bb ten actual0c records
    FontTextMap mFontTextMap; //bc..c7 actual VC6 tree0c
    bool mAdjustHeightToFitText; //c8; natural c9..cb padding
    int mLeftTextPad,mTopTextPad,mRightTextPad,mBottomTextPad; //cc..db
    int mLeftFocusPad,mTopFocusPad,mRightFocusPad,mBottomFocusPad; //dc..eb
    int mPushDX,mPushDY,mDefaultTextColor; //ec,f0,f4
    bool mHasCheck,mScaleImage,mWrapText,mDrawGrayText,mIsTransparent; //f8..fc
    unsigned char mHAlign,mVAlign,mFocusStyle; //fd,fe,ff
};
}
#endif
