// C:\EEX\Low-Level Engine\Graphics Engine\DX7 Display Plug-ins\DX7HRDisplay\DX7HRDisplay.cpp (file name inferred:
// the renderer's own unit, the plug-in entry points; the T&L twin has its own folder)
// Unit flags (hypothesis, docs/CALIBRATION.md "dx7hr"): -O1 -G6 -MD -GR
//
// 0x1000105d..0x10001f51: the U2D key init chains, the exported inline members of the DX7 classes this unit is the
// first to see (DX7StdColorVertexArray, DX7Rasterizer, DX7Screen, DX7Surface; header order), the plug-in's two
// globals (name, rasterizer), DllMain and the PIM entry points, then the vector/UList COMDATs and the template-static
// init chain head.
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "lle/LowLevelEngine.h"
#include "lle/U2DGeometry.h"
#include "dx7/DX7Rasterizer.h"
#include "dx7/DX7Screen.h"
#include "dx7/DX7Surface.h"

// The U2D key init chains, then the exported inline members (dllexport classes, header order):
// SYNTHETIC: DX7HR 0x1000105d
// SYNTHETIC: DX7HR 0x10001067
// SYNTHETIC: DX7HR 0x10001078
// SYNTHETIC: DX7HR 0x10001084
// SYNTHETIC: DX7HR 0x1000108f
// SYNTHETIC: DX7HR 0x10001099
// SYNTHETIC: DX7HR 0x100010aa
// SYNTHETIC: DX7HR 0x100010b6
// SYNTHETIC: DX7HR 0x100010c1
// SYNTHETIC: DX7HR 0x100010cb
// SYNTHETIC: DX7HR 0x100010dc
// SYNTHETIC: DX7HR 0x100010e8
// SYNTHETIC: DX7HR 0x100010f3
// SYNTHETIC: DX7HR 0x100010fd
// SYNTHETIC: DX7HR 0x1000110e
// SYNTHETIC: DX7HR 0x1000111a
// SYNTHETIC: DX7HR 0x10001125
// SYNTHETIC: DX7HR 0x1000112f
// SYNTHETIC: DX7HR 0x10001140
// SYNTHETIC: DX7HR 0x1000114c
// SYNTHETIC: DX7HR 0x10001157
// SYNTHETIC: DX7HR 0x10001161
// SYNTHETIC: DX7HR 0x10001172
// SYNTHETIC: DX7HR 0x1000117e
// SYNTHETIC: DX7HR 0x10001189
// SYNTHETIC: DX7HR 0x10001193
// SYNTHETIC: DX7HR 0x100011a4
// SYNTHETIC: DX7HR 0x100011b0
// SYNTHETIC: DX7HR 0x100011bb
// SYNTHETIC: DX7HR 0x100011de
// SYNTHETIC: DX7HR 0x100011f3
// SYNTHETIC: DX7HR 0x1000124d
// SYNTHETIC: DX7HR 0x1000125f
// SYNTHETIC: DX7HR 0x10001271
// SYNTHETIC: DX7HR 0x1000129f
// SYNTHETIC: DX7HR 0x100012c7
// SYNTHETIC: DX7HR 0x10001322
// SYNTHETIC: DX7HR 0x10001326
// SYNTHETIC: DX7HR 0x1000132a
// SYNTHETIC: DX7HR 0x1000132e
// SYNTHETIC: DX7HR 0x10001332
// SYNTHETIC: DX7HR 0x10001336
// SYNTHETIC: DX7HR 0x1000133a
// SYNTHETIC: DX7HR 0x10001347
// SYNTHETIC: DX7HR 0x1000134b
// SYNTHETIC: DX7HR 0x1000134f
// SYNTHETIC: DX7HR 0x1000135f
// SYNTHETIC: DX7HR 0x10001375
// SYNTHETIC: DX7HR 0x10001379
// SYNTHETIC: DX7HR 0x10001387
// SYNTHETIC: DX7HR 0x1000139f
// SYNTHETIC: DX7HR 0x100013a3
// SYNTHETIC: DX7HR 0x10001427
// SYNTHETIC: DX7HR 0x100014a8
// SYNTHETIC: DX7HR 0x10001503
// SYNTHETIC: DX7HR 0x10001507
// SYNTHETIC: DX7HR 0x10001514
// SYNTHETIC: DX7HR 0x10001527
// SYNTHETIC: DX7HR 0x1000152b
// SYNTHETIC: DX7HR 0x1000152f
// SYNTHETIC: DX7HR 0x10001533
// SYNTHETIC: DX7HR 0x10001537
// SYNTHETIC: DX7HR 0x1000153b
// SYNTHETIC: DX7HR 0x1000153f
// SYNTHETIC: DX7HR 0x10001543
// SYNTHETIC: DX7HR 0x10001547
// SYNTHETIC: DX7HR 0x1000154b
// SYNTHETIC: DX7HR 0x1000154f
// SYNTHETIC: DX7HR 0x10001553
// SYNTHETIC: DX7HR 0x10001557
// SYNTHETIC: DX7HR 0x1000155b
// SYNTHETIC: DX7HR 0x1000155f
// SYNTHETIC: DX7HR 0x10001563
// SYNTHETIC: DX7HR 0x100015da
// SYNTHETIC: DX7HR 0x10001646

