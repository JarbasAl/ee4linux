// C:\EEX\Low-Level Engine\File System\FSPersistentList.cpp (file name inferred: no __FILE__; RTTI
// .?AVFSPersistentList@@)
// Unit flags (docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>

#include "lle/LowLevelEngineCore.h"
#include "lle/UWideString.h"
#include "lle/UString.h"
#include "lle/FSMemoryFile.h"
#include "lle/TSScheduler.h"
#include "lle/TSReadWriteLock.h"
#include "lle/FSRestorable.h"
#include "lle/FSPersistent.h"
#include "TSLockable.h"
#include "lle/FSPersistentList.h"

// Compiler-generated for this unit (names in analysis/lle/names.txt):
// SYNTHETIC: LLE 0x1000bdb4
// SYNTHETIC: LLE 0x1000bdbe
// SYNTHETIC: LLE 0x1000bdc8
// SYNTHETIC: LLE 0x1000bdd4
// TEMPLATE: LLE 0x1000c100

TSLockable FSPersistentList::sCreateSingletonLock;
FSPersistentList* FSPersistentList::sSingleton;

// FUNCTION: LLE 0x1000bdfb
// Creates an empty registry of persistent types
FSPersistentList::FSPersistentList()
{
}

// FUNCTION: LLE 0x1000be25
// Destroys the registry, deleting every registered prototype
FSPersistentList::~FSPersistentList()
{
    std::map<long, FSPersistent*>::iterator thePersistent;
    for (thePersistent = this->mPersistents.begin(); thePersistent != this->mPersistents.end(); ++thePersistent)
        delete thePersistent->second;
    this->mPersistents.clear();
}

// FUNCTION: LLE 0x1000be88
// Returns a new default object of the registered type with the given tag, or NULL when no type has that tag
FSPersistent* FSPersistentList::CreateDefaultPersistent(long inTagID)
{
    FSPersistent* theResult = NULL;
    std::map<long, FSPersistent*>::iterator thePersistent;
    for (thePersistent = this->mPersistents.begin(); thePersistent != this->mPersistents.end(); ++thePersistent) {
        if (thePersistent->second->GetTagID() == inTagID) {
            theResult = thePersistent->second->CreateDefault();
            break;
        }
    }
    return theResult;
}

// FUNCTION: LLE 0x1000bed8
// Deletes the singleton registry under the creation lock (inForced is not used)
void FSPersistentList::Shutdown(bool inForced)
{
    FSPersistentList::sCreateSingletonLock.WriteLock();
    if (FSPersistentList::sSingleton) {
        delete FSPersistentList::sSingleton;
        FSPersistentList::sSingleton = NULL;
    }
    FSPersistentList::sCreateSingletonLock.WriteUnlock();
}

// FUNCTION: LLE 0x1000bf05
// Returns the singleton registry, creating it on first use (double-checked under the creation lock)
FSPersistentList* FSPersistentList::Instance()
{
    if (FSPersistentList::sSingleton == NULL) {
        FSPersistentList::sCreateSingletonLock.WriteLock();
        if (FSPersistentList::sSingleton == NULL)
            FSPersistentList::sSingleton = new FSPersistentList;
        FSPersistentList::sCreateSingletonLock.WriteUnlock();
    }
    return FSPersistentList::sSingleton;
}

// FUNCTION: LLE 0x1000bf54
// Registers a prototype under its tag; for a tag already taken deletes the prototype and returns kErrorAlreadyExists
long FSPersistentList::RegisterPersistent(FSPersistent* inPersistent)
{
    long theResult = kErrorOK;
    long theTagID = inPersistent->GetTagID();
    this->WriteLock();
    std::map<long, FSPersistent*>::iterator theFound = this->mPersistents.find(theTagID);
    if (theFound == this->mPersistents.end())
        this->mPersistents.insert(std::map<long, FSPersistent*>::value_type(theTagID, inPersistent));
    else {
        delete inPersistent;
        theResult = kErrorAlreadyExists;
    }
    this->WriteUnlock();
    return theResult;
}

// Compiler-generated, continued:
// TEMPLATE: LLE 0x1000bfd1
// TEMPLATE: LLE 0x1000bffc
// TEMPLATE: LLE 0x1000c0b2
// TEMPLATE: LLE 0x1000c0c6
