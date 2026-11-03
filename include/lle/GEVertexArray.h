// C:\EEX\Low-Level Engine\Graphics Engine\GEVertexArray.h (RECONSTRUCTION). Abstract (DX7HRDisplay
// DX7StandardVertexArray 0x105c8 gives the slot order); GEVertexArray.cpp ~0x100367fc holds ctor and dtor.
#ifndef LLE_GEVERTEXARRAY_H
#define LLE_GEVERTEXARRAY_H

class GEColor;
class GEVector;
class GE3DPoint;
class GETextureCoordinate;

class LLE_API GEVertexArray {               // sizeof 4 (copy ctor 0xebfa: vptr only)
public:
    GEVertexArray();
    virtual ~GEVertexArray();               // slot 0
    virtual void CopyFrom(const GEVertexArray& inArray) = 0;
    virtual void GetColor(GEColor& outColor, unsigned long inIndex) const = 0;
    virtual void GetNormal(GEVector& outNormal, unsigned long inIndex) const = 0;
    virtual float GetOpacity(unsigned long inIndex) const = 0;
    virtual void GetPoint(GE3DPoint& outPoint, unsigned long inIndex) const = 0;     // slot 0x14
    virtual unsigned long GetSize() const = 0;
    virtual void GetTextureCoordinate(GETextureCoordinate& outCoordinate, unsigned long inIndex) const = 0;
    virtual unsigned long GetType() const = 0;
    virtual void SetColor(const GEColor& inColor, unsigned long inIndex) = 0;        // slot 0x24
    virtual void SetNormal(const GEVector& inNormal, unsigned long inIndex) = 0;
    virtual void SetOpacity(float inOpacity, unsigned long inIndex) = 0;              // slot 0x2c
    virtual void SetPoint(const GE3DPoint& inPoint, unsigned long inIndex) = 0;      // slot 0x30
    virtual void SetTextureCoordinate(const GETextureCoordinate& inCoordinate, unsigned long inIndex) = 0;
    virtual void SetVertex(const GEColor& inColor, const GE3DPoint& inPoint, const GEVector& inNormal,
                           const GETextureCoordinate& inCoordinate, float inOpacity, unsigned long inIndex) = 0;
    virtual unsigned long SizeOf() const = 0;
};

#endif
