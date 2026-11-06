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

// Actual128B bitmap resource asset; tablequota0.
static const unsigned char gHandCursorData[] = 
{
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0c,
	0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0c,0x00,0x00,0x00,0x0d,0x80,0x00,0x00,
	0x0d,0xb0,0x00,0x00,0x0d,0xb4,0x00,0x00,0x0d,0xb6,0x00,0x00,0xcf,0xf6,0x00,
	0x00,0xef,0xfe,0x00,0x00,0x6f,0xfe,0x00,0x00,0x2f,0xfe,0x00,0x00,0x3f,0xfe,
	0x00,0x00,0x1f,0xfe,0x00,0x00,0x1f,0xfc,0x00,0x00,0x0f,0xfc,0x00,0x00,0x0f,
	0xfc,0x00,0x00,0x07,0xf8,0x00,0x00,0x07,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
// Actual128B bitmap resource asset; tablequota0.
static const unsigned char gHandCursorMaskData[] = 
{
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xf3,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xe1,
	0xff,0xff,0xff,0xe1,0xff,0xff,0xff,0xe0,0x7f,0xff,0xff,0xe0,0x0f,0xff,0xff,
	0xe0,0x03,0xff,0xff,0xe0,0x01,0xff,0xfe,0x20,0x00,0xff,0xfe,0x00,0x00,0xff,
	0xfe,0x00,0x00,0xff,0xff,0x00,0x00,0xff,0xff,0x80,0x00,0xff,0xff,0x80,0x00,
	0xff,0xff,0xc0,0x00,0xff,0xff,0xc0,0x01,0xff,0xff,0xe0,0x01,0xff,0xff,0xe0,
	0x01,0xff,0xff,0xf0,0x03,0xff,0xff,0xf0,0x03,0xff,0xff,0xf0,0x03,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
// Native3d08d0 support0: COLORREF to logicalRGB conversion.
static DWORD MyGetSysColor(int index) {
    DWORD c=GetSysColor(index);
    return ((c&0xff0000)>>16)|(c&0xff00)|((c&0xff)<<16);
}

// FUNCTION: WON 0x007d0900
MSWindowManager::MSWindowManager(HINSTANCE theHinstance)
{
	if(theHinstance==NULL)
		theHinstance = (HINSTANCE)GetModuleHandle(NULL);

	mInstance = theHinstance;

	Cursor::SetStandardCursor(StandardCursor_Hand, new MSCursor(CreateCursor(mInstance,12,4,32,32,gHandCursorMaskData,gHandCursorData),true));
	Cursor::SetStandardCursor(StandardCursor_Arrow, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW))));
	Cursor::SetStandardCursor(StandardCursor_IBeam, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_IBEAM))));
	Cursor::SetStandardCursor(StandardCursor_EastWest, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEWE))));
	Cursor::SetStandardCursor(StandardCursor_NorthSouth, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZENS))));
	Cursor::SetStandardCursor(StandardCursor_NorthSouthEastWest, new MSCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_SIZEALL))));
}

// FUNCTION: WON 0x007d0b70
void MSWindowManager::GetSystemColorScheme()
{
	mColorScheme->SetStandardColor(StandardColor_3DDarkShadow,MyGetSysColor(COLOR_3DDKSHADOW));
	mColorScheme->SetStandardColor(StandardColor_3DFace,MyGetSysColor(COLOR_3DFACE));
	mColorScheme->SetStandardColor(StandardColor_3DHilight,MyGetSysColor(COLOR_3DHILIGHT));

	mColorScheme->SetStandardColor(StandardColor_3DShadow,MyGetSysColor(COLOR_3DSHADOW));
	mColorScheme->SetStandardColor(StandardColor_Scrollbar,MyGetSysColor(COLOR_SCROLLBAR));

	mColorScheme->SetStandardColor(StandardColor_ButtonText,MyGetSysColor(COLOR_BTNTEXT));
	mColorScheme->SetStandardColor(StandardColor_GrayText,MyGetSysColor(COLOR_GRAYTEXT));
	mColorScheme->SetStandardColor(StandardColor_Hilight,MyGetSysColor(COLOR_HIGHLIGHT));
	mColorScheme->SetStandardColor(StandardColor_HilightText,MyGetSysColor(COLOR_HIGHLIGHTTEXT));

	mColorScheme->SetStandardColor(StandardColor_ToolTipBack,MyGetSysColor(COLOR_INFOBK));
	mColorScheme->SetStandardColor(StandardColor_ToolTipText,MyGetSysColor(COLOR_INFOTEXT));

	mColorScheme->SetStandardColor(StandardColor_MenuBack,MyGetSysColor(COLOR_MENU));
	mColorScheme->SetStandardColor(StandardColor_MenuText,MyGetSysColor(COLOR_MENUTEXT));

	mColorScheme->SetStandardColor(StandardColor_Back,MyGetSysColor(COLOR_WINDOW));
	mColorScheme->SetStandardColor(StandardColor_Text,MyGetSysColor(COLOR_WINDOWFRAME));
}

// FUNCTION: WON 0x007d0c40
void MSWindowManager::GetNextEvent()
{
	MSG msg;
	if(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else
		mTargetMsgLoopDepth = 0;
}

// FUNCTION: WON 0x007d0c90
void MSWindowManager::GetScreenRect(WONRectangle &theRect) 
{ 
	theRect.x = theRect.y = 0;
	theRect.width = GetSystemMetrics(SM_CXSCREEN);
	theRect.height = GetSystemMetrics(SM_CYSCREEN);
}

// FUNCTION: WON 0x007d0cc0
void MSWindowManager::Signal()
{
	MSWindow *aWindow = (MSWindow*)mDefaultWindow;
	if(aWindow==NULL)
		return;

	if(aWindow->mLB8==NULL)
		return;

	::PostMessage(aWindow->mLB8,WM_USER,0,0);
}

// FUNCTION: WON 0x007d0cf0
bool MSWindowManager::SetScreenResolutionHook(int theWidth, int theHeight)
{
	DEVMODE dm;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm );

	
	if (dm.dmPelsWidth!=theWidth || dm.dmPelsHeight!=theHeight)
	{
		dm.dmPelsWidth = theWidth;
		dm.dmPelsHeight = theHeight;
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
		ChangeDisplaySettings(&dm,CDS_FULLSCREEN);
	}
	return true;
}

// FUNCTION: WON 0x007d0d50
void MSWindowManager::GetScreenResolutionHook(int &theWidth, int &theHeight)
{
	DEVMODE dm;
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm );
	theWidth = dm.dmPelsWidth;
	theHeight = dm.dmPelsHeight;
}

// FUNCTION: WON 0x007d0b60
MSWindowManager::~MSWindowManager() {}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007d08d0
// (unsigned long __cdecl MyGetSysColor(int)    [?MyGetSysColor@@YAKH@Z])
// TEMPLATE: WON 0x007d0b40
// (public: virtual void * __thiscall WONAPI::MSWindowManager::`scalar deleting destructor'(unsigned int)    [??_GMSWindowManager@WONAPI@@UAEPAXI@Z])
