// Text segment metrics and selection on original-owned rich text objects.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <cwctype>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextSegment.h"
#include "ee/wonapi/WONLobbyTextWrap.h"
namespace WONAPI {

// FUNCTION: WON 0x00709E70
int TextLineSegment::GetMinWidth()
{
	if(mMinWidth==0)
	{
		int aWidth = 0;
		int aPos = 0;
		while(aPos<mText.length())
		{
			int aChar = mText[aPos];
			if(aChar==' ')
			{
				if(aWidth>mMinWidth)
					mMinWidth = aWidth;

				aWidth = 0;
			}
			else
				aWidth += mFont->Virtual10(aChar);

			aPos++;
		}

		if(mMinWidth==0)  // no space found
			mMinWidth = aWidth;
	}

	return mMinWidth;
}

// Support0 cached getter / forwarding overload.
// FUNCTION: WON 0x00709ed0
int TextLineSegment::GetWidth()
{
	if(mWidth==0)
		mWidth = mFont->GetStringWidth(mText);

	return mWidth;
}

// FUNCTION: WON 0x00709F00
int TextLineSegment::GetFirstWrapWidth()
{
	int aWidth = 0;
	int aPos = 0;
	while(aPos<mText.length())
	{
		int aChar = mText[aPos];
		if(aChar==' ')
			return aWidth;
		else
			aWidth += mFont->Virtual10(aChar);

		aPos++;
	}

	return aWidth;
}

// FUNCTION: WON 0x00709F40
int TextLineSegment::GetSelChar(int x, int y, int theStartChar, int theEndChar)
{
	int aWidth = 0;
	int aPos = theStartChar;
	while(aPos<theEndChar)
	{
		int aCharWidth = mFont->Virtual10(mText[aPos]);
		if(aWidth + (aCharWidth>>1) >= x)
			break;

		aWidth += aCharWidth;
		aPos++;
	}

	return aPos;

}

// Support0 cached getter / forwarding overload.
int TextLineSegment::GetSelChar(int x, int y)
{
	return GetSelChar(x,y,0,mText.length());
}

// FUNCTION: WON 0x00709FC0
void TextLineSegment::GetSelText(GUIString &theText, ElementSelectionInfo &theInfo)
{
	if(mStartSelIndex<theInfo.mStartSelSegment || mStartSelIndex>theInfo.mEndSelSegment)
		return;

	int aStartPos = 0;
	int anEndPos = mText.length();
	bool partial = false;
	if(theInfo.mStartSelSegment==mStartSelIndex)
	{
		aStartPos = theInfo.mStartSelSegmentPos;
		partial = true;
	}
	if(theInfo.mEndSelSegment==mStartSelIndex)
	{
		anEndPos = theInfo.mEndSelSegmentPos;
		partial = true;
	}

	if(partial)
		theText.append(mText.substr(aStartPos,anEndPos-aStartPos));
	else
		theText.append(mText);
}

// FUNCTION: WON 0x0070A260
bool TextLineSegment::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	if(theSelIndex!=mStartSelIndex)
		return false;

	if(theSelChar<0 || theSelChar>mText.length())
		return false;

	x = mAbsX + mFont->GetStringWidth(mText,0,theSelChar);
	y = mAbsY;
	return true;
}

// FUNCTION: WON 0x0070A320
bool TextLineSegment::FindPartial(ElementFindInfo &theInfo, int startChar, int endChar)
{
	int p1 = startChar, p2 = theInfo.mCurFoundLength;
	const GUIString &aText = theInfo.mFindText;
	int len2 = aText.length();
	const bool caseSensitive = theInfo.mCaseSensitive;

	while(p1<endChar && p2<len2)
	{
		if(caseSensitive)
		{
			if(mText[p1]!=aText[p2])
				break;
		}
		else if(towupper(mText[p1])!=towupper(aText[p2]))
			break;

		p1++; p2++;
	}

	if(p1<endChar && p2<len2)
		return false;
	else
	{
		theInfo.mCurFoundLength = p2;
		theInfo.mFoundEndSegment = mStartSelIndex;
		theInfo.mFoundEndChar = p1;
		return true;
	}
}


