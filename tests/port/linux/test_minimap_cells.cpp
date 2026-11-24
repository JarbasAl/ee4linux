// Mini map cell indices (src/Empire Earth/Map/EEMiniMap.cpp). A unit at tile (21, 33) on the standard map maps to
// cell (-1, 23) after the map-to-cell transform and the x87 conversion. The unguarded index was -41, which wrote
// before the fixed cell arrays and corrupted the mini map. The expected values below are that index and the grid's
// limits, worked out by hand.
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

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

// The native suite links the Win32 layer, not the exe module, so checking the helper alone would not catch a removal
// of the clamps at their call sites. Read the production source and fail if an unclamped cell expression returns.
static bool CallSitesAreClamped()
{
    char theResolved[4096];
    const char* theAbsolute = realpath(__FILE__, theResolved);
    const std::string theTest(theAbsolute ? theAbsolute : __FILE__);
    const std::string kMarker = "/tests/port/linux/";
    const std::size_t thePos = theTest.find(kMarker);
    if (thePos == std::string::npos) {
        fprintf(stderr, "FAIL: cannot locate the source tree from %s\n", theTest.c_str());
        return false;
    }
    const std::string theSource = theTest.substr(0, thePos) + "/src/Empire Earth/Map/EEMiniMap.cpp";
    std::ifstream theFile(theSource);
    if (!theFile) {
        fprintf(stderr, "FAIL: cannot read %s\n", theSource.c_str());
        return false;
    }
    std::ostringstream theText;
    theText << theFile.rdbuf();
    const std::string theCode = theText.str();

    // Each shape below was in the fault path: a transform result or an array index with no clamp.
    static const char* const kBad[] = {
        "theCellX = UMFloatToLong(",       "theCellY = UMFloatToLong(",
        "theOldCellX = UMFloatToLong(",    "theOldCellY = UMFloatToLong(",
        "inX = UMFloatToLong(",            "inY = UMFloatToLong(",
        "mMapUnits[theCellX * 0x40",       "mMapUnits[theOldCellX * 0x40",
        "mMapUnits[inX * 0x40",            "mPlayerUnits[theCellX * 0x40",
        "mPlayerUnits[inX * 0x40",         "mU1015C[inY * 0x40",
        "mU1015C[inX * 0x40",
    };
    bool theOk = true;
    for (const char* thePattern : kBad) {
        if (theCode.find(thePattern) != std::string::npos) {
            fprintf(stderr, "FAIL: unclamped cell expression in EEMiniMap.cpp: %s\n", thePattern);
            theOk = false;
        }
    }
    // RedrawCell writes pixels with inX and inY directly, so its entry clamp keeps those writes in range too.
    static const char* const kNeeded[] = {
        "#include \"ee/EEMiniMapCells.h\"",
        "inX = EEMiniMapCells::Clamp(inX);",
        "inY = EEMiniMapCells::Clamp(inY);",
    };
    for (const char* thePattern : kNeeded) {
        if (theCode.find(thePattern) == std::string::npos) {
            fprintf(stderr, "FAIL: missing clamp in EEMiniMap.cpp: %s\n", thePattern);
            theOk = false;
        }
    }
    return theOk;
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
    Check(CallSitesAreClamped(), "the production cell-index sites still use the clamp");

    printf("%d checks, %d failures\n", sChecks, sFailures);
    return sFailures == 0 ? 0 : 1;
}
