// C:\EEX\Low-Level Engine\Graphics Engine\GETextureCoordinate.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x10033b1f..0x10033bef: Load and Save (everything else is header-inline). A failed read or write rewinds the file
// to where the coordinate started.
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/FSFileSpec.h"
#include "lle/GETextureCoordinate.h"

// FUNCTION: LLE 0x10033b1f
// Reads the U and V coordinates from an open file; on failure rewinds to where it started
long GETextureCoordinate::Load(FSFileSpec& inFile)
{
    long theResult;
    long thePosition;
    if (inFile.GetIsOpen()) {
        theResult = inFile.GetPosition(thePosition);
        if (theResult == kErrorOK) {
            theResult = inFile.Read(this->mU);
            if (theResult == kErrorOK)
                theResult = inFile.Read(this->mV);
            if (theResult != kErrorOK)
                inFile.SetPosition(thePosition, kFileOriginBegin, false);
        }
    } else
        theResult = kErrorNotOpen;
    return theResult;
}

// FUNCTION: LLE 0x10033b87
// Writes the U and V coordinates to an open file; on failure rewinds to where it started
long GETextureCoordinate::Save(FSFileSpec& inFile)
{
    long theResult;
    long thePosition;
    if (inFile.GetIsOpen()) {
        theResult = inFile.GetPosition(thePosition);
        if (theResult == kErrorOK) {
            theResult = inFile.Write(this->mU);
            if (theResult == kErrorOK)
                theResult = inFile.Write(this->mV);
            if (theResult != kErrorOK)
                inFile.SetPosition(thePosition, kFileOriginBegin, false);
        }
    } else
        theResult = kErrorNotOpen;
    return theResult;
}
