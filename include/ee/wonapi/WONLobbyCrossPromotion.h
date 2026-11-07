// Complete existing SDK widget owners moved from CrossPromotionButton.cpp; no duplicate class or field/lifetime change.
#ifndef EE_WONAPI_WONLOBBYCROSSPROMOTION_H
#define EE_WONAPI_WONLOBBYCROSSPROMOTION_H
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyImageButton.h"
namespace WONAPI {
class SetCrossPromotionEvent;
class ImageTextButton : public ImageButton {
public:
    ImageTextButton(const GUIString& = "");
    virtual void Paint(Graphics&);
    virtual bool VirtualAC();
    SmartPtr<Font> mFont; //5c
    GUIString mText; //60
    GUIString mBrowse; //64
};
class CrossPromotionButton : public ImageTextButton {
public:
    CrossPromotionButton(); //3c96a0 actual complete70 owner
    void Fn3C98B0(SetCrossPromotionEvent*);
    void Fn3C9A30(Image*);
    virtual void HandleComponentEvent(ComponentEvent*);
    SmartPtr<Image> mDefaultImage; //68
    void SetDefaultBrowse(const GUIString& inBrowse) { mDefaultBrowse=inBrowse; }
    GUIString mDefaultBrowse; //6c
};
}
#endif
