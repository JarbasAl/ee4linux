// C:\EEX\Low-Level Engine\Graphics Engine\GE2DVertex.h (RECONSTRUCTION). A struct; all members inline (GEBitmap.cpp
// emits the ctor and the implicit copy members first, 0x1000f2d4..).
#ifndef LLE_GE2DVERTEX_H
#define LLE_GE2DVERTEX_H

struct LLE_API GE2DVertex {                 // sizeof 0x34
    GE2DVertex() {}                         // 0xf2d4: only the members' default ctors
    ~GE2DVertex() {}

    GE2DPoint mLocation;                    // +0x00
    GEColor mDiffuseColor;                  // +0x08 (provisional names)
    GEColor mSpecularColor;                 // +0x14
    GETextureCoordinate mTextureCoordinate; // +0x20
    float mZ;                               // +0x28
    float mRHW;                             // +0x2c
    float mOpacity;                         // +0x30
};

#endif
