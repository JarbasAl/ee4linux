// EEAirTrafficControl.h (RECONSTRUCTION; file name inferred; provisional member names). RTTI
// EEAirTrafficControl (vtable 0x4422e8: ??_G, Load +4, Save +8; ctor 0x14e87c; sizeof 0x20; EEAirTrafficControl.cpp
// 0x14e839..0x151c28). One per player: the airports (carrying capacity and landing / takeoff queues) the player's
// aircraft are routed to. The strings of Dump name +0x18 "max cargo weight" and +0x1c "used cargo weight".
// Requires lle/LowLevelEngine.h, ee/EEUnit.h.
#ifndef EE_EEAIRTRAFFICCONTROL_H
#define EE_EEAIRTRAFFICCONTROL_H

#include <stdio.h>
#include <deque>
#include <vector>

class EEComplexUnit;
class EEPlayer;
class FSFile;

// One queued aircraft (8 bytes): the unit (referenced) and the kind of request (Save writes the unit's id and this)
struct EEATCItem {
    EEComplexUnit* mUnit;                   // +0x00
    unsigned long mKind;                    // +0x04
};

#ifdef EE_PORT
// The retail item is 8 bytes: a 4-byte unit pointer and the kind behind it. A 64-bit build's is 16, and each field
// keeps its place behind the wider pointer.
static_assert(__builtin_offsetof(EEATCItem, mKind) == sizeof(void*), "mKind sits behind the unit pointer");
static_assert(sizeof(EEATCItem) == 2 * sizeof(void*), "an item is a pointer and a kind, padded to the pointer's size");
#endif

// One airport (0x2c bytes; the vector's element, the same size as EEPGSPlayerReport so the vector COMDATs are ICF-shared)
struct EEATCAirport {
    EEATCAirport() : mAirport(NULL) {}      // 0x14ea04 (out of line, right after its first user)
    std::deque<EEATCItem> mLanding;         // +0x00 (request kind 1)
    std::deque<EEATCItem> mTakeoff;         // +0x14
    EEComplexUnit* mAirport;                // +0x28 (referenced)
};

// A replaced unit is an EEPUnitSwap (ee/EEPhysics.h): EEPlayer::UnitsSwapped passes one vector to 0x14f88d and 0x1fc6e8
struct EEPUnitSwap;

// A candidate airport for a reroute (12 bytes; 0x14f32f fills and sorts a vector of them)
struct EEATCCandidate {
    bool operator<(const EEATCCandidate& inOther) const { return mDistance < inOther.mDistance; }
    EEComplexUnit* mAirport;                // +0x00
    float mDistance;                        // +0x04 (squared tile distance)
    long mFree;                             // +0x08 (the cargo weight still free there)
};

#ifdef EE_PORT
// 12 bytes retail: the pointer, then the distance and the free weight. A 64-bit build's is 16.
static_assert(__builtin_offsetof(EEATCCandidate, mDistance) == sizeof(void*), "mDistance sits behind the airport pointer");
static_assert(sizeof(EEATCCandidate) == sizeof(void*) + 8, "a candidate is a pointer, a float and a long");
#endif

