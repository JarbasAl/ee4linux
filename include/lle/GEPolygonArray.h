// C:\EEX\Low-Level Engine\Graphics Engine\GEPolygonArray.h (RECONSTRUCTION). Abstract (DX7HRDisplay DX7PolygonArray
// gives the slot order); GEPolygonArray.cpp ~0x10023209 holds ctor and dtor.
#ifndef LLE_GEPOLYGONARRAY_H
#define LLE_GEPOLYGONARRAY_H

class LLE_API GEPolygonArray {              // sizeof 4 (copy ctor 0x11176: vptr only)
public:
    GEPolygonArray();
    virtual ~GEPolygonArray();
    virtual unsigned long GetSize() const = 0;
    virtual unsigned long GetVertexIndex1(unsigned long inPolygon) const = 0;
    virtual unsigned long GetVertexIndex2(unsigned long inPolygon) const = 0;
    virtual unsigned long GetVertexIndex3(unsigned long inPolygon) const = 0;
    virtual void SetVertexIndex1(unsigned long inPolygon, unsigned long inIndex) = 0;
    virtual void SetVertexIndex2(unsigned long inPolygon, unsigned long inIndex) = 0;
    virtual void SetVertexIndex3(unsigned long inPolygon, unsigned long inIndex) = 0;
    virtual void SetVertexIndices(unsigned long inPolygon, unsigned long inIndex1, unsigned long inIndex2,
                                  unsigned long inIndex3) = 0;   // (inPolygon first: DX7PolygonArray.cpp)
    virtual unsigned long SizeOf() const = 0;
};

#endif