// Support0 virtual forwarding Paint overload30a900.
// FUNCTION: WON 0x0070a900
void TextLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	Paint(g,x,y,mStartSelIndex,0,mText.length(),GetWidth(),GetHeight(),theInfo);
}

// FUNCTION: WON 0x0070A950
void TextLineSegment::Paint(Graphics &g, int x, int y, int theSelIndex, int startChar, int endChar, int theWidth, int theHeight, ElementPaintInfo &theInfo)
{
	g.SetFont(mFont.get());
	if(theSelIndex>=theInfo.mStartSelSegment && theSelIndex<=theInfo.mEndSelSegment)
	{
		int aStartSelPos = startChar;
		int anEndSelPos = endChar;
		bool fullSelect = true;
		if(theSelIndex==theInfo.mStartSelSegment)
		{
			fullSelect = false;
			aStartSelPos = theInfo.mStartSelSegmentPos;
			if(aStartSelPos<startChar)
				aStartSelPos = startChar;
		}
		if(theSelIndex==theInfo.mEndSelSegment)
		{
			fullSelect = false;
			anEndSelPos = theInfo.mEndSelSegmentPos;
			if(anEndSelPos>endChar)
				anEndSelPos = endChar;
		}

		if(fullSelect)
		{
			g.SetSysColor(7);
			g.FillRect(x,theInfo.mLineY,theWidth,theInfo.mLineHeight);
			g.SetSysColor(8);
			g.DrawString(mText,startChar,endChar-startChar,x,y);
			return;
		}
		else if(aStartSelPos<anEndSelPos)
		{
			int aStartWidth = mFont->GetStringWidth(mText,startChar,aStartSelPos-startChar);
			int aSelWidth = mFont->GetStringWidth(mText,aStartSelPos,anEndSelPos-aStartSelPos);
			g.SetColor(mColor);
			g.DrawString(mText,startChar,aStartSelPos-startChar,x,y);
			g.DrawString(mText,anEndSelPos,endChar-anEndSelPos,x+aStartWidth+aSelWidth,y);
			g.SetSysColor(7);
			g.FillRect(x+aStartWidth,theInfo.mLineY,aSelWidth,theInfo.mLineHeight);
			g.SetSysColor(8);
			g.DrawString(mText,aStartSelPos,anEndSelPos-aStartSelPos,x+aStartWidth,y);
			return;
		}
	}

	g.SetColor(mColor);
	g.DrawString(mText,startChar,endChar-startChar,x,y);
}


// FUNCTION: WON 0x0070A590
void TextLineSegment::Wrap(MultiLineBuilder *theContainer, bool needSegmentOnFirstLine)
{
	int aCurPos = 0;
	const GUIString &aStr = mText;
	Font *aFM = mFont.get();
	int aTextBoxWidth = theContainer->GetCurMaxLineWidth() - theContainer->CurLine()->GetWidth();
	
	int aWidth = 0;
	int aBreakWidth = 0;

	while(aCurPos < aStr.length())
	{
		int aStartPos = aCurPos;
		int aBreakPos = aStartPos-1;
		while(aCurPos < aStr.length())
		{
			int aChar = aStr[aCurPos];
			if(aChar==' ') 
			{
				aBreakPos = aCurPos;
				aBreakWidth = aWidth;
			}

			aWidth += aFM->Virtual10(aChar);

			if(aWidth>aTextBoxWidth && (aBreakPos>=aStartPos || !needSegmentOnFirstLine))
				break;

			aCurPos++;
		}

		if(aCurPos==aStr.length())
		{
			aBreakPos = aStr.length();
			aBreakWidth = aWidth;
		}
	
		needSegmentOnFirstLine = true;
		if(aBreakPos<aStartPos) // didn't find break
		{
			theContainer->AddLine();
			aCurPos = aStartPos;
			aTextBoxWidth = theContainer->GetCurMaxLineWidth();
			aWidth = aBreakWidth = 0;
			continue;
		}

		theContainer->AddSegmentToCurLine(new TextPieceLineSegment(this, aStartPos, aBreakPos, aBreakWidth));
		aCurPos = aBreakPos + 1; // Skip the space where we wrapped
		if(aCurPos>=aStr.length())
			break;

		theContainer->AddLine();
		aTextBoxWidth = theContainer->GetCurMaxLineWidth();
		aWidth = aFM->GetStringWidth(aStr,aCurPos, aStr.length()-aCurPos);
		if(aWidth <= aTextBoxWidth)
		{
			theContainer->AddSegmentToCurLine(new TextPieceLineSegment(this, aCurPos, aStr.length(), aWidth));
			break;
		}
		
		aWidth = aBreakWidth = 0;		
	}
}

