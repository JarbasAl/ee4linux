// Original-backed TextListItem behavior; original historical TU grouping remains provisional.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x6fa2c0
void TextListItem::Virtual08(ListArea* inList)
{
    if(mFont.get()==NULL) mFont=inList->mFont;
    if(mL1C==-1) mL1C=inList->GetTextColor();
    mL10=mFont->GetStringWidth(mText)+2;
    mHeight=mFont->GetHeight();
    if(mImage.get()!=NULL) {
        mL10+=mImage->mWidth+2;
        if(mImage->mHeight>mHeight) mHeight=mImage->mHeight;
    }
}
// Native2fa360/19, pure case-insensitive compare forwarding support0.
// FUNCTION: WON 0x006fa360
int TextListItem::Virtual10(const ListItem* inOther,ListArea* inList) const
{
    return mText.CompareNoCase(static_cast<const TextListItem*>(inOther)->mText);
}
// FUNCTION: WON 0x6fa380
void TextListItem::Virtual04(Graphics& inGraphics,ListArea* inList)
{
    unsigned long aColor=mL1C;
    if(mB08) {
        const SelectionColor& aSelection=inList->GetSelColor();
        inGraphics.SetColor(aSelection.GetBackgroundColor(inGraphics));
        int aWidth=inList->mL12C;
        if(!inList->ListFlagSet(0x10)) aWidth=mL10;
        int aX=0;
        Image* anImage=mImage.get();
        if(anImage!=NULL && !inList->ListFlagSet(0x20)) {
            int anImageWidth=anImage->mWidth;
            aX=anImageWidth+2;
            aWidth-=anImageWidth+2;
        }
        inGraphics.FillRect(aX,0,aWidth,mHeight);
        aColor=aSelection.GetForegroundColor(inGraphics,mL1C);
    }
    inGraphics.SetColor(aColor);
    inGraphics.SetFont(mFont.get());
    int aTextX=2;
    Image* anImage=mImage.get();
    if(anImage!=NULL) {
        int anImageY=(mHeight-anImage->mHeight)/2;
        if(anImageY<0) anImageY=0;
        inGraphics.DrawImage(anImage,2,anImageY);
        aTextX=mImage->mWidth+4;
    }
    inGraphics.DrawString(mText,aTextX,(mHeight-mFont->GetHeight())/2);
}
}