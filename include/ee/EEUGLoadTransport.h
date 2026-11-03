// EEUGLoadTransport.h (RECONSTRUCTION; file name inferred). Loading units into a transport (RTTI EEUGLoadTransport >
// EEUGLoadContainer > EEUnitGoal; vtable 0x4477f8; sizeof 0x68: its new-site size; EEUGLoadTransport.cpp
// 0x2271c7..0x227621). Names are provisional. Requires what ee/EEUGLoadContainer.h requires.
#ifndef EE_EEUGLOADTRANSPORT_H
#define EE_EEUGLOADTRANSPORT_H

#include "ee/EEUGLoadContainer.h"

class EEUGLoadTransport : public EEUGLoadContainer {
public:
    // 0x22720a (its Virtual10 copies through it)
    EEUGLoadTransport(EEComplexUnit* inTransport, const UReferenceContainer<EEComplexUnit*>& inUnits,
                      const U2DPoint<long>& inTransportTile, const U2DPoint<long>& inUnitTile,
                      float inMinimumDistanceSquared, bool inArg);
    // 0x227286: then Virtual98 and ComputeLoadDistance (EEMLoadTransport-style callers)
    EEUGLoadTransport(EEComplexUnit* inTransport, std::vector<EEComplexUnit*>& inUnits, bool inArg);
    EEUGLoadTransport(bool inArg);          // 0x2272b6 (EEUnitGoal::CreateGoal)
    virtual ~EEUGLoadTransport();           // +0x00 0x2272d0 (??_G 0x22726a)
    virtual EEUnitGoal* Clone();            // +0x10 0x2273a2
    virtual void DumpOutOfSynch(FILE* inFile);   // +0x14 0x2273e2
    virtual long GetOutOfSynchChecksum();   // +0x18 0x2273ee
    virtual void GetDebugLine(UWideString& ioText);  // +0x1c 0x2273f3
    virtual void GetDebugText(UWideString& ioText);  // +0x20 0x227405
    virtual long GetStartAction();          // +0x30 0x227264 (0x1f40; the base's and the other goals' fold into it)
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile);   // +0x44 0x22745c
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile);   // +0x58 0x22755b
    virtual bool EvaluateCondition(EEComplexUnit* inUnit, unsigned long inEvent);  // +0x64 0x227574
    virtual long GetUsedCapacity();         // +0x78 0x22723a (the transport's free capacity)
    virtual long GetCapacity();             // +0x7c 0x22724d (its class's capacity)
    virtual bool CanContain(EEComplexUnit* inUnit);  // +0x80 0x227426
    virtual long GetRemainingLoad();        // +0x84 0x22743b (the space the units need)
    virtual long GetFreeCapacity();         // +0x88 0x22725a (the transport's load)
    virtual bool LoadUnit(EEComplexUnit* inUnit);    // +0x90 0x227475 (loads inUnit)
    virtual void ComputeLoadDistance();     // +0x94 0x2272db (the loading distance)
    virtual void ComputeRendezvous();       // +0x98 0x227340 (the rendezvous tiles)

    static const UString sData;             // "EEUGLoadTransport-Data" (init chain 0x2271c7; Load/Save pass it on)
};

#endif
