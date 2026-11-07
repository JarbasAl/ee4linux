// WONGUI WindowManager native behavior; archived SDK corroborates source contract.
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
#include "ee/wonapi/WONLobbyDelayLoadImage.h"
#include "LostDebug.h"
using namespace WONAPI;
// FUNCTION: WON 0x0072b660
WindowManager::WindowManager() {
    // LOST-DEBUG: none: exact only
    EE_LOST_TRACE(0);
    EE_LOST_TRACE(0);
    EE_LOST_TRACE(0);
    EE_LOST_TRACE(0);
    EE_LOST_TRACE(0);
    mThreadId=GetCurrentThreadId();
    mIsFullScreen=false;
    mOriginalScreenWidth=mOriginalScreenHeight=0;
    mLastLoseFocusTick=0;
    mImageDecoder.AddDecoder(new BMPDecoder,"bmp");
    mImageDecoder.AddDecoder(new WIMDecoder,"wim");
    mDefaultWindowManager=this;
    mLastTimerTick=GetTickCount();
    mEndPopup=false;
    mDefaultWindow=NULL;mWindowCount=0;
    mCurMsgLoopDepth=0;mTargetMsgLoopDepth=0;mDialogResult=0;
    mLastInputTick=0;
    mModalWindow=NULL;mModalComponent=NULL;mPopupOwnerWindow=NULL;
    mFocusWindow=NULL;mMouseDownWindow=NULL;
    mIsValid=true;mInDoPopup=false;mInEndPopup=false;mPopupFlags=0;
    mPopupIsUp=false;mBA5=true;
    mDoMouseCapturePopupSupport=false;mCallingTimers=false;
    mColorScheme=new ColorScheme;
    InitColorScheme();
    // Native leaves cachecount94/fullscreenwidthb0/fullscreenheightb4 untouched.
}
// FUNCTION: WON 0x0072ba90
// (??_GWindowManager@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x0072bab0
// (??1?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE@XZ)

// FUNCTION: WON 0x0072bb70
// (??1?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE@XZ)

// FUNCTION: WON 0x0072bc30
// (??1?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE@XZ)

// FUNCTION: WON 0x0072bcf0
// (??1?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE@XZ)

// FUNCTION: WON 0x0072bdb0
// (??1?$_Tree@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@std@@@std@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE@XZ)

// FUNCTION: WON 0x0072be70
// (??1?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE@XZ)

// FUNCTION: WON 0x0072bf30
// (??1?$_Tree@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE@XZ)

// FUNCTION: WON 0x0072bff0
WindowManager::~WindowManager()
{
	Fn32D5B0();
}

// FUNCTION: WON 0x0072c230
void WindowManager::InitColorScheme()
{
	mColorScheme->SetStandardColor(0,0x000000);
	mColorScheme->SetStandardColor(1,0xD0D0D0);
	mColorScheme->SetStandardColor(2,0xFFFFFF);
	mColorScheme->SetStandardColor(3,0x848484);
	mColorScheme->SetStandardColor(4,0xDEDEDE);

	mColorScheme->SetStandardColor(5,0x000000);
	mColorScheme->SetStandardColor(6,0x848484);
	mColorScheme->SetStandardColor(7,0x08246B);
	mColorScheme->SetStandardColor(8,0xFFFFFF);

	mColorScheme->SetStandardColor(9,0xFFFFDD);
	mColorScheme->SetStandardColor(10,0x000000);

	mColorScheme->SetStandardColor(11,0xD0D0D0);
	mColorScheme->SetStandardColor(12,0);

	mColorScheme->SetStandardColor(13,0xFFFFFF);
	mColorScheme->SetStandardColor(14,0x000000);

	mColorScheme->SetStandardColor(15,0x0000FF);
	mColorScheme->SetStandardColor(16,0xFF0000);
}

// FUNCTION: WON 0x0072c2d0
void WindowManager::NotifyLostFocus()
{
	mFocusWindow = NULL;
	mLastLoseFocusTick = GetTickCount();
	if(mLastLoseFocusTick==0)
		mLastLoseFocusTick = 1;
}

// FUNCTION: WON 0x0072c300
void WindowManager::NotifyGotFocus(Window *theWindow)
{
	mFocusWindow = theWindow;
	mLastLoseFocusTick = 0;
}

// FUNCTION: WON 0x0072c320
void WindowManager::SetScreenResolution(int theWidth, int theHeight)
{
	int aWidth = 0,aHeight = 0;
	GetScreenResolutionHook(aWidth, aHeight);
	if(aWidth!=theWidth && aHeight!=theHeight)
	{
		SetScreenResolutionHook(theWidth,theHeight);
		mOriginalScreenWidth = aWidth;
		mOriginalScreenHeight = aHeight;
	}
}

// FUNCTION: WON 0x0072c390
void WindowManager::RestoreScreenResolution()
{
	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = anItr->get();
		if(aWindow->mWindowState==Window::WindowState_Open && aWindow->mMinimizeOnResolutionRestore)
			aWindow->Minimize(true);

		++anItr;
	}

	if(mOriginalScreenWidth!=0 && mOriginalScreenHeight!=0)
	{
		SetScreenResolution(mOriginalScreenWidth, mOriginalScreenHeight);
		mOriginalScreenWidth = 0;
		mOriginalScreenHeight = 0;
	}
}

// FUNCTION: WON 0x0072c440
Window* WindowManager::GetPopupWindow()
{
	if(mPopupIsUp)
		return mPopupList.front()->GetRoot();
	else
		return NULL;
}

// FUNCTION: WON 0x0072c470
void WindowManager::SetNamedFont(const std::string &theName, Font *theFont)
{
	mNamedFontMap[theName] = theFont;
}

// FUNCTION: WON 0x0072c4d0
Font* WindowManager::GetNamedFont(const std::string &theName)
{
	NamedFontMap::iterator anItr = mNamedFontMap.find(theName);
	if(anItr==mNamedFontMap.end())
		return GetDefaultFont();
	else
		return anItr->second.get();
}

