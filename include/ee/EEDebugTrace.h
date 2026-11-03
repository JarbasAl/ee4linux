// EEDebugTrace.h (RECONSTRUCTION; file name inferred; provisional function name). A debug trace that the
// shipping build compiled to nothing: its COMDAT 0x9ba76 (after EECPUnitSelector.cpp's functions, its first user) only
// destroys the by-value UString argument (`lea ecx, [esp + 8]; jmp ??1UString`); callers push (format, UString copy,
// id). Also called by 0x1b28c0 (EEScenarioManager) and 0x241c2d. Requires lle/LowLevelEngine.h (UString).
#ifndef EE_EEDEBUGTRACE_H
#define EE_EEDEBUGTRACE_H

inline void EEDebugTrace(const char* inFormat, UString inName, long inID)
{
}
// without the id (0x241c2d pushes only the format and the UString copy; ICF-folded into the same COMDAT 0x9ba76)
inline void EEDebugTrace(const char* inFormat, UString inName)
{
}

// with the id before the name (EEAIContinentManager 0x21fff: format, unit id, the class name copy; its COMDAT
// 0x26b20 destroys the third argument)
inline void EEDebugTrace(const char* inFormat, long inID, UString inName)
{
}

#endif
