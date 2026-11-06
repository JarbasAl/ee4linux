// Native-authoritative InputBox editing/selection/history; SDK semantic reference.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <ctype.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x006f7f70
bool InputBox::NumericFilter(InputBox *theInputBox, GUIString &theStr)
{
	int aPos = 0;
	if(theStr[0]=='-')
	{
		if(theInputBox->mSelStartChar>0)
			return false;

		aPos++;
	}

	for(;aPos<theStr.length();aPos++)
		if(!isdigit(theStr[aPos]))
			return false;

	return true;
}

// FUNCTION: WON 0x006f8400
void InputBox::AdjustTextRect()
{
	int aLeftPad = mLeftPad;
	int aRightPad = mRightPad;
	int aTopPad = mTopPad;
	int aBottomPad = mBottomPad;

	if(mIcon.get()!=NULL)
		aLeftPad += mIcon->mWidth+2;

	mTextRect.x = aLeftPad;
	mTextRect.width = Width() - aLeftPad-aRightPad;

	mTextRect.y = aTopPad;
	mTextRect.height = Height() - aTopPad-aBottomPad;
}


// FUNCTION: WON 0x006f8450
void InputBox::SetIcon(Image *theIcon)
{
	mIcon = theIcon;
	AdjustTextRect();
	AdjustPosition();
	Invalidate();
}


// FUNCTION: WON 0x006f8510
void InputBox::Virtual08()
{
	Component::Virtual08();
	AdjustTextRect();
}


// FUNCTION: WON 0x006f8530
void InputBox::Virtual0C(int &width, int &height)
{
	width = 100;
	height = mFont->GetHeight() + 6;
}


// FUNCTION: WON 0x006f8560
void InputBox::SetCharWidth(int theEm)
{
	Fn2F4B90(mFont->Virtual10('M')*theEm + 8,Height());
}

// FUNCTION: WON 0x006f8590
void InputBox::Virtual48()
{
	Component::Virtual48();
	mCursorDelay = 0;
	mCursorOn = true;
	Fn2F57D0(true);
	if(mSelectOnFocus)
		SelectRange(0,-1);

	Invalidate();
}

// FUNCTION: WON 0x006f85d0
void InputBox::Virtual4C()
{
	Component::Virtual4C();
	mCursorOn = false;
	Fn2F57D0(false);
	Invalidate();
}

// FUNCTION: WON 0x006f8600
void InputBox::CheckSel(int x, int y, bool newStart)
{
	if(!mAllowSelection)
		return;

	if(x<0) x = 0;
	else if(x>Width()) x = Width();
	
	x += mHorzOffset - mTextRect.x;

	Font* aFM = mFont.get();
	int aSelChar = mBackwardsSel?mSelStartChar:mSelEndChar;
	int aWidth = aFM->GetStringWidth(mText, 0, aSelChar);
	if(aWidth >= x)
	{
		aSelChar--;
		while(aSelChar>=0 && aWidth>=x)
		{
			aWidth-=aFM->Virtual10(mText[aSelChar]);
			if(aWidth>=x)
				aSelChar--;
		}
		aSelChar++;
	}
	else
	{
		while(aSelChar<mText.length() && aWidth<x)
		{
			aWidth+=aFM->Virtual10(mText[aSelChar]);

				aSelChar++;
		}
	}
	if(aSelChar<0)
		aSelChar=0;
	if(aSelChar>mText.length())
		aSelChar = mText.length();

	if(newStart)
	{
		mSelStartChar = mSelEndChar = aSelChar;
		mBackwardsSel = false;
	}
	else
	{
		if(mBackwardsSel)
			mSelStartChar = aSelChar;
		else
			mSelEndChar = aSelChar;

		if(mSelStartChar > mSelEndChar)
		{
			mBackwardsSel = !mBackwardsSel;
			std::swap(mSelStartChar,mSelEndChar);
		}
	}
	
	Invalidate();
}

