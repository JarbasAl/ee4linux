// HTML input controls and select completion use actual original widget owners.
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
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
#include "ee/wonapi/WONLobbyHTMLImageSegments.h"
#include "ee/wonapi/WONLobbyImageButton.h"
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyMSBorderPainters.h"
namespace WONAPI {
// FUNCTION: WON 0x0073b8b0
void HTMLDisplayGen::HandleInput(HTMLInputTag* tag)
{
    if(tag->IsEnd()) {
        if(mCurCombo.get()!=NULL) {
            AddDisplaySegment(new ComponentLineSegment(mCurCombo.get()));
            mCanBreak=false;
            mLastWasSpace=false;
            mCurCombo=NULL;
        }
        return;
    }
    SmartPtr<LineSegment> segment;
    SmartPtr<Font> font=mWindow->GetFont(FontDescriptor("Tahoma",FontStyle_Plain,10));
    switch(tag->mInputType) {
    case HTMLInputType_Button:
    case HTMLInputType_Submit:
    case HTMLInputType_Reset:
        {
            SmartPtr<MSButton> button=new MSButton(tag->mValue);
            button->SetFont(font.get());
            button->Fn2F4E90(0,0);
            segment=new ComponentLineSegment(button.get());
            break;
        }
    case HTMLInputType_Radio:
        {
            SmartPtr<MSRadioButton> radio=new MSRadioButton;
            radio->SetTransparent(true);
            radio->Fn2F4B90(15,12);
            radio->Fn2F3370(8,false);
            segment=new ComponentLineSegment(radio.get());
            break;
        }
    case HTMLInputType_Image:
        {
            Image* image=tag->mImage.get();
            if(image==NULL)
                break;
            SmartPtr<ImageButton> button=new ImageButton;
            button->Fn2F4B90(image->mWidth,image->mHeight);
            button->SetImage(ImageButtonType_Normal,image);
            button->SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_Hand));
            segment=new ComponentLineSegment(button.get());
            break;
        }
    case HTMLInputType_Checkbox:
    case HTMLInputType_File:
    case HTMLInputType_Hidden:
        break;
    case HTMLInputType_Select:
        mCurCombo=new MSComboBox;
        mCurCombo->Fn2F4B90(50,22);
        break;
    case HTMLInputType_Text:
    case HTMLInputType_Password:
        {
            SmartPtr<MSInputBox> input=new MSInputBox;
            input->mFont=font;
            if(tag->mInputType==HTMLInputType_Password)
                input->SetPasswordChar('*');
            else if(tag->mReadOnly)
                input->SetComponentFlags(8,false);
            input->Fn2F4B90(tag->mSize*6+24,22);
            input->mMaxChars=tag->mMaxLength;
            input->SetText(tag->mValue,true);
            segment=new ComponentLineSegment(input.get());
            break;
        }
    }
    if(segment.get()!=NULL) {
        AddDisplaySegment(segment.get());
        mCanBreak=false;
        mLastWasSpace=false;
    }
}
}

// FUNCTION: WON 0x0073bea0
// (??_GMSInputBox@WONAPI@@UAEPAXI@Z)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0073bec0
// (public: virtual __thiscall WONAPI::MSInputBox::~MSInputBox(void))
