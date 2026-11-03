// C:\EEX\Low-Level Engine\User Interface\UITextureBackground.h (RECONSTRUCTION). A background drawn from a texture
// file. Every member is inline: UIBFull.cpp (the first includer) emits them at 0x1005c84e..0x1005c9f9; there is no
// code in UITextureBackground.cpp's own range in this stretch. Vtable 0x10098bb4. sizeof 0x78 (??_E stride).
// Requires lle/UIBackground.h and the GETextureDescription definition.
#ifndef LLE_UITEXTUREBACKGROUND_H
#define LLE_UITEXTUREBACKGROUND_H

#include "lle/UIBackground.h"

class LLE_API UITextureBackground : public UIBackground {
public:
    UITextureBackground(unsigned long inBackgroundID, const UString& inTextureFileName,
                        const GETextureDescription& inDescription, const GERectangle& inTextureRectangle,
                        const UILayoutUtility& inLayout, UIForm* inForm, unsigned long inDrawPriority, bool inVisible)
        : UIBackground(inBackgroundID, inLayout, inForm, inDrawPriority, inVisible),
          mTextureRectangle(inTextureRectangle), mTextureFileName(inTextureFileName), mTextureDescription(inDescription)
    {
    }
    virtual ~UITextureBackground() {}
    const GETextureDescription& GetTextureDescription() const { return this->mTextureDescription; }
    const UString& GetTextureFileName() const { return this->mTextureFileName; }
    const GERectangle& GetTextureRectangle() const { return this->mTextureRectangle; }
    void SetTextureDescription(GETextureDescription& inDescription) { this->mTextureDescription = inDescription; }
    void SetTextureRectangle(GERectangle& inRectangle) { this->mTextureRectangle = inRectangle; }
protected:
    GERectangle mTextureRectangle;          // +0x44
    UString mTextureFileName;               // +0x54
    GETextureDescription mTextureDescription;   // +0x64
};

#endif
