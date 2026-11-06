// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "ee/wonapi/EELobbySound.h"
#include "ee/wonapi/WONLobbyImageButton.h"
#include "ee/wonapi/WONLobbyEvents.h"
using namespace WONAPI;
namespace WONAPI {
// Actual native RTTI hierarchy; complete real owner fields68/70/80.
/* HISTORICAL_CLASS_DECLARATIONS_MOVED_TO_WONLobbyCrossPromotion_H
class ImageTextButton : public ImageButton {
public:
    ImageTextButton(const GUIString&);
    virtual void Paint(Graphics&);
    virtual bool VirtualAC();
    SmartPtr<Font> mFont; //5c
    GUIString mText; //60
    GUIString mBrowse; //64
};
// ImageTextButtonConfig declaration hoisted to WONLobbyControlConfig.h; existing bodies unchanged.
class CrossPromotionButton : public ImageTextButton {
public:
    CrossPromotionButton(); //3c96a0 actual complete70 owner
    void Fn3C98B0(SetCrossPromotionEvent*);
    void Fn3C9A30(Image*);
    virtual void HandleComponentEvent(ComponentEvent*);
    SmartPtr<Image> mDefaultImage; //68
    GUIString mDefaultBrowse; //6c
};
*/
// CrossPromotionButtonConfig declaration hoisted to WONLobbyControlConfig.h; existing bodies unchanged.
// FUNCTION: WON 0x007c9080
ImageTextButton::ImageTextButton(const GUIString& text) {
    SetComponentFlags(0x100,true);
    mFont=Fn2F5B20();
    mText=text;
}
// FUNCTION: WON 0x007c9280
void ImageTextButton::Paint(Graphics& g) {
    ImageButton::Paint(g);
    int down=(mButtonFlags>>5)&1;
    g.SetFont(mFont.get());
    g.SetColor(0);
    g.DrawString(mText,30+down,(Height()-mFont->GetHeight()-4)/2+down);
}
// FUNCTION: WON 0x007c92e0
bool ImageTextButton::VirtualAC() {
    if(mBrowse.empty()) return true;
    Browse(mBrowse);
    return false;
}
// FUNCTION: WON 0x007c98b0
void CrossPromotionButton::Fn3C98B0(SetCrossPromotionEvent* event) {
    if(event->mImage) {
        mImages[ImageButtonType_Normal]=event->mImage;
        mBrowse=GUIString(new GUIStringData(event->mLink));
    } else {
        mImages[ImageButtonType_Normal]=mDefaultImage;
        mBrowse=mDefaultBrowse;
    }
    Invalidate();
}
// FUNCTION: WON 0x007c9a30
void CrossPromotionButton::Fn3C9A30(Image* image) {
    mDefaultImage=image;
    if(!mImages[ImageButtonType_Normal].get())
        mImages[ImageButtonType_Normal]=mDefaultImage;
}
// FUNCTION: WON 0x007c9ad0
void CrossPromotionButton::HandleComponentEvent(ComponentEvent* event) {
    if(event->mType==100049) Fn3C98B0(static_cast<SetCrossPromotionEvent*>(event));
    else Component::HandleComponentEvent(event);
}
// FUNCTION: WON 0x007c9af0
bool CrossPromotionButtonConfig::Virtual24(const std::string& key) {
    if(key=="DEFAULTIMAGE")
        mCrossPromotionButton->Fn3C9A30(ReadImage(false));
    else if(key=="DEFAULTBROWSE")
        mCrossPromotionButton->mDefaultBrowse=Fn3842F0(false);
    else
        return ImageButtonConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x00765980
void CrossPromotionButtonConfig::Virtual34(Component* component) {
    ImageButtonConfig::Virtual34(component);
    mCrossPromotionButton=static_cast<CrossPromotionButton*>(component);
}

// FUNCTION: WON 0x007c9380
bool ImageTextButtonConfig::Virtual24(const std::string& key) {
    if(key=="FONT")
        mImageTextButton->mFont=ReadFont();
    else if(key=="TEXT")
        mImageTextButton->mText=Fn3842F0(false);
    else if(key=="BROWSELOCATION")
        mImageTextButton->mBrowse=Fn3842F0(false);
    else
        return ImageButtonConfig::Virtual24(key);
    return true;
}
// Actual physical ICF365980 shared with CrossPromotionButtonConfig setter; no duplicate quota.
void ImageTextButtonConfig::Virtual34(Component* component) {
    ImageButtonConfig::Virtual34(component);
    mImageTextButton=static_cast<ImageTextButton*>(component);
}
// FUNCTION: WON 0x007c96a0
CrossPromotionButton::CrossPromotionButton()
    : ImageTextButton(GUIString(new GUIStringData("")))
{
    Fn2F5360(true);
}

}

namespace WONAPI {
// FUNCTION: WON 0x00765700
ComponentConfig* ImageTextButtonConfig::CfgFactory()
{
    return new ImageTextButtonConfig;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00765840
ComponentConfig* MaxSplitterButtonConfig::CfgFactory()
{
    return new MaxSplitterButtonConfig;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00765920
ComponentConfig* CrossPromotionButtonConfig::CfgFactory()
{
    return new CrossPromotionButtonConfig;
}
}


namespace WONAPI {
// FUNCTION: WON 0x007c9590
MaxSplitterButton::MaxSplitterButton(Direction direction) : MSArrowButton(direction)
{
    Fn2F4B90(15,15);
    Fn2F3370(2,false);
}

// FUNCTION: WON 0x007c9620
void MaxSplitterButton::Paint(Graphics& g)
{
    MSArrowButton::Paint(g);
    if(IsChecked()) {
        int x=Width()/2-3,y=Height()/2-2;
        if(IsPressed()) {
            ++x;
            ++y;
        }
        g.SetSysColor(5);
        g.FillRect(x,y,7,4);
    }
}

// FUNCTION: WON 0x007c9680
bool MaxSplitterButtonConfig::Virtual24(const std::string& instruction)
{
    if(MSArrowButtonConfig::Virtual24(instruction))
        return true;
    return false;
}

// FUNCTION: WON 0x007658a0
void MaxSplitterButtonConfig::Virtual34(Component* component)
{
    MSArrowButtonConfig::Virtual34(component);
    mButton=static_cast<MaxSplitterButton*>(component);
}

// FUNCTION: WON 0x007658c0
Component* MaxSplitterButtonConfig::CompFactory()
{
    return new MaxSplitterButton;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c91d0
// (public: virtual __thiscall WONAPI::ImageTextButton::~ImageTextButton(void)    [??1ImageTextButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c9610
// (public: virtual __thiscall WONAPI::MaxSplitterButton::~MaxSplitterButton(void)    [??1MaxSplitterButton@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007c97b0
// (public: virtual __thiscall WONAPI::CrossPromotionButton::~CrossPromotionButton(void)    [??1CrossPromotionButton@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c95f0
// (public: virtual void * __thiscall WONAPI::MaxSplitterButton::`scalar deleting destructor'(unsigned int)    [??_GMaxSplitterButton@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c91b0
// (public: virtual void * __thiscall WONAPI::ImageTextButton::`scalar deleting destructor'(unsigned int)    [??_GImageTextButton@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007c9790
// (public: virtual void * __thiscall WONAPI::CrossPromotionButton::`scalar deleting destructor'(unsigned int)    [??_GCrossPromotionButton@WONAPI@@UAEPAX)
