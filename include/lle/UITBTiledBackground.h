// C:\EEX\Low-Level Engine\User Interface\UITBTiledBackground.h (RECONSTRUCTION). A tiled background repeated top to
// bottom only (one column). UITBTiledBackground.cpp (0x1007e47c..0x1007e579); the implicit copy members are emitted
// by UIManager.cpp. Vtable 0x10099a84. sizeof 0x7c. Requires lle/UITiledBackground.h.
#ifndef LLE_UITBTILEDBACKGROUND_H
#define LLE_UITBTILEDBACKGROUND_H

#include "lle/UITiledBackground.h"

class LLE_API UITBTiledBackground : public UITiledBackground {
public:
    UITBTiledBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                        const GETextureDescription& inDescription, const GERectangle& inTextureRectangle,
                        const UILayoutUtility& inLayout, UIForm* inForm, unsigned long inDrawPriority, bool inVisible);
    virtual ~UITBTiledBackground();
    virtual long DrawBackground(const GERectangle& inRectangle);
};

#endif
