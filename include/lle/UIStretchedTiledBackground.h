// C:\EEX\Low-Level Engine\User Interface\UIStretchedTiledBackground.h (RECONSTRUCTION). A tiled background whose
// tiles are the rectangle divided into a fixed number of steps per axis. UIStretchedTiledBackground.cpp
// (0x1007e312..0x1007e47c); the implicit copy members are emitted by UIManager.cpp. Vtable 0x10099a5c. sizeof 0x84.
// Requires lle/UITiledBackground.h.
#ifndef LLE_UISTRETCHEDTILEDBACKGROUND_H
#define LLE_UISTRETCHEDTILEDBACKGROUND_H

#include "lle/UITiledBackground.h"

class LLE_API UIStretchedTiledBackground : public UITiledBackground {
public:
    UIStretchedTiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                               const GETextureDescription& inDescription, const GERectangle& inTextureRectangle,
                               float inHorizontalCount, float inVerticalCount, const UILayoutUtility& inLayout,
                               UIForm* inForm, unsigned long inDrawPriority, bool inVisible);
    virtual ~UIStretchedTiledBackground();
    virtual long DrawBackground(const GERectangle& inRectangle);
protected:
    float mHorizontalCount;                 // +0x7c (provisional names)
    float mVerticalCount;                   // +0x80
};

#endif