// FUNCTION: WON 0x0072c540
Font* WindowManager::GetDefaultFont()
{
	if(mDefaultFont.get()==NULL)
		{
        mDefaultFont=mDefaultWindow->GetFont(FontDescriptor("Tahoma",0,8));
    }

	return mDefaultFont.get();
}

// FUNCTION: WON 0x0072c700
Font* WindowManager::GetDefaultFontMod(int theStyle, int theSize)
{
	Font *aFont = GetDefaultFont();
	FontDescriptor aDesc = aFont->mDescriptor;
	aDesc.mStyle = theStyle;
	if(theSize>0)
		aDesc.mSize = theSize;

	return mDefaultWindow->GetFont(aDesc).get();
}

// FUNCTION: WON 0x0072c7f0
void WindowManager::SetCacheImages(bool cache)
{
	if(cache)
		mCacheImageCount++;
	else
	{
		mCacheImageCount--;
		if(mCacheImageCount<=0)
		{
			mCacheImageCount = 0;
			mNativeImageMap.clear();
			mRawImageMap.clear();
		}
	}
}

// FUNCTION: WON 0x0072c950
SmartPtr<NativeImage> WindowManager::DecodeImage(DisplayContext *theContext, const char *theFilePath)
{
	if(theContext==NULL)
		return NULL;

	if(mCacheImageCount)
	{
		NativeImageMap::iterator anItr = mNativeImageMap.insert(NativeImageMap::value_type(theFilePath,NULL)).first;
		if(anItr->second.get()==NULL)
			anItr->second = mImageDecoder.Decode(theContext,theFilePath);

		return anItr->second;
	}
	else
		return mImageDecoder.Decode(theContext,theFilePath);
}

// FUNCTION: WON 0x0072cc20
// (??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@QAE@XZ)

// FUNCTION: WON 0x0072ccb0
SmartPtr<NativeImage> WindowManager::DecodeImage(const char *theFilePath)
{
	if(mDefaultWindow==NULL)
		return NULL;

	return DecodeImage(mDefaultWindow->mDisplayContext.get(),theFilePath);
}

// FUNCTION: WON 0x0072ccf0
SmartPtr<Image> WindowManager::DecodeDelayImage(const char *theFilePath)
{
	if(mCacheImageCount)
	{
		std::pair<DelayImageMap::iterator,bool> aRet;
		aRet = mDelayImageMap.insert(DelayImageMap::value_type(theFilePath,NULL));

		if(aRet.second)
		{
			ImageAttributes anAttr;
			if(mImageDecoder.GetImageAttributes(theFilePath,anAttr))
				aRet.first->second = new DelayLoadImage(theFilePath,anAttr.mWidth,anAttr.mHeight);
		}

		return aRet.first->second;
	}
	else
	{
		ImageAttributes anAttr;
		if(mImageDecoder.GetImageAttributes(theFilePath,anAttr))
			return new DelayLoadImage(theFilePath,anAttr.mWidth,anAttr.mHeight);
		else
			return NULL;
	}
}

// FUNCTION: WON 0x0072d0d0
// (??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@QAE@XZ)

// FUNCTION: WON 0x0072d160
SmartPtr<RawImage> WindowManager::DecodeImageRaw(const char *theFilePath)
{
	if(mCacheImageCount)
	{
		RawImageMap::iterator anItr = mRawImageMap.insert(RawImageMap::value_type(std::string(theFilePath),SmartPtr<RawImage>())).first;

		if(anItr->second.get()==NULL)
			anItr->second = mImageDecoder.DecodeRaw(theFilePath);

		return anItr->second;
	}
	else
		return mImageDecoder.DecodeRaw(theFilePath);
}

// FUNCTION: WON 0x0072d400
// (??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@QAE@XZ)

// FUNCTION: WON 0x0072d490
void WindowManager::SetSoundDecoder(SoundDecoder *theDecoder)
{
	mSoundDecoder = theDecoder;
}

// FUNCTION: WON 0x0072d4f0
SmartPtr<Sound> WindowManager::DecodeSound(const SoundDescriptor &theDesc)
{
	if(mSoundDecoder.get()==NULL)
		return new Sound(theDesc);
	else
		return mSoundDecoder->DecodeDescriptor(theDesc);
}

// FUNCTION: WON 0x0072d5b0
void WindowManager::Fn32D5B0()
{
	bool hadWindows = !mWindowSet.empty();
	AutoCrit aCrit(mDataCrit);
	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = anItr->get();
		aWindow->Close();
		++anItr;
	}

	aCrit.Leave();

	if(hadWindows)
		ProcessEvents();

	if(mIsFullScreen)
		RestoreScreenResolution();
}

// FUNCTION: WON 0x0072d6a0
bool WindowManager::AddWindow(Window *theWindow)
{
	AutoCrit aCrit(mDataCrit);
	if(mWindowSet.empty())
		mDefaultWindow = theWindow;

	if(mWindowSet.insert(theWindow).second)
		mWindowCount++;

	return true;
}

// FUNCTION: WON 0x0072d840
bool WindowManager::RemoveWindow(Window *theWindow)
{
	AutoCrit aCrit(mDataCrit);
		
	if(mWindowSet.erase(theWindow)>0)
	{
		mWindowCount--;
		if(mWindowCount<=0)
			mTargetMsgLoopDepth = 0;

		if(mWindowSet.empty())
			mDefaultWindow = NULL;
		else 
			mDefaultWindow = mWindowSet.begin()->get();

		
		WindowEventList::iterator anItr = mWindowEventList.begin();
		while(anItr!=mWindowEventList.end())
		{
			WindowEventBase *anEvent = anItr->get();
			if(anEvent->mWindow==theWindow)
				mWindowEventList.erase(anItr++);
			else
				++anItr;
		}

		if(theWindow==mModalWindow)
			Fn32DDB0(-1,NULL,NULL);

		if(theWindow==mPopupWindowSave.get())
			mPopupWindowSave = NULL;

		return true;
	}

	return false;
}

