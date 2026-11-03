// EETileCoordinate.h (RECONSTRUCTION; file name inferred). A map tile's coordinates. The original has ONE tile type,
// U2DPoint<long>: arguments pass straight between the two spellings everywhere, 41 COMDAT addresses carried both names,
// and 0x8d421 is U2DPoint<long>::DistanceTo. The name stays as a typedef for readability; it is
// not a distinct type.
#ifndef EE_EETILECOORDINATE_H
#define EE_EETILECOORDINATE_H

#include "lle/U2DPoint.h"   // exe view: no key statics, no includes (safe in a widely included header)
typedef U2DPoint<long> EETileCoordinate;

#endif
