// WONGUI MSWindow native behavior; archived SDK corroborates source contract.
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
// FUNCTION: WON 0x007d0d90
MSWindow::MSWindow()
{
	mLB8 = NULL;
	mLBC = 0;
	mBC4 = false;
	mBC5 = false;
}

// FUNCTION: WON 0x007d0de0
MSWindow::~MSWindow()
{
	if(mLB8!=NULL)
	{
		SetWindowLongA(mLB8, GWL_USERDATA, 0);
		DestroyWindow(mLB8);
	}
}

// FUNCTION: WON 0x007d0e70
SmartPtr<Font> MSWindow::Virtual04(const FontDescriptor &theDescriptor)
{
	return new MSFont(mDisplayContext.get(),theDescriptor);
}

// FUNCTION: WON 0x007d11d0
void MSWindow::Virtual2C()
{
	Window::Virtual2C();

	if(mLB8!=NULL)
	{
		DestroyWindow(mLB8);
		mLB8 = NULL;
	}
}

// FUNCTION: WON 0x007d1200
void MSWindow::Virtual10()
{
	std::string aTitle = mTitle;
	SetWindowTextA(mLB8,aTitle.c_str());
}

// FUNCTION: WON 0x007d1300
void MSWindow::Virtual14()
{
	HICON anIcon = NULL;
	if(mIcon.get()!=NULL)
		anIcon = ((MSNativeIcon*)mIcon.get())->mIcon;

	SendMessageA(mLB8,WM_SETICON,(WPARAM)ICON_SMALL,(LPARAM)anIcon);
}

// FUNCTION: WON 0x007d1330
void MSWindow::SetPosSize(int x, int y, int width, int height)
{
	if(mWindowState!=WindowState_Open)
		return;

	if(width==-1 || height==-1)
	{
		WONRectangle aRect;
		GetRect(aRect,false);
		if(width==-1)
			width = aRect.width;

		if(height==-1)
			height = aRect.height;
	}

	MoveWindow(mLB8, x, y, width, height, TRUE);	
}

// FUNCTION: WON 0x007d13a0
void MSWindow::GetRect(WONRectangle &theRectR, bool clientArea)
{
	RECT aRect;
	if(clientArea)
	{
		GetClientRect(mLB8, &aRect);
		POINT aPoint = { aRect.left, aRect.top };
		ClientToScreen(mLB8,&aPoint);
		theRectR.x = aPoint.x;
		theRectR.y = aPoint.y;
	}
	else
	{
		GetWindowRect(mLB8,&aRect);
		theRectR.x = aRect.left;
		theRectR.y = aRect.top;
	}

	theRectR.width = aRect.right - aRect.left;
	theRectR.height = aRect.bottom - aRect.top;
}

// FUNCTION: WON 0x007d1430
void MSWindow::Virtual3C()
{
	::SetFocus(mLB8);
}

// FUNCTION: WON 0x007d1440
void MSWindow::Virtual20(bool show)
{
	int aCmd = SW_SHOW;
	if(mIsPopup)
		aCmd = SW_SHOWNA;

	if(!show)
		aCmd = SW_HIDE;


	::ShowWindow(mLB8, aCmd);
}

// FUNCTION: WON 0x007d1470
void MSWindow::Virtual24(bool minimize)
{
	int aCmd = minimize?SW_MINIMIZE:SW_RESTORE;
	::ShowWindow(mLB8, aCmd);
}

