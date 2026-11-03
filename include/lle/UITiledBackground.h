// C:\EEX\Low-Level Engine\User Interface\UITiledBackground.h (RECONSTRUCTION). A bitmap background repeated over the
// rectangle. UITiledBackground.cpp (0x1007fb46..0x1007fd7d) holds the ctor, dtor and the two draw members; the
// implicit copy members are emitted by UILRTiledBackground.cpp, the first includer. Vtable 0x10099a0c
// (UIBitmapBackground's 8 slots, then DrawTiledBackground). No data of its own (sizeof 0x7c). Requires
// lle/UIBitmapBackground.h.
#ifndef LLE_UITILEDBACKGROUND_H
#define LLE_UITILEDBACKGROUND_H

#include "lle/UIBitmapBackground.h"

class LLE_API UITiledBackground : public UIBitmapBackground {
public:
    UITiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                      const GETextureDescription& inDescription, const GERectangle& inTextureRectangle,
                      const UILayoutUtility& inLayout, UIForm* inForm, unsigned long inDrawPriority, bool inVisible);
    virtual ~UITiledBackground();
    virtual long DrawBackground(const GERectangle& inRectangle);
    // slot 0x20: draws the tiles [inFirstRow, inLastRow] x [inFirstColumn, inLastColumn] clipped to inClip
    virtual long DrawTiledBackground(const GERectangle& inClip, unsigned long inFirstRow, unsigned long inLastRow,
                                     unsigned long inFirstColumn, unsigned long inLastColumn, float inWidth,
                                     float inHeight);
};

#endif
