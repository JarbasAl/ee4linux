// Unit 11 (0x154a5..0x15c98): tile-grid and mask queries, square and line marking. Names PROVISIONAL (the
// symbols are bound in analysis/default/names.txt: include this header rather than redeclaring them).
// Requires <vector>, lle/LowLevelEngine.h.
#ifndef UNIT11_H
#define UNIT11_H

bool IsAnyMaskedTileClear(const std::vector<std::vector<unsigned char> >& inMask,
                          const std::vector<std::vector<unsigned long> >& inTiles, unsigned long inX, unsigned long inY,
                          unsigned long inRadius);
bool AreAllMasked(const std::vector<std::vector<unsigned char> >& inMask,
                  const std::vector<std::vector<unsigned char> >& inOther, unsigned long inX, unsigned long inY,
                  unsigned long inRadius);
// NOTE (worker B): bool? EELandClumps 0x9c18 tests `al` after the call (a long result would be `test eax,eax`);
// EELandClumps.cpp declares it bool locally for now.
bool IsSquareOpen(std::vector<std::vector<unsigned char> >& inMask, unsigned long inX, unsigned long inY,
                  unsigned long inRadius);
bool Unit11_1581c(const std::vector<std::vector<unsigned long> >& inTiles, unsigned long inX, unsigned long inY);
bool IsTileOfType(const std::vector<std::vector<unsigned long> >& inTiles, unsigned long inX, unsigned long inY,
                  unsigned long inType, bool inSpecial);
void MarkSquare(std::vector<std::vector<unsigned char> >& ioMask, unsigned long inX, unsigned long inY,
                unsigned long inSize);
void MarkLine(std::vector<std::vector<unsigned char> >& ioMask, const GE2DPoint& inFrom, const GE2DPoint& inTo,
              float inWidth);

#endif