class EEAirTrafficControl {
public:
    // the save key (0x531cb8, guard byte 0x531cb0: the guarded dtor 0x14e860; only EEAirTrafficControl.cpp and
    // EEMessage.cpp include this header)
    static const UString sData;             // "EEAirTrafficControl Data"
    EEAirTrafficControl(EEPlayer* inPlayer);   // 0x14e87c (EEPlayer::Reset passes this)
    virtual ~EEAirTrafficControl();         // ??_G 0x14e8b7, dtor 0x14e8d3
    virtual long Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile* inFile);   // +0x04 0x14f75c
    virtual long Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile* inFile);   // +0x08 0x14fead

    void AddCargoWeight(EEComplexUnit* inUnit);  // 0x14e9a8 (used cargo += the unit's weight)
    void AddAirport(EEComplexUnit* inAirport);   // 0x14e9bb
    long QueueRequest(EEComplexUnit* inAirport, EEComplexUnit* inUnit, long inKind);  // 0x14ea34 (0 queued, 3 no such airport, 7 no room)
    long GetFreeCargoWeight(EEComplexUnit* inAirport);  // 0x14eaf7 (the airport's free cargo weight, -1 if unknown)
    long GetRoom(std::vector<EEATCAirport>::iterator& inSlot);    // 0x14eb2c
    bool RemoveFromTakeoffQueues(EEComplexUnit* inUnit);  // 0x14ec8c (drops inUnit from the takeoff queues)
    bool ClearAirportQueue(EEComplexUnit* inAirport, long inKind);  // 0x14ed30 (drops one takeoff request of that kind)
    void RemoveFromEntry(std::vector<EEATCAirport>::iterator& inSlot, EEComplexUnit* inUnit);  // 0x14eed0 (0: every request)
    void DropDeadPlanes(std::vector<EEATCAirport>::iterator& inSlot);  // 0x14f033 (drops the dead)
    void Dump(FILE* inFile);                // 0x14f15d
    void GetCandidates(EEComplexUnit* inAirport, std::vector<EEATCCandidate>& outCandidates);  // 0x14f32f
    long GetChecksum();                     // 0x14f47d (an out-of-sync checksum)
    unsigned long GetQueueLength(EEComplexUnit* inAirport);  // 0x14f5fa (its takeoff queue's length)
    long GetQueuePosition(EEComplexUnit* inUnit, long inWhich);  // 0x14f61c (inUnit's place in its airport's queue, -1)
    bool IsLandingDone(std::vector<EEATCAirport>::iterator& inSlot, EEComplexUnit* inUnit);  // 0x14f6d8
    bool IsTakeOffDone(std::vector<EEATCAirport>::iterator& inSlot, EEComplexUnit* inUnit);  // 0x14f6e9
    void ReplaceUnits(std::vector<EEPUnitSwap>& inSwaps);  // 0x14f88d (replaces the old units by the new in the queues)
    void SendPlanesAway(std::vector<EEATCAirport>::iterator& inSlot);  // 0x14fa2b
    void RemoveCargoWeight(EEComplexUnit* inUnit);  // 0x14fc39 (used cargo -= the unit's weight)
    void RemoveAirport(EEComplexUnit* inAirport);  // 0x14fc4c (the airport is gone)
    void RemovePlane(EEComplexUnit* inAirport, EEComplexUnit* inUnit);  // 0x14fcc1
    long RepairPointers(unsigned long inMajorVersion, unsigned long inMinorVersion);   // 0x14fcf4 (ids back to units after a load)
    EEComplexUnit* FindNearestAirport(EEComplexUnit* inUnit);  // 0x15014c (the nearest airport that fits inUnit)
    void Update();                          // 0x1501d4
    bool UpdateLandings(std::vector<EEATCAirport>::iterator& inSlot, bool* outFlag);  // 0x150277
    bool UpdateTakeoffs(std::vector<EEATCAirport>::iterator& inSlot);  // 0x150487
    // UpdateLandings's per-unit test after each landing request: an always-false body (ICF-folded into `xor al, al; ret
    // 4` 0xbb118) (provisional name)
    bool BlocksTakeoffs(EEComplexUnit* inUnit);

    // inline (out-of-line COMDAT 0x20b671 in the unit classes' range; EEUCPlanetSpaceship::Virtual74) (provisional
    // name)
    unsigned long GetNumSlots() { return this->mSlots.size(); }
    // inline (EEUCPlanetSpaceship::Virtual74 0x618e34: begin expanded, _Ptrit::operator+ out of line at the slot-0
    // site, one level deeper than the caller) (provisional name)
    EEATCAirport& GetSlot(unsigned long inIndex) { return this->mSlots[inIndex]; }

    std::vector<EEATCAirport> mSlots;       // +0x04
    EEPlayer* mPlayer;                      // +0x14 (ctor argument: the owner)
    long mMaxCargo;                         // +0x18
    long mUsedCargo;                        // +0x1c
};

#endif
