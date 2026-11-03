// C:\EEX\Empire Earth\Persistent\EEPChecksum.cpp (file name inferred: no __FILE__; the class family and its keys
// "EEPChecksum-..."). A saved ESChecksum: 0x149c3c..0x14a009 (where the unit ends is not checked yet).
// Unit flags: the exe's module set. Data: the vtables 0x442254/0x442234; the keys' guard byte 0x531a28.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "lle/FSRestorable.h"
#include "lle/FSPersistent.h"

#include "ee/EEPChecksum.h"
#include "ee/EEPersistentTag.h"

const UString EEPChecksum::sNumChecksums("EEPChecksum-NumChecksums");
const UString EEPChecksum::sChecksum("EEPChecksum-Checksum");
const UString EEPChecksum::sVerboseMode("EEPChecksum-VerboseMode");

// FUNCTION: EXE 0x549d05
// Creates an empty saved checksum chunk, verbose or not
EEPChecksum::EEPChecksum(bool inVerbose)
    : ESChecksum(inVerbose), FSPersistent(kPersistentChecksum)
{
}

// FUNCTION: EXE 0x549d3c
// Creates an empty verbose saved checksum chunk (persistent factory)
FSPersistent* EEPChecksum::CreateDefault() const
{
    return new EEPChecksum(true);
}

// FUNCTION: EXE 0x549d5f
// Adds the chunk's total checksum to a running checksum as "EEPChecksum"
void EEPChecksum::GetChecksum(ESChecksum& ioChecksum) const
{
    unsigned long theChecksum = this->GetTotalChecksum();
    ioChecksum.AddChecksum("EEPChecksum", theChecksum);
}

// FUNCTION: EXE 0x549d9b
// Reads the verbose flag and the described checksum entries from a saved game; returns the first read error
long EEPChecksum::Load(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& inFile)
{
    long theCount = 0;
    UString theDescription;
    ESChecksumEntry theEntry;
    unsigned long theValue;
    this->mEntries.clear();
    long theResult = inFile.Read(this->mVerbose);
    if (theResult != kErrorOK)
        return theResult;
    theResult = inFile.Read(theCount);
    if (theResult != kErrorOK)
        return theResult;
    for (long i = 0; i < theCount && theResult == kErrorOK; i++) {
        theResult = theDescription.Load(inFile);
        if (theResult != kErrorOK)
            continue;
        theEntry.SetDescription(theDescription);
        theResult = inFile.Read(theValue);
        if (theResult == kErrorOK) {
            theEntry.SetChecksum(theValue);
            this->mEntries.push_back(theEntry);
        }
    }
    return theResult;
}

// FUNCTION: EXE 0x549e5c
// Writes the verbose flag and the described checksum entries to a save; returns the first write error
long EEPChecksum::Save(unsigned long inMajorVersion, unsigned long inMinorVersion, FSFile& outFile)
{
    std::vector<ESChecksumEntry>::const_iterator it = this->mEntries.begin();
    long theResult = outFile.Write(this->mVerbose);
    if (theResult != kErrorOK)
        return theResult;
    theResult = outFile.Write((long)this->mEntries.size());
    if (theResult != kErrorOK)
        return theResult;
    for (; it != this->mEntries.end() && theResult == kErrorOK; ++it) {
        theResult = it->GetDescription().Save(outFile);
        if (theResult == kErrorOK)
            theResult = outFile.Write(it->GetChecksum());
    }
    return theResult;
}

// TEMPLATE: EXE 0x549edb
// std::vector<ESChecksumEntry>::push_back (first used by Load)

// TEMPLATE: EXE 0x549f07
// std::vector<ESChecksumEntry>::clear

// TEMPLATE: EXE 0x549f42
// ??H?$_Ptrit@VESChecksumEntry@@HPAV1@AAV1@PAV1@AAV1@@std@@QBE?AV01@H@Z
// TEMPLATE: EXE 0x549f57
// std::copy for ESChecksumEntry (vector erase)

// TEMPLATE: EXE 0x549f85
// std::copy_backward for ESChecksumEntry (vector insert)

// SYNTHETIC: EXE 0x549fb8
// (the FSPersistent vtable's adjustor thunk for the deleting destructor: this -= 0x18)

// SYNTHETIC: EXE 0x549fc0
// (the scalar deleting destructor)

// FUNCTION: EXE 0x549fdc
// Destroys the saved checksum chunk
// the inline destructor (ee/EEPChecksum.h), after its first user

// SYNTHETIC: EXE 0x549c3c
// SYNTHETIC: EXE 0x549c46
// SYNTHETIC: EXE 0x549c57
// SYNTHETIC: EXE 0x549c63
// SYNTHETIC: EXE 0x549c7f
// SYNTHETIC: EXE 0x549c89
// SYNTHETIC: EXE 0x549c9a
// SYNTHETIC: EXE 0x549ca6
// SYNTHETIC: EXE 0x549cc2
// SYNTHETIC: EXE 0x549ccc
// SYNTHETIC: EXE 0x549cdd
// SYNTHETIC: EXE 0x549ce9