// FUNCTION: WON 0x0072ddf0
void WindowManager::ExitMessageLoop()
{
	mTargetMsgLoopDepth--;
}

// FUNCTION: WON 0x0072e7d0
int WindowManager::MessageLoop()
{
	if(mWindowSet.empty())
		return -1;


	AddRef();

	mTargetMsgLoopDepth =  ++mCurMsgLoopDepth;
	while(mCurMsgLoopDepth<=mTargetMsgLoopDepth)
	{
		GetNextEvent();

	}


	mCurMsgLoopDepth--;
	int aDialogResult = mDialogResult;
	Release();

	return aDialogResult;
}

// FUNCTION: WON 0x0072e850
void WindowManager::PostEvent(WindowEventBase *theEvent, bool needSignal)
{
	WONAPI::AutoCrit aCrit(mDataCrit);
	mWindowEventList.push_back(theEvent);
	if(mCurMsgLoopDepth<=0)
		needSignal = true;

	aCrit.Leave();

	if(needSignal)
		Signal();
}

// FUNCTION: WON 0x0072e960
void WindowManager::AddTimerEvent(WindowEventBase *theEvent)
{
	mTimerEvents.insert(theEvent);
}

// FUNCTION: WON 0x0072ea70
void WindowManager::RemoveTimerEvent(WindowEventBase *theEvent)
{
	if(mCallingTimers)
		mRemoveTimerEvents.insert(theEvent);
	else
		mTimerEvents.erase(theEvent);
}

// FUNCTION: WON 0x0072f3f0
void WindowManager::SubscribeToBroadcast(Component *theComponent, bool subscribe)
{
	if(subscribe)
		mBroadcastSet.insert(theComponent);
	else
		mBroadcastSet.erase(theComponent);
}

// FUNCTION: WON 0x0072f5d0
void WindowManager::ClearSubscriptions()
{
	mBroadcastSet.clear();
}

// FUNCTION: WON 0x0072f680
void WindowManager::Fn32F680(ComponentEvent *theEvent)
{
	BroadcastSet::iterator anItr = mBroadcastSet.begin();
	while(anItr!=mBroadcastSet.end())
	{
		Component *aComponent = anItr->get();
		aComponent->HandleComponentEvent(theEvent);
		++anItr;
	}
}

// FUNCTION: WON 0x0072f700
WindowEventBase* WindowManager::PeekNextEvent()
{
	AutoCrit aCrit(mDataCrit);
	if(mWindowEventList.empty())
		return NULL;
	else
		return mWindowEventList.front().get();
}

// FUNCTION: WON 0x0072f760
void WindowManager::ProcessEvents()
{

	DWORD aTick = GetTickCount();
	DWORD anElapsed = aTick - mLastTimerTick;
	if(anElapsed >= 50)
	{
		if(anElapsed >= 1000)
			anElapsed = 1000;

		mLastTimerTick = aTick;

		if(mPopupIsUp && mFocusWindow!=mPopupOwnerWindow && ((bool)(((unsigned)mPopupFlags>>6)&1)))
		{
			if(mFocusWindow!=NULL && mFocusWindow->mIsPopup)
			{
				mEndPopup = false;
				mPopupOwnerWindow->Virtual3C();
			}
			else if(mEndPopup)
				EndPopup(NULL);
			else
				mEndPopup = true;
		}
		else
			mEndPopup = false;

		if(mLastLoseFocusTick!=0 && mFocusWindow==NULL && aTick-mLastLoseFocusTick>50)
		{
			mLastLoseFocusTick = 0;
			if(mIsFullScreen)
				RestoreScreenResolution();
		}	

		WindowSet::iterator anItr = mWindowSet.begin();
		while(anItr!=mWindowSet.end())
		{
			Window *aWindow = anItr->get();
			aWindow->Virtual0C(anElapsed);
			++anItr;
		}

		if(!mTimerEvents.empty())
		{
			mCallingTimers = true;
			for(WindowEventSet::iterator aTimerItr = mTimerEvents.begin(); aTimerItr != mTimerEvents.end(); ++aTimerItr)
				(*aTimerItr)->Deliver();
			mCallingTimers = false;
		}

		if(!mRemoveTimerEvents.empty()) 
		{
			for(WindowEventSet::iterator aTimerItr = mRemoveTimerEvents.begin(); aTimerItr != mRemoveTimerEvents.end(); ++aTimerItr)
				mTimerEvents.erase(*aTimerItr);
		}
	}

	WONAPI::AutoCrit aCrit(mDataCrit, false);
	while(!mWindowEventList.empty()  && mCurMsgLoopDepth<=mTargetMsgLoopDepth)
	{
		aCrit.Enter();
		SmartPtr<WindowEventBase> anEvent = mWindowEventList.front();
		mWindowEventList.pop_front();
		aCrit.Leave();

		DeliverEvent(anEvent.get());
	}


	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = anItr->get();
		aWindow->Virtual08();
		++anItr;
	}
}


