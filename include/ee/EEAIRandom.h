// C:\EEX\Empire Earth\AI Thread\EEAIRandom.h (RECONSTRUCTION; file name inferred; provisional names).
// The AI's random_shuffle generator and the by-reference shuffle helper. Evidence: the generator's operator() is the
// COMDAT 0x2cc49 (thiscall, ret 4: max(n - 1, 0) through gPhysics->GetRandom(0, ..)), called out of line by every
// _Random_shuffle copy (0x2cbf2, 0x406ea); the helper is the 39-byte COMDAT that builds the (empty) generator in a
// dead parameter slot and passes copies of both iterators (0x2cb99, 0x3ff2d, EECPResearchManager's 0x8b55b: three
// copies, so a template in a header). Requires ee/EEPhysics.h, <algorithm>.
#ifndef EE_EEAIRANDOM_H
#define EE_EEAIRANDOM_H

#include <algorithm>

struct EEAIRandomGenerator {
    long operator()(long inRange)
    {
        long theMaximum = inRange - 1;
        if (theMaximum < 0)
            theMaximum = 0;
        return gPhysics->GetRandom(0L, theMaximum);
    }
};

template<class _RI> void EEAIRandomShuffle(const _RI& inFirst, const _RI& inLast)
{
    EEAIRandomGenerator theGenerator;
    std::random_shuffle(inFirst, inLast, theGenerator);
}

#endif
