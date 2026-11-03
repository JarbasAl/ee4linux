// Unit 12 (0x15c98..0x18af3): resource and object placement. Names PROVISIONAL (the symbols are bound in
// analysis/default/names.txt: include this header rather than redeclaring them).
// Requires <vector>, lle/LowLevelEngine.h, ee/EERandomMap.h.
#ifndef UNIT12_H
#define UNIT12_H

typedef std::vector<std::vector<GE2DPoint> > EERMSiteLists;    // one list of placed sites per player

void PlaceResourcesNear(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind,
                        unsigned long inX, unsigned long inY, unsigned long inRadius, unsigned long inPlayer);
void PlaceResource(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind,
                   const GE2DPoint& inPosition, unsigned long inPlayer);
bool IsSteep(EERandomMap* inMap, unsigned long inX, unsigned long inY);
bool CanPlaceResource(EERandomMap* inMap, long inKind, unsigned long inX, unsigned long inY);
unsigned long CountSteepCells(EERandomMap* inMap, unsigned long inX, unsigned long inY, unsigned long inRadius);
void ResetPlayerOrder();
void PlaceResources(EERandomMap* ioMap, EERMSiteLists& ioSites);
void PlaceResourceKind(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind);
bool IsGoodResourceSite(EERandomMap* inMap, const EERMResourceKind& inKind, unsigned long inRadius,
                        unsigned long inX, unsigned long inY);
void PlaceResourceAroundPlayer(EERandomMap* ioMap, EERMSiteLists& ioSites, const EERMResourceKind& inKind,
                               unsigned long inPlayer);
void PlacePlayerObjects(EERandomMap* ioMap);
void PlaceObjects(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind);
void PlaceObjectsForPlayer(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, unsigned long inPlayer);
void PlaceObject(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, const GE2DPoint& inPosition,
                 unsigned long inPlayer);
void PlaceObjectAt(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, const GE2DPoint& inPosition,
                   unsigned long inPlayer);
bool IsWellSpaced(EERandomMap* inMap, EERMSiteLists& inSites, unsigned long inKind, unsigned long inX,
                  unsigned long inY, unsigned long inPlayer);
bool IsShoreSite(EERandomMap* inMap, unsigned long inX, unsigned long inY);
void PlaceObjectsByOrder(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind);
void PlaceObjectsNearPlayer(EERandomMap* ioMap, EERMSiteLists& ioSites, unsigned long inKind, unsigned long inPlayer);
const GE2DPoint& GetFartherPoint(EERandomMap* inMap, unsigned long inPlayer, const GE2DPoint& inA,
                                 const GE2DPoint& inB);
long GetObjectCount(EERandomMap* inMap, unsigned long inKind, unsigned long inPlayer);
void PlaceSpacedObjects(EERandomMap* ioMap, EERMSiteLists& ioSites);
void BuildPlayerOrder(EERandomMap* ioMap);

#endif
