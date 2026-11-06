// Skinned button text/image states, layout and painting on actual original owners.
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
#include "ee/wonapi/WONLobbyWONButton.h"
namespace WONAPI {

// FUNCTION: WON 0x00718060
void WONButton::VirtualB0(const GUIString &theText)
{
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second.get();
		aText->SetText(theText);
		++anItr;
	}

	AdjustHeightToFitText(); 
}

// FUNCTION: WON 0x00718100
void WONButton::Virtual08()
{
	Button::Virtual08();
	if(!mWrapText)
		return;

	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second.get();
		aText->SetMaxWidth(Width()-mLeftTextPad-mRightTextPad);
		++anItr;
	}

	AdjustHeightToFitText();
}

// FUNCTION: WON 0x007181A0
void WONButton::AdjustHeightToFitText()
{
	if(!mAdjustHeightToFitText)
		return;

	int width,height;
	GetTextSize(width,height);

	if(Height()-mTopTextPad-mBottomTextPad!=height)
		Fn2F4B90(Width(),height+mTopTextPad+mBottomTextPad);
}

// FUNCTION: WON 0x00718220
void WONButton::SetWrapText(bool wrap)
{
	if(mWrapText==wrap)
		return;

	mWrapText = wrap;
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second.get();
		aText->SetWrap(wrap);

		++anItr;
	}
}

// FUNCTION: WON 0x007182C0
void WONButton::SetButtonState(int theState, Image *theImage, int theColor, Font *theFont, bool checked)
{
	int checkedIndex = checked?1:0;
	ButtonState &aState = mStates[theState][checkedIndex];
	if(checkedIndex)
		mHasCheck = true;

	SmartPtr<Font> aFont;
	if(mFontTextMap.empty() && theFont==NULL)
		{ aFont = WONAPI::WindowManager::GetDefaultWindowManager()->GetDefaultFont(); theFont = aFont.get(); }

	if(theFont!=NULL)
	{
		FontTextMap::iterator anItr = mFontTextMap.insert(FontTextMap::value_type(theFont,NULL)).first;
		if(anItr->second.get()==NULL)
		{
			SmartPtr<WrappedText> aText = new WrappedText;
			aText->SetCentered(mHAlign==HorzAlign_Center);
			aText->SetWrap(false);
			aText->SetFont(theFont);
			anItr->second = aText;
		}

		aState.mText = anItr->second;
	}

	aState.mImage = theImage;
	aState.mColor = theColor;

}

// FUNCTION: WON 0x00718580
void WONButton::GetTextSize(int &width, int &height)
{
	width = 0; height = 0;
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second.get();
		if(aText->GetWidth() > width)
			width = aText->GetWidth();

		if(aText->GetHeight() > height)
			height = aText->GetHeight();

		++anItr;
	}
}

// FUNCTION: WON 0x00718640
void WONButton::Virtual0C(int &width, int &height)
{
	GetTextSize(width,height);
	for(int i=0; i<5; i++)
	{
		for(int j=0; j<2; j++)
		{
			Image *anImage = mStates[i][j].mImage.get();
			if(anImage!=NULL)
			{
				if(!mScaleImage && anImage->mWidth>width)
					width = anImage->mWidth;
			
				if(!mScaleImage && anImage->mHeight > height)
					height = anImage->mHeight;
			}
		}
	}


	width += mLeftTextPad + mRightTextPad;
	height += mTopTextPad + mBottomTextPad;
}

// FUNCTION: WON 0x007186E0
void WONButton::SetHorzAlign(HorzAlign theHorzAlign)
{
	if(theHorzAlign==mHAlign)
		return;

	if(mHAlign!=HorzAlign_Center && theHorzAlign!=HorzAlign_Center)
		return;

	mHAlign = theHorzAlign;

	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		WrappedText *aText = anItr->second.get();
		aText->SetCentered(mHAlign==HorzAlign_Center); 
		++anItr;
	}
}

// FUNCTION: WON 0x00718790
void WONButton::SetTextColor(int theColor)
{
	if(!mScaleImage)
	{
		for(int i=0; i<5; i++)
			for(int j=0; j<2; j++)
				mStates[i][j].mColor = theColor;
	}
	else
	{
		mStates[0][0].mColor = theColor;
		mStates[0][1].mColor = theColor;
		mStates[4][0].mColor = theColor;
		mStates[4][1].mColor = theColor;
	}
}

