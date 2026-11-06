// Source-only reconstructed WONGUI WrappedText; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"



using namespace WONAPI;

// FUNCTION: WON 0x0074a7c0
void WrappedText::Init()
{
	mWrap = true;
	mIsCentered = false;
	mWidth = mHeight = mMaxWidth = 0;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074a7e0
WrappedText::WrappedText()
{
	Init();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


// FUNCTION: WON 0x0074a880
// (??_GWrappedText@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0074a8a0
// (??1WrappedText@WONAPI@@UAE@XZ)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074a9b0
void WrappedText::CalcWrap()
{
	if(mFont.get()==NULL)
		return;

	if(mRichText.get()!=NULL)
	{
		if(mWrap)
		{
			if(mMaxWidth==0)
				return;

			mRichText->SetMaxWidth(mMaxWidth);
		}
		else
			mRichText->SetMaxWidth(mRichText->GetMaxWidth()+10);

		mWidth = mRichText->GetWidth();
		mHeight = mRichText->GetHeight();
		return;
	}

	if(!mWrap)
	{
		mWidth = mFont->GetStringWidth(mText,0,mText.length());
		mHeight = mFont->GetHeight();
		return;
	}

	if(mMaxWidth==0)
		return;

	mBreakList.clear();

	int aBreakPos = -1, aBreakWidth = 0;
	int aPos = 0, aWidth = 0, aMaxWidth = 0;
	while(aPos<mText.length())
	{
		int aChar = mText[aPos];
		int aCharWidth = mFont->Virtual10(aChar);
		if(aChar==' ')
		{
			aBreakPos = aPos;
			aBreakWidth = aWidth;
		}
		else if(aChar=='\r' || aChar=='\n')
		{
			aBreakPos = aPos;
			aBreakWidth = aWidth;
			if(aChar=='\r' && mText[aPos+1]=='\n')
				aPos++;

			aPos++;
			mBreakList.push_back(BreakInfo(aBreakPos,aPos,aBreakWidth));
			if(aBreakWidth > aMaxWidth)
				aMaxWidth = aBreakWidth;

			aBreakPos = -1;
			aWidth = 0;
			aBreakWidth = 0;
			continue;
		}

		if(aWidth+aCharWidth>mMaxWidth)
		{
			if(aBreakPos!=-1)
			{
				mBreakList.push_back(BreakInfo(aBreakPos,aBreakPos+1,aBreakWidth));
				aPos = aBreakPos;
				if(aBreakWidth > aMaxWidth)
					aMaxWidth = aBreakWidth;
			}
			else
			{
				mBreakList.push_back(BreakInfo(aPos,aPos,aWidth));
				if(aWidth > aMaxWidth)
					aMaxWidth = aWidth;
	
				if(aWidth>0)
					aPos--;
			}

			aBreakPos = -1;
			aWidth = 0;
			aBreakWidth = 0;
		}
		else
			aWidth += aCharWidth;

		aPos++;
	}
	if(aWidth>aMaxWidth)
		aMaxWidth = aWidth;

	if(aMaxWidth==0)
		mWidth = aWidth;
	else
		mWidth = aMaxWidth;

	mHeight = (mBreakList.size() + 1)*mFont->GetHeight();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074acc0
void WrappedText::SetText(const GUIString &theText)
{
	mText = theText;
	CalcWrap();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// SetRichText archive body is deferred/commented; no source credit.



///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074ad20
void WrappedText::SetWrap(bool wrap)
{
	if(wrap!=mWrap)
	{
		mWrap = wrap;
		CalcWrap();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074ad40
void WrappedText::SetCentered(bool isCentered)
{
	mIsCentered = isCentered;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074ad50
void WrappedText::SetFont(Font *theFont)
{
	mFont = theFont;
	CalcWrap();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074adb0
void WrappedText::SetMaxWidth(int theMaxWidth)
{
	if(theMaxWidth<0)
		theMaxWidth = 0;

	if(theMaxWidth!=mMaxWidth)
	{
		int anOldMax = mMaxWidth;
		mMaxWidth = theMaxWidth;
		if(!mWrap)
			return;

		bool needCalc = mRichText.get() || anOldMax==0 || !mBreakList.empty() || mMaxWidth<anOldMax;
		if(needCalc)
			CalcWrap();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x0074adf0
void WrappedText::Paint(Graphics &g, int x, int y)
{
	if(mRichText.get()!=NULL)
	{
		ElementPaintInfo anInfo;
		anInfo.mClipX = x;
		anInfo.mClipY = y;
		anInfo.mClipWidth = GetWidth();
		anInfo.mClipHeight = mHeight==0 ? mFont->GetHeight() : mHeight;
		anInfo.mStartSelSegment = anInfo.mEndSelSegment = -1;
		mRichText->Paint(g,x,y,anInfo);
		return;
	}

	if(mFont.get()==NULL)
		return;

	g.SetFont(mFont.get());

	int aYPos = y;
	int aStrPos = 0;
	for(BreakList::iterator anItr = mBreakList.begin(); anItr!=mBreakList.end(); ++anItr)
	{
		if(aStrPos>=mText.length())
			break;

		int aBreakPos = anItr->mBreakPos;
		int anXPos = x;
		if(mIsCentered)
			anXPos += (mWidth - anItr->mWidth)/2;

		g.DrawString(mText, aStrPos, aBreakPos - aStrPos, anXPos, aYPos);
		aYPos += mFont->GetHeight();
		aStrPos = anItr->mNextStartPos;
	}
	if(aStrPos<mText.length())
	{
		if(mIsCentered && !mBreakList.empty())
		{
			int aWidth = mFont->GetStringWidth(mText,aStrPos,mText.length()-aStrPos);
			int anXPos = x + (mWidth - aWidth)/2;
			g.DrawString(mText, aStrPos, mText.length()-aStrPos, anXPos, aYPos);
		}
		else
			g.DrawString(mText, aStrPos, mText.length()-aStrPos, x, aYPos);
	}
}	

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; zero Native/primary/physical credit.

WrappedText::WrappedText(const GUIString &theText)
{
	Init();
	mText = theText;
}
}