// FUNCTION: WON 0x006f8750
void InputBox::Virtual2C(int x, int y, int theButton)
{
	
	if(theButton==0)
	{
		CheckSel(x,y,(GetKeyMod() & 1)?false:true);

		if(mAllowSelection)
			mSelecting = true;

		Fn2F5890();	
	}
}


// FUNCTION: WON 0x006f8790
void InputBox::Virtual30(int x, int y, int theButton)
{

	if(theButton==0)
		mSelecting = false;
}


// FUNCTION: WON 0x006f87a0
void InputBox::Virtual28()
{
	Component::Virtual28();
	mSelecting = false;
}


// FUNCTION: WON 0x006f87b0
void InputBox::Virtual38(int x, int y)
{
	CheckSel(x,y,false);
}


// FUNCTION: WON 0x006f87d0
void InputBox::SetCursorBlinkRate(int theOnTime, int theOffTime) 
{
	if(theOffTime<=0)
		theOffTime = theOnTime;

	mCursorOnTime = theOnTime;
	mCursorOffTime = theOffTime;
}

// FUNCTION: WON 0x006f8800
bool InputBox::Virtual44(int theDelta)
{
	mCursorDelay+=theDelta;
	if(mCursorOn && mCursorDelay>mCursorOnTime)
	{
		mCursorDelay%=mCursorOnTime;
		mCursorOn = false;
		Invalidate();
	}
	else if(!mCursorOn && mCursorDelay>mCursorOffTime)
	{
		mCursorDelay%=mCursorOffTime;
		mCursorOn = true;
		Invalidate();		
	}

	if(mSelecting)
	{
		int x,y;
		GetMousePos(x,y);
		if(!(x>=0 && x<=Width() && y>=0 && y<=Height()))
		{
			int aCharPos = mBackwardsSel?mSelStartChar:mSelEndChar;

			if(x>Width() && aCharPos!=mText.length())
			{
				mHorzOffset += x-Width();
				if(mHorzOffset<0)
					mHorzOffset = 0;
			}
			else if(x<0 && aCharPos!=0)
				mHorzOffset += x;

			CheckSel(x,y,false);

			aCharPos = mBackwardsSel?mSelStartChar:mSelEndChar;
			if(aCharPos==mText.length())
			{
				int aTextWidth = mFont->GetStringWidth(mText);
				if(aTextWidth + mHorzOffset > mTextRect.width)
					mHorzOffset = aTextWidth-mTextRect.width+mFont->GetStringWidth(GUIString("M"));
			}
			if(aCharPos==0)
				mHorzOffset = 0;

			if(mHorzOffset<0)
				mHorzOffset = 0;

		}
	}
	return true;
}

// FUNCTION: WON 0x006f8a20
void InputBox::AdjustPosition()
{
	Font* aFM = mFont.get();
	int aCursorPos = mBackwardsSel?mSelStartChar:mSelEndChar;

	int aPreWidth = aFM->GetStringWidth(mText, 0, aCursorPos);
	int anM = aFM->Virtual10('M');

	if(aPreWidth - mHorzOffset < 0)
	{
		mHorzOffset = aPreWidth - 10*anM;
		if(mHorzOffset<0)
			mHorzOffset = 0;
	}

	if(aPreWidth - mHorzOffset >= mTextRect.width)
	{
		if(mTextRect.width!=0 && aPreWidth-mHorzOffset!=0)
			mHorzOffset = aPreWidth - mTextRect.width + anM;
	}
}

// FUNCTION: WON 0x006f8aa0
void InputBox::SelectRange(int theStartPos, int theEndPos)
{

	if(theStartPos<0) theStartPos = mText.length();
	if(theEndPos<0) theEndPos = mText.length();
	if(theStartPos>mText.length()) theStartPos = mText.length();
	if(theEndPos>mText.length()) theEndPos = mText.length();

	mBackwardsSel = theStartPos>theEndPos;
	if(mBackwardsSel)
		std::swap(theStartPos,theEndPos);

	mSelStartChar = theStartPos;
	mSelEndChar = theEndPos;
	AdjustPosition();
	Invalidate();
}

