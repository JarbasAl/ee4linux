// WONGUI Window native behavior; no partial RootContainer allocation.
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
// FUNCTION: WON 0x0072a020
Window::~Window()
{
}

// FUNCTION: WON 0x0072a230
SmartPtr<Font> Window::GetFont(const FontDescriptor &theDescriptor)
{
	std::map<FontDescriptor,SmartPtr<Font> >::iterator anItr = mFontMap.insert(std::map<FontDescriptor,SmartPtr<Font> >::value_type(theDescriptor, NULL)).first;
	SmartPtr<Font> aFont = anItr->second;
	if(aFont.get()==NULL)
	{
		aFont = Virtual04(theDescriptor);
		anItr->second = aFont;
	}

	return aFont;
}

// FUNCTION: WON 0x0072ab80
bool Window::CanClose()
{
	bool childrenCanClose = true;
	for(std::set<Window*>::iterator anItr = mChildWindowSet.begin(); anItr!=mChildWindowSet.end(); ++anItr)
	{
		Window *aChild = *anItr;
		if(!aChild->CanClose())
			childrenCanClose = false;
	}


	if(!childrenCanClose)
		return false;

	if(!mRoot->Virtual5C())
		return false;

	if(mAllowCloseFunc!=NULL && !mAllowCloseFunc(this))
		return false;

	return true;
}

// FUNCTION: WON 0x0072ac30
bool Window::Close()
{
	if(mWindowState!=WindowState_Open)
		return true;

	if(!CanClose())
		return false;

	mWindowState = WindowState_Closing;
	Virtual48(new InputEvent(14));
	return true;
}

// FUNCTION: WON 0x0072ac90
void Window::CheckResize(DWORD theTick)
{
	if(mNeedResize && theTick-mLastResizeTick>mResizeDelay && mSuspendRootResize<=0)
	{
		SmartPtr<NativeImage> anImage = CreateImage(mNewWidth,mNewHeight);
		Graphics &g = anImage->GetGraphics();
		g.DrawImage(mOffscreenImage.get(),0,0);
		mOffscreenImage = anImage;
		mRoot->Fn2F4B90(mNewWidth, mNewHeight);
		mRoot->Invalidate();
		mNeedResize = false;
		mLastResizeTick = GetTickCount();
	}
}

// FUNCTION: WON 0x0072adc0
void Window::SetTitle(const GUIString &theTitle)
{
	mTitle = theTitle;
	if(mWindowState==WindowState_Open)
		Virtual10();
}

// FUNCTION: WON 0x0072ae20
void Window::SetIcon(NativeIcon *theIcon)
{
	mIcon = theIcon;
	if(mWindowState==WindowState_Open)
		Virtual14();
}

// FUNCTION: WON 0x0072ae80
void Window::Center(bool inParent)
{
	WONRectangle aScreenRect, aWindowRect;
	if(!inParent || mParent==NULL || !mParent->Virtual4C())
		mManager->GetScreenRect(aScreenRect);
	else
		mParent->GetRect(aScreenRect,false);

	GetRect(aWindowRect,false);

	int left = aScreenRect.x + (aScreenRect.width - aWindowRect.width)/2;
	int top = aScreenRect.y + (aScreenRect.height - aWindowRect.height)/2;
	if(left<0)
		left = 0;
	if(top<0)
		top = 0;

	SetPosSize(left,top,aWindowRect.width, aWindowRect.height);
}

// FUNCTION: WON 0x0072af30
void Window::SetClientSize(int width, int height)
{
	WONRectangle aRect;
	GetRect(aRect,false);
	SetPosSize(aRect.x,aRect.y,width,height);
}

// FUNCTION: WON 0x0072af70
void Window::Show(bool show)
{
	mIsShowing = show;
	if(!show)
	{
		if(mMouseIsInClientArea || mMouseIsDown[0])
			HandleMouseExit();
	}

	Virtual20(show);
}

