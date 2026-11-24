// Mini map cell indices (src/Empire Earth/Map/EEMiniMap.cpp). A unit at tile (21, 33) on the standard map maps to
// cell (-1, 23) after the map-to-cell transform and the x87 conversion. The unguarded index was -41, which wrote
// before the fixed cell arrays and corrupted the mini map. The expected values below are that index and the grid's
// limits, worked out by hand.
#include <cstdio>

#include "../../../include/ee/EEMiniMapCells.h"

static int sChecks, sFailures;

static void Check(bool inOk, const char* inWhat)
{
    ++sChecks;
    if (!inOk) {
        ++sFailures;
        fprintf(stderr, "FAIL: %s\n", inWhat);
    }
}

int main()
{
    Check(EEMiniMapCells::kGrid == 64, "the battle grid has 64 cells per side");
    Check(EEMiniMapCells::Clamp(-1) == 0, "the observed edge cell clamps to zero");
    Check(EEMiniMapCells::Clamp(23) == 23, "an in-range cell is unchanged");
    Check(EEMiniMapCells::Clamp(63) == 63, "the last cell is unchanged");
    Check(EEMiniMapCells::Clamp(64) == 63, "one past the grid clamps to the last cell");
    Check(EEMiniMapCells::Clamp(-10000) == 0, "the off-map sentinel clamps to zero");
    Check(EEMiniMapCells::Flatten(-1, 23) == 23, "the reproduced index stays inside the arrays");
    Check(EEMiniMapCells::Flatten(0, 0) == 0, "the first cell is index zero");
    Check(EEMiniMapCells::Flatten(63, 63) == 4095, "the last cell is the last array element");
    Check(EEMiniMapCells::Flatten(64, 23) == 63 * 64 + 23, "an out-of-range x flattens to the last row");
    Check(EEMiniMapCells::Flatten(-1, -1) == 0, "both out-of-range cells flatten to zero");

    printf("%d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