// FUNCTION: WON 0x006f8e10
void InputBox::SetSelText(const GUIString &theText, bool sendEvent)
{	
	const GUIString &aText = GetText();
	GUIString aPasteStr = theText;
	
	for(int i=0; i<aPasteStr.length(); i++)
	{
		int aChar = aPasteStr[i];
		if(aChar<32)
		{
			aPasteStr.resize(i);
			break; 
		}
	}
	if(mInputFilter && !mInputFilter(this,aPasteStr))
		return;

	if(mMaxChars>0 && aText.length()+aPasteStr.length()-(mSelEndChar-mSelStartChar)>mMaxChars)
	{
		int aNumCharsAllowed = mMaxChars-aText.length()+(mSelEndChar-mSelStartChar);
		if(aNumCharsAllowed<0)
			aNumCharsAllowed = 0;

		aPasteStr.resize(aNumCharsAllowed);
	}

	if(aPasteStr.length()>0 || mSelStartChar!=mSelEndChar)
	{
		int aSelStartChar = mSelStartChar, aSelEndChar = mSelEndChar;
		mSelStartChar+=aPasteStr.length();
		mSelEndChar = mSelStartChar;
		GUIString aNewStr;
		aNewStr.append(aText,0,aSelStartChar);
		aNewStr.append(aPasteStr);
		aNewStr.append(aText,aSelEndChar,aText.length()-aSelEndChar);
		SetTextPrv(aNewStr, sendEvent);
	}
	
	mCursorDelay = 0;
	mCursorOn = true;
	AdjustPosition();
	Invalidate();
}

// FUNCTION: WON 0x006f90c0
int InputBox::MoveCursor(bool left, bool wholeWord, const GUIString &theStr, int thePos)
{			
	int dx = left ? -1 : 1;
	
	thePos+=dx;
	if(wholeWord)
	{		
		while(thePos>=0 && thePos<theStr.length())
		{
			if((theStr[thePos]!=' '))
				thePos+=dx;
			else
				break; 
		}		

		while(thePos>=0 && thePos<theStr.length())
		{
			if((theStr[thePos]==' '))
				thePos+=dx;
			else
				break; 
		}		
	
		if(left)
			thePos++;
	}
	
	if(thePos<0)
		thePos = 0;
	if(thePos>theStr.length())
		thePos = theStr.length();

	return thePos;
}

// FUNCTION: WON 0x006f9140
void InputBox::Fn2F9140(const GUIString &theText, bool clearTraversalState)
{
	if(theText.length()==0)
		return;

	int aHistoryPos = mInputHistoryPos;
	while(mInputHistory.size()>=mMaxInputHistory)
	{
		mInputHistory.pop_front();
		aHistoryPos--;
	}
		
	if(theText.length()>0 && mMaxInputHistory>0)
	{
		if(mInputHistory.empty() || mInputHistory.back()!=theText) 
			mInputHistory.push_back(theText);
	}
	if(aHistoryPos>=0 && aHistoryPos<mInputHistory.size() && mInputHistory[aHistoryPos]==theText)
		mInputHistoryPos = aHistoryPos;
	else
		mInputHistoryPos = mInputHistory.size();

	if(clearTraversalState)
		mHaveTraversedHistory = false;

}

// FUNCTION: WON 0x006f92f0
void InputBox::SetInputHistory(const GUIString &theText, bool clearTraversalState)
{
	if(theText.length()==0)
		return;

	int aPos = mInputHistoryPos;
	if(aPos>=0 && aPos<mInputHistory.size())
		mInputHistory[aPos] = theText;
	else
	{
		Fn2F9140(theText, clearTraversalState);
		mInputHistoryPos = aPos;
	}

	if(clearTraversalState)
		mHaveTraversedHistory = false;
}