// FUNCTION: WON 0x007d1490
void MSWindow::Virtual28(bool topMost)
{
	if (topMost)
		::SetWindowPos(mLB8, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	else
		::SetWindowPos(mLB8, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

// FUNCTION: WON 0x007d14f0
void MSWindow::Virtual1C(bool show)
{
	if(!show)
		SetCursor(NULL);

}

// FUNCTION: WON 0x007d1510
bool MSWindow::Virtual4C()
{
	return ::IsWindowVisible(mLB8)?true:false;
}

// FUNCTION: WON 0x007d1670
void MSWindow::Virtual44(bool on)
{
	if(on)
		SetCapture(mLB8);
	else
		ReleaseCapture();
}
// FUNCTION: WON 0x007d2150
bool MSWindow::Virtual08() {
    if(!Window::Virtual08()) return false;
    if(mRoot->mInvalidState!=0) {
        const WONRectangle& r=mRoot->mInvalidRect;
        RECT rect={r.x,r.y,r.x+r.width,r.y+r.height};
        ::InvalidateRect(mLB8,&rect,FALSE);
    }
    return true;
}
// FUNCTION: WON 0x007d21d0
void MSWindow::Virtual0C(DWORD elapsed) {
    Window::Virtual0C(elapsed);
    if(mMouseIsInClientArea) {
        POINT point;GetCursorPos(&point);
        HWND hwnd=WindowFromPoint(point);
        ScreenToClient(mLB8,&point);
        if(hwnd!=mLB8 || !mRoot->Contains(point.x,point.y)) {
            if(hwnd!=mLB8) {
                MSWindow* popup=(MSWindow*)mManager->GetPopupWindow();
                if(popup!=NULL && (mManager->mPopupFlags&0x4000)!=0 && popup->mLB8==hwnd) return;
            }
            Virtual48(new InputEvent(13));
        }
    }
}
// FUNCTION: WON 0x007d22c0
void MSWindow::Virtual54() {
    mDisplayContext=new DisplayContext(::GetDC(mLB8));
    mOffscreenImage=CreateImage(mRoot->Width(),mRoot->Height());
    SetTimer(mLB8,0,50,NULL);
}
// FUNCTION: WON 0x007d23f0
void MSWindow::Virtual50() {
    PAINTSTRUCT ps;BeginPaint(mLB8,&ps);
    const RECT& paint=ps.rcPaint;
    if(mRoot->mInvalidState!=0) {
        const WONRectangle& r=mRoot->mInvalidRect;
        RECT invalid={r.x,r.y,r.x+r.width,r.y+r.height};
        if(invalid.left<paint.left || invalid.top<paint.top || invalid.right>paint.right || invalid.bottom>paint.bottom)
            ::InvalidateRect(mLB8,&invalid,FALSE);
        mRoot->Virtual14(mOffscreenImage->GetGraphics());
        mRoot->Paint(mOffscreenImage->GetGraphics());
    }
    mOffscreenImage->Draw(mDisplayContext.get(),0,0,0,0,mOffscreenImage->mWidth,mOffscreenImage->mHeight);
    EndPaint(mLB8,&ps);
}

#include <imm.h>
#include <stdio.h>
#include <stdlib.h>
// Native566b9c actual default owner HWND; source setter support0.
static HWND gMSWindowDefaultOwnerHWND = NULL;
// FUNCTION: WON 0x007d08c0
void WONAPI::LobbyFn3D08C0(HWND owner) { gMSWindowDefaultOwnerHWND=owner; }

// FUNCTION: WON 0x007d0f00
void MSWindow::Create(const WONRectangle& theWindowRectR)
{
	Window::Create(theWindowRectR);
	CalcCodePage();

	static int aCreateCount = 0;
	char aBuf[50];
	sprintf(aBuf,"WndClass%d",aCreateCount++);

	WNDCLASS wc;
	wc.lpszClassName = aBuf;
	wc.lpfnWndProc = StaticWindowProc;
	wc.style = CS_OWNDC | CS_DBLCLKS; 
	wc.hInstance = ((MSWindowManager*)mManager)->mInstance;
	wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	RegisterClass( &wc );

	HWND aParent = NULL;
	int aStyle = WS_OVERLAPPEDWINDOW;
	int aStyleEx = 0;

	if(mParent!=NULL)
	{
		aParent = ((MSWindow*)mParent)->mLB8;
		aStyle |= WS_POPUPWINDOW;

	}
	else
		aParent = gMSWindowDefaultOwnerHWND;

	if(mStyle & 0x20)
	{
		aStyle = WS_POPUP;

	}

	if(mStyle & 0x04) 
		aStyle &= ~(WS_SYSMENU);

	if(mStyle & 0x08)
		aStyle &= ~(WS_MINIMIZEBOX);

	if(mStyle & 0x10)
		aStyle &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);

	int x = theWindowRectR.x, y = theWindowRectR.y;
	if(mStyle&0x80)
		x = y = CW_USEDEFAULT;


	std::string aTitle = mTitle;
	HWND aWnd = CreateWindowEx(
		aStyleEx,
		aBuf,
		aTitle.c_str(),
		aStyle,
		x,
		y,
		theWindowRectR.width,
		theWindowRectR.height,
		aParent,
		NULL,
		((MSWindowManager*)mManager)->mInstance,
		this
   );



	if(aWnd && (mStyle & 0x40) && !(mStyle & 0x20))
	{
		
		
		WONRectangle aClientRect;
		WONRectangle aScreenRect;
		GetRect(aScreenRect,false);
		GetRect(aClientRect,true);

		int aWidthDelta = theWindowRectR.width - aClientRect.width;
		int aHeightDelta = theWindowRectR.height - aClientRect.height;





		int aTargetWidth = aScreenRect.width + aWidthDelta;
		int aTargetHeight = aScreenRect.height + aHeightDelta;

		SetPosSize(aScreenRect.x, aScreenRect.y, aTargetWidth, aTargetHeight);
	}

	if(mStyle&0x01)
		Center(true);



	if(!(mStyle&0x02))
	{
		mIsShowing = true;
		Virtual20(true);
	}
	else
		mIsShowing = false;

	if(mIcon.get()!=NULL)
		Virtual14();
}

// Requires actual D call contracts; include <imm.h> in owning TU.

// Native3d2120 support0: actual free cdecl forwarding helper.
static void SendMsgToParent(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam) {
    HWND parent=::GetParent(hwnd);
    if(parent!=NULL) ::PostMessage(parent,msg,wParam,lParam);
}

// FUNCTION: WON 0x007d0e20
void MSWindow::CalcCodePage()
{
	char aBuf[32];
	unsigned short aLangId = ((WORD)GetKeyboardLayout(0))&0xffff;
	GetLocaleInfo(MAKELCID(aLangId,SORT_DEFAULT),LOCALE_IDEFAULTANSICODEPAGE,aBuf,32);
	mCodePage = atoi(aBuf);
	if(mCodePage==0)
		mCodePage = CP_ACP;
}

// FUNCTION: WON 0x007d1530
LRESULT CALLBACK MSWindow::StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	MSWindow *aWindow = (MSWindow*)GetWindowLong(hwnd, GWL_USERDATA);
	if(aWindow==NULL)
	{
		if(msg==WM_CREATE || msg==WM_NCCREATE)
		{
			CREATESTRUCT *aStruct = (CREATESTRUCT*)lParam;
			aWindow = (MSWindow*)aStruct->lpCreateParams;
			aWindow->mLB8 = hwnd;
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)aWindow);
		}
		else
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
		
	return aWindow->WindowProc(hwnd, msg, wParam, lParam);
}

