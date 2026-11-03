// C:\EEX\Low-Level Engine\User Interface\UILRTiledBackground.h (RECONSTRUCTION). A tiled background repeated left to
// right only (one row). UILRTiledBackground.cpp (0x10078a70..0x10078c81). Vtable 0x10099a34. sizeof 0x7c. Requires
// lle/UITiledBackground.h.
#ifndef LLE_UILRTILEDBACKGROUND_H
#define LLE_UILRTILEDBACKGROUND_H

#include "lle/UITiledBackground.h"

class LLE_API UILRTiledBackground : public UITiledBackground {
public:
    UILRTiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                        const GETextureDescription& inDescription, const GERectangle& inTextureRectangle,
                        const UILayoutUtility& inLayout, UIForm* inForm, unsigned long inDrawPriority, bool inVisible);
    virtual ~UILRTiledBackground();
    virtual long DrawBackground(const GERectangle& inRectangle);
};

#endif
