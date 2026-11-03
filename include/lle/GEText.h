// C:\EEX\Low-Level Engine\Graphics Engine\GEText.h (RECONSTRUCTION; added for the User Interface units).
// A string drawn with a GEFont. The exported members sit in GEBoundingBox.cpp's inline block (0x10013318..
// 0x10013501) plus ComputeTextExtent (0x10032ae2). sizeof 0x50 (UIButton: operator new(0x50)). Only members the UI
// units call are declared; those the UI code calls out of line are declared without a body here (inferred:
// the real header probably has most of them inline).
#ifndef LLE_GETEXT_H
#define LLE_GETEXT_H

#include "lle/GEFont.h"   // the inline dtor dereferences mFont
#include "lle/GETextAlignment.h"

class LLE_API GEText {
public:
    // inline members in declaration order (GEBoundingBox.cpp 0x10013318..); the copy ctor is implicit (0x100134a9)
    GEText(GEFont* inFont)
        : mTextExtent(0.0f, 0.0f, 0.0f, 0.0f), mText(0x100)
    {
        this->mFont = inFont;
        this->mFont->Reference();
        this->mIsTransparent = false;
        this->mAlignment = kTextAlignLeft;
    }
    ~GEText() { this->mFont->Dereference(); }  // inline: UIGetStringCutOffIndex, ~GERasterizer, UIComboBox expand it
    void operator=(const GEText& inText)
    {
        this->mFont->Dereference();
        this->mFont = inText.mFont;
        this->mFont->Reference();
        this->mIsTransparent = inText.mIsTransparent;
        this->mAlignment = inText.mAlignment;
        this->mText = inText.mText;
        this->mTextExtent = inText.mTextExtent;
        this->mFgColor = inText.mFgColor;
        this->mBgColor = inText.mBgColor;
        this->mLocation = inText.mLocation;
    }
    const GEColor& GetBgColor() const { return this->mBgColor; }
    const GEColor& GetFgColor() const { return this->mFgColor; }
    GEFont* GetFont() const { return this->mFont; }
    bool GetIsTransparent() const { return this->mIsTransparent; }
    const GE2DPoint& GetLocation() const { return this->mLocation; }
    const UWideString& GetText() const { return this->mText; }
    const GERectangle& GetTextExtent() const { return this->mTextExtent; }
    unsigned long GetAlignment() const { return this->mAlignment; }
    void SetAlignment(unsigned long inAlignment) { this->mAlignment = inAlignment; }
    void SetBgColor(const GEColor& inColor) { this->mBgColor = inColor; }
    void SetFgColor(const GEColor& inColor) { this->mFgColor = inColor; }
    void SetFont(GEFont* inFont)
    {
        this->mFont->Dereference();
        this->mFont = inFont;
        this->mFont->Reference();
        this->ComputeTextExtent();
    }
    void SetIsTransparent(bool inTransparent) { this->mIsTransparent = inTransparent; }
    void SetLocation(const GE2DPoint& inLocation) { this->mLocation = inLocation; }
    void SetLocation(float inX, float inY) { this->mLocation.SetXY(inX, inY); }
    // inline: UIButton::SetText 0x1005edf1 expands it (UWideString::operator= then the private ComputeTextExtent)
    void SetText(const UWideString& inText)
    {
        this->mText = inText;
        this->ComputeTextExtent();
    }
private:
    void ComputeTextExtent();

    GE2DPoint mLocation;                    // +0x00
    GEColor mBgColor;                       // +0x08
    GEColor mFgColor;                       // +0x14
    GERectangle mTextExtent;                // +0x20
    UWideString mText;                      // +0x30
    GEFont* mFont;                          // +0x44 (referenced)
    unsigned long mAlignment;               // +0x48
    bool mIsTransparent;                    // +0x4c
};

#endif
