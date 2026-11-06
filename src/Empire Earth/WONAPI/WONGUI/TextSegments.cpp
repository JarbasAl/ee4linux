// Native text segment selection, wrapping, painting and hit position algorithms.
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
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00710aa0
void TextSegmentBase::Wrap(TextArea *theTextArea, TextLine *theLine)
{
	if(!theLine->mSegmentList.empty())
		theLine = theTextArea->AddLine();

	theLine->mWidth = theTextArea->mWrapIndentSize;
	theTextArea->AddSeg(this);
}

// FUNCTION: WON 0x00710ae0
void TextSegmentBase::GetSelText(TextArea *theTextArea, GUIString &theStr)
{
	if(mLineBreak && theTextArea->mSelStartChar<=mAbsStartChar && theTextArea->mSelEndChar>mAbsEndChar)
		theStr.append("\r\n");
}

// FUNCTION: WON 0x00710d70
void TextSegment::CalcDimensions()
{
	mWidth = mFont->GetStringWidth(mText);
	mHeight = mFont->GetHeight();
	mAscent = mFont->GetAscent();
}

// FUNCTION: WON 0x710db0
void TextSegment::GetSelText(TextArea *theTextArea, GUIString &theStr)
{
	int aSelStartChar = theTextArea->mSelStartChar;
	int aSelEndChar = theTextArea->mSelEndChar;

	if(mAbsStartChar > aSelStartChar)
		aSelStartChar = mAbsStartChar;

	if(mAbsEndChar < aSelEndChar)
		aSelEndChar = mAbsEndChar;

	if(aSelStartChar >= aSelEndChar)
		return;

	aSelStartChar -= mAbsStartChar;
	aSelEndChar -= mAbsStartChar;

	theStr.append(mText, aSelStartChar, aSelEndChar-aSelStartChar);
	if(mLineBreak && theTextArea->mSelEndChar>mAbsEndChar)
		theStr.append("\r\n");
}

// FUNCTION: WON 0x710fb0
void TextSegment::Wrap(TextArea *theTextArea, TextLine *theLine)
{
	mLine = NULL; // set mLine to NULL then reset it to aLine on the first line to which we add a segment piece

	int aCurPos = 0;
	GUIString &aStr = mText;
	Font *aFM = mFont.get();
	int aTextBoxWidth = theTextArea->mScrollArea.width; 
	TextLine *aLine = theLine;

	while(aCurPos < aStr.length())
	{
		int aWidth = aLine->mWidth;
		int aBreakWidth = aLine->mWidth;
		int aStartPos = aCurPos;
		int aBreakPos = aStartPos-1;
		while(aCurPos < aStr.length())
		{
			int aChar = aStr[aCurPos];
			aWidth += aFM->Virtual10(aChar);
			if(aChar==' ') 
			{
				aBreakPos = aCurPos;
				aBreakWidth = aWidth;
			}

			if(aWidth > aTextBoxWidth)
				break;

			aCurPos++;
		}

		bool forceBreak = false;
		if(aBreakPos < aStartPos)
		{
			if(aWidth < aTextBoxWidth)
				aBreakPos = aStr.length(); 
			else if(aLine->mSegmentList.empty()) // force the break in the middle of the word if we're at the beginning of the line
			{
				aBreakPos = aCurPos;
				aBreakWidth = aTextBoxWidth;
				if(aBreakPos==aStartPos)	// need at least one character per line
					aBreakPos++;

				forceBreak = true;
			}
			else	// otherwise try again on new line
				aCurPos = 0;
		}

		if(aBreakPos >= aStartPos)
		{
			if(aBreakPos!=aStartPos)
			{
				theTextArea->AddSeg(new TextSegmentPiece(this, aStartPos, aBreakPos, aBreakWidth/*aTextBoxWidth-aLine->mWidth*/));
				if(mLine==NULL)
					mLine = aLine;
			}

			aCurPos = aBreakPos;
			if(!forceBreak)	// Skip the space where we wrapped
				aCurPos++;
		}

		if(aCurPos>=aStr.length())
			break;

		aLine = theTextArea->AddLine(mHeight, mAscent);
		if(theTextArea->mLineAlign==HorzAlign_Left)
			aLine->mWidth = theTextArea->mWrapIndentSize;

		aWidth = aFM->GetStringWidth(aStr, aCurPos, aStr.length()-aCurPos);
		if(aWidth <= aTextBoxWidth - aLine->mWidth)
		{
			theTextArea->AddSeg(new TextSegmentPiece(this, aCurPos, aStr.length(), aWidth));
			if(mLine==NULL)
				mLine = aLine;

			break;
		}
	}
}

// FUNCTION: WON 0x711200
void TextSegment::Paint(Graphics &g, TextArea *theTextArea, int theStartChar, int theEndChar, int xpos, int ypos, int lineHeight)
{
	int aSelStartChar = theTextArea->mSelStartChar - mAbsStartChar;
	int aSelEndChar = theTextArea->mSelEndChar - mAbsStartChar;

	if(theStartChar > aSelStartChar)
		aSelStartChar = theStartChar;

	if(theEndChar < aSelEndChar)
		aSelEndChar = theEndChar;

	g.SetColor(mColor);	
	g.SetFont(mFont.get());
	
	bool fullSelection = aSelStartChar==theStartChar && aSelEndChar==theEndChar;
	if(!fullSelection)
		g.DrawString(mText, theStartChar, theEndChar - theStartChar, xpos, ypos);

	if(aSelEndChar > aSelStartChar)
	{
		int x = xpos + mFont->GetStringWidth(mText,theStartChar,aSelStartChar-theStartChar);
		int aWidth = mFont->GetStringWidth(mText,aSelStartChar,aSelEndChar-aSelStartChar);

		g.SetColor(theTextArea->mSelColor.GetBackgroundColor(g));
		g.FillRect(x,0,aWidth,lineHeight);
		
		g.SetColor(theTextArea->mSelColor.GetForegroundColor(g,mColor));
		g.DrawString(mText,aSelStartChar,aSelEndChar-aSelStartChar,x,ypos);
	}
}

// FUNCTION: WON 0x00711330
void TextSegment::Paint(Graphics &g, TextArea *theTextArea)
{
	Paint(g,theTextArea,0,mText.length(),mx,my,mLine->mHeight);
}

// FUNCTION: WON 0x711360
int TextSegment::CharAt(int theWidth, int theStartChar, int theEndChar)
{
	int aPos = theStartChar;

	int aWidth = 0;
	int aCharWidth = 0;
	while(aPos<theEndChar && aWidth<theWidth)
	{
		aCharWidth = mFont->Virtual10(mText[aPos]);
		aWidth+=aCharWidth;
		aPos++;
	}

	if(theWidth>=0 && theWidth<aWidth-aCharWidth/2)
		aPos--;

	return aPos + mAbsStartChar;	
}

// FUNCTION: WON 0x007113c0
int TextSegment::CharAt(int x)
{
	return CharAt(x-mx, 0, mText.length());
}

// FUNCTION: WON 0x007114a0
void TextSegmentPiece::Paint(Graphics &g, TextArea *theTextArea)
{
	mSeg->Paint(g,theTextArea,GetStartChar(),GetEndChar(),mx,my,mLine->mHeight);
}

// FUNCTION: WON 0x007114d0
int TextSegmentPiece::CharAt(int x)
{
	return mSeg->CharAt(x-mx,GetStartChar(),GetEndChar());
}
}