// FUNCTION: WON 0x0070A2C0
bool TextPieceLineSegment::GetSelCharPos(int &x, int &y, int theSelIndex, int theSelChar)
{
	if(theSelIndex!=mStartSelIndex)
		return false;

	if(theSelChar<mStartChar || theSelChar>mEndChar)
		return false;

	x = mAbsX + mSegment->mFont->GetStringWidth(mSegment->mText,mStartChar,theSelChar-mStartChar);
	y = mAbsY;
	return true;
}

// Support0 direct forwarding virtuals on the actual borrowed text piece.
int TextPieceLineSegment::GetSelChar(int x,int y) { return mSegment->GetSelChar(x,y,mStartChar,mEndChar); }
void TextPieceLineSegment::Paint(Graphics& g,int x,int y,ElementPaintInfo& info) {
    mSegment->Paint(g,x,y,mStartSelIndex,mStartChar,mEndChar,GetWidth(),GetHeight(),info);
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK nonvirtual background-only selection algorithm.
// Existing target Paint and virtual slots remain unchanged; no wrapper/provider forcing.
void TextLineSegment::PaintSelection(Graphics &g, int x, int y, int theSelIndex, int startChar, int endChar, int theWidth, int theHeight, ElementPaintInfo &theInfo)
{
	g.SetFont(mFont.get());
	if(theSelIndex>=theInfo.mStartSelSegment && theSelIndex<=theInfo.mEndSelSegment)
	{
		int aStartSelPos = startChar;
		int anEndSelPos = endChar;
		bool fullSelect = true;
		if(theSelIndex==theInfo.mStartSelSegment)
		{
			fullSelect = false;
			aStartSelPos = theInfo.mStartSelSegmentPos;
			if(aStartSelPos<startChar)
				aStartSelPos = startChar;
		}
		if(theSelIndex==theInfo.mEndSelSegment)
		{
			fullSelect = false;
			anEndSelPos = theInfo.mEndSelSegmentPos;
			if(anEndSelPos>endChar)
				anEndSelPos = endChar;
		}

		if(fullSelect)
		{
			g.SetSysColor(7);
			g.FillRect(x,theInfo.mLineY,theWidth,theInfo.mLineHeight);
//			g.ApplyColorSchemeColor(StandardColor_HilightText);
//			g.DrawString(mText,startChar,endChar-startChar,x,y);
			return;
		}
		else if(aStartSelPos<anEndSelPos)
		{
			int aStartWidth = mFont->GetStringWidth(mText,startChar,aStartSelPos-startChar);
			int aSelWidth = mFont->GetStringWidth(mText,aStartSelPos,anEndSelPos-aStartSelPos);
//			g.SetColor(mColor);
//			g.DrawString(mText,startChar,aStartSelPos-startChar,x,y);
//			g.DrawString(mText,anEndSelPos,endChar-anEndSelPos,x+aStartWidth+aSelWidth,y);
			g.SetSysColor(7);
			g.FillRect(x+aStartWidth,theInfo.mLineY,aSelWidth,theInfo.mLineHeight);
//			g.ApplyColorSchemeColor(StandardColor_HilightText);
//			g.DrawString(mText,aStartSelPos,anEndSelPos-aStartSelPos,x+aStartWidth,y);
			return;
		}
	}

//	g.SetColor(mColor);
//	g.DrawString(mText,startChar,endChar-startChar,x,y);
}
}
// TEMPLATE: WON 0x0070a7d0
// (LineSegment::SetSelIndex, header inline; out-of-line copy for the vtable)
