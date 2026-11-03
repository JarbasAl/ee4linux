// C:\EEX\Empire Earth\Random Map\Random Map Generators\Default\EEFlatClumps.h (name PROVISIONAL)
// RTTI .?AVEEFlatClumps@@ (vtable 0x191c0). Requires EEClumps.h.
#ifndef EEFLATCLUMPS_H
#define EEFLATCLUMPS_H

class EEFlatClumps : public EEClumps {          // proven sizeof >= 0xa5 (+0xa4 byte)
public:
    EEFlatClumps(EERandomMap* inMap, std::vector<unsigned long>* inDistances, unsigned long inClumpCount,
                 float inSpread, unsigned long inTargetSize, long inType, bool inFlag);
    virtual ~EEFlatClumps();                                // slot 0 (0x90bf; deleting 0x90a3)
    virtual bool IsValidLocation(long inX, long inY);       // slot 2 (0x91f9)
    virtual bool AddLocation(const GE2DPoint& inLocation);  // slot 4 (0x90ca)

    std::vector<EERMPlayer>* mPlayers;          // +0xa0 (map +0xe0)
    bool mFlag;                                 // +0xa4
};

#endif
