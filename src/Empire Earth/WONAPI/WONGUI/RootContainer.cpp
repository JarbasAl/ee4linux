// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;

// FUNCTION: WON 0x007040d0
RootContainer::RootContainer(Window *theWindow)
{
	mModalComponent = NULL;
	mWindow = theWindow;
	mCursorComponent = NULL;
	SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_Arrow));
	SetComponentFlags(0x8 | 0x40000,true);

	mAllowDrag = true;
	mDragX = mDragY = 0;
	mDragging = false;
	mIsolateTabFocus = true;
}

// FUNCTION: WON 0x00704510
void RootContainer::AddBG(BackgroundGrabber *theGrabber)
{
	if(theGrabber==NULL || !theGrabber->NeedDrawBG())
		return;

	mBGStructMap[(int)theGrabber->mImageOrder] = BGStruct(theGrabber->mImageX,theGrabber->mImageY,theGrabber->mBGImage.get(), theGrabber->mClipRect);
	theGrabber->Clear();
}

// FUNCTION: WON 0x00704710
Component* RootContainer::VirtualAC(int x, int y)
{
	Component *aComponent = Container::VirtualAC(x,y);


	return aComponent;
}

// FUNCTION: WON 0x00704730
void RootContainer::Virtual08()
{
	Container::Virtual08();
	mInvalidRect=mBounds;
}

// FUNCTION: WON 0x00704760
void RootContainer::Virtual14(Graphics &g)
{

	Container::Virtual14(g);
	std::map<int,BGStruct>::reverse_iterator anItr = mBGStructMap.rbegin();
	while(anItr!=mBGStructMap.rend())
	{
		BGStruct &aStruct = anItr->second;
		WONRectangle anImageRect(aStruct.mImageX,aStruct.mImageY,aStruct.mImage->mWidth,aStruct.mImage->mHeight);
		anImageRect.DoIntersect(aStruct.mClipRect);
		g.DrawImage(aStruct.mImage.get(), anImageRect.x, anImageRect.y, anImageRect.x - aStruct.mImageX, anImageRect.y - aStruct.mImageY, anImageRect.width, anImageRect.height); 


		++anItr;
	}
	mBGStructMap.clear();

}

// FUNCTION: WON 0x00704900
void RootContainer::Paint(Graphics &g)
{
	g.mColorScheme=GetColorScheme();
	g.PushClipRect(mInvalidRect.x, mInvalidRect.y, mInvalidRect.width, mInvalidRect.height);
	Container::Paint(g);
	g.PopClipRect();

	mInvalidRect.x = mInvalidRect.y = mInvalidRect.width = mInvalidRect.height = 0;
}

// FUNCTION: WON 0x00704970
bool RootContainer::VirtualC0(Component *theChild)
{
	return Container::VirtualC0(theChild);
}

// FUNCTION: WON 0x00704980
void RootContainer::Virtual60(WONRectangle &theRect)
{
	mInvalidRect.DoUnion(theRect);
}

// FUNCTION: WON 0x007049a0
bool RootContainer::Virtual54(int theKey)
{
	Container::Virtual54(theKey);

	if(mKeyboardListener.get()!=NULL)
		mKeyboardListener->KeyEvent(theKey,0);	

	return true;
}

// FUNCTION: WON 0x007049d0
bool RootContainer::Virtual58(int theKey)
{
	Container::Virtual58(theKey);
	if(mKeyboardListener.get()!=NULL)
		mKeyboardListener->KeyEvent(theKey,1);

	return true;
}

// FUNCTION: WON 0x00704a00
bool RootContainer::Virtual50(int theKey)
{
	Container::Virtual50(theKey);
	if(mKeyboardListener.get()!=NULL)
		mKeyboardListener->KeyEvent(theKey,2);

	return true;
}

// FUNCTION: WON 0x00704a30
void RootContainer::Virtual34(int x, int y)
{
	Container::Virtual34(x,y);
	if(mCursorComponent!=NULL)
	{
		int mx,my;
		mCurrentCursor->GetComponent(mx,my);
		x-=mx; y-=my; 

		mCursorComponent->Invalidate();
		mCursorComponent->Fn2F4B70(x,y);
		WONRectangle aRect(x,y,mCursorComponent->Width(),mCursorComponent->Height());
		Virtual60(aRect);
	}
}

// FUNCTION: WON 0x00704ac0
void RootContainer::Virtual38(int x, int y)
{
	if(!mDragging)
	{
		Container::Virtual38(x,y);
		if(mCursorComponent!=NULL)
		{
			int mx,my;
			mCurrentCursor->GetComponent(mx,my);
			x-=mx; y-=my; 

			mCursorComponent->Invalidate();
			mCursorComponent->Fn2F4B70(x,y);
			WONRectangle aRect(x,y,mCursorComponent->Width(),mCursorComponent->Height());
			Virtual60(aRect);
		}
	}
	else
	{
		int aNewX = Left() + (x - mDragX);
		int aNewY = Top() + (y - mDragY);
		if(aNewX==Left() && aNewY==Top())
			return;

		Window *aWindow = mWindow;
		WONRectangle r;
		aWindow->GetRect(r,false);
		aWindow->SetPosSize(r.x+aNewX,r.y+aNewY,-1,-1);
	}
}

// FUNCTION: WON 0x00704bd0
void RootContainer::Virtual2C(int x, int y, int theButton)
{
	Container::Virtual2C(x,y,theButton);
	if(theButton==0 && mAllowDrag)
	{
		Window *aWindow = mWindow;
		Component *aMouseChild = mMouseChild;
		if(aWindow->mIsOneComponentWindow && aMouseChild!=NULL)
			aMouseChild = aMouseChild->Virtual98();

		if(aMouseChild!=NULL)
		{
			if(aMouseChild->ComponentFlagSet(0x20))
			{
				mDragging = true;
				mDragX = x;
				mDragY = y;
			}
		}
	}
}