// FUNCTION: WON 0x0072dbe0
static void SetModal(Component* component,bool modal) {
    component->SetComponentFlags(0x8000,modal);
    Container* parent=component->mParent;
    while(parent!=NULL) {
        parent->SetComponentFlags(0x8000,modal);
        if(modal) parent->EnterModal(); else parent->LeaveModal();
        parent=parent->mParent;
    }
}
// Actual SDK inline rectangle predicate expanded from real target fields; support0.
static inline bool PopupRectContains(const WONRectangle& r,int x,int y) {
    return x>=r.x && x<r.x+r.width && y>=r.y && y<r.y+r.height;
}
// FUNCTION: WON 0x0072da30
void WindowManager::CalcModality()
{
	
	if(mPopupInputComponent.get()!=NULL)
		SetModal(mPopupInputComponent.get(),false);
	if(mModalComponent!=NULL)
		SetModal(mModalComponent,false);

	bool modal = false;
	mPopupIsUp = false;
	if(!mPopupList.empty())
	{
		
		mPopupIsUp = true;
		PopupList::iterator anItr = mPopupList.begin();
		while(anItr!=mPopupList.end())
		{
			Component *aPopup = *anItr;
			aPopup->SetComponentFlags(0x8000 | 0x10000,true);
			++anItr;
		}

		if(((mPopupFlags & (PopupFlag_IsModal))?true:false))
			modal = true;
	}
	
	if(mModalComponent!=NULL)
	{
		if(!modal) 
			SetModal(mModalComponent,true);

		modal = true;
	}

	if(!mPopupIsUp && mPopupOwnerComponent.get()!=NULL)
		mPopupOwnerComponent = NULL;

	if(mPopupInputComponent.get()!=NULL)
	{ 
		if(mPopupIsUp)
			SetModal(mPopupInputComponent.get(),true);
		else
			mPopupInputComponent = NULL;
	}

	WindowSet::iterator anItr = mWindowSet.begin();
	while(anItr!=mWindowSet.end())
	{
		Window *aWindow = anItr->get();
		if(aWindow==mModalWindow && !mPopupIsUp && mModalComponent==aWindow->mRoot.get())
			aWindow->SetModal(false);
		else
			(*anItr)->SetModal(modal);
		
		++anItr;
	}
}

// FUNCTION: WON 0x0072dc30
bool WindowManager::NotModalWindow(Window *theWindow)
{
	if(mModalWindow!=NULL && theWindow!=mModalWindow)
		return true;
	else
		return false;
}

// FUNCTION: WON 0x0072dc50
int WindowManager::DoDialog(Window *theWindow, Component *theComponent)
{
	SmartPtr<Component> anOldDialog = mModalComponent;
	SmartPtr<Window> anOldModalWindow = mModalWindow;
	mDialogResult = 0;

	if(anOldDialog.get()!=NULL)
		anOldDialog->SetComponentFlags(0x8000,false);

	if(theComponent==NULL)
		theComponent = theWindow->mRoot.get();

	if(mModalComponent!=NULL)
		SetModal(mModalComponent,false);

	mModalWindow = theWindow;
	mModalComponent = theComponent;
	CalcModality();
	mModalWindow->Virtual3C();
	theComponent->Fn2F5890();


		MessageLoop();

	SetModal(mModalComponent,false);
	mModalWindow = anOldModalWindow.get();
	mModalComponent = anOldDialog.get();
	CalcModality();




	return mDialogResult;
}

// FUNCTION: WON 0x0072ddb0
void WindowManager::Fn32DDB0(int theResult, Window *theWindow, Component *theDialog)
{
	if(theWindow!=NULL && theWindow!=mModalWindow)
		return;

	if(theDialog!=NULL && theDialog!=mModalComponent)
		return;

	mDialogResult = theResult;
	mTargetMsgLoopDepth--;
}

// FUNCTION: WON 0x0072de00
void WindowManager::AdjustPopupPosition(Window *theParent, Component *theComponent, bool inOwnWindow, const WONRectangle &theAvoidRect)
{
	WONRectangle aBoundingRect;
	WONRectangle anAvoidRect = theAvoidRect;
	if(inOwnWindow)
	{
		GetScreenRect(aBoundingRect);

		WONRectangle aWindowRect;
		theParent->GetRect(aWindowRect, true);








		aBoundingRect.x -= aWindowRect.Left();
		aBoundingRect.y -= aWindowRect.Top();
	}
	else
	{
		RootContainer* aRoot = theParent->mRoot.get();
		aBoundingRect = aRoot->GetBounds();

	}

	int dx = 0, dy = 0;
	if(theComponent->Right() > aBoundingRect.Right())
	{
		dx = aBoundingRect.Right() - theComponent->Right();
		theComponent->Fn2F4B70(theComponent->Left() + dx, theComponent->Top());
		if(anAvoidRect.Width()>0 && theComponent->Left()<=theAvoidRect.Right() && theComponent->Right()>=theAvoidRect.x)
			theComponent->Fn2F4B70(anAvoidRect.Left()-theComponent->Width(),theComponent->Top());
	}
	else if(theComponent->Left() < aBoundingRect.x)
	{
		dx = aBoundingRect.Left() - theComponent->Left();
		theComponent->Fn2F4B70(theComponent->Left() + dx,theComponent->Top());
		if(anAvoidRect.Width()>0 && theComponent->Left()<=theAvoidRect.Right() && theComponent->Right()>=theAvoidRect.x)
			theComponent->Fn2F4B70(anAvoidRect.Right(),theComponent->Top());
	}

	if(theComponent->Bottom() > aBoundingRect.Bottom())
	{
		dy = aBoundingRect.Bottom() - theComponent->Bottom();
		theComponent->Fn2F4B70(theComponent->Left(), theComponent->Top() + dy);
		if(anAvoidRect.Height()>0 && theComponent->Top()<=theAvoidRect.Bottom() && theComponent->Bottom()>=theAvoidRect.y)
			theComponent->Fn2F4B70(theComponent->Left(), anAvoidRect.Top()-theComponent->Height());
	}
	else if(theComponent->Top() < aBoundingRect.y)
	{
		dy = aBoundingRect.Top() - theComponent->Top();
		theComponent->Fn2F4B70(theComponent->Left(), theComponent->Top() + dy);
		if(anAvoidRect.Height()>0 && theComponent->Top()<=theAvoidRect.Bottom() && theComponent->Bottom()>=theAvoidRect.y)
			theComponent->Fn2F4B70(theComponent->Left(), anAvoidRect.Bottom());
	}
}

