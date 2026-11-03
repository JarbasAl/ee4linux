// C:\EEX\Low-Level Engine\Graphics Engine\GEPoint.h (RECONSTRUCTION). A one-vertex point in a rasterizer vertex array.
// GEPoint.cpp (~0x100230ea) holds the ctors; the dtor and SetTexture are shared with GELine (ICF).
#ifndef LLE_GEPOINT_H
#define LLE_GEPOINT_H

class GETexture;

class LLE_API GEPoint {                     // sizeof 0xc (ctors 0x230ea/0x2315d store the screen count at +8)
public:
    GEPoint();
    GEPoint(GE2DPoint& inPoint, GEColor& inColor, float inOpacity);
    ~GEPoint();
    void GetColor(GEColor& outColor) const { this->mVertices->GetColor(outColor, 0); }
    float GetOpacity() const { return this->mVertices->GetOpacity(0); }
    void GetPoint(GE2DPoint& outPoint) const
    {
        GE3DPoint thePoint;
        this->mVertices->GetPoint(thePoint, 0);
        outPoint.SetX(thePoint.GetX());
        outPoint.SetY(thePoint.GetY());
    }
    GETexture* GetTexture(unsigned long inScreen) const { return this->mTextures[inScreen]; }
    void GetTextureCoordinate(GETextureCoordinate& outCoordinate) { this->mVertices->GetTextureCoordinate(outCoordinate, 0); }
    GEVertexArray* GetVertexArray() { return this->mVertices; }
    void SetColor(const GEColor& inColor) { this->mVertices->SetColor(inColor, 0); }
    void SetOpacity(float inOpacity) { this->mVertices->SetOpacity(inOpacity, 0); }
    void SetPoint(GE2DPoint& inPoint)
    {
        GE3DPoint thePoint;
        thePoint.SetX(inPoint.GetX());
        thePoint.SetY(inPoint.GetY());
        this->mVertices->SetPoint(thePoint, 0);
    }
    void SetTexture(GETexture* inTexture, unsigned long inScreen);
    void SetTextureCoordinate(GETextureCoordinate& inCoordinate) { this->mVertices->SetTextureCoordinate(inCoordinate, 0); }
private:
    GEVertexArray* mVertices;               // +0x00
    GETexture** mTextures;                  // +0x04
    unsigned long mScreenCount;             // +0x08 proven by the ctors (GetScreenCount) and the dtor loop (0x16a11)
};

#endif
