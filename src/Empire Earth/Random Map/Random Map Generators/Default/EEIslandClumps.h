// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EEIslandClumps.h (name PROVISIONAL)
// RTTI .?AVEEIslandClumps@@ (vtable 0x191ec). Requires EELandClumps.h.
#ifndef EEISLANDCLUMPS_H
#define EEISLANDCLUMPS_H

class EEIslandClumps : public EELandClumps {    // proven sizeof 0xb4 (Default.cpp 0x78c5: operator new(0xb4))
public:
    EEIslandClumps(EERandomMap* inMap, EECellGrid* inClaimed, std::vector<unsigned long>* inDistances,
                   unsigned long inClumpCount, float inSpread, unsigned long inTargetSize, long inPlayer,
                   bool inFlag);
    virtual ~EEIslandClumps();                              // slot 0 (0x9414; deleting 0x93f8)
    virtual bool IsValidLocation(long inX, long inY);       // slot 2 (0x941f)

    bool IsNearOther(unsigned long inX, unsigned long inY); // 0x9451

    unsigned long mRiverWidth;                  // +0xb0 (map +0x3c8)
};

#endif
