// C:\EEX\Low-Level Engine\Network Engine\Network Engine.cpp (file name inferred: no __FILE__; by analogy with
// Error System.cpp / Thread System.cpp: sorts after NETimestampedMessage.cpp)
// Unit flags (docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/PIMModule.h"
#include "lle/NEProtocol.h"

// FUNCTION: LLE 0x1004b190
// Loads every network plug-in ('ntwk') in a directory as a protocol; fails if none is usable
long NEInitialize(FSFileSpec& inDirectory)
{
    PIMModule* theModule;
    NEProtocol* theProtocol;
    long theResult = URegisterShutdown(NEShutdown);
    if (theResult != kErrorOK)
        return theResult;
    theResult = PIMModule::GetFirstModule(inDirectory, 'ntwk', theModule);
    while (theResult == kErrorOK) {
        theProtocol = new NEProtocol(theModule);
        if (!theModule->IsValid() && theProtocol != NULL)
            delete theProtocol;
        theResult = PIMModule::GetNextModule(theModule);
    }
    if (theResult == kErrorNoMoreFiles)
        theResult = NEProtocol::GetCount() > 0 ? kErrorOK : kErrorNotFound;
    return theResult;
}

// FUNCTION: LLE 0x1004b235
// Destroys every registered network protocol at shutdown
void NEShutdown(bool inForced)
{
    NEProtocol* theProtocol = NULL;
    while (NEProtocol::GetCount() > 0) {
        NEProtocol::GetProtocol(0, theProtocol);
        delete theProtocol;
    }
}
