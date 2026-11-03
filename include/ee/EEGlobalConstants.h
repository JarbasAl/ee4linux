// EEGlobalConstants.h (RECONSTRUCTION; file name inferred). Named global constants the game reads through memory
// from one .rdata word shared by units of many folders (an extern const: never folded into an immediate, and not a
// per-unit copy, which a `static const` would be). Names are provisional; the defining unit is not identified yet
// (bound by address in analysis/exe/names.txt). Orchestrator decision 3.
#ifndef EE_EEGLOBALCONSTANTS_H
#define EE_EEGLOBALCONSTANTS_H

// 0x43bf0c: 0.01f, distinct from the pooled __real 0x43a688; read by 30 sites in 14 units (RTSMap::SmoothVertex,
// EEE3DEarthquake, EEHerd, EECPAttackSubGroup, EEPMLand, ...)
extern const float kHeightEpsilon;

#endif