// FUNCTION: WON 0x007187E0
void WONButton::SetFont(Font *theFont)
{
	mFontTextMap.clear();
	SmartPtr<WrappedText> aText = mStates[0][0].mText.get();
	aText->SetFont(theFont);
	mFontTextMap[theFont] = aText;
	
	for(int i=0; i<5; i++)
		for(int j=0; j<2; j++)
			mStates[i][j].mText = aText;
}

// FUNCTION: WON 0x00718970
void WONButton::CopyAttributes(WONButton *from)
{
	Button::CopyAttributes(from);
	mLeftTextPad		= from->mLeftTextPad;
	mTopTextPad			= from->mTopTextPad;
	mRightTextPad		= from->mRightTextPad;
	mBottomTextPad		= from->mBottomTextPad;
	mLeftFocusPad		= from->mLeftFocusPad;
	mTopFocusPad		= from->mTopFocusPad;
	mRightFocusPad		= from->mRightFocusPad;
	mBottomFocusPad		= from->mBottomFocusPad;
	mPushDX				= from->mPushDX;
	mPushDY				= from->mPushDY;
	mHasCheck			= from->mHasCheck;
	mScaleImage			= from->mScaleImage;
	mHAlign				= from->mHAlign;
	mVAlign				= from->mVAlign;
	mDrawGrayText		= from->mDrawGrayText;
	mIsTransparent		= from->mIsTransparent;
	mFocusStyle			= from->mFocusStyle;
	mDefaultTextColor	= from->mDefaultTextColor;
	if(from->ComponentFlagSet(0x100))
		SetComponentFlags(0x100,true);
	if(from->ComponentFlagSet(0x200))
		SetComponentFlags(0x200,true);

	mFontTextMap = from->mFontTextMap;
	FontTextMap::iterator anItr = mFontTextMap.begin();
	while(anItr!=mFontTextMap.end())
	{
		anItr->second = new WrappedText;
		anItr->second->SetCentered(mHAlign==HorzAlign_Center);
		anItr->second->SetWrap(mWrapText);
		anItr->second->SetFont(anItr->first);
		++anItr;
	}


	for(int i=0; i<5; i++)
	{
		for(int j=0; j<2; j++)
		{
			ButtonState &src = from->mStates[i][j];
			ButtonState &dst = mStates[i][j];

			dst.mColor = src.mColor;

			if(src.mImage.get()!=NULL)
			{
				if(mScaleImage)
					dst.mImage = src.mImage->Duplicate();
				else
					dst.mImage = src.mImage;
			}

			if(src.mText.get()!=NULL)
				dst.mText = mFontTextMap[src.mText->mFont.get()];
		}
	}
}

// FUNCTION: WON 0x00718DB0
void WONButton::DrawButtonText(Graphics &g, WrappedText *theText, int theColor)
{
	if(theText==NULL)
		return;

	int x,y;
	if(mScaleImage)
	{
		switch(mHAlign)
		{
			case HorzAlign_Left: x = mLeftTextPad; break;
			case HorzAlign_Center: x = (Width() - theText->GetWidth() - mLeftTextPad - mRightTextPad)/2 + mLeftTextPad; break;
			case HorzAlign_Right: x = Width() - theText->GetWidth() - mRightTextPad; break;

		}
		y = (Height() - theText->GetHeight())/2;
		if(IsPressed())
		{
			x+=mPushDX;
			y+=mPushDY;
		}
	}
	else
	{
		x = mLeftTextPad;
		y = (Height() - theText->GetHeight())/2;
	}

	if(IsDisabled() && mDrawGrayText)
	{
		g.SetSysColor(StandardColor_3DHilight);
		theText->Paint(g,x+1,y+1);
		g.SetSysColor(StandardColor_GrayText);
		theText->Paint(g,x,y);
	}
	else
	{
		g.SetColor(theColor);
		theText->Paint(g,x,y);
		if(mFocusStyle && HasFocus() && mParent->mHasUsedDialogKeys)
		{
			if(mScaleImage)
				g.DrawDottedRect(mLeftFocusPad,mTopFocusPad,Width()-mLeftFocusPad-mRightFocusPad,Height()-mTopFocusPad-mBottomFocusPad);
			else
				g.DrawDottedRect(mLeftTextPad-1,y-1,theText->GetWidth()+2,theText->GetHeight()+2);
		}
		
	}
}