static int SmallerOf(int a,int b);
// FUNCTION: WON 0x006f93b0
bool InputBox::Virtual54(int theKey)
{
	const GUIString &aText = GetText();

	int aKeyMod = GetKeyMod();
	bool shift = (aKeyMod & 1)?true:false;
	bool ctrl = (aKeyMod & 4)?true:false; 

	if(!mAllowSelection)
		shift = false;
	
	switch(theKey)
	{		
		case 13: 

			
			if(aText.length()>0)
				Fn2F5380(20,0);
			return true;

		case 38:	
		case 40:
		{

			int aSize=SmallerOf(mMaxInputHistory,mInputHistory.size());
			if(aSize>0)
			{
				if(!mHaveTraversedHistory && mInputHistoryPos<mInputHistory.size())
					mHaveTraversedHistory = true;
				else
				{
					mHaveTraversedHistory = true;
					SetInputHistory(GetText());
					if(theKey==38)
						mInputHistoryPos--;
					else
						mInputHistoryPos++;
				}
				
				if(mInputHistoryPos<0)
					mInputHistoryPos = 0;
			
				if(mInputHistoryPos>=aSize)
				{
					SetText(GUIString(""),true);
					mInputHistoryPos = mInputHistory.size();
				}
				else
				{
					SetText(mInputHistory[mInputHistoryPos],true);
					SelectRange(GetText().length(),-1);
				}
			}
		}
		break;
		
		case 8: 
		case 46:  
		{
			GUIString left, right;
			if(mSelStartChar!=mSelEndChar) 
			{
				left.append(aText,0,mSelStartChar);
				right.append(aText,mSelEndChar,aText.length()-mSelEndChar);
			}
			else 
			{
				if(theKey==8) 
				{
					if(mSelStartChar>0)
						left.append(aText,0,mSelStartChar-1);

					right.append(aText,mSelEndChar,aText.length()-mSelEndChar);
				}
				else  
				{
					left.append(aText,0,mSelStartChar);
					
					if(mSelEndChar<aText.length())
						right.append(aText,mSelEndChar+1,aText.length()-(mSelEndChar+1));
				}
			}	
								
			mSelStartChar = mSelEndChar = left.length();
			GUIString total = left;
			total.append(right);
			SetTextPrv(total);
		}
		break;

		case 36: 
			if(!shift)
				mSelEndChar = 0;
			else if(!mBackwardsSel)
				mSelEndChar = mSelStartChar;				
			
			mSelStartChar = 0;
			mBackwardsSel =  mSelStartChar!=mSelEndChar;				
			break;
			
		case 35: 
			if(!shift)
				mSelStartChar = aText.length();
			else if(mBackwardsSel)
				mSelStartChar = mSelEndChar;

			mBackwardsSel = false;
			mSelEndChar = aText.length();
			break;

		case 37:
		case 39: 
			if(!shift && mSelStartChar!=mSelEndChar) 
			{
				if(theKey==37)
					mSelEndChar = mSelStartChar;
				else
					mSelStartChar = mSelEndChar;
			}
			else if(mBackwardsSel)
			{
				mSelStartChar = MoveCursor(theKey==37,ctrl,mText,mSelStartChar);
				
				if(!shift)
					mSelEndChar = mSelStartChar;
			}
			else
			{
				mSelEndChar = MoveCursor(theKey==37,ctrl,mText,mSelEndChar);
				
				if(!shift)
					mSelStartChar = mSelEndChar;					
			}						

			if(mSelStartChar>mSelEndChar)
			{
				mBackwardsSel = !mBackwardsSel;
				std::swap(mSelStartChar, mSelEndChar);
			}
			else if(mSelStartChar==mSelEndChar)
				mBackwardsSel = false;
			
			break;
							
		default:  
			return false;
	}
	
	mCursorDelay = 0;
	mCursorOn = true;
	AdjustPosition();
	Invalidate();	
	return true;
}

