// C:\EEX\Low-Level Engine\Graphics Engine\GETextureCoordinate.h (RECONSTRUCTION). GETextureCoordinate.cpp
// (~0x10033b1f) holds Load/Save; everything else is inline (ICF-folded with the GE2DPoint twins).
#ifndef LLE_GETEXTURECOORDINATE_H
#define LLE_GETEXTURECOORDINATE_H

class FSFileSpec;

class LLE_API GETextureCoordinate {         // sizeof 8: u, v
public:
    GETextureCoordinate() : mU(0.0f), mV(0.0f) {}
    GETextureCoordinate(float inU, float inV) : mU(inU), mV(inV) {}
    ~GETextureCoordinate() {}
    GETextureCoordinate& operator=(const GETextureCoordinate& inCoordinate)
        { this->mU = inCoordinate.mU; this->mV = inCoordinate.mV; return *this; }
    bool operator==(const GETextureCoordinate& inCoordinate) const
        { return this->mU == inCoordinate.mU && this->mV == inCoordinate.mV; }
    bool operator!=(const GETextureCoordinate& inCoordinate) const
        { return this->mU != inCoordinate.mU || this->mV != inCoordinate.mV; }
    float GetU() const { return this->mU; }
    float GetV() const { return this->mV; }
    void SetU(float inU) { this->mU = inU; }
    void SetV(float inV) { this->mV = inV; }
    long Load(FSFileSpec& inFile);
    long Save(FSFileSpec& inFile);

    float mU;                               // +0x00
    float mV;                               // +0x04
};

#endif
