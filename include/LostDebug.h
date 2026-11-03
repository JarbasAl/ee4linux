// LostDebug.h -- NOT ORIGINAL SOURCE. A synthetic header of this decompilation project (docs/LOST_DEBUG.md).
//
// Why it exists: in a few functions the shipping build carried inline-budget cost from code the release build
// compiled away (debug-only lines: assert `__LINE__` gaps, a base-game twin with extra code, debug-only strings or
// macros). VC6 /Ob1 charges a per-function inline budget while it expands inlines; the deepest (then earliest)
// expansions are refused once it is spent, so lost lines change which helpers the original calls out of line.
// We cannot reproduce the lost lines faithfully, and inventing plausible statements is rejected (docs/REJECTED.md
// entries 3/3b). By owner decision (2026-10-24) a DELIBERATE, clearly marked filler may stand in for their cost:
//
//     // LOST-DEBUG: <independent evidence that code was compiled away here>
//     EE_LOST_DEBUG(n);
//
// Rule: only with independent evidence of compiled-away code, never as plain tuning; every use is listed in
// docs/LOST_DEBUG.md with its n and evidence.
//
// What one unit is (probes: build/scratch_filler/, docs/LOST_DEBUG.md):
// - exactly the inline-budget cost of one discarded `std::vector<T>::size()` (Dinkum 2.33 / VC6 RTM -O1, measured
//   step for step against `size()` for 0..22 units before 6 vector<0x54>[] accesses, and in tcp ResolveAddress);
// - no code, no data, no symbols, no `$E` numbers, no strings: the call is expanded, its result is discarded and
//   the loads through the null pointer are dead, so nothing of it survives (the object is byte-identical to one
//   compiled without the line, apart from the budget effect on the function's other inlines);
// - EE_LOST_DEBUG(n) is one expression statement on ONE source line (asserts carry their line explicitly as
//   ESVERIFY_AT(expr, N) since 2026-11-02, so adding it moves nothing).
// Simpler shapes cost NOTHING and cannot work: empty inlines, constant returns, any free or static inline function
// (even `a == 0 ? 0 : b / a`, or the same body as Cost() on parameters or globals), member functions that only read
// a member, or whose `?:` arm is an unscaled (char*) pointer difference. VC6 charges a leaf expansion only for this
// `size()` shape: loads through `this` + `?:` + a scaled pointer difference.
// Limit: under extreme pressure (about 23+ units on top of a heavy function) VC6 refuses the unit itself and emits
// `?Cost@EELostDebugUnit@@QBEIXZ` out of line: the object then has that symbol, which is wrong. Check the object.
// Include it AFTER the unit's other #includes: in a -GR unit any new identifier declared before the RTTI classes'
// headers reorders the unit's .rdata$r RTTI COMDATs; after them, the object is byte-identical (tcp, mixer, Default,
// lle ESCallerLog/UIManager, dx7hr DX7HRDisplay, exe EEALasso). C++ only (a member function).
#ifndef EE_LOSTDEBUG_H
#define EE_LOSTDEBUG_H

struct EELostDebugUnit {
    unsigned Cost() const { return this->mFirst == NULL ? 0 : this->mLast - this->mFirst; }
    int* mFirst;
    int* mLast;
};

#define EE_LOST_DEBUG_UNIT (((const EELostDebugUnit*)0)->Cost())
#define EE_LOST_DEBUG_1 EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_2 EE_LOST_DEBUG_1, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_3 EE_LOST_DEBUG_2, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_4 EE_LOST_DEBUG_3, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_5 EE_LOST_DEBUG_4, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_6 EE_LOST_DEBUG_5, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_7 EE_LOST_DEBUG_6, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_8 EE_LOST_DEBUG_7, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_9 EE_LOST_DEBUG_8, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_10 EE_LOST_DEBUG_9, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_11 EE_LOST_DEBUG_10, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_12 EE_LOST_DEBUG_11, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_13 EE_LOST_DEBUG_12, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_14 EE_LOST_DEBUG_13, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_15 EE_LOST_DEBUG_14, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_16 EE_LOST_DEBUG_15, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_17 EE_LOST_DEBUG_16, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_18 EE_LOST_DEBUG_17, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_19 EE_LOST_DEBUG_18, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_20 EE_LOST_DEBUG_19, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_21 EE_LOST_DEBUG_20, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_22 EE_LOST_DEBUG_21, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_23 EE_LOST_DEBUG_22, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_24 EE_LOST_DEBUG_23, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_25 EE_LOST_DEBUG_24, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_26 EE_LOST_DEBUG_25, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_27 EE_LOST_DEBUG_26, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_28 EE_LOST_DEBUG_27, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_29 EE_LOST_DEBUG_28, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_30 EE_LOST_DEBUG_29, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_31 EE_LOST_DEBUG_30, EE_LOST_DEBUG_UNIT
#define EE_LOST_DEBUG_32 EE_LOST_DEBUG_31, EE_LOST_DEBUG_UNIT

// n units of inline-budget cost (1..32), one expression statement: `EE_LOST_DEBUG(12);`
#define EE_LOST_DEBUG(n) ((void)(EE_LOST_DEBUG_##n))

// A `case` body the shipping build compiled away, where the jump table still keeps the label: VC6 drops a label whose
// body is only `break` (also `;`, `{}`, `x;`), but keeps one whose statement the optimiser removes later. A
// self-assignment of a local is that statement and emits nothing (exe EECalamityManager::LoadCalamityClasses 0xae070).
// Use: `case N: EE_LOST_CASE(theLocal); break;` with a `// LOST-DEBUG:` comment and a row in docs/LOST_DEBUG.md.
#define EE_LOST_CASE(local) ((local) = (local))

// A read whose value the shipping build discards but still performs, because the object is `volatile` (a check or
// trace that compiled away around it, e.g. a dropped ESVERIFY): the load stays, nothing uses it (exe RTSPhysics::Main,
// the dead `mL60` read after mThreadDone->Set()). Use: `EE_LOST_READ(theObject->mMember);` with a `// LOST-DEBUG:`
// comment and a row in docs/LOST_DEBUG.md (owner, 2026-10-25).
#define EE_LOST_READ(expr) ((void)(expr))

// The argument of a trace or debug call the shipping build compiled away, whose evaluation the build still performs
// because it calls out-of-line code (exe EEScenarioManager's dtor 0x1afe66: a third `mV0D4.begin() + i` whose result
// is discarded; in 0x1b28c0 the same expression feeds the trace call 0x9ba76). Use: `EE_LOST_TRACE(expr);` with a
// `// LOST-DEBUG:` comment and a row in docs/LOST_DEBUG.md (owner, 2026-10-25).
#define EE_LOST_TRACE(expr) ((void)(expr))

#endif