// FUNCTION: WON 0x0072b120
void Window::HandleMouseExit()
{
	ShowNativeCursor(true); // show cursor when it leaves the window

	mMouseIsInClientArea = false;
	mMouseX = (mRoot->Left()+mRoot->Width())+1;
	mMouseY = (mRoot->Top()+mRoot->Height())+1;
	mMouseIsDown[0] = mMouseIsDown[1] = mMouseIsDown[2] = false;
	mRoot->Virtual28();
	mCurrentCursor = NULL;
}

// FUNCTION: WON 0x0072b170
void Window::HandleKeyEvent(KeyEvent *theEvent)
{
	mKeyMod = theEvent->mKeyMod;
	switch(theEvent->mL08)
	{
		case 8:
			mRoot->Virtual54(theEvent->mKey);
			break;

		case 9:
			mRoot->Virtual58(theEvent->mKey);
			break;

		case 10:
			mRoot->Virtual50(theEvent->mKey);
			break;
	}
}

// FUNCTION: WON 0x0072b1c0
void Window::HandleMouseDown(MouseEvent *theEvent)
{
	int aButton = theEvent->mButtonOrAmount;
	mMouseIsDown[aButton] = true;

	// Check for DoubleClick
	bool isDoubleClick = false;
	if(theEvent->mL08==4)
		isDoubleClick = true;
	else if(theEvent->mL08==5)
	{
		if(!mDetectDoubleClick)
		{
			for(int i=0; i<3; i++)
				mButtonDownTick[i] = 0;

			mDetectDoubleClick = true;
			mDoubleClickX = theEvent->mx;
			mDoubleClickY = theEvent->my;
		}

		DWORD &aPrevTick = mButtonDownTick[aButton];
		DWORD aTick = GetTickCount();
		if(aTick==0) // 0 has special meaning
			aTick = 1;
		
		if(aPrevTick!=0 && aTick-aPrevTick<250)
		{
			isDoubleClick = true;
			aPrevTick = 0;
		}
		else
			aPrevTick = aTick;
	}
	
	mButtonDoubleClick[aButton] = isDoubleClick;
	mRoot->Virtual2C(mMouseX, mMouseY, aButton);
}

// FUNCTION: WON 0x0072b260
void Window::HandleMouseEvent(MouseEvent *theEvent)
{
	mMouseX = theEvent->mx;
	mMouseY = theEvent->my;
	mKeyMod = theEvent->mKeyMod;

	if(!mMouseIsInClientArea && !mMouseIsDown[0])
	{
		mMouseIsInClientArea = true;
		mRoot->Virtual24(mMouseX, mMouseY);
		CheckSetCursor(true);
	}

	switch(theEvent->mL08)
	{		
		case 2:
			mLastMoveTick = GetTickCount();
			if(mDetectDoubleClick && mMouseX!=mDoubleClickX || mMouseY!=mDoubleClickY)
				mDetectDoubleClick = false;

			if(mMouseIsDown[0])
				mRoot->Virtual38(mMouseX, mMouseY); 
			else
				mRoot->Virtual34(mMouseX, mMouseY);
			break;

		case 3:
		case 4:
		case 5:
			HandleMouseDown(theEvent);
			break;

		case 6:
		{
			mLastMoveTick = 0;

			int aButton = theEvent->mButtonOrAmount;
			mMouseIsDown[aButton] = false;

			if(aButton==0)
			{
				if(!mMouseIsInClientArea)
				{
					HandleMouseExit(); // Call MouseExit when dragging is done
					break;
				}
			}
			mRoot->Virtual30(mMouseX, mMouseY, aButton);			
		}
		break;

		case 7:
			mRoot->Virtual3C(theEvent->mButtonOrAmount);
			break;
	}

}

// FUNCTION: WON 0x0072b390
void Window::HandleInputEvent(InputEvent *theEvent)
{
	switch(theEvent->mL08)
	{
		case 1: 
		{
			SizeEvent *anEvent = (SizeEvent*)theEvent;
			if(anEvent->mWidth!=-1 && anEvent->mHeight!=-1)
			{
				if(mRoot->Width()!=anEvent->mWidth || mRoot->Height()!=anEvent->mHeight)
				{
					mNeedResize = true;
					mNewWidth = anEvent->mWidth;
					mNewHeight = anEvent->mHeight;
					CheckResize(GetTickCount());
				}
			}
		}
		break;
		
		case 11:
			mRoot->Virtual48();
			break;

		case 12:
			mRoot->Virtual4C();				
			break;

		case 13:
			mMouseIsInClientArea = false;
			if(!mMouseIsDown[0])
				HandleMouseExit();
			break;
	}
}