// FUNCTION: WON 0x0072e040
void WindowManager::DoPopup(const PopupParams &theParams)
{
	
	SmartPtr<Window> aParent = theParams.mParent;
	SmartPtr<Component> aPopup = theParams.mPopup;

	if(mInDoPopup || mInEndPopup)
		return;

	bool additional = (theParams.mFlags&PopupFlag_Additional) && !mPopupList.empty();
	Window *aFocusCheckWindow = aParent.get();
	if(additional)
		aFocusCheckWindow = mPopupOwnerWindow;


	if(mFocusWindow!=aFocusCheckWindow)
		return;

	mInDoPopup = true;
	if(mPopupIsUp && !additional)
		EndPopup(NULL);

	int x = theParams.mx;
	int y = theParams.my;
	if(x==-1)
		x = aParent->mMouseX;
	if(y==-1)
		y = aParent->mMouseY;

	aPopup->Fn2F4B70(x,y);

	mEndPopup = false;

	if(!additional)
	{
		mPopupFlags = theParams.mFlags;
		mPopupOwnerWindow = aParent.get();
		mPopupInputComponent = theParams.mInputComponent;
	}

	mPopupList.push_back(aPopup.get());
	bool inOwnWindow = mBA5;
	if(((mPopupFlags & (PopupFlag_InOwnWindow))?true:false))
		inOwnWindow = true;
	else if(((mPopupFlags & (PopupFlag_NotInOwnWindow))?true:false))
		inOwnWindow = false;

	if(((mPopupFlags & (PopupFlag_AdjustPosition))?true:false))
		AdjustPopupPosition(aParent.get(),aPopup.get(),inOwnWindow,theParams.mAvoidRect);

	SmartPtr<Window> aPopupWindow;
	if(inOwnWindow)
	{
		WONRectangle aRect;
		aParent->GetRect(aRect,true);
		aRect.x+=aPopup->Left();
		aRect.y+=aPopup->Top();
		aRect.width = aPopup->Width();
		aRect.height = aPopup->Height();

		if(mPopupWindowSave.get()!=NULL)
		{
			if(mPopupWindowSave->mParent!=aParent.get())
				mPopupWindowSave->Close();
			else
			{
				mPopupWindowSave->SetPosSize(aRect.x, aRect.y, aRect.width, aRect.height);
				mPopupWindowSave->mIsPopup = true;
				mPopupWindowSave->Show(true);
				aPopupWindow = mPopupWindowSave;
			}
			mPopupWindowSave = NULL;
		}
				
		if(aPopupWindow.get()==NULL)
		{
			aPopupWindow = new MSWindow;
		// LOST-DEBUG: none: exact only
		EE_LOST_DEBUG(3);
			aPopupWindow->mIsOneComponentWindow = true;
			aPopupWindow->mStyle=0x20;
			aPopupWindow->SetParent(aParent.get());
			aPopupWindow->mIsPopup = true;

			aPopupWindow->Create(aRect);
		}
		aPopupWindow->mIsPopup = true;

		if(((mPopupFlags & (PopupFlag_WantInput))?true:false))
		{
			mPopupOwnerWindow->Virtual3C();	
			mPopupOwnerWindow->mRoot->Virtual4C();
		}

		aPopup->Fn2F4B70(0,0);
	}
	else
	{
		aPopupWindow = aParent;
		aPopup->SetComponentFlags(0x100,true);
		if(((mPopupFlags & (PopupFlag_DontWantMouse))?true:false))
			aPopup->SetComponentFlags(0x800,true);
	}

	mPopupOwnerComponent = theParams.mOwnerComponent;
	if(theParams.mOwnerComponent!=NULL && aPopup->GetOwnColorScheme()==NULL)
		aPopup->Fn2F59A0(theParams.mOwnerComponent->GetColorScheme());

	aPopup->SetComponentFlags(0x1000,true);
	aPopupWindow->AddComponent(aPopup.get());
	if(((mPopupFlags & (PopupFlag_WantInput))?true:false))
		aPopup->Fn2F5890();

	if(mDoMouseCapturePopupSupport)
		mPopupOwnerWindow->Virtual44(true);

	mInDoPopup = false;
	CalcModality();
}

// FUNCTION: WON 0x0072e4d0
void WindowManager::EndPopup(Component *thePopup)
{
	if(mPopupList.empty())
		return;

	if(thePopup!=NULL && !thePopup->ComponentFlagSet(0x10000))
		return;

	bool inEndPopup = mInEndPopup;
	mInEndPopup = true; 
	while(!mPopupList.empty() && mPopupList.back()!=thePopup)
		EndPopup(mPopupList.back());

	if(mPopupList.empty())
	{
		mInEndPopup = inEndPopup;
		return;
	}

	mEndPopup = false;

	if(mPopupIsUp)
	{
		SmartPtr<Component> aPopupComponent = mPopupList.back();
		SmartPtr<Window> aPopupWindow = aPopupComponent->GetRoot();
		SmartPtr<Window> aPopupOwnerWindow = mPopupOwnerWindow;
		mPopupList.pop_back();

		if(mPopupFlags&PopupFlag_FireEventOnEnd)
			aPopupComponent->Fn2F5380(30,0);

		if(mPopupList.empty())
		{
			if(mDoMouseCapturePopupSupport)
				mPopupOwnerWindow->Virtual44(false);

			mPopupOwnerWindow = NULL;
			mMouseDownWindow = NULL;
		}

		if(aPopupWindow.get()!=aPopupOwnerWindow.get())
		{
			aPopupWindow->mIsPopup = false; 
			if(mPopupList.empty())
				mPopupWindowSave = aPopupWindow;

			aPopupWindow->mRoot->RemoveAllChildren();
			if(mPopupWindowSave.get()!=NULL)
			{
				aPopupWindow->mRoot->RemoveAllChildren();
				if(!mInDoPopup)
					aPopupWindow->Show(false);
			}
			else
				aPopupWindow->Close();
			
			if(mPopupList.empty() && ((mPopupFlags & (PopupFlag_WantInput))?true:false))
			{
				if(mFocusWindow==aPopupOwnerWindow.get())
					aPopupOwnerWindow->mRoot->Virtual48();
			}
		}
		else
		{
			aPopupComponent->SetComponentFlags(0x8000 | 0x10000,false);
			aPopupWindow->mRoot->VirtualC4(aPopupComponent.get());
		}

		CalcModality();
	}

	mInEndPopup = inEndPopup;
}

