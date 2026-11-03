// DX7 Multimon.cpp's functions (RECONSTRUCTION, names provisional): GetMonitorInfoA resolved at run time.
#ifndef DX7_DX7MULTIMON_H
#define DX7_DX7MULTIMON_H

#include <multimon.h>

bool DX7GetMonitorInfo(HMONITOR inMonitor, LPMONITORINFO outInfo);
void DX7InitializeMultimon(unsigned long inOSType);

#endif
