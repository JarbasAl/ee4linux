// C:\EEX\Low-Level Engine\Graphics Engine\Graphics Engine.h (RECONSTRUCTION; Graphics Engine.cpp 0x38527..0x38abf).
// Graphics Engine start-up (loads the rasterizer plug-ins) and pixel-format packing.
#ifndef LLE_GRAPHICSENGINE_H
#define LLE_GRAPHICSENGINE_H

class FSFileSpec;
class GEColor;

LLE_API long GEInitialize(FSFileSpec& inDirectory, HINSTANCE__* const inInstance);
LLE_API unsigned long GEPackColor(unsigned long inFormat, const GEColor& inColor, float inAlpha);
LLE_API unsigned long GEPackColor(unsigned long inFormat, unsigned long inRed, unsigned long inGreen,
                                  unsigned long inBlue, unsigned long inAlpha);
LLE_API unsigned char GEUnpackRed(unsigned long inFormat, unsigned long inColor);
LLE_API unsigned char GEUnpackGreen(unsigned long inFormat, unsigned long inColor);
LLE_API unsigned char GEUnpackBlue(unsigned long inFormat, unsigned long inColor);
LLE_API unsigned char GEUnpackAlpha(unsigned long inFormat, unsigned long inColor);

#endif
