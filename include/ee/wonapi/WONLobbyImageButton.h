#ifndef EE_WONLOBBYIMAGEBUTTON_H
#define EE_WONLOBBYIMAGEBUTTON_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
enum ImageButtonType { ImageButtonType_Normal, ImageButtonType_Over, ImageButtonType_Down,
    ImageButtonType_Disabled, ImageButtonType_Checked, ImageButtonType_None };
// Actual RTTI ImageButton, complete5c; five real image owners and stretch flag.
class ImageButton : public Button {
protected:
    virtual ~ImageButton(); //2f7e20, explicit native own-vptr destructor
    SmartPtr<Image> mImages[5]; //44..54
    bool mStretch; //58
public:
    ImageButton(); //2f7d70
    void SetImage(ImageButtonType type,Image* image) { mImages[type]=image; } //actual SDK inline, native38ad5d
    virtual void Virtual0C(int&,int&); //2f7e80, GetDesiredSize
    virtual void Paint(Graphics&); //2f7ed0
};
// Borrowed config prefix7c. No config allocation/constructor admission here.
class ImageButtonConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //39ad10 genuine SDK static API, original external provider only
    static Component* CompFactory(); //39acb0 genuine SDK static API, original external provider only
    ImageButtonConfig();
    virtual bool Virtual24(const std::string&); //38acb0
    virtual void Virtual34(Component*); //38f8d0 actual shared body
    ImageButton* mImageButton; //78 borrowed
};
bool Browse(const std::string&); //2a30e0 actual MiscUtil API, original-owned
}
#endif
