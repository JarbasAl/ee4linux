// CMU Map Generator.h: entry points of the CMU unit (rva 0x1000..0x76f6) called from other units.
// File and function names PROVISIONAL (the debug strings name CMUCreateFlatMap / CMUCreateLandWaterMap /
// CMUCreateTrees); signatures proven by codegen: every one returns long 0 (`xor eax,eax` epilogue).
#ifndef CMU_MAP_GENERATOR_H
#define CMU_MAP_GENERATOR_H

#include <vector>

struct EERandomMap;
class EEClumps;

// The clump factory CMUCreateLandWaterMap calls once per player (Default.cpp 0x78c5): map, the player's clump
// sizes (element of a local vector<vector<long> >), player index, clump count, a float, and a long. PROVISIONAL.
// (worker B) the sizes are unsigned: EEClumps keeps the vector as mDistances (+0x6c) and converts its elements
// with the unsigned fild-qword idiom (0x8586, 0x889b).
typedef EEClumps* (*CMUClumpsFactory)(EERandomMap* ioMap, std::vector<unsigned long>& inSizes, unsigned long inPlayer,
                                      unsigned long inCount, float inFactor, unsigned long inValue);

// 0x2412: grid[x][y] != 0 for x, y inside the (square) grid. bool (worker B: callers test al; the body is the
// `return a && b && c;` shape, exact). Name PROVISIONAL.
bool CMUIsCellSet(const std::vector<std::vector<unsigned char> >& inGrid, unsigned long inX, unsigned long inY);
long CMUCreateFlatMap(EERandomMap* ioMap);                                    // 0x115e
long CMUCreateLandWaterMap(EERandomMap* ioMap, CMUClumpsFactory inFactory);    // 0x15e5
long CMUCreateTrees(EERandomMap* ioMap);                                       // 0x1d93
long CMUGenerateNeutralPositions(EERandomMap* ioMap, float inCount);           // 0x244e (inCount unused)
long CMUGeneratePlayerPositions(EERandomMap* ioMap);                           // 0x2843
long CMUGenerateResources(EERandomMap* ioMap);                                 // 0x307e
long CMUGenerateShallows(EERandomMap* ioMap);                                  // 0x3146

#endif
