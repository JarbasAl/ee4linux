// C:\EEX\Low-Level Engine\Graphics Engine\DX7 Display Plug-ins\Common\DX7 Multimon.cpp (file name inferred: the
// unit sits before DX7HRDisplay.cpp's init chains, so its name sorts before "DX7HRDisplay.cpp"; shared with the T&L
// renderer, byte-identical there)
// UNIT-FLAGS: -Os -Oy -Ob1 -Gs -Gf -Gy -G6 -MD -GR
// The unit was built without global optimisation (/O1 minus /Og: memory-resident locals, `leave`); 0x10001000 and
// 0x1000102a are bind_only under these flags, 12/37 under -O1, 52/72 under -Od. /Oy and /GR are not discriminated.
//
// 0x10001000..0x1000105d: GetMonitorInfoA through a pointer resolved at run time (Windows 98/2000 and later only; on
// Windows 95/NT4 the pointer stays NULL) (provisional names).
#include <windows.h>
#include <multimon.h>

typedef BOOL (WINAPI* DX7GetMonitorInfoProc)(HMONITOR, LPMONITORINFO);
DX7GetMonitorInfoProc gGetMonitorInfo;

// FUNCTION: DX7HR 0x10001000
// Fills outInfo for a monitor through the resolved GetMonitorInfoA (pointer unchecked); false for a NULL monitor
bool DX7GetMonitorInfo(HMONITOR inMonitor, LPMONITORINFO outInfo)
{
    BOOL theResult;
    if (inMonitor != NULL)
        theResult = gGetMonitorInfo(inMonitor, outInfo);
    else
        theResult = FALSE;
    return theResult == TRUE;
}

// FUNCTION: DX7HR 0x1000102a
// Resolves GetMonitorInfoA from USER32 when the OS type is 2 or 3
void DX7InitializeMultimon(unsigned long inOSType)
{
    if (inOSType == 2 || inOSType == 3) {
        HMODULE theModule = GetModuleHandleA("USER32.DLL");
        gGetMonitorInfo = (DX7GetMonitorInfoProc)GetProcAddress(theModule, "GetMonitorInfoA");
    }
}

// DX7HRTnLDisplay.dll (the T&L twin builds this unit unchanged): the same functions at their twin addresses.
// FUNCTION: DX7HRTNL 0x10001000
// FUNCTION: DX7HRTNL 0x1000102a
