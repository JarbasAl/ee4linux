// C:\EEX\Low-Level Engine\Sound Engine\SEScore.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
#include <windows.h>
#include <dmusici.h>

#include "lle/LowLevelEngine.h"
#include "lle/SEMixer.h"
#include "lle/SEScore.h"

// Compiler-generated for this unit (names in analysis/lle/names.txt):

// FUNCTION: LLE 0x1004eef6
// Destroys the score: deletes its motif scores, unloads its segment and releases it (motifs are not released)
SEScore::~SEScore()
{
    for (unsigned long i = 0; i < this->mSecondaryScores.size(); i++)
        delete this->mSecondaryScores[i];
    this->mSegment->SetParam(GUID_Unload, 0xffffffff, 0, 0, SEMixer::GetActiveMixer()->GetConductor().GetPerformance());
    if (!this->mIsSecondaryScore)
        this->mSegment->Release();
}

// FUNCTION: LLE 0x1004ef63
// Creates a secondary score for every motif of the segment's style (main scores only)
// NEAR: the original tests EnumMotif's result with `cmp eax, ebx` (the zero register) instead of `test eax, eax`
void SEScore::CreateSecondaryScores()
{
    IDirectMusicStyle* theStyle;
    IDirectMusicSegment* theMotif;
    WCHAR theName[MAX_PATH];
    DWORD theIndex = 0;
    bool theDone = false;
    if (this->mIsSecondaryScore ||
        !SUCCEEDED(this->mSegment->GetParam(GUID_IDirectMusicStyle, 0xffffffff, 0, 0, NULL, &theStyle)))
        return;
    do {
        if (theStyle->EnumMotif(theIndex, theName) == S_OK) {
            if (theStyle->GetMotif(theName, &theMotif) == S_OK)
                this->mSecondaryScores.push_back(new SEScore(theMotif, true));
        } else
            theDone = true;
        theIndex++;
    } while (!theDone);
}

// FUNCTION: LLE 0x1004f034
// Returns how many times the score repeats (0 on failure)
unsigned long SEScore::GetRepeatCount()
{
    DWORD theCount;
    if (FAILED(this->mSegment->GetRepeats(&theCount)))
        theCount = 0;
    return theCount;
}

// FUNCTION: LLE 0x1004f052
// Sets how many times the score repeats
void SEScore::SetRepeatCount(unsigned long inCount)
{
    this->mSegment->SetRepeats(inCount);
}

// Template COMDATs emitted in this unit (markers at the end):
// TEMPLATE: LLE 0x1004f062
