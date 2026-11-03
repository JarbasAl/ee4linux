// C:\EEX\Low-Level Engine\Error System\ESCallerLog.cpp (file name inferred: no __FILE__; RTTI .?AVESCallerLog@@)
// Unit flags (docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UWideString.h"
#include "lle/UString.h"
#include "lle/ESCallerLog.h"

// Compiler-generated for this unit (names in analysis/lle/names.txt):
// TEMPLATE: LLE 0x1000209a
// SYNTHETIC: LLE 0x10001c1b
// TEMPLATE: LLE 0x10001fe4
// SYNTHETIC: LLE 0x1000210b
// TEMPLATE: LLE 0x10002378
// TEMPLATE: LLE 0x100023ad
// TEMPLATE: LLE 0x10002663
// TEMPLATE: LLE 0x1000269d
// TEMPLATE: LLE 0x10002831
// TEMPLATE: LLE 0x10002869
// TEMPLATE: LLE 0x100028ad

// FUNCTION: LLE 0x10001c76
// Creates a caller log with a name, an auto-report flag and a report interval (0 means every call)
ESCallerLog::ESCallerLog(const UString& inName, bool inAutoReport, unsigned long inReportInterval)
{
    this->mName = inName;
    this->mCallCount = 0;
    this->mAutoReport = inAutoReport;
    this->mReportInterval = inReportInterval ? inReportInterval : 1;
}

// FUNCTION: LLE 0x10001cc9
// Destroys the caller log (members only)
ESCallerLog::~ESCallerLog()
{
}

// FUNCTION: LLE 0x10001ce4
// Counts one more call from the given caller; with auto-report on, reports every interval calls
void ESCallerLog::RecordCaller(const UString& inCaller)
{
    if (this->mCallers.find(inCaller) == this->mCallers.end())
        this->mCallers[inCaller] = 1;
    else
        this->mCallers[inCaller] = this->mCallers[inCaller] + 1;
    if (!this->mAutoReport)
        return;
    this->mCallCount++;
    if (this->mCallCount == this->mReportInterval) {
        this->Report();
        this->mCallCount = 0;
    }
}

/// One report line per caller, empty in the retail build (inferred shape): the original computes theTotal (a
// percentage) yet prints nothing, and Report calls the map<ulong, UString> _Tree(pred, alloc) ctor out of line
// (0x1fc3): an empty inline call in the second loop supplies that budget pressure (Report and 0x1fc3 exact).
static inline void ESCallerLogLine(const UString& inCaller, unsigned long inCount, float inPercent) {}

// FUNCTION: LLE 0x10001d4e
// Sorts the callers by call count and computes their total; prints nothing in the retail build
void ESCallerLog::Report()
{
    std::map<unsigned long, UString> theSorted;
    float theTotal = 0.0f;
    std::map<UString, unsigned long>::iterator theCaller;
    std::map<unsigned long, UString>::iterator theEntry;
    for (theCaller = this->mCallers.begin(); theCaller != this->mCallers.end(); ++theCaller) {
        theSorted[theCaller->second] = theCaller->first;
        theTotal += theCaller->second;
    }
    for (theEntry = theSorted.begin(); theEntry != theSorted.end(); ++theEntry)
        ESCallerLogLine(theEntry->second, theEntry->first, theEntry->first * 100.0f / theTotal);
}

// Compiler-generated, continued:
// SYNTHETIC: LLE 0x10001b8d
// SYNTHETIC: LLE 0x10001be2
// SYNTHETIC: LLE 0x10001de4
// TEMPLATE: LLE 0x10001e1d
// SYNTHETIC: LLE 0x10001e48
// SYNTHETIC: LLE 0x10001e7c
// SYNTHETIC: LLE 0x10001ec7
// SYNTHETIC: LLE 0x10001ef2
// SYNTHETIC: LLE 0x10001f63
// SYNTHETIC: LLE 0x10001fc3
// SYNTHETIC: LLE 0x10002127
// SYNTHETIC: LLE 0x100021aa
// SYNTHETIC: LLE 0x100021e4
