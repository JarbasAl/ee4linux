// (RECONSTRUCTION; file name inferred; function and constant names are provisional).
// Saved-game file helpers next to EEPSaveGameHeader.cpp (0x14ce98, 0x14d5ac) and the save version constants the
// lobby stores into an EEPSaveGameHeader before LoadFromFile (.rdata 0x4420ac / 0x4420b0).
// Requires <vector>, lle/LowLevelEngine.h.
#ifndef EE_EESAVEGAMEFILES_H
#define EE_EESAVEGAMEFILES_H

class EEGameOptions;
class FSFile;
class FSFileSpec;

extern const unsigned long kSaveGameMajorVersion;   // 0x4420ac
extern const unsigned long kSaveGameMinorVersion;   // 0x4420b0

// 0x14ce98: opens inName in data\inDirectory (the lobby's load checks); keseOK and outFile on success
// (gDataPath->FindFile(inDirectory + inName, outFile), hence FSFileSpec*&)
long EEOpenSaveGameFile(const UString& inName, const UString& inDirectory, FSFileSpec*& outFile);
// 0x14d946: re-opens inFile, loads its header and checks its options (0x14da7c); outError gets the message when not
bool EEValidateSaveGameFile(FSFile& inFile, UWideString& outError);
// 0x14d5ac: compares a saved game's options with the current ones; fills outMissing with what does not match
bool EECheckSavedGameOptions(EEGameOptions& inSaved, EEGameOptions& inCurrent, bool inFlag,
                             std::vector<UWideString>& outMissing, long inCount);

#endif