// FUNCTION: WON 0x00704c40
void RootContainer::Virtual24(int x, int y)
{
	Container::Virtual24(x,y);
}

// FUNCTION: WON 0x00704c60
void RootContainer::Virtual28()
{
	Container::Virtual28();
	mDragging = false;
	if(mCursorComponent!=NULL)
		VirtualC4(mCursorComponent);
}

// FUNCTION: WON 0x00704c90
void RootContainer::Virtual30(int x, int y, int theButton)
{
	Container::Virtual30(x,y,theButton);

	if(theButton==0)
		mDragging = false;
}

// FUNCTION: WON 0x00704cc0
void RootContainer::VirtualC4(Component *theChild)
{

	if(theChild==mCursorComponent)
		mCursorComponent = NULL;

	Container::VirtualC4(theChild);
}

// FUNCTION: WON 0x00704cf0
void RootContainer::Virtual78(Cursor *theCursor)
{
	Container::Virtual78(theCursor);
	if(theCursor==NULL)
		theCursor = Fn2F4B00();

	Component *aComponent = NULL;
	int mx = 0, my = 0;
	if(theCursor!=NULL)
		aComponent = theCursor->GetComponent(mx,my);

	if(mCursorComponent!=aComponent)
	{
		if(mCursorComponent!=NULL)
			VirtualC4(mCursorComponent);

		mCursorComponent = aComponent;
		if(aComponent!=NULL)
		{
			if(aComponent->mParent!=NULL)
			{
				aComponent->mParent->VirtualC4(aComponent);
				BackgroundGrabber *aBG = aComponent->GetGrabBG();
				if(aBG!=NULL)
				{
					aBG->Clear();
					aBG->mBGImage = NULL; 
				}
			}

			aComponent->SetComponentFlags(0x100 | 0x800 | 0x1000, true);
			aComponent->SetVisible(true);
			
			int x = mWindow->mMouseX-mx; 
			int y = mWindow->mMouseY-my; 
			aComponent->Fn2F4B70(x,y);

			Fn2F6A60(aComponent);
			Component *aTop = mChildList.back().get();
			if(aTop!=aComponent)
				aComponent->EnsureOnTopOf(aTop);
		}
	}
}

// Native complete destructor3043c0: actual map,set,listener members then external Container base.

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00704230
// (public: virtual void * __thiscall WONAPI::RootContainer::`scalar deleting destructor'(unsigned int)    [??_GRootContainer@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00704300
// (public: __thiscall std::map<int,struct WONAPI::RootContainer::BGStruct,struct std::less<int>,class std::allocator<struct std::pair<int const ,struct W)
// TEMPLATE: WON 0x007046e0
// (public: __thiscall WONAPI::RootContainer::BGStruct::~BGStruct(void)    [??1BGStruct@RootContainer@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x0071bd20
// (public: __thiscall std::pair<int const ,struct WONAPI::RootContainer::BGStruct>::~pair<int const ,struct WONAPI::RootContainer::BGStruct>(void)    [??)
// TEMPLATE: WON 0x0071c100
// (public: struct std::pair<class std::_Tree<int,struct std::pair<int const ,struct WONAPI::RootContainer::BGStruct>,struct std::map<int,struct WONAPI::R)
// TEMPLATE: WON 0x0071c1f0
// (public: class std::_Tree<int,struct std::pair<int const ,struct WONAPI::RootContainer::BGStruct>,struct std::map<int,struct WONAPI::RootContainer::BGS)
// TEMPLATE: WON 0x0071dca0
// (public: __thiscall WONAPI::RootContainer::BGStruct::BGStruct(struct WONAPI::RootContainer::BGStruct const &)    [??0BGStruct@RootContainer@WONAPI@@QAE)
// TEMPLATE: WON 0x0071e380
// (public: class std::_Tree<int,struct std::pair<int const ,struct WONAPI::RootContainer::BGStruct>,struct std::map<int,struct WONAPI::RootContainer::BGS)
// TEMPLATE: WON 0x0071e870
// (protected: void __thiscall std::_Tree<int,struct std::pair<int const ,struct WONAPI::RootContainer::BGStruct>,struct std::map<int,struct WONAPI::RootC)
// TEMPLATE: WON 0x0071e8b0
// (protected: class std::_Tree<int,struct std::pair<int const ,struct WONAPI::RootContainer::BGStruct>,struct std::map<int,struct WONAPI::RootContainer::)
// TEMPLATE: WON 0x00724d10
// (void __cdecl std::_Construct(struct std::pair<int const ,struct WONAPI::RootContainer::BGStruct> *,struct std::pair<int const ,struct WONAPI::RootCont)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071e340
// (protected: void __thiscall std::_Tree<class WONAPI::Component *,class WONAPI::Component *,struct std::_Setk<class WONAPI::Component *,struct std::less)
// TEMPLATE: WON 0x00724fe0
// (public: void * __thiscall std::pair<int const ,struct WONAPI::RootContainer::BGStruct>::`scalar deleting destructor'(unsigned int)    [??_G?$pair@$$CB)
// TEMPLATE: WON 0x007043c0
// (RootContainer implicit destructor)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00704250
// (public: __thiscall std::set<class WONAPI::Component *,struct std::less<class WONAPI::Component *>,class std::allocator<class WONAPI::Component *> >::~)
