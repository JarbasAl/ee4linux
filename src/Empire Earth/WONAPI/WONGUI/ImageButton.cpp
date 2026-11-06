// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/EELobbySound.h"
#include "ee/wonapi/WONLobbyImageButton.h"
using namespace WONAPI;
// FUNCTION: WON 0x006f7d70
ImageButton::ImageButton() {
    Fn2F3370(1,true);
    SetComponentFlags(8,true);
    SetComponentFlags(0x4000,false);
    mStretch=false;
}
// FUNCTION: WON 0x006f7e20
ImageButton::~ImageButton() {}
// FUNCTION: WON 0x006f7e80
void ImageButton::Virtual0C(int& outWidth,int& outHeight) {
    int width=0,height=0;
    for(int i=0;i<5;++i) {
        Image* image=mImages[i].get();
        if(image) {
            if(image->mWidth>width) width=image->mWidth;
            if(image->mHeight>height) height=image->mHeight;
        }
    }
    outWidth=width;
    outHeight=height;
}
// FUNCTION: WON 0x006f7ed0
void ImageButton::Paint(Graphics& g) {
    Component::Paint(g);
    Image* image=NULL;
    if(IsDisabled()) image=mImages[ImageButtonType_Disabled].get();
    else if(IsChecked() && mImages[ImageButtonType_Checked].get())
        image=mImages[ImageButtonType_Checked].get();
    else if(IsPressed()) image=mImages[ImageButtonType_Down].get();
    else if(IsMouseOver()) image=mImages[ImageButtonType_Over].get();
    if(!image) image=mImages[ImageButtonType_Normal].get();
    if(!image) return;
    if(mStretch && (image->mWidth!=Width() || image->mHeight!=Height()))
        image->SetSize(Width(),Height());
    g.DrawImage(image,0,0);
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f7e00
// (protected: virtual void * __thiscall WONAPI::ImageButton::`scalar deleting destructor'(unsigned int)    [??_GImageButton@WONAPI@@MAEPAXI@Z])
