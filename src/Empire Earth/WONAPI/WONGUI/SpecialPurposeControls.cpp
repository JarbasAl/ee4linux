// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// SOURCE_ONLY_NO_NATIVE_ID: complete genuine same-SDK specialized controls.
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbySpecialPurposeControls.h"
namespace WONAPI {
ColHeaderButton::ColHeaderButton()
{
	mFont = GetFont(FontDescriptor("Verdana",FontStyle_Bold,7));
	mBackColor = 0xffffff;
	mIsUp = true;
	Fn2F3370(0x0008, true);
	mMultiListBox = NULL;
	mCol = -1;
}

	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
ColHeaderButton::~ColHeaderButton()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void ColHeaderButton::DrawButtonText(Graphics &g, int xoff, int yoff)
{
	g.SetFont(mFont);
	Font *aFM = mFont;
	int aTextWidth = aFM->GetStringWidth(mText);
	int aTextHeight = aFM->GetHeight();

	int x = 0;
	int y = 0;

	x+=xoff;
	y+=yoff;

	g.SetColor(0x000000);
	g.DrawString(mText, x, y);
}

///////////////////////////////////////////////////////////////////////////////
void ColHeaderButton::Activate()
{
	if (((mButtonFlags&0x0004)!=0)) // Change up down state if checked
	{
		mIsUp = !mIsUp;
	}

	if (mMultiListBox.get() != NULL && mCol >= 0)
	{
		// Sort the Col
		mMultiListBox->SetSortColumn(mCol, mIsUp?ListSortType_Ascending:ListSortType_Descending);
	}

	Button::Fn2F3440();
}

///////////////////////////////////////////////////////////////////////////////
void ColHeaderButton::Paint(Graphics &g)
{
	Button::Paint(g);

	// Background
	g.SetColor(mBackColor);
	g.FillRect(0,0,Width(),Height());

	DrawButtonText(g,2,0);

	if(((mButtonFlags&0x0004)!=0)) // Draw arrow
	{	
		int aTextWidth = mFont->GetStringWidth(mText) + 2;
		int x = Width()/2;

		// Make sure arrow is in center or just to the right of text
		if (aTextWidth + 6 > x)
		{
			x = aTextWidth + 6;
		}
			
		int y = 5;
		

		if(mIsUp)
		{
			for(int i=0; i<7; i++)
			{
				int w = i/2;
				g.DrawLine(x-w, y+i, x+w+1, y+i);	
			}
		}
		else 
		{
			for(int i=0; i<7; i++)
			{
				int w = i/2;
				g.DrawLine(x-(3-w), y+i, x+(3-w), y+i);	
			}

			if(IsDisabled())
				g.SetPixel(x+1,y+4,0xFFFFFF);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
ImageCheckButton::ImageCheckButton(int theNormalControlID, int theCheckedControlID) : mAlternateControlID(theCheckedControlID)
{
	mControlId=theNormalControlID;
	Fn2F3370(0x0001,true);
	SetComponentFlags(0x000008,true);
}

///////////////////////////////////////////////////////////////////////////////
ImageCheckButton::~ImageCheckButton(void)
{
}

///////////////////////////////////////////////////////////////////////////////
void ImageCheckButton::AlternateControlIDs(void)
{
	int aSavedID = mAlternateControlID;
	mAlternateControlID = mControlId;
	mControlId=aSavedID;
}

///////////////////////////////////////////////////////////////////////////////
void ImageCheckButton::Paint(Graphics &g)
{
	Component::Paint(g);

	Image *anImage = NULL;
	if(IsMouseOver())
	{
		if(((mButtonFlags&0x0004)!=0))
			anImage = mImages[ImageCheckButtonType_CheckedOver];
		else
			anImage = mImages[ImageCheckButtonType_Over];
	}
	else
	{
		if(((mButtonFlags&0x0004)!=0))
			anImage = mImages[ImageCheckButtonType_Checked];
		else
			anImage = mImages[ImageCheckButtonType_Normal];
	}

	if(anImage==NULL)
		anImage = mImages[ImageCheckButtonType_Normal];

	if(anImage!=NULL)
		g.DrawImage(anImage,0,0);
}

///////////////////////////////////////////////////////////////////////////////
CheckButtonSplitterContainer::CheckButtonSplitterContainer(void)
{
	mNormalSplitter = NULL;
	mMaxMinSplitter = NULL;
	mFirstMinButton = NULL;
	mSecondMinButton = NULL;
	mFirstMaxButton = NULL;
	mSecondMaxButton = NULL;
	mStatus = Status_Normal;
}

///////////////////////////////////////////////////////////////////////////////
CheckButtonSplitterContainer::~CheckButtonSplitterContainer(void)
{
}

///////////////////////////////////////////////////////////////////////////////
void CheckButtonSplitterContainer::AddSplitterBars(BorderComponent* theNormalSplitter, BorderComponent* theMaxMinSplitter)
{
	if(theNormalSplitter)
	{
		mNormalSplitter = theNormalSplitter;
		mNormalSplitter->SetComponentFlags(0x000020, true);
		Fn2F6A60(mNormalSplitter);
	}
	if(theMaxMinSplitter)
	{
		mMaxMinSplitter = theMaxMinSplitter;
		mMaxMinSplitter->SetComponentFlags(0x000020, true);
		mMaxMinSplitter->SetVisible(false);
		Fn2F6A60(mMaxMinSplitter);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CheckButtonSplitterContainer::AddMinButtons(ImageCheckButton* theFirstMinButton, ImageCheckButton* theSecondMinButton)
{
	if(theFirstMinButton)
	{
		mFirstMinButton = theFirstMinButton;
		mFirstMinButton->SetComponentFlags(0x000040, true);
		mFirstMinButton->Fn2F3370(0x0002, false);
		Fn2F6A60(mFirstMinButton);
	}
	if(theSecondMinButton)
	{
		mSecondMinButton = theSecondMinButton;
		mSecondMinButton->SetComponentFlags(0x000040, true);
		mSecondMinButton->Fn2F3370(0x0002, false);
		Fn2F6A60(mSecondMinButton);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CheckButtonSplitterContainer::AddMaxButtons(ImageCheckButton* theFirstMaxButton, ImageCheckButton* theSecondMaxButton)
{
	if(theFirstMaxButton)
	{
		mFirstMaxButton = theFirstMaxButton;
		mFirstMaxButton->SetComponentFlags(0x000040, true);
		mFirstMaxButton->Fn2F3370(0x0002, false);
		Fn2F6A60(mFirstMaxButton);
	}
	if(theSecondMaxButton)
	{
		mSecondMaxButton = theSecondMaxButton;
		mSecondMaxButton->SetComponentFlags(0x000040, true);
		mSecondMaxButton->Fn2F3370(0x0002, false);
		Fn2F6A60(mSecondMaxButton);
	}
}

///////////////////////////////////////////////////////////////////////////////
void CheckButtonSplitterContainer::ChangeAlternateButtonState(void)
{
	if(mStatus == Status_Max)
	{
		if(mFirstMinButton && ((mFirstMinButton->mButtonFlags&0x0004)!=0))
		{
			mFirstMinButton->SetCheck(false);
			mFirstMinButton->AlternateControlIDs();
		}
		if(mSecondMinButton && ((mSecondMinButton->mButtonFlags&0x0004)!=0))
		{
			mSecondMinButton->SetCheck(false);
			mSecondMinButton->AlternateControlIDs();
		}
	}
	else if(mStatus == Status_Min)
	{
		if(mFirstMaxButton && ((mFirstMaxButton->mButtonFlags&0x0004)!=0))
		{
			mFirstMaxButton->SetCheck(false);
			mFirstMaxButton->AlternateControlIDs();
		}
		if(mSecondMaxButton && ((mSecondMaxButton->mButtonFlags&0x0004)!=0))
		{
			mSecondMaxButton->SetCheck(false);
			mSecondMaxButton->AlternateControlIDs();
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
void CheckButtonSplitterContainer::HandleComponentEvent(ComponentEvent *theEvent)
{
	if(theEvent->mType == ComponentEvent_ButtonPressed)
	{
		ImageCheckButton* aButton = dynamic_cast<ImageCheckButton*>(theEvent->mComponent);
		if(aButton)
		{
			Container::HandleComponentEvent(theEvent);

			bool isChecked = ((aButton->mButtonFlags&0x0004)!=0);
			mStatus = Status_Normal;
			aButton->AlternateControlIDs();
			if(theEvent->mComponent == mFirstMinButton)
			{
				if(isChecked)
					mStatus = Status_Min;

				if(mSecondMinButton)
				{
					mSecondMinButton->SetCheck(isChecked);
					mSecondMinButton->AlternateControlIDs();
				}
			}
			else if(theEvent->mComponent == mSecondMinButton)
			{
				if(isChecked)
					mStatus = Status_Min;

				if(mFirstMinButton)
				{
					mFirstMinButton->SetCheck(isChecked);
					mFirstMinButton->AlternateControlIDs();
				}
			}
			else if(theEvent->mComponent == mFirstMaxButton)
			{
				if(isChecked)
					mStatus = Status_Max;

				if(mSecondMaxButton)
				{
					mSecondMaxButton->SetCheck(isChecked);
					mSecondMaxButton->AlternateControlIDs();
				}
			}
			else if(theEvent->mComponent == mSecondMaxButton)
			{
				if(isChecked)
					mStatus = Status_Max;

				if(mFirstMaxButton)
				{
					mFirstMaxButton->SetCheck(isChecked);
					mFirstMaxButton->AlternateControlIDs();
				}
			}

			if(!isChecked && mNormalSplitter)
			{
				if(mNormalSplitter && !mNormalSplitter->IsVisible())
					mNormalSplitter->SetVisible(true);
				if(mMaxMinSplitter && mNormalSplitter && mMaxMinSplitter->IsVisible())
					mMaxMinSplitter->SetVisible(false);
			}
			if(isChecked && mMaxMinSplitter)
			{
				if(mMaxMinSplitter && !mMaxMinSplitter->IsVisible())
					mMaxMinSplitter->SetVisible(true);
				if(mNormalSplitter && mMaxMinSplitter && mNormalSplitter->IsVisible())
					mNormalSplitter->SetVisible(false);
			}

			ChangeAlternateButtonState();
			return;
		}
	}
	Container::HandleComponentEvent(theEvent);
}

void CheckButtonSplitterContainer::Virtual2C(int x, int y, int theButton)
{
	if(theButton==MouseButton_Left && mMaxMinSplitter && mMaxMinSplitter->IsVisible() && mMaxMinSplitter->Contains(x, y))
	{
		switch(mStatus)
		{
		case Status_Min:
			if(mFirstMinButton)
				mFirstMinButton->Fn2F3440();
			else if(mSecondMinButton)
				mSecondMinButton->Fn2F3440();
			break;
		case Status_Max:
			if(mFirstMaxButton)
				mFirstMaxButton->Fn2F3440();
			else if(mSecondMaxButton)
				mSecondMaxButton->Fn2F3440();
			break;
		default:
			break;
		}
	}
	Container::Virtual2C(x, y, theButton); 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BorderImageButton::Paint(Graphics &g)
{
	Component::Paint(g);

	BorderComponent *anImage = NULL;
	if(IsDisabled())
		anImage = mImages[ImageButtonType_Disabled];
	else if(((mButtonFlags&0x0004)!=0) && mImages[ImageButtonType_Checked].get()!=NULL)
		anImage = mImages[4];
	else if(((mButtonFlags&0x0020)!=0))
		anImage = mImages[ImageButtonType_Down];
	else if(IsMouseOver())// || HasFocus())
		anImage = mImages[ImageButtonType_Over];

	if(anImage==NULL)
		anImage = mImages[ImageButtonType_Normal];

	if(anImage!=NULL)
	{
		anImage->Invalidate();
		anImage->Paint(g);
	}
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void BorderImageButton::Virtual08()
{
	for(int i = 0; i < 5; i++)
	{
		if(mImages[i].get() != NULL)
			mImages[i]->Fn2F4B90(Width(), Height());
	}
}
}
