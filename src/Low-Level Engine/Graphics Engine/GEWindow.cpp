// C:\EEX\Low-Level Engine\Graphics Engine\GEWindow.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x10038020..0x10038527: GEDeviceContext's static init chains (sHDC 0x38020, sLock 0x38071), GEDeviceContext and
// GEWindow members, then the map<HWND__*, GEDCCriticalSection> COMDATs its dtor pulls in (~_Tree, erase(F, L),
// erase(P), _Erase). Every other GEWindow member is header-inline (GEBitmap.cpp emits them).
//
// Compiler-generated for this unit (names in analysis/lle/names.txt):
// SYNTHETIC: LLE 0x10038020
// SYNTHETIC: LLE 0x1003802a
// SYNTHETIC: LLE 0x1003804a
// SYNTHETIC: LLE 0x10038056
// SYNTHETIC: LLE 0x10038071
// SYNTHETIC: LLE 0x1003807b
// SYNTHETIC: LLE 0x1003809f
// SYNTHETIC: LLE 0x100380ab
// TEMPLATE: LLE 0x10038199
// TEMPLATE: LLE 0x100381c4
// TEMPLATE: LLE 0x10038235
// TEMPLATE: LLE 0x100384ec
#include <windows.h>

#define LLE_GEWINDOW_CPP    // GEDeviceContext's statics are visible here only (see GEWindow.h) (provisional name)
#include "lle/LowLevelEngineCore.h"
#include "lle/GE2DPoint.h"
#include "lle/TSScheduler.h"
#include "GEWindow.h"

std::map<HWND__*, GEDeviceContext::GEDCCriticalSection> GEDeviceContext::sHDC;
GEDeviceContext::GEDCCriticalSection GEDeviceContext::sLock;

// FUNCTION: LLE 0x100380c8
// Returns the window's device context (GetDC)
HDC__* GEDeviceContext::GetDeviceContext(HWND__* inWindow)
{
    return GetDC(inWindow);
}

// FUNCTION: LLE 0x100380d3
// Releases a window's device context; always succeeds
long GEDeviceContext::ReleaseDeviceContext(HWND__* inWindow, HDC__* inContext)
{
    ReleaseDC(inWindow, inContext);
    return kErrorOK;
}

// FUNCTION: LLE 0x100380e4
// Destroys the window object, destroying the native window only in full-screen mode
GEWindow::~GEWindow()
{
    if (this->mIsFullScreen)
        DestroyWindow(this->mWindow);
}

// FUNCTION: LLE 0x100380f3
// Returns the screen position of the window's client area
const GE2DPoint& GEWindow::GetClientOrigin()
{
    POINT thePoint = {0, 0};
    ClientToScreen(this->mWindow, &thePoint);
    this->mClientOrigin.SetX((float)thePoint.x);
    this->mClientOrigin.SetY((float)thePoint.y);
    return this->mClientOrigin;
}

// FUNCTION: LLE 0x10038120
// Resizes the window; with depth 1 the size is the client area (frame added), else the whole window
void GEWindow::Resize(unsigned long inWidth, unsigned long inHeight)
{
    if (this->mDepth == 1) {
        RECT theRect;
        GetWindowRect(this->mWindow, &theRect);
        theRect.left = 0;
        theRect.top = 0;
        theRect.right = inWidth;
        theRect.bottom = inHeight;
        AdjustWindowRect(&theRect, WS_OVERLAPPEDWINDOW, FALSE);
        SetWindowPos(this->mWindow, NULL, 0, 0, theRect.right - theRect.left, theRect.bottom - theRect.top, SWP_NOMOVE);
    } else {
        SetWindowPos(this->mWindow, NULL, 0, 0, inWidth, inHeight, SWP_NOMOVE);
    }
    this->mWidth = inWidth;
    this->mHeight = inHeight;
}