// FUNCTION: WON 0x006f9950
static int SmallerOf(int a,int b)
{
	return a>b ? b : a;
}


// FUNCTION: WON 0x006f9960
const GUIString& InputBox::GetText() const
{
	if(mPassword==0)
		return mText;
	else
		return mPasswordText;
}


// FUNCTION: WON 0x006f9970
void InputBox::SetPasswordChar(char theChar)
{
	mPassword = theChar;
	SetText(mText,false);
}


// FUNCTION: WON 0x006f9990
void InputBox::SetText(const GUIString &theText, bool sendEvent)
{
	SetTextPrv(theText,sendEvent);
	mSelStartChar = mSelEndChar = 0;
	AdjustPosition();
	Invalidate();
}


// FUNCTION: WON 0x006f99c0
void InputBox::Clear(bool sendEvent)
{
	SetText(gEmptyGUIString,sendEvent);
}
}

// Build-only next complete InputBox functions; canonical unchanged until C release.
namespace WONAPI {
// FUNCTION: WON 0x006f8b10
void InputBox::CopySelection()
{
	Clipboard::Set(mText.substr(mSelStartChar,mSelEndChar - mSelStartChar));	
}

// FUNCTION: WON 0x006f8c30
bool InputBox::Virtual50(int theKey)
{
	const GUIString &aText = GetText();
	GUIString aPasteStr;

	if(theKey==22) 	// CTRL-V
		aPasteStr = Clipboard::Get();
	else if(theKey==3)	// CTRL-C
	{
		CopySelection();
		return true;
	}
	else if(theKey==24)	// CTRL-X
	{
		CopySelection();
	}
	else if(theKey>31)
		aPasteStr.Fill(theKey,0,1);
	else
		return true;
	
	SetSelText(aPasteStr);
	return true;
}

// FUNCTION: WON 0x006f99e0
void InputBox::SetTextPrv(const GUIString &theText, bool sendEvent)
{
	if(mPassword==0)
		mText = theText;
	else
	{
		mPasswordText = theText;
		mText.Fill(mPassword,0,theText.length());
	}



	if(sendEvent)
		Fn2F5380(21,0);
}

// FUNCTION: WON 0x006f9ac0
void InputBox::Paint(Graphics &g)
{
	Component::Paint(g);

	int aTextColor;
	if(IsDisabled() && mUseDisabledColors)
	{
		mDisabledBackground.Paint(g,0,0,Width(),Height(),mHorzOffset,0);
		aTextColor = mDisabledTextColor;
	}
	else
	{
		mBackground.Paint(g,0,0,Width(),Height(),mHorzOffset,0);
		aTextColor = mTextColor;
	}

	if(mIcon.get()!=NULL)
		g.DrawImage(mIcon.get(),4,(Height()-mIcon->mHeight)/2);

	g.Translate(mTextRect.x , mTextRect.y);
	g.PushClipRect(0,0,mTextRect.width, mTextRect.height);
	g.Translate(-mHorzOffset,0);
	
	g.SetFont(mFont.get());
	g.SetColor(aTextColor);
	g.DrawString(mText, 0, 0);
	int aCursorHeight = mFont->GetHeight();//mTextRect.height;

	if(HasFocus())
	{
		if(mSelStartChar!=mSelEndChar)
		{
			Font *aFM = mFont.get();
			int x = aFM->GetStringWidth(mText, 0, mSelStartChar);
			int aWidth = aFM->GetStringWidth(mText, mSelStartChar, mSelEndChar - mSelStartChar);
			g.SetColor(mSelColor.GetBackgroundColor(g));
			g.FillRect(x, 0, aWidth, aCursorHeight);
			g.SetColor(mSelColor.GetForegroundColor(g,aTextColor));
			g.PushClipRect(x, 0, aWidth, aCursorHeight);
			g.DrawString(mText, 0, 0);
			g.PopClipRect();
			if(mCursorOn && !mUnderlineCursor)
			{
				if(mBackwardsSel)
					g.DrawLine(x,0,x,aCursorHeight);
				else
					g.DrawLine(x+aWidth-1,0,x+aWidth-1,aCursorHeight);
			}
		}
		else if(mCursorOn)
		{
			int aCursorPos = mSelStartChar;
			Font *aFM = mFont.get();
			int anXPos = aFM->GetStringWidth(mText, 0, aCursorPos);
			if(mUnderlineCursor)
			{
				int aWidth;
				if(aCursorPos < mText.length())
					aWidth = mFont->Virtual10(mText[aCursorPos]);
				else
					aWidth = mFont->Virtual10('M');
				g.DrawLine(anXPos, aCursorHeight-1, anXPos+aWidth-1, aCursorHeight-1);
			}
			else
				g.DrawLine(anXPos, 0, anXPos, aCursorHeight);
		}
	}
	
	g.Translate(mHorzOffset,0);
	g.PopClipRect();
	g.Translate(-mTextRect.x, -mTextRect.y);

}

}

