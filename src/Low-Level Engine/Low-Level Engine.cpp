// C:\EEX\Low-Level Engine\Low-Level Engine.cpp (file name inferred: no __FILE__; the unit sorts between Input System\IS*
// and Network Engine\NE*, and Rich counts one more C++ object than the named units)
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x1003bb89..0x1003bc2d: only two profiling call logs and their init chains. Nothing references them (UMCos/UMSin in
// the math unit do not), so they are leftovers of a profiling build (provisional names).
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/ESCallerLog.h"

static ESCallerLog sUMCosCallerLog("UMCos Low-Level", true, 500000);
static ESCallerLog sUMSinCallerLog("UMSin Low-Level", true, 500000);

// Compiler-generated for this unit (names in analysis/lle/names.txt):

// Header-inline members emitted here (markers at the end):
// SYNTHETIC: LLE 0x1003bb89
// SYNTHETIC: LLE 0x1003bb93
// SYNTHETIC: LLE 0x1003bbc5
// SYNTHETIC: LLE 0x1003bbd1
// SYNTHETIC: LLE 0x1003bbdb
// SYNTHETIC: LLE 0x1003bbe5
// SYNTHETIC: LLE 0x1003bc17
// SYNTHETIC: LLE 0x1003bc23