// FUNCTION: WON 0x0072e790
void WindowManager::CheckEndPopup(Component *theAncestor)
{
	if(!mPopupIsUp || mPopupOwnerComponent.get()==NULL)
		return;

	Component *aComponent = mPopupOwnerComponent.get();
	while(aComponent!=NULL)
	{
		if(aComponent==theAncestor)
		{
			EndPopup(NULL);
			return;
		}
		aComponent = aComponent->mParent;
	}
}

// FUNCTION: WON 0x0072ec60
void WindowManager::FindMouseWindow(MouseEvent *theEvent)
{
	if(!mPopupIsUp)
		return;

	if(((mPopupFlags & (PopupFlag_DontWantMouse))?true:false) && theEvent->mWindow->mIsPopup)
	{
		WONRectangle aPopupRect, anOwnerRect;
		theEvent->mWindow->GetRect(aPopupRect,true);
		mPopupOwnerWindow->GetRect(anOwnerRect,true);
		anOwnerRect.x -= aPopupRect.x;
		anOwnerRect.y -= aPopupRect.y;
		if(PopupRectContains(anOwnerRect,theEvent->mx,theEvent->my))
		{
			theEvent->mWindow = mPopupOwnerWindow;
			theEvent->mx -= anOwnerRect.x;
			theEvent->my -= anOwnerRect.y;
			return;
		}
	}

	if(!mDoMouseCapturePopupSupport)
		return;

	WONRectangle anOwnerRect;
	theEvent->mWindow->GetRect(anOwnerRect,true);
	if(mMouseDownWindow!=NULL && mMouseDownWindow->mMouseIsDown[0])
	{
		WONRectangle aRect;
		mMouseDownWindow->GetRect(aRect,true);
		aRect.x -= anOwnerRect.x;
		aRect.y -= anOwnerRect.y;

		theEvent->mWindow = mMouseDownWindow;
		theEvent->mx -= aRect.x;
		theEvent->my -= aRect.y;
		return;
	}

	PopupList::iterator anItr  = mPopupList.end();
	do
	{
		WONRectangle aRect;

		--anItr;
		Window *aWindow = (*anItr)->GetRoot();
		aWindow->GetRect(aRect,true);
		aRect.x -= anOwnerRect.x;
		aRect.y -= anOwnerRect.y;

		if(PopupRectContains(aRect,theEvent->mx,theEvent->my))
		{
			theEvent->mWindow = aWindow;
			theEvent->mx -= aRect.x;
			theEvent->my -= aRect.y;
			return;
		}
	} while(anItr!=mPopupList.begin());
}

// FUNCTION: WON 0x0072ee40
bool WindowManager::CheckPopupClick(MouseEvent *theEvent)
{
	FindMouseWindow(theEvent);
	if(theEvent->mButtonOrAmount==0)
		mMouseDownWindow = theEvent->mWindow;

	if(((mPopupFlags & (1<<theEvent->mButtonOrAmount))?true:false)) 
	{
		int x = theEvent->mx, y = theEvent->my;
		Container *aContainer = theEvent->mWindow->mRoot.get();
		bool endPopup = true;
		while(true)
		{
			x-=aContainer->Left();
			y-=aContainer->Top();

			Component *aClickChild = aContainer->VirtualB0(x, y);
			if(aClickChild!=NULL)
			{ 
				if(aClickChild==mPopupInputComponent.get() || ((aClickChild->mComponentBits & 0x10000)?true:false))
				{
					endPopup = false;
					break;
				}
				else if(mPopupInputComponent.get()==NULL)
					break;
				else
				{
					aContainer = dynamic_cast<Container*>(aClickChild);
					if(aContainer==NULL)
						break;
				}
			}
			else
				break;
		}

		if(endPopup)
		{
			bool eat = ((mPopupFlags & (PopupFlag_EatMouseDownOnEnd))?true:false);
			EndPopup(NULL);
			if(eat)
				return true;
		}
	}

	if(theEvent->mWindow->mIsPopup)
		mPopupOwnerWindow->Virtual3C();	

	return false;
}