namespace WONAPI {
// Native2f7fd0..2f81c9: font/cursor/flags/background and input state setup,505B.
// FUNCTION: WON 0x006f7fd0
InputBox::InputBox()
{
    mSelStartChar=mSelEndChar=0;
    mBackwardsSel=false;
    mSelecting=false;
    mHorzOffset=0;
    mCursorOn=false;
    mUnderlineCursor=false;
    mSelectOnFocus=false;
    mCursorOnTime=mCursorOffTime=500;
    mFont=GetNamedFont("InputBox");
    SetComponentFlags(0x4008,true);
    mTextColor=0x8000000e;
    mDisabledTextColor=(int)0x80000006;
    mDisabledBackground.SetColor((int)0x80000001);
    mUseDisabledColors=true;
    mPassword=0;
    mAllowSelection=true;
    mMaxChars=0;
    SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_IBeam));
    mLeftPad=mRightPad=4;
    mTopPad=mBottomPad=3;
    mInputHistoryPos=0;
    mMaxInputHistory=100;
    mHaveTraversedHistory=false;
    mInputFilter=NULL;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f81d0
// (public: virtual void * __thiscall WONAPI::InputBox::`scalar deleting destructor'(unsigned int)    [??_GInputBox@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006f81f0
// (public: __thiscall WONAPI::Background::~Background(void)    [??1Background@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x0071b780
// (public: __thiscall std::deque<class WONAPI::GUIString,class std::allocator<class WONAPI::GUIString> >::~deque<class WONAPI::GUIString,class std::alloc)
// TEMPLATE: WON 0x0071ee40
// (protected: void __thiscall std::deque<class WONAPI::GUIString,class std::allocator<class WONAPI::GUIString> >::_Tidy(void)    [?_Tidy@?$deque@VGUIStri)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f8270
// (public: virtual __thiscall WONAPI::InputBox::~InputBox(void)    [??1InputBox@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x0071c350
// (protected: void __thiscall std::deque<class WONAPI::GUIString,class std::allocator<class WONAPI::GUIString> >::_Growmap(unsigned int)    [?_Growmap@?$)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071ed00
// (protected: void __thiscall std::deque<class WONAPI::GUIString,class std::allocator<class WONAPI::GUIString> >::_Xlen(void)const )
// TEMPLATE: WON 0x00723510
// (public: __thiscall WONAPI::SmartPtr<class WONAPI::GUIStringData>::SmartPtr<class WONAPI::GUIStringData>(class WONAPI::SmartPtr<class WONAPI::GUIString)
// TEMPLATE: WON 0x0071c480
// (public: class WONAPI::GUIString & __thiscall std::deque<class WONAPI::GUIString,class std::allocator<class WONAPI::GUIString> >::iterator::operator*(v)