// FUNCTION: WON 0x0072b460
bool Window::Virtual08()
{
	if(mWindowState==WindowState_Open)
	{
		bool force = false;
		if(mMouseIsInClientArea)
		{
			DWORD aTick = GetTickCount();
			if(aTick - mLastCheckCursorTick >= 1000)
			{
				force = true;
				mLastCheckCursorTick = aTick; 
			}
		}
		
		CheckSetCursor(force);
		return true;
	}
	else
		return false;
}

// FUNCTION: WON 0x0072b4b0
void Window::Virtual0C(DWORD theElapsed)
{
	if(mMouseIsInClientArea && mLastMoveTick!=0)
	{
		if(!mMouseIsDown[0] && !mMouseIsDown[1] && !mMouseIsDown[2])
		{
			DWORD anElapsed = GetTickCount() - mLastMoveTick;
			if(anElapsed >= 100)
				mRoot->Virtual40(anElapsed);
		}
	}

	CheckResize(GetTickCount());

	if(mRoot->mTimerFlags!=0)
		mRoot->Virtual44(theElapsed);

}

// FUNCTION: WON 0x00729c30
Window::Window()
{
	mManager = WONAPI::WindowManager::GetDefaultWindowManager();
	mParent = NULL;

	mRoot = new RootContainer(this);
	mCurrentCursor = NULL;
	mMouseIsDown[0] = mMouseIsDown[1] = mMouseIsDown[2] = false;
	mMouseIsInClientArea = false;
	mNativeCursorShowing = true;
	mDetectDoubleClick = false;

	mMouseX = mMouseY = mKeyMod = 0;
	mLastMoveTick = 0;
	mLastCheckCursorTick = 0;

	mMinWidth = 0;
	mMinHeight = 0;
	mMaxWidth = 64000;
	mMaxHeight = 64000;

	mNeedResize = false;
	mNewWidth = mNewHeight = 0;
	mResizeDelay = 0;
	mLastResizeTick = 0;

	mStyle = 0;
	mWindowState = WindowState_Closed;
	mIsShowing = false;

	mIsPopup = false;
	mIsOneComponentWindow = false;
	mSuspendRootResize = 0;
	mAllowCloseFunc = NULL;
	mMinimizeOnResolutionRestore = false;

	mTitle = "Sierra App";
}

// FUNCTION: WON 0x0072a210
void Window::SetParent(Window *theParent)
{
	mParent = theParent;
}

// FUNCTION: WON 0x0072a220
Window* Window::GetDefaultWindow()
{
	return WONAPI::WindowManager::GetDefaultWindowManager()->mDefaultWindow;
}

// FUNCTION: WON 0x0072a8c0
SmartPtr<NativeImage> Window::CreateImage(int theWidth, int theHeight)
{
	return mDisplayContext->CreateImage(theWidth,theHeight);
}

// FUNCTION: WON 0x0072a900
void Window::Create(const WONRectangle& theWindowRectR)
{
	if(mIcon.get()==NULL)
		mIcon = mManager->mDefaultWindowIcon.get();

	mWindowState = WindowState_Open;

	if(mParent!=NULL)
		mParent->mChildWindowSet.insert(this);
	
	mRoot->Fn2F4B90(theWindowRectR.width, theWindowRectR.height);

	mManager->AddWindow(this);
}

// FUNCTION: WON 0x0072aa60
void Window::Virtual2C()
{
	ShowNativeCursor(true); 
	mWindowState = WindowState_Closed;
	mIsShowing = false;

	if(mParent!=NULL)
		mParent->mChildWindowSet.erase(this);
}

// FUNCTION: WON 0x0072afc0
void Window::ShowNativeCursor(bool show)
{



	mNativeCursorShowing = show;
	Virtual1C(show);
}