// FUNCTION: WON 0x0072ef20
void WindowManager::DeliverEvent(WindowEventBase *theEvent)
{
	switch(theEvent->mL08)
	{
		case InputEvent_MouseDoubleClick:
		case InputEvent_MouseDownCheckDoubleClick:
		case InputEvent_MouseDown:
		{
			MouseEvent *anEvent = (MouseEvent*)theEvent;
			if(mPopupIsUp)
			{
				if(CheckPopupClick(anEvent))
					break;
			}
			if(mFocusWindow!=anEvent->mWindow && !anEvent->mWindow->mIsPopup && !NotModalWindow(anEvent->mWindow))
				anEvent->mWindow->Virtual3C();

			if(anEvent->mButtonOrAmount==0 && (!mPopupIsUp || !mDoMouseCapturePopupSupport))
				anEvent->mWindow->Virtual44(true);
		}

		if(theEvent->mWindow->mMouseIsInClientArea) 
			theEvent->mWindow->HandleMouseEvent((MouseEvent*)theEvent);
		break;

		case InputEvent_MouseMove:
		{
			WindowEventBase *nextEvent = PeekNextEvent();
			if(nextEvent!=NULL && nextEvent->mL08==InputEvent_MouseMove && nextEvent->mWindow==theEvent->mWindow)
				return; 

			FindMouseWindow((MouseEvent*)theEvent);
			if(mPopupIsUp && theEvent->mL08==InputEvent_MouseMove && ((mPopupFlags & (PopupFlag_EndOnMouseMove))?true:false))
				EndPopup(NULL);

			theEvent->mWindow->HandleMouseEvent((MouseEvent*)theEvent);
			break;
		}

		case InputEvent_MouseUp:
		{
			MouseEvent *anEvent = (MouseEvent*)theEvent;
			FindMouseWindow(anEvent);
			if(anEvent->mButtonOrAmount==0)
				mMouseDownWindow = NULL;

			if(anEvent->mButtonOrAmount==0 && (!mPopupIsUp || !mDoMouseCapturePopupSupport))
				theEvent->mWindow->Virtual44(false);

			theEvent->mWindow->HandleMouseEvent((MouseEvent*)theEvent);
			break;
		}

		case InputEvent_MouseWheel:
		{
			MouseEvent *anEvent = (MouseEvent*)theEvent;
			FindMouseWindow(anEvent);
			if(mPopupIsUp && !((mPopupFlags & (PopupFlag_DontWantMouse))?true:false))
			{
				
				Component *aPopupComponent = mPopupList.back();
				aPopupComponent->Virtual3C(anEvent->mButtonOrAmount);
				return;


			}
			theEvent->mWindow->HandleMouseEvent(anEvent);
			break;
		}

		case InputEvent_KeyDown:
		case InputEvent_KeyUp:
		case InputEvent_KeyChar:
			if(mPopupIsUp)
			{
				int aKey = ((KeyEvent*)theEvent)->mKey;
				if(theEvent->mL08==InputEvent_KeyDown)
				{	
					if(((mPopupFlags & (PopupFlag_EndOnKeyDown))?true:false) || ((mPopupFlags & (PopupFlag_EndOnEscape))?true:false) && aKey==VK_ESCAPE)
					{
						EndPopup(NULL);
						break;
					}
				}
				
				if(((mPopupFlags & (PopupFlag_WantInput))?true:false))
				{
					Window *aPopupWindow = mPopupList.back()->GetRoot();
					if(aPopupWindow!=theEvent->mWindow)
						theEvent->mWindow = aPopupWindow;
				}
			}
			theEvent->mWindow->HandleKeyEvent((KeyEvent*)theEvent);
			break;

		case InputEvent_GotFocus:
			if(mIsFullScreen)
				SetScreenResolution(mFullScreenWidth,mFullScreenHeight);

			NotifyGotFocus(theEvent->mWindow);
			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			if(NotModalWindow(theEvent->mWindow))
				mModalWindow->Virtual3C();
			return;

		case InputEvent_LostFocus:
			NotifyLostFocus();
			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			return;

		case InputEvent_SizeChange:
			if(theEvent->mWindow==mPopupOwnerWindow)
				EndPopup(NULL);			
			else
			{
				WindowEventBase *nextEvent = PeekNextEvent();
				if(nextEvent!=NULL && nextEvent->mL08==InputEvent_SizeChange && nextEvent->mWindow==theEvent->mWindow)
				{
					SizeEvent *s1 = (SizeEvent*)theEvent;
					SizeEvent *s2 = (SizeEvent*)nextEvent;
					if(s2->mx==-1)
						s2->mx = s1->mx;
					if(s2->my==-1)
						s2->my = s1->my;
					if(s2->mWidth==-1)
						s2->mWidth = s1->mWidth;
					if(s2->mHeight==-1)
						s2->mHeight = s1->mHeight;
					return; 
				}
			}

			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			return;


		case InputEvent_MouseExit:
			theEvent->mWindow->HandleInputEvent((InputEvent*)theEvent);
			return;

		case InputEvent_WindowClose:
		{
			if(mPopupIsUp)
				if(theEvent->mWindow==mPopupOwnerWindow || theEvent->mWindow->mIsPopup)
					EndPopup(NULL);




			if(mFocusWindow==theEvent->mWindow)
				NotifyLostFocus();

			if(mMouseDownWindow==theEvent->mWindow)
				mMouseDownWindow = NULL;

			theEvent->mWindow->Virtual2C();
			SmartPtr<WindowEventBase> aCloseEvent = theEvent->mWindow->mCloseEvent;
			if(aCloseEvent.get()!=NULL)
			{
				aCloseEvent->mWindow = theEvent->mWindow;
				aCloseEvent->Deliver();
			}

			RemoveWindow(theEvent->mWindow);
		}
		return;


		default:
			theEvent->Deliver();
			return;
	}

	mLastInputTick = GetTickCount();
}

// Genuine SDK WindowManager.cpp primitive class static definitions; no dynamic initializer.
WindowManager* WindowManager::mDefaultWindowManager = NULL;
DWORD WindowManager::mThreadId = 0;

// Genuine same-library SDK bodies; no new Native identity is asserted.
Font* WindowManager::GetNamedFontMod(const std::string& theName, int theStyle, int theSize)
{
    Font* aFont = GetNamedFont(theName);
    FontDescriptor aDesc = aFont->GetDescriptor();
    aDesc.mStyle = theStyle;
    if(theSize > 0)
        aDesc.mSize = theSize;

    return mDefaultWindow->GetFont(aDesc).get();
}

SmartPtr<Font> WindowManager::GetDefaultFont(const FontDescriptor& theDescriptor)
{
    Window* aWindow = Window::GetDefaultWindow();
    if(aWindow == NULL)
        return NULL;
    else
        return aWindow->GetFont(theDescriptor);
}

DisplayContext* WindowManager::GetDefaultDisplayContext()
{
    Window* aWindow = Window::GetDefaultWindow();
    if(aWindow == NULL)
        return NULL;
    else
        return aWindow->GetDisplayContext();
}

bool WindowManager::CheckModalValid()
{
    if(mModalWindow != NULL)
    {
        if(mModalWindow->mWindowState != Window::WindowState_Open)
            return false;

        if(!mModalWindow->Virtual4C())
            return false;

        if(mModalComponent != NULL)
        {
            if(!mModalComponent->IsVisible())
                return false;

            if(mModalComponent->Fn2F58F0() != mModalWindow->mRoot.get())
                return false;
        }
    }

    return true;
}