// FUNCTION: WON 0x00718F90
void WONButton::GetButtonState(Graphics &g, int *theColor, WrappedText **theText, Image **theImage)
{
	int aState = 0;
	if(IsDisabled())
		aState = 2;
	else if(IsPressed())
		aState = 1;
	else if(IsMouseOver() && ((mButtonFlags&1)!=0))
		aState = 3;
	else if(IsDefaultButton())
		aState = 4;

	int aCheckedIndex = (mHasCheck && IsChecked())?1:0;

	ButtonState &aStateStruct = mStates[aState][aCheckedIndex];

	Image *anImage = aStateStruct.mImage.get();
	int aColor = aStateStruct.mColor;
	WrappedText *aText = aStateStruct.mText.get();

	if(anImage==NULL) anImage = mStates[0][aCheckedIndex].mImage.get();
	if(aColor==-1) aColor = mStates[0][aCheckedIndex].mColor;
	if(aText==NULL) aText = mStates[0][aCheckedIndex].mText.get();

	if(aCheckedIndex==1)
	{
		if(anImage==NULL) anImage = mStates[0][0].mImage.get();
		if(aColor==-1) aColor = mStates[0][0].mColor;
		if(aText==NULL) aText = mStates[0][0].mText.get();
	}

	if(aColor==-1)
		aColor = mDefaultTextColor;

	if(theText) *theText = aText;
	if(theColor) *theColor = aColor;
	if(theImage) *theImage = anImage;
}

// FUNCTION: WON 0x00719080
void WONButton::Paint(Graphics &g)
{
	Button::Paint(g);
	if(!mIsTransparent)
	{
		g.SetSysColor(StandardColor_3DFace);
		g.FillRect(0,0,Width(),Height());
	}

	Image *anImage; 
	int aColor; 
	WrappedText *aText; 
	GetButtonState(g,&aColor,&aText,&anImage);

	if(mScaleImage)
	{
		if(anImage!=NULL)
		{
			if(anImage->mWidth!=Width() || anImage->mHeight!=Height())
				anImage->SetSize(Width(),Height());

			g.DrawImage(anImage,0,0);
		}
		DrawButtonText(g,aText,aColor);
	}
	else
	{
		int y = 0;

		if(anImage!=NULL)
		{
			int aHeight = anImage->mHeight;
			if(mVAlign==HorzAlign_Center) y = (Height() - aHeight - mTopTextPad - mBottomTextPad)/2;
			else if(mVAlign==VertAlign_Bottom) y = Height() - aHeight - mTopTextPad - mBottomTextPad;

			g.DrawImage(anImage,0,y + mTopTextPad);
		}
		DrawButtonText(g,aText,aColor);
	}
}

// Support0 scalar-state/forwarding helper, no primary quota.
// FUNCTION: WON 0x00718200
void WONButton::SetAdjustHeightToFitText(bool adjust)
{
	if(adjust==mAdjustHeightToFitText)
		return;

	mAdjustHeightToFitText = adjust;
	AdjustHeightToFitText();
}

// Support0 scalar-state/forwarding helper, no primary quota.
// FUNCTION: WON 0x007182a0
void WONButton::SetTransparent(bool isTransparent)
{
	mIsTransparent = isTransparent;
	if(isTransparent)
		SetComponentFlags(0x100,true);
}
}

namespace WONAPI {
// FUNCTION: WON 0x007180e0
const GUIString& WONButton::GetText()
{
    if(mFontTextMap.empty()) return gEmptyGUIString;
    return mFontTextMap.begin()->second->GetText();
}
}

namespace WONAPI {
// FUNCTION: WON 0x00718780
void WONButton::SetVertAlign(VertAlign align)
{
    mVAlign=align;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00722490
// (public: class std::_Tree<class WONAPI::Font *,struct std::pair<class WONAPI::Font * const,class WONAPI::SmartPtr<class WONAPI::WrappedText> >,struct s)
