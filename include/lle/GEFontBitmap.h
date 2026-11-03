// C:\EEX\Low-Level Engine\Graphics Engine\GEFontBitmap.h (RECONSTRUCTION). GEFontBitmap.cpp 0x100152e4..0x100166d3.
// The implicit copy members (0x1001064c/0x1001068b) are emitted by GEBoundingBox.cpp's inline block.
#ifndef LLE_GEFONTBITMAP_H
#define LLE_GEFONTBITMAP_H

#include <vector>
#include "lle/GETextureCoordinate.h"

class GEBitmap;
class GEFont;
class GERasterizer;
class GEScreen;
class GESurface;
class GEText;
class GETexture;

// One glyph cell of the font textures (provisional names). sizeof 0x2c (vector<GEFBCharacter>: size / 0x2c).
// The implicit default ctor (0x16206), operator= (0x14b3c: GETextureCoordinate/GERectangle operator= field order)
// and the empty dtor (vector _Destroy calls ??_G(0); the ctor's resize temporary calls it out of line) are proven
// by codegen; the copy ctor is trivial (rep movsd 11).
struct GEFBCharacter {
    ~GEFBCharacter() {}

    GETextureCoordinate mTopLeft;           // +0x00 texture coordinates (GEBitmap::GetTextureCoordinatesPerVertex)
    GETextureCoordinate mBottomRight;       // +0x08
    GERectangle mRect;                      // +0x10 the glyph's cell in its texture
    GETexture* mTexture;                    // +0x20 (referenced)
    float mXScale;                          // +0x24 GEBitmap::GetXScale after ComputeStretchedCoordsAndScale
    float mYScale;                          // +0x28
};

class LLE_API GEFontBitmap : public UReferenceCounter {   // sizeof 0x24 (vector deleting dtor 0x106be)
public:
    GEFontBitmap(GERasterizer* inRasterizer, GEFont* inFont, unsigned long inScreen);
    bool operator==(GEFont* inFont);
    long Draw(GEText* inText, GERasterizer* inRasterizer, unsigned long inScreen);
    long Draw(GEText* inText, const GERectangle& inRectangle, GERasterizer* inRasterizer, unsigned long inScreen);
protected:
    virtual ~GEFontBitmap();                // 0x159ac (protected: released through Dereference)
private:
    GETexture* CreateNewFontTexture(GEScreen* inScreen);
    long CreateSurfaceAlpha(GESurface* inSurface);

    std::vector<GEFBCharacter> mCharacters; // +0x08 indexed by character - mFirstCharacter
    GEBitmap* mBitmap;                      // +0x18 (dereferenced by the dtor)
    unsigned long m1C;                      // +0x1c
    unsigned long mFirstCharacter;          // +0x20 tmFirstChar
};

#endif