void WindowManager::RemoveAllTimerEvents()
{
    if(mCallingTimers)
        mRemoveTimerEvents = mTimerEvents;
    else
        mTimerEvents.clear();
}

// Genuine SDK event-pump dispatcher; Native attribution remains separate.
void WindowManager::PumpOnce()
{
	GetNextEvent();
}

// FUNCTION: WON 0x0072fc40
// (??1?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@QAE@XZ)

// FUNCTION: WON 0x0072fcd0
// (?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAE?AViterator@12@_NPAU_Node@12@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@2@@Z)

// FUNCTION: WON 0x0072ffd0
// (?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAE?AViterator@12@_NPAU_Node@12@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@2@@Z)

// FUNCTION: WON 0x007302d0
// (?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAE?AViterator@12@_NPAU_Node@12@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@2@@Z)

// FUNCTION: WON 0x007305d0
// (?erase@?$_Tree@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@std@@@std@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@V312@0@Z)

// FUNCTION: WON 0x00730680
// (?equal_range@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AU?$pair@Viterator@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@V123@@2@ABV?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@Z)

// FUNCTION: WON 0x007306e0
// (?_Insert@?$_Tree@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@std@@@std@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@5@$0A@@std@@IAE?AViterator@12@_NPAU_Node@12@ABV?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@Z)

// FUNCTION: WON 0x007309e0
// (?_Inc@const_iterator@?$_Tree@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@$0A@@std@@QAEXXZ)

// FUNCTION: WON 0x00730a30
// (?erase@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@V312@0@Z)

// FUNCTION: WON 0x00730ae0
// (?_Insert@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@IAE?AViterator@12@_NPAU_Node@12@ABV?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@Z)

// FUNCTION: WON 0x00730de0
// (?erase@?$_Tree@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@V312@0@Z)

// FUNCTION: WON 0x00730e90
// (?_Insert@?$_Tree@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@$0A@@std@@IAE?AViterator@12@_NPAU_Node@12@ABV?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@Z)

// FUNCTION: WON 0x007318f0
// (??0?$pair@Viterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$_Mapk@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@_N@std@@QAE@ABViterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$_Mapk@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@1@AB_N@Z)

// FUNCTION: WON 0x00731a60
// (?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE?AViterator@12@V312@@Z)

// FUNCTION: WON 0x00731f80
// (?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAEXPAU_Node@12@@Z)

// FUNCTION: WON 0x00731fc0
// (?_Insert@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAE?AViterator@12@_NPAU_Node@12@ABU?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@2@@Z)

// FUNCTION: WON 0x007322e0
// (?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE?AViterator@12@V312@@Z)

// FUNCTION: WON 0x00732800
// (?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAEXPAU_Node@12@@Z)

// FUNCTION: WON 0x00732840
// (?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE?AViterator@12@V312@@Z)

// FUNCTION: WON 0x00732d60
// (?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAEXPAU_Node@12@@Z)

// FUNCTION: WON 0x00732da0
// (?erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAE?AViterator@12@V312@@Z)

// FUNCTION: WON 0x007332c0
// (?_Erase@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@IAEXPAU_Node@12@@Z)

// FUNCTION: WON 0x00733300
// (?erase@?$_Tree@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@std@@@std@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@V312@@Z)

// FUNCTION: WON 0x007337f0
// (?_Erase@?$_Tree@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@std@@@std@@UComp@12@V?$allocator@V?$SmartPtr@VWindow@WONAPI@@@WONAPI@@@5@$0A@@std@@IAEXPAU_Node@12@@Z)

// FUNCTION: WON 0x00733830
// (?_Incsize@?$list@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@@std@@IAEXI@Z)

// FUNCTION: WON 0x00733980
// (?erase@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@V312@@Z)

// FUNCTION: WON 0x00733e70
// (?_Erase@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@IAEXPAU_Node@12@@Z)

// FUNCTION: WON 0x00733eb0
// (?erase@?$_Tree@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@V12@U_Kfn@?$set@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@V312@@Z)

// FUNCTION: WON 0x007343a0
// (?lower_bound@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@ABV?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@Z)

// FUNCTION: WON 0x007343e0
// (?upper_bound@?$_Tree@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@5@$0A@@std@@QAE?AViterator@12@ABV?$SmartPtr@VWindowEventBase@WONAPI@@@WONAPI@@@Z)

// FUNCTION: WON 0x00734420
// (?_Erase@?$_Tree@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@V12@U_Kfn@?$_Setk@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@std@@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@4@@std@@U?$less@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@V?$allocator@V?$SmartPtr@VComponent@WONAPI@@@WONAPI@@@5@$0A@@std@@IAEXPAU_Node@12@@Z)

// FUNCTION: WON 0x00734f70
// (?_Inc@const_iterator@?$_Tree@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@2@U_Kfn@?$map@V?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@UStringLessNoCase@4@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@@2@UStringLessNoCase@WONAPI@@V?$allocator@U?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@@2@$0A@@std@@QAEXXZ)

// FUNCTION: WON 0x007352a0
// (??_G?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VFont@WONAPI@@@WONAPI@@@std@@QAEPAXI@Z)

// FUNCTION: WON 0x007352c0
// (??_G?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VNativeImage@WONAPI@@@WONAPI@@@std@@QAEPAXI@Z)

// FUNCTION: WON 0x007352e0
// (??_G?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VRawImage@WONAPI@@@WONAPI@@@std@@QAEPAXI@Z)

// FUNCTION: WON 0x00735300
// (??_G?$pair@$$CBV?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@V?$SmartPtr@VImage@WONAPI@@@WONAPI@@@std@@QAEPAXI@Z)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0072fab0
// (public: class WONAPI::SmartPtr<class WONAPI::Font> & __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocat)
// TEMPLATE: WON 0x00735010
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WON)
// TEMPLATE: WON 0x00735090
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WON)
// TEMPLATE: WON 0x00735110
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WON)
// TEMPLATE: WON 0x00735220
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WON)