// FUNCTION: WON 0x007d15a0
void MSWindow::MyGetKeyState()
{
	int aState;
	 
	aState = (GetKeyState(VK_SHIFT)>>15)&1;
	mLBC = aState;

	aState = (GetKeyState(VK_MENU)>>15)&1;
	mLBC |= (aState<<1);

	aState = (GetKeyState(VK_CONTROL)>>15)&1;
	mLBC |= (aState<<2);
}

// FUNCTION: WON 0x007d1600
void MSWindow::PostMouseEvent(WindowEventType theType, int theButtonOrWheelAmount, LPARAM lParam)
{
	Virtual48(new MouseEvent(theType, (short)LOWORD(lParam), (short)HIWORD(lParam), mLBC, theButtonOrWheelAmount));
}

// FUNCTION: WON 0x007d16a0
void MSWindow::DoMinimizeOnAltTab()
{
	HWND aWnd = mLB8;
	HWND aParent = ::GetParent(mLB8);
	while(aParent!=NULL)
	{
		aWnd = aParent;
		aParent = ::GetParent(mLB8);
	}
	
	ShowWindow(aWnd, SW_MINIMIZE);
}

// FUNCTION: WON 0x007d16e0
LRESULT MSWindow::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	
	switch(msg)
	{
		case WM_ACTIVATE:
			if(LOWORD(wParam)!=WA_INACTIVE && mManager->NotModalWindow(this))
			{
				MSWindow *aModalWindow = (MSWindow*)mManager->mModalWindow;

				ShowWindow(aModalWindow->mLB8,SW_SHOW);
				return 0;
			}
			else
				return DefWindowProc(hwnd,msg,wParam,lParam);
			break;

		case WM_CLOSE:
			Close();
			return 1;

		case WM_MOUSEACTIVATE:
			if(mIsPopup)
				return MA_NOACTIVATE;
			else
				return DefWindowProc(hwnd,msg,wParam,lParam);
			break;

		case WM_NCLBUTTONDOWN:
		case WM_NCMBUTTONDOWN:
		case WM_NCRBUTTONDOWN:
		case WM_NCLBUTTONDBLCLK:
		case WM_NCMBUTTONDBLCLK:
		case WM_NCRBUTTONDBLCLK:
			mManager->EndPopup(NULL);
			if(mManager->NotModalWindow(this))
				return 0;
			else
				return DefWindowProc(hwnd,msg,wParam,lParam);
			break;

		case WM_CREATE:
			Virtual54();
		break;
		case WM_SYSCOLORCHANGE:


			break;

		case WM_SIZE:
			if(wParam==SIZE_RESTORED || wParam==SIZE_MAXIMIZED)
				Virtual48(new SizeEvent(-1,-1,(short)LOWORD(lParam), (short)HIWORD(lParam)));
			break;

		case WM_MOVE:
			Virtual48(new SizeEvent((short)LOWORD(lParam), (short)HIWORD(lParam),-1,-1));
			break;

		case WM_MOUSEMOVE:		PostMouseEvent(InputEvent_MouseMove, 0, lParam);break;

		case WM_LBUTTONDOWN:	PostMouseEvent(InputEvent_MouseDown, 0, lParam); break; 
		case WM_LBUTTONDBLCLK:	PostMouseEvent(InputEvent_MouseDoubleClick,	0, lParam); break;
		case WM_LBUTTONUP:		PostMouseEvent(InputEvent_MouseUp, 0, lParam); break; 

		case WM_MBUTTONDOWN:	PostMouseEvent(InputEvent_MouseDown, 1, lParam); break; 
		case WM_MBUTTONDBLCLK:	PostMouseEvent(InputEvent_MouseDoubleClick,	1, lParam); break;
		case WM_MBUTTONUP:		PostMouseEvent(InputEvent_MouseUp, 1, lParam); break;

		case WM_RBUTTONDOWN:	PostMouseEvent(InputEvent_MouseDown, 2, lParam); break; 
		case WM_RBUTTONDBLCLK:	PostMouseEvent(InputEvent_MouseDoubleClick,	2, lParam); break;
		case WM_RBUTTONUP:		PostMouseEvent(InputEvent_MouseUp,		2, lParam); break;

		case 0x020A: // WM_MOUSEWHEEL; actual target message, gated out by VC6 WINVER.
		{
			
			WONRectangle aRect;
			GetRect(aRect,true); 
			short mx = LOWORD(lParam); 
			short my = HIWORD(lParam);
			lParam = MAKELPARAM(mx - aRect.x, my - aRect.y);
			PostMouseEvent(InputEvent_MouseWheel,	(short)HIWORD(wParam), lParam); 
			break;
		}

		case WM_INPUTLANGCHANGE: 
		{
			CalcCodePage();
			return DefWindowProc(hwnd,msg,wParam,lParam);
		}

		case WM_IME_COMPOSITION:
			 

			if (lParam & GCS_RESULTSTR) 
			{
				
				HIMC hIMC = ImmGetContext(hwnd);
				if (hIMC) 
				{
					DWORD dwSize = ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, NULL, 0);
					std::wstring aStr;
					aStr.resize(dwSize/2);
					ImmGetCompositionStringW(hIMC, GCS_RESULTSTR, (char*)aStr.data(),dwSize); 
					for(int i=0; i<aStr.length(); i++)
						Virtual48(new KeyEvent(InputEvent_KeyChar, aStr.at(i), mLBC)); 
				}
				ImmReleaseContext(hwnd, hIMC);

			}
			else 
				return DefWindowProc(hwnd,msg,wParam,lParam); 
	
			break;

		case WM_IME_CHAR:			
		{
			wchar_t aChar = wParam;
			MultiByteToWideChar(mCodePage,0,(const char*)&wParam,2,&aChar,1);
			Virtual48(new KeyEvent(InputEvent_KeyChar, aChar, mLBC)); 
			break;
		}

		case WM_CHAR:			
		{
			wchar_t aChar = wParam;
			MultiByteToWideChar(mCodePage,0,(const char*)&wParam,2,&aChar,1);
			Virtual48(new KeyEvent(InputEvent_KeyChar, aChar, mLBC)); 
			break;
		}

		case WM_KEYDOWN:		MyGetKeyState(); Virtual48(new KeyEvent(InputEvent_KeyDown,	wParam, mLBC)); if(mBC5) SendMsgToParent(hwnd,msg,wParam,lParam); break;
		case WM_KEYUP:			MyGetKeyState(); Virtual48(new KeyEvent(InputEvent_KeyUp, wParam, mLBC)); if(mBC5) SendMsgToParent(hwnd,msg,wParam,lParam); break;
		
		
		case WM_SYSKEYDOWN:	
		case WM_SYSKEYUP:		
			if(wParam==VK_TAB && mBC4)
			{
				DoMinimizeOnAltTab();
				return 1;
			}

			MyGetKeyState(); 
			if(mBC5) SendMsgToParent(hwnd,msg,wParam,lParam);
			return DefWindowProc(hwnd,msg,wParam,lParam); 


		case WM_KILLFOCUS:		Virtual48(new InputEvent(InputEvent_LostFocus)); break;
		case WM_SETFOCUS:		MyGetKeyState(); Virtual48(new InputEvent(InputEvent_GotFocus)); break;

		case WM_DESTROY:
			KillTimer(mLB8,0);
			SetWindowLong(hwnd, GWL_USERDATA, 0);
			mLB8 = NULL;
			Close();
			
			break;

		case WM_TIMER:
			
			break;

		case WM_SIZING:
		{
			LPRECT aRect = (LPRECT) lParam;

			
			
			if (aRect->right - aRect->left < mMinWidth)
			{
				aRect->right = aRect->left + mMinWidth; 
			}
			
			if ( aRect->bottom - aRect->top < mMinHeight)
			{
				aRect->bottom = aRect->top + mMinHeight;
			}

			
			
			if (aRect->right - aRect->left > mMaxWidth)
			{
				aRect->right = aRect->left + mMaxWidth;
			}
			
			if ( aRect->bottom - aRect->top > mMaxHeight)
			{
				aRect->bottom = aRect->top + mMaxHeight;
			}
		}
		break;

		case WM_SYSCOMMAND:
		{
			if (wParam == SC_MAXIMIZE) 
			{
				int aMaximizedWidth = GetSystemMetrics(SM_CXMAXIMIZED);
				int aMaximizedHeight = GetSystemMetrics(SM_CYMAXIMIZED);

				
				if (mMaxWidth < aMaximizedWidth || mMaxHeight < aMaximizedHeight)
				{
					DefWindowProc(hwnd,msg,wParam,lParam); 
					int x, y, cx, cy;
					
					if (mMaxWidth < aMaximizedWidth)
					{
						x =  (aMaximizedWidth - mMaxWidth) / 2;
						cx = mMaxWidth;
					}
					else
					{
						x = 0;
						cx = aMaximizedWidth;
					}
					
					if (mMaxHeight < aMaximizedHeight)
					{
						y =  (aMaximizedHeight - mMaxHeight) / 2;
						cy = mMaxHeight;
					}
					else
					{
						y = 0;
						cy = aMaximizedHeight;
					}
					
					SetWindowPos(hwnd,NULL,x,y,cx,cy,SWP_NOZORDER);
				}
				else 
				{
					return DefWindowProc(hwnd,msg,wParam,lParam);
				}
			}
	
			else
			{
				return DefWindowProc(hwnd,msg,wParam,lParam);
			}
		}
		break;

		case WM_PAINT:
			Virtual50();
		break;

		case WM_USER: 
			
			break;

		default:
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}

	mManager->ProcessEvents();

	return 1;
}

// Native3d14d0 support0: actual caller already selected a nativeCursor.
// FUNCTION: WON 0x007d14d0
void MSWindow::Virtual18(Cursor* cursor) { ::SetCursor(((MSCursor*)cursor)->mNativeCursor); }

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007d0dc0
// (public: virtual void * __thiscall WONAPI::MSWindow::`scalar deleting destructor'(unsigned int)    [??_GMSWindow@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007d2120
// (void __cdecl SendMsgToParent(struct HWND__ *,unsigned int,unsigned int,long)    [?SendMsgToParent@@YAXPAUHWND__@@IIJ@Z])
