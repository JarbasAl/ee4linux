// C:\EEX\Low-Level Engine\User Interface\UIBitmapBackground.h (RECONSTRUCTION). A background drawn as one
// stretched bitmap. UIBitmapBackground.cpp (0x1005d72a..0x1005db0f) emits the inline members first. Vtable
// 0x10098c88 (UIBackground's 7 slots, then DrawStrectchedBackground, spelling as exported). sizeof 0x7c (??_E
// stride). Requires lle/UITextureBackground.h.
#ifndef LLE_UIBITMAPBACKGROUND_H
#define LLE_UIBITMAPBACKGROUND_H

#include "lle/UITextureBackground.h"

class GEBitmap;
class GETexture;

class LLE_API UIBitmapBackground : public UITextureBackground {
public:
    UIBitmapBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                       const GETextureDescription& inDescription, const GERectangle& inTextureRectangle,
                       const UILayoutUtility& inLayout, UIForm* inForm, unsigned long inDrawPriority, bool inVisible);
    virtual ~UIBitmapBackground();
    virtual long DrawBackground(const GERectangle& inRectangle);
    virtual long DrawStrectchedBackground(const GERectangle& inSource, const GERectangle& inDestination);  // 0x1c
    GEBitmap* GetBitmap() { return this->mBitmap; }
    virtual long LoadBackground();
    void SetBitmapTexture(GETexture* inTexture);
    virtual long UnloadBackground();
protected:
    GEBitmap* mBitmap;                      // +0x78 (created by the ctor)
};

#endif
