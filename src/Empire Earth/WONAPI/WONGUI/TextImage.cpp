// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: same-SDK inline text image selection/render/copy behavior.
#include <windows.h>
#include <string>
#include <list>
#include <deque>
#include <vector>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextImage.h"
namespace WONAPI {
void TextImage::Paint(Graphics &g, TextArea *theTextArea)
{
	if(theTextArea->mSelStartChar<=mAbsStartChar && theTextArea->mSelEndChar>=mAbsEndChar)
	{
		g.SetColor(theTextArea->mSelColor.GetBackgroundColor(g));
		g.FillRect(mx,0,mWidth,mLine->mHeight);
	}

	g.DrawImage(mImage,mx,my);
}

void TextImage::CalcDimensions()
{
	mWidth = mImage->mWidth;
	mHeight = mImage->mHeight;
	mAscent = mHeight;
}

void TextImage::GetSelText(TextArea *theTextArea, GUIString &theStr)
{
	if(theTextArea->mSelStartChar<=mAbsStartChar && theTextArea->mSelEndChar>=mAbsEndChar)
	{
		theStr.append(mText);
		if(mLineBreak && theTextArea->mSelEndChar>mAbsEndChar)
			theStr.append("\r\n");
	}
}
}
