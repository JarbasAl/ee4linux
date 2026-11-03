// C:\EEX\Low-Level Engine\Graphics Engine\GETextureSet.h (RECONSTRUCTION). A grid of textures tiled over the terrain.
// GETextureSet.cpp (0x100346a7..0x100349a3) holds the ctor, dtor and SetTexture; the rest is inline (GEBoundingBox.cpp
// emits it first, 0x10012426..).
#ifndef LLE_GETEXTURESET_H
#define LLE_GETEXTURESET_H

#include <vector>

class GETexture;
class GETextureCoordinate;

class LLE_API GETextureSet {                // sizeof 0x1c (implicit copy ctor 0x124cb, operator= 0x124f2)
public:
    GETextureSet(unsigned long inNumTexturesInWidth, unsigned long inNumTexturesInHeight,
                 unsigned long inTextureTileCoverage);
    ~GETextureSet();
    unsigned long GetNumTexturesInHeight() { return this->mNumTexturesInHeight; }
    unsigned long GetNumTexturesInWidth() { return this->mNumTexturesInWidth; }
    GETexture* GetTexture(unsigned long inX, unsigned long inY)
    {
        return this->mTextures[(inX / this->mTextureTileCoverage) % this->mNumTexturesInWidth]
                        [(inY / this->mTextureTileCoverage) % this->mNumTexturesInHeight];
    }
    void GetTextureCoordinate(unsigned long inX, unsigned long inY, GETextureCoordinate& outCoordinate)
    {
        outCoordinate.SetU((float)(inX % this->mTextureTileCoverage) / (float)this->mTextureTileCoverage);
        outCoordinate.SetV((float)(inY % this->mTextureTileCoverage) / (float)this->mTextureTileCoverage);
    }
    // 0x124aa: only the X index reaches the caller (the Y index parameter is by value in the export's signature)
    void GetTextureIndices(unsigned long inX, unsigned long inY, unsigned long& outX, unsigned long outY)
    {
        outX = (inX / this->mTextureTileCoverage) % this->mNumTexturesInWidth;
        outY = (inY / this->mTextureTileCoverage) % this->mNumTexturesInHeight;
    }
    unsigned long GetTextureTileCoverage() { return this->mTextureTileCoverage; }
    void SetTexture(unsigned long inX, unsigned long inY, GETexture* inTexture);
    void SetTextureTileCoverage(unsigned long inCoverage) { this->mTextureTileCoverage = inCoverage; }

private:
    std::vector<std::vector<GETexture*> > mTextures;   // +0x00 [x][y]
    unsigned long mNumTexturesInWidth;                 // +0x10
    unsigned long mNumTexturesInHeight;                // +0x14
    unsigned long mTextureTileCoverage;                // +0x18
};

#endif
