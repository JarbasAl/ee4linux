// EEMathInline.h (RECONSTRUCTION; file name inferred; provisional function name). Small integer helpers
// the game code expands inline. The exe has no /Oi, so the CRT abs() is a call to _abs; the AI code instead computes an
// integer absolute value inline as `sub; jns; neg` (EECPDefenseCommander::Fn77EFD 0x477efd: three uses, the value
// left in eax and reused for the store), i.e. an inline overload next to the engine's UMAbs(float).
#ifndef EE_EEMATHINLINE_H
#define EE_EEMATHINLINE_H

inline long UMAbs(long inValue)
{
    return inValue < 0 ? -inValue : inValue;
}

#endif