UString gName("Direct3D");
DX7Rasterizer gRasterizer(gName);
DX7Rasterizer* gRasterizerInstance = &gRasterizer;
unsigned long gType = 'rast';

// gName's and gRasterizer's init chains:
// SYNTHETIC: DX7HR 0x100016a2
// SYNTHETIC: DX7HR 0x100016ac
// SYNTHETIC: DX7HR 0x100016bd
// SYNTHETIC: DX7HR 0x100016c9
// SYNTHETIC: DX7HR 0x100016d4
// SYNTHETIC: DX7HR 0x100016de
// SYNTHETIC: DX7HR 0x100016ee
// SYNTHETIC: DX7HR 0x100016fa

// FUNCTION: DX7HR 0x10001704
// Plug-in DLL entry point; does nothing and always succeeds
BOOL WINAPI DllMain(HINSTANCE inInstance, DWORD inReason, LPVOID inReserved)
{
    return TRUE;
}

// FUNCTION: DX7HR 0x1000170a
// Returns the plug-in's name ("Direct3D")
extern "C" const UString* PIMGetName()
{
    return &gName;
}

// FUNCTION: DX7HR 0x10001710
// Returns the plug-in type ('rast', a rasterizer)
extern "C" unsigned long PIMGetType()
{
    return gType;
}

// FUNCTION: DX7HR 0x10001716
// Tells whether the renderer can run: activates it, checks it found at least one screen, then deactivates it
extern "C" bool PIMIsValid()
{
    bool theResult;
    if (gRasterizerInstance->Activate() == kErrorOK && gRasterizerInstance->GetScreenCount() > 0)
        theResult = true;
    else
        theResult = false;
    gRasterizerInstance->Deactivate();
    return theResult;
}

// FUNCTION: DX7HR 0x1000174c
// Returns the plug-in's rasterizer
extern "C" GERasterizer* RPIMGetRasterizer()
{
    return gRasterizerInstance;
}

// COMDATs this unit is the first (link order) to instantiate (the inline copy members' vectors, the UList pools),
// then the template-static init chain head and atexit registrars:
// TEMPLATE: DX7HR 0x10001752
// TEMPLATE: DX7HR 0x10001799
// TEMPLATE: DX7HR 0x100017e3
// TEMPLATE: DX7HR 0x10001931
// TEMPLATE: DX7HR 0x10001947
// TEMPLATE: DX7HR 0x10001a5b
// TEMPLATE: DX7HR 0x10001a6e
// TEMPLATE: DX7HR 0x10001a7a
// TEMPLATE: DX7HR 0x10001a86
// TEMPLATE: DX7HR 0x10001abf
// TEMPLATE: DX7HR 0x10001ae4
// SYNTHETIC: DX7HR 0x10001b1e
// TEMPLATE: DX7HR 0x10001b46
// SYNTHETIC: DX7HR 0x10001b61
// TEMPLATE: DX7HR 0x10001b6d
// SYNTHETIC: DX7HR 0x10001b88
// TEMPLATE: DX7HR 0x10001b94
// SYNTHETIC: DX7HR 0x10001baf
// TEMPLATE: DX7HR 0x10001bbb
// SYNTHETIC: DX7HR 0x10001bd6
// TEMPLATE: DX7HR 0x10001be2
// TEMPLATE: DX7HR 0x10001c07
// TEMPLATE: DX7HR 0x10001c29
// TEMPLATE: DX7HR 0x10001c4e
// TEMPLATE: DX7HR 0x10001c70
// TEMPLATE: DX7HR 0x10001ccd
// TEMPLATE: DX7HR 0x10001cf2
// TEMPLATE: DX7HR 0x10001d14
// TEMPLATE: DX7HR 0x10001d3c
// TEMPLATE: DX7HR 0x10001d71
// TEMPLATE: DX7HR 0x10001d96
// TEMPLATE: DX7HR 0x10001db8
// TEMPLATE: DX7HR 0x10001e1c
// TEMPLATE: DX7HR 0x10001e37
// TEMPLATE: DX7HR 0x10001e69
// TEMPLATE: DX7HR 0x10001ea3
// TEMPLATE: DX7HR 0x10001edd
// TEMPLATE: DX7HR 0x10001f17
