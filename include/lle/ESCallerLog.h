// C:\EEX\Low-Level Engine\Error System\ESCallerLog.h (file name inferred): counts calls per caller name and
// reports them every mReportInterval calls. ESCallerLog.cpp 0x1b8d..0x28ca; vtable 0x10097508 (dtor only).
// Requires <map> and UString.
#ifndef LLE_ESCALLERLOG_H
#define LLE_ESCALLERLOG_H

#include <map>

class LLE_API ESCallerLog {                 // sizeof 0x2c
public:
    // implicit copy ctor (0x1b8d) and operator= (0x1be2), emitted at the head of the unit
    ESCallerLog(const UString& inName, bool inAutoReport, unsigned long inReportInterval);   // 0x1c76
    virtual ~ESCallerLog();                 // 0x1cc9

    void RecordCaller(const UString& inCaller);   // 0x1ce4
    void Report();                          // 0x1d4e

protected:
    std::map<UString, unsigned long> mCallers;    // +0x04 (comparator byte +0x04, allocator byte +0x05, head +0x08,
                                                  // size +0x0c)
    UString mName;                          // +0x10
    unsigned long mReportInterval;          // +0x20 (0 -> 1)
    unsigned long mCallCount;               // +0x24
    bool mAutoReport;                       // +0x28
};

#endif
