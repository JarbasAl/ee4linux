// EESMIDMap.h (RECONSTRUCTION; file name inferred). The id map EESMCondition/EESMEffect
// remap their trigger ids through (the _Tree::find/insert COMDATs 0x820bd/0x82007, ICF-shared; 4-byte key and value;
// inferred types) and the cdecl helper 0x1b9fcf (EEScenarioManager.cpp): ioID becomes inMap[ioID], or 0 if the map
// has no entry (provisional names).
#ifndef EE_EESMIDMAP_H
#define EE_EESMIDMAP_H

#include <map>

typedef std::map<unsigned long, unsigned long> EESMIDMap;
void EERemapID(unsigned long& ioID, EESMIDMap& inMap);

#endif