static bool gWindowSkipDecodeImages = false;

// FUNCTION: WON 0x0072a640
void Window::SetSkipDecodeImages(bool skip) {
    gWindowSkipDecodeImages = skip;
}

// FUNCTION: WON 0x0072a650
SmartPtr<NativeImage> Window::DecodeImage(const char* path) {
    if(gWindowSkipDecodeImages) {
        static SmartPtr<NativeImage> image;
        if(image.get()==NULL) {
            image=CreateImage(10,10);
            image->GetGraphics().SetColor(0);
            image->GetGraphics().FillRect(0,0,20,20);
        }
        return image;
    }
    return mManager->DecodeImage(mDisplayContext.get(),path);
}

// FUNCTION: WON 0x0072a800
SmartPtr<RawImage> Window::DecodeImageRaw(const char* path) {
    if(gWindowSkipDecodeImages)
        return new RawImage8(10,10,NULL);
    return mManager->DecodeImageRaw(path);
}

// Native32b040 support0: forwards the actual nonvirtual root AddChild wrapper.
// FUNCTION: WON 0x0072b040
void Window::AddComponent(Component* component) {
    mRoot->Fn2F6A60(component);
}

// Native32b060 support0: forwards the actual modal helpers.
// FUNCTION: WON 0x0072b060
void Window::SetModal(bool modal) {
    if(modal) mRoot->EnterModal(); else mRoot->LeaveModal();
}

// FUNCTION: WON 0x0072afd0
void Window::CheckSetCursor(bool force) {
    if(!force && mRoot->mCurrentCursor.get()==mCurrentCursor)
        return;
    mCurrentCursor=mRoot->mCurrentCursor.get();
    Cursor* cursor=mRoot->mCurrentCursor.get();
    if(cursor==NULL)
        cursor=Cursor::GetStandardCursor(StandardCursor_Arrow);
    cursor=cursor->GetNative();
    if(cursor!=NULL) {
        ShowNativeCursor(true);
        Virtual18(cursor);
    } else {
        ShowNativeCursor(false);
    }
}

namespace WONAPI {
// FUNCTION: WON 0x0072a1e0
void Window::Fn32A1E0()
{
    mRoot->Invalidate();
    RootContainer* root=mRoot.get();
    root->mInvalidRect.DoUnion(root->GetBounds());
}
}

namespace WONAPI {
// FUNCTION: WON 0x0072afb0
void Window::Minimize(bool minimize)
{
    Virtual24(minimize);
}
}

// Genuine same-library SDK bodies; no new Native identity is asserted.
// FUNCTION: WON 0x0072a8f0
NativeImage* Window::GetOffscreenImage()
{
    return mOffscreenImage.get();
}

Graphics& Window::GetGraphics()
{
    return mOffscreenImage->GetGraphics();
}

void Window::SuspendRootResize(bool suspend)
{
    if(suspend)
        mSuspendRootResize++;
    else
    {
        mSuspendRootResize--;
        if(mSuspendRootResize <= 0)
        {
            mSuspendRootResize = 0;
            CheckResize(GetTickCount());
        }
    }
}

void Window::RemoveComponent(Component* theComponent)
{
    mRoot->VirtualC4(theComponent);
}

bool Window::IsMouseDown(MouseButton theButton)
{
    return mMouseIsDown[theButton];
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071de90
// (public: class std::_Tree<class WONAPI::Window *,class WONAPI::Window *,struct std::set<class WONAPI::Window *,struct std::less<class WONAPI::Window *>)
// TEMPLATE: WON 0x00729e90
// (public: virtual void * __thiscall WONAPI::Window::`scalar deleting destructor'(unsigned int)    [??_GWindow@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00729eb0
// (public: __thiscall std::set<class WONAPI::Window *,struct std::less<class WONAPI::Window *>,class std::allocator<class WONAPI::Window *> >::~set<class)
// TEMPLATE: WON 0x0072a5b0
// (public: __thiscall std::pair<struct WONAPI::FontDescriptor const ,class WONAPI::SmartPtr<class WONAPI::Font> >::~pair<struct WONAPI::FontDescriptor co)
