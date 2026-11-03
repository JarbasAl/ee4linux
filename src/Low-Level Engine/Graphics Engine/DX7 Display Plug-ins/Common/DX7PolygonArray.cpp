// C:\EEX\Low-Level Engine\Graphics Engine\DX7 Display Plug-ins\Common\DX7PolygonArray.cpp
// Unit flags (hypothesis, docs/CALIBRATION.md "dx7hr"): -O1 -G6 -MD -GR
//
// 0x1000295d..0x10002b04. No init chains: the unit includes no header with static objects.
#include <windows.h>

#include "lle/LowLevelEngine.h"
#include "dx7/DX7PolygonArray.h"

// Compiler-generated for the dllexport class, emitted at its definition (implicit copy ctor, operator=), and the
// vector deleting dtor:
// SYNTHETIC: DX7HR 0x1000295d
// SYNTHETIC: DX7HR 0x10002986
// SYNTHETIC: DX7HR 0x100029a1

// FUNCTION: DX7HR 0x100029fc
// Creates a polygon array with room for inSize triangles (three 16-bit vertex indices each)
DX7PolygonArray::DX7PolygonArray(unsigned long inSize)
{
    this->mIndices = new unsigned short[inSize * 3];
    this->mIndexCount = inSize * 3;
    this->mSize = inSize;
}

// FUNCTION: DX7HR 0x10002a2f
// Destroys the polygon array, freeing its index buffer
DX7PolygonArray::~DX7PolygonArray()
{
    delete[] this->mIndices;
}

// FUNCTION: DX7HR 0x10002a4b
// Returns the number of triangles
unsigned long DX7PolygonArray::GetSize() const
{
    return this->mSize;
}

// FUNCTION: DX7HR 0x10002a4f
// Returns a triangle's first vertex index
unsigned long DX7PolygonArray::GetVertexIndex1(unsigned long inPolygon) const
{
    return this->mIndices[inPolygon * 3];
}

// FUNCTION: DX7HR 0x10002a60
// Returns a triangle's second vertex index
unsigned long DX7PolygonArray::GetVertexIndex2(unsigned long inPolygon) const
{
    return this->mIndices[inPolygon * 3 + 1];
}

// FUNCTION: DX7HR 0x10002a72
// Returns a triangle's third vertex index
unsigned long DX7PolygonArray::GetVertexIndex3(unsigned long inPolygon) const
{
    return this->mIndices[inPolygon * 3 + 2];
}

// FUNCTION: DX7HR 0x10002a84
// Sets a triangle's first vertex index (stored as 16 bits)
void DX7PolygonArray::SetVertexIndex1(unsigned long inPolygon, unsigned long inIndex)
{
    this->mIndices[inPolygon * 3] = (unsigned short)inIndex;
}

// FUNCTION: DX7HR 0x10002a9a
// Sets a triangle's second vertex index (stored as 16 bits)
void DX7PolygonArray::SetVertexIndex2(unsigned long inPolygon, unsigned long inIndex)
{
    this->mIndices[inPolygon * 3 + 1] = (unsigned short)inIndex;
}

// FUNCTION: DX7HR 0x10002ab1
// Sets a triangle's third vertex index (stored as 16 bits)
void DX7PolygonArray::SetVertexIndex3(unsigned long inPolygon, unsigned long inIndex)
{
    this->mIndices[inPolygon * 3 + 2] = (unsigned short)inIndex;
}

// FUNCTION: DX7HR 0x10002ac8
// Sets all three vertex indices of a triangle (stored as 16 bits)
void DX7PolygonArray::SetVertexIndices(unsigned long inPolygon, unsigned long inIndex1, unsigned long inIndex2,
                                       unsigned long inIndex3)
{
    this->mIndices[inPolygon * 3] = (unsigned short)inIndex1;
    this->mIndices[inPolygon * 3 + 1] = (unsigned short)inIndex2;
    this->mIndices[inPolygon * 3 + 2] = (unsigned short)inIndex3;
}

// FUNCTION: DX7HR 0x10002afc
// Returns the array's memory size: the index buffer plus 12 bytes
unsigned long DX7PolygonArray::SizeOf() const
{
    return this->mIndexCount * sizeof(unsigned short) + 12;
}

// DX7HRTnLDisplay.dll (the T&L twin builds this unit unchanged): the same functions at their twin addresses.
// SYNTHETIC: DX7HRTNL 0x100029dc
// SYNTHETIC: DX7HRTNL 0x10002a05
// SYNTHETIC: DX7HRTNL 0x10002a20
// FUNCTION: DX7HRTNL 0x10002a7b
// FUNCTION: DX7HRTNL 0x10002aae
// FUNCTION: DX7HRTNL 0x10002aca
// FUNCTION: DX7HRTNL 0x10002ace
// FUNCTION: DX7HRTNL 0x10002adf
// FUNCTION: DX7HRTNL 0x10002af1
// FUNCTION: DX7HRTNL 0x10002b03
// FUNCTION: DX7HRTNL 0x10002b19
// FUNCTION: DX7HRTNL 0x10002b30
// FUNCTION: DX7HRTNL 0x10002b47
// FUNCTION: DX7HRTNL 0x10002b7b
