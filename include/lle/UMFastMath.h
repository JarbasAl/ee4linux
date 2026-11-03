// C:\EEX\Low-Level Engine\Utilities\UMFastMath.h (RECONSTRUCTION; name from the export ?Sqrt@UMFastMath@@SAMM@Z).
#ifndef LLE_UMFASTMATH_H
#define LLE_UMFASTMATH_H

#include <math.h>

class LLE_API UMFastMath {
public:
    // inline: emitted first in the GE2DPoint.cpp block (0x1000c129), `fld; fstp qword; call sqrt`
    static float Sqrt(float inValue) { return (float)sqrt(inValue); }
    static void BuildSqrtTable();           // exported 0x10080c07
};

#endif
