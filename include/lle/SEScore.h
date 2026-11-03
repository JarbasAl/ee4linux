// C:\EEX\Low-Level Engine\Sound Engine\SEScore.h (RECONSTRUCTION; SEScore.cpp 0x4eef6..0x4f08e). A DirectMusic
// segment loaded by the conductor, plus its style's motifs as secondary scores. The inline members are emitted
// first by SEConductor.cpp (0x4c6ff..0x4c7ae). Requires lle/LowLevelEngine.h and <dmusici.h>.
#ifndef LLE_SESCORE_H
#define LLE_SESCORE_H

#include <vector>

struct IDirectMusicSegment;

class LLE_API SEScore {                     // proven sizeof 0x1c (operator new(0x1c) in CreateSecondaryScores)
    friend class SEConductor;
public:
    bool GetIsSecondaryScore() const { return this->mIsSecondaryScore; }
    unsigned long GetRepeatCount();
    SEScore* GetSecondaryScore(unsigned long inIndex) const { return this->mSecondaryScores[inIndex]; }
    unsigned long GetSecondaryScoreCount() const { return this->mSecondaryScores.size(); }
    void SetRepeatCount(unsigned long inCount);
private:
    IDirectMusicSegment* GetSegment() const { return this->mSegment; }
    SEScore(IDirectMusicSegment* inSegment, bool inIsSecondaryScore)
    {
        this->mSegment = inSegment;
        this->mIsSecondaryScore = inIsSecondaryScore;
        this->CreateSecondaryScores();
    }
    virtual ~SEScore();
    void CreateSecondaryScores();

    std::vector<SEScore*> mSecondaryScores; // +0x04 the style's motifs
    IDirectMusicSegment* mSegment;          // +0x14
    bool mIsSecondaryScore;                 // +0x18
};

#endif
