// HMUFractal: the height-map fractal (U2DFractal whose value generator follows the random map's land/water
// tiles). Module-local; names PROVISIONAL except the class name (RTTI .?AVHMUFractal@@).
// Requires lle/LowLevelEngine.h, lle/U2DFractal.h, ee/EERandomMap.h.
#ifndef HMUFRACTAL_H
#define HMUFRACTAL_H

class HMUFractal : public U2DFractal {     // proven sizeof 0x10 (CMUCreateHeightMap 0xaf67 stack object)
public:
    HMUFractal(EERandomMap* inMap) : U2DFractal(inMap->mGridSize), mMap(inMap) {}
    virtual ~HMUFractal() {}

protected:
    virtual float GenerateFractalValue(URandom& inRandom, float inRange, float inScale, float inAverage,
                                       unsigned long inX, unsigned long inY);

    EERandomMap* mMap;                      // proven +0x0c
};

long CMUCreateHeightMap(EERandomMap* ioMap);
long CMUFilterHeightMap(EERandomMap* ioMap);

#endif
