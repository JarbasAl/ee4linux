// Original text-segment construction overloads; initialization/forwarding support0.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <deque>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
#include "ee/wonapi/WONLobbyTextImage.h"
namespace WONAPI {
// FUNCTION: WON 0x00713c60
// FUNCTION: WON 0x00713c80
// FUNCTION: WON 0x00713da0
void TextArea::AddSegment(const GUIString& text,bool lineBreak)
{
    AddSegment(text,mTextColor,lineBreak);
}
// SUPPORT-ONLY: EXE 0x00713c80
void TextArea::AddSegment(const GUIString& text,int color,bool lineBreak)
{
    SmartPtr<TextSegment> seg=new TextSegment;
    seg->mText=text;
    seg->mColor=color;
    seg->mFont=mFont;
    seg->mLineBreak=lineBreak;
    AddSegment(seg.get());
}
// SUPPORT-ONLY: EXE 0x00713da0
void TextArea::AddSegment(const GUIString& text,int color,Font* font,bool lineBreak)
{
    SmartPtr<TextSegment> seg=new TextSegment;
    seg->mText=text;
    seg->mColor=color;
    seg->mFont=font;
    seg->mLineBreak=lineBreak;
    AddSegment(seg.get());
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK retained inline-image construction.
void TextArea::AddImage(Image *theImage, const GUIString &theCopyText, bool lineBreak)
{
	SmartPtr<TextImage> aSeg = new TextImage;
	aSeg->mImage = theImage;
	aSeg->mText = theCopyText;
	aSeg->mLineBreak = lineBreak;
	AddSegment(aSeg.get());
}
}
