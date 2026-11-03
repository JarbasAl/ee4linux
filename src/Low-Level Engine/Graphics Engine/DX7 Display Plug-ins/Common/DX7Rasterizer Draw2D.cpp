// C:\EEX\Low-Level Engine\Graphics Engine\DX7 Display Plug-ins\Common\DX7Rasterizer Draw2D.cpp (file name
// inferred: the second of the four DX7Rasterizer units in link order; it holds the 2D draws)
// Unit flags (hypothesis, docs/CALIBRATION.md "dx7hr"): -O1 -G6 -MD -GR
//
// 0x10002b04..0x1000383a: the U2D key init chains, DX7Rasterizer::Draw for bitmaps, lines, points, rhombi and text
// (export order), the vector<GESurface*> iterator operator+ COMDAT, then the template-static init chain head and
// its atexit registrars.
#include <windows.h>
#include <ddraw.h>
#include <d3d.h>

#include "dx7/DX7Rasterizer.h"
#include "dx7/DX7PolygonArray.h"
#include "dx7/DX7StandardVertexArray.h"
#include "dx7/DX7Surface.h"
#include "dx7/DX7Screen.h"
#include "lle/GEBitmap.h"
#include "lle/GEFont.h"
#include "lle/GEFontBitmap.h"
#include "lle/GELine.h"
#include "lle/GEPoint.h"
#include "lle/GERhombus.h"
#include "lle/GEText.h"

// The U2D key init chains:
// SYNTHETIC: DX7HR 0x10002b04
// SYNTHETIC: DX7HR 0x10002b0e
// SYNTHETIC: DX7HR 0x10002b1f
// SYNTHETIC: DX7HR 0x10002b2b
// SYNTHETIC: DX7HR 0x10002b36
// SYNTHETIC: DX7HR 0x10002b40
// SYNTHETIC: DX7HR 0x10002b51
// SYNTHETIC: DX7HR 0x10002b5d
// SYNTHETIC: DX7HR 0x10002b68
// SYNTHETIC: DX7HR 0x10002b72
// SYNTHETIC: DX7HR 0x10002b83
// SYNTHETIC: DX7HR 0x10002b8f
// SYNTHETIC: DX7HR 0x10002b9a
// SYNTHETIC: DX7HR 0x10002ba4
// SYNTHETIC: DX7HR 0x10002bb5
// SYNTHETIC: DX7HR 0x10002bc1
// SYNTHETIC: DX7HR 0x10002bcc
// SYNTHETIC: DX7HR 0x10002bd6
// SYNTHETIC: DX7HR 0x10002be7
// SYNTHETIC: DX7HR 0x10002bf3
// SYNTHETIC: DX7HR 0x10002bfe
// SYNTHETIC: DX7HR 0x10002c08
// SYNTHETIC: DX7HR 0x10002c19
// SYNTHETIC: DX7HR 0x10002c25
// SYNTHETIC: DX7HR 0x10002c30
// SYNTHETIC: DX7HR 0x10002c3a
// SYNTHETIC: DX7HR 0x10002c4b
// SYNTHETIC: DX7HR 0x10002c57

// FUNCTION: DX7HR 0x10002c62
// Draws a bitmap as a textured quad, rewriting its vertex UVs in place into the texture's frame; 9 without texture
long DX7Rasterizer::Draw(GEBitmap* inBitmap, unsigned long inScreen)
{
    unsigned short theIndices[6] = {0, 1, 2, 2, 1, 3};
    long theResult;
    GETextureCoordinate theTopLeft;
    GETextureCoordinate theTopRight;
    GETextureCoordinate theBottomRight;
    GETextureCoordinate theBottomLeft;
    this->mDrawingAccess->Enter();
    this->Set2DMode(inScreen);
    GETexture* theTexture = inBitmap->GetTexture(inScreen);
    if (theTexture != NULL) {
        GEVertexArray* theVertices = inBitmap->GetVertexArray();
        float theUVSize = theTexture->GetFrameUVSize();
        if (theUVSize != 1.0f) {
            GETextureCoordinate theCorner = theTexture->GetFrameTopLeftCorner();
            theVertices->GetTextureCoordinate(theTopLeft, 0);
            theVertices->GetTextureCoordinate(theTopRight, 1);
            theVertices->GetTextureCoordinate(theBottomRight, 2);
            theVertices->GetTextureCoordinate(theBottomLeft, 3);
            theTopLeft.SetU(theTopLeft.GetU() * theUVSize + theCorner.GetU());
            theTopLeft.SetV(theTopLeft.GetV() * theUVSize + theCorner.GetV());
            theTopRight.SetU(theTopRight.GetU() * theUVSize + theCorner.GetU());
            theTopRight.SetV(theTopRight.GetV() * theUVSize + theCorner.GetV());
            theBottomRight.SetU(theBottomRight.GetU() * theUVSize + theCorner.GetU());
            theBottomRight.SetV(theBottomRight.GetV() * theUVSize + theCorner.GetV());
            theBottomLeft.SetU(theBottomLeft.GetU() * theUVSize + theCorner.GetU());
            theBottomLeft.SetV(theBottomLeft.GetV() * theUVSize + theCorner.GetV());
            theVertices->SetTextureCoordinate(theTopLeft, 0);
            theVertices->SetTextureCoordinate(theTopRight, 1);
            theVertices->SetTextureCoordinate(theBottomRight, 2);
            theVertices->SetTextureCoordinate(theBottomLeft, 3);
        }
        this->SetTexture(theTexture, inScreen);
        D3DVERTEX_STD_COLOR* theBuffer;
        this->LockVB(4, inScreen, theBuffer);
        memcpy(theBuffer + this->mDX7ScreenStates[inScreen]->mVertexBufferPosition,
               static_cast<DX7StdColorVertexArray*>(theVertices)->mVertices, 4 * sizeof(D3DVERTEX_STD_COLOR));
        theResult = this->UnlockVB(inScreen);
        while (true) {
            if (this->mDX7ScreenStates[inScreen]->mDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST,
                    this->mDX7ScreenStates[inScreen]->mVertexBuffer, this->mDX7ScreenStates[inScreen]->mVertexBufferPosition, 4,
                    theIndices, 6, 0) != DDERR_WASSTILLDRAWING)
                break;
            Sleep(0);
        }
        this->mDX7ScreenStates[inScreen]->mVertexBufferPosition += 4;
    } else
        theResult = 9;
    this->mDrawingAccess->Leave();
    return theResult;
}

// The half-pixel-ish nudge applied to line end points (a named constant: the original reloads it for every add).
// (provisional name)
static const float kLineOffset = 0.001f;

// FUNCTION: DX7HR 0x10002e88
// Draws lines as one line list, nudging both ends by 0.001; remaps only each first vertex's UV into the texture frame
long DX7Rasterizer::Draw(GELine* inLine, unsigned long inCount, unsigned long inScreen)
{
    GETextureCoordinate theCoordinate;
    unsigned long theVertexCount = inCount * 2;
    long theResult;
    this->mDrawingAccess->Enter();
    this->Set2DMode(inScreen);
    GETexture* theTexture = inLine->GetTexture(inScreen);
    bool theHasTexture = theTexture != NULL;
    this->SetTexture(theTexture, inScreen);
    D3DVERTEX_STD_COLOR* theBuffer;
    this->LockVB(theVertexCount, inScreen, theBuffer);
    theBuffer += this->mDX7ScreenStates[inScreen]->mVertexBufferPosition;
    GELine* theLine = inLine;
    for (unsigned long i = 0; i < inCount; i++, theLine++) {
        GEVertexArray* theVertices = theLine->GetVertexArray();
        if (theHasTexture) {
            float theUVSize = theTexture->GetFrameUVSize();
            if (theUVSize != 1.0f) {
                GETextureCoordinate theCorner = theTexture->GetFrameTopLeftCorner();
                theVertices->GetTextureCoordinate(theCoordinate, 0);
                theCoordinate.SetU(theCoordinate.GetU() * theUVSize + theCorner.GetU());
                theCoordinate.SetV(theCoordinate.GetV() * theUVSize + theCorner.GetV());
                theVertices->SetTextureCoordinate(theCoordinate, 0);
            }
        }
        memcpy(theBuffer, static_cast<DX7StdColorVertexArray*>(theVertices)->mVertices, 2 * sizeof(D3DVERTEX_STD_COLOR));
        theBuffer[0].x += kLineOffset;
        theBuffer[0].y += kLineOffset;
        theBuffer[1].x += kLineOffset;
        theBuffer[1].y += kLineOffset;
        theBuffer += 2;
    }
    theResult = this->UnlockVB(inScreen);
    while (true) {
        if (this->mDX7ScreenStates[inScreen]->mDevice->DrawPrimitiveVB(D3DPT_LINELIST,
                this->mDX7ScreenStates[inScreen]->mVertexBuffer, this->mDX7ScreenStates[inScreen]->mVertexBufferPosition,
                theVertexCount, 0) != DDERR_WASSTILLDRAWING)
            break;
        Sleep(0);
    }
    this->mDX7ScreenStates[inScreen]->mVertexBufferPosition += theVertexCount;
    this->mDrawingAccess->Leave();
    return theResult;
}

// FUNCTION: DX7HR 0x10003042
// Draws points as one point list, remapping each point's UV in place into the texture frame
long DX7Rasterizer::Draw(GEPoint* inPoint, unsigned long inCount, unsigned long inScreen)
{
    GETextureCoordinate theCoordinate;
    long theResult;
    this->mDrawingAccess->Enter();
    this->Set2DMode(inScreen);
    GETexture* theTexture = inPoint->GetTexture(inScreen);
    bool theHasTexture = theTexture != NULL;
    this->SetTexture(theTexture, inScreen);
    D3DVERTEX_STD_COLOR* theBuffer;
    this->LockVB(inCount, inScreen, theBuffer);
    theBuffer += this->mDX7ScreenStates[inScreen]->mVertexBufferPosition;
    GEPoint* thePoint = inPoint;
    for (unsigned long i = 0; i < inCount; i++, thePoint++) {
        GEVertexArray* theVertices = thePoint->GetVertexArray();
        if (theHasTexture) {
            float theUVSize = theTexture->GetFrameUVSize();
            if (theUVSize != 1.0f) {
                GETextureCoordinate theCorner = theTexture->GetFrameTopLeftCorner();
                theVertices->GetTextureCoordinate(theCoordinate, 0);
                theCoordinate.SetU(theCoordinate.GetU() * theUVSize + theCorner.GetU());
                theCoordinate.SetV(theCoordinate.GetV() * theUVSize + theCorner.GetV());
                theVertices->SetTextureCoordinate(theCoordinate, 0);
            }
        }
        memcpy(theBuffer, static_cast<DX7StdColorVertexArray*>(theVertices)->mVertices, sizeof(D3DVERTEX_STD_COLOR));
        theBuffer++;
    }
    theResult = this->UnlockVB(inScreen);
    while (true) {
        if (this->mDX7ScreenStates[inScreen]->mDevice->DrawPrimitiveVB(D3DPT_POINTLIST,
                   this->mDX7ScreenStates[inScreen]->mVertexBuffer, this->mDX7ScreenStates[inScreen]->mVertexBufferPosition,
                   inCount, 0) != DDERR_WASSTILLDRAWING)
            break;
        Sleep(0);
    }
    this->mDX7ScreenStates[inScreen]->mVertexBufferPosition += inCount;
    this->mDrawingAccess->Leave();
    return theResult;
}

// FUNCTION: DX7HR 0x100031b6
// Draws a rhombus as an untextured quad
long DX7Rasterizer::Draw(GERhombus* inRhombus, unsigned long inScreen)
{
    unsigned short theIndices[6] = {0, 1, 2, 2, 1, 3};
    long theResult;
    this->mDrawingAccess->Enter();
    this->Set2DMode(inScreen);
    this->SetTexture(NULL, inScreen);
    GEVertexArray* theVertices = inRhombus->GetVertexArray();
    D3DVERTEX_STD_COLOR* theBuffer;
    this->LockVB(4, inScreen, theBuffer);
    memcpy(theBuffer + this->mDX7ScreenStates[inScreen]->mVertexBufferPosition,
           static_cast<DX7StdColorVertexArray*>(theVertices)->mVertices, 4 * sizeof(D3DVERTEX_STD_COLOR));
    theResult = this->UnlockVB(inScreen);
    while (true) {
        if (this->mDX7ScreenStates[inScreen]->mDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST,
                   this->mDX7ScreenStates[inScreen]->mVertexBuffer, this->mDX7ScreenStates[inScreen]->mVertexBufferPosition, 4,
                   theIndices, 6, 0) != DDERR_WASSTILLDRAWING)
            break;
        Sleep(0);
    }
    this->mDX7ScreenStates[inScreen]->mVertexBufferPosition += 4;
    this->mDrawingAccess->Leave();
    return theResult;
}

// FUNCTION: DX7HR 0x100032a9
// Draws a text with GDI on the target's text DC, aligned; shadow fonts get a black or white shadow 1 pixel down-right
long DX7Rasterizer::Draw(GEText* inText, GESurface* inTarget)
{
    long theResult = kErrorOK;
    if (inTarget->GetIsTextLocked() || inTarget->TextLock() == kErrorOK) {
        HDC theDC = inTarget->GetTextDC();
        int theOldBkMode = SetBkMode(theDC, inText->GetIsTransparent() ? TRANSPARENT : OPAQUE);
        COLORREF theOldTextColor = SetTextColor(theDC, RGB((BYTE)(inText->GetFgColor().GetRed() * 255.0f),
                                                           (BYTE)(inText->GetFgColor().GetGreen() * 255.0f),
                                                           (BYTE)(inText->GetFgColor().GetBlue() * 255.0f)));
        COLORREF theOldBkColor = SetBkColor(theDC, RGB((BYTE)(inText->GetBgColor().GetRed() * 255.0f),
                                                       (BYTE)(inText->GetBgColor().GetGreen() * 255.0f),
                                                       (BYTE)(inText->GetBgColor().GetBlue() * 255.0f)));
        GEFont* theFont = inText->GetFont();
        theFont->Reference();
        HGDIOBJ theOldFont = SelectObject(theDC, theFont->GetOSFont());
        GE2DPoint theLocation = inText->GetLocation();
        SIZE theSize;
        if (GetTextExtentPoint32A(theDC, inText->GetText(), inText->GetText().GetLength(), &theSize)) {
            if (inText->GetAlignment() == kTextAlignCenter)
                theLocation.SetX(theLocation.GetX() - theSize.cx / 2);
            else if (inText->GetAlignment() == kTextAlignRight)
                theLocation.SetX(theLocation.GetX() - theSize.cx);
        }
        if (theFont->GetFontStyle() & kFontStyleShadow) {
            BYTE theShade = inText->GetFgColor().GetRed() + (inText->GetFgColor().GetGreen() +
                            inText->GetFgColor().GetBlue()) > 0.1f ? 0 : 255;
            COLORREF theColor = SetTextColor(theDC, RGB(theShade, theShade, theShade));
            if (!ExtTextOutA(theDC, (long)theLocation.GetX() + 1, (long)theLocation.GetY() + 1, 0, NULL,
                             inText->GetText(), inText->GetText().GetTCHARLength(), NULL)) {
                ESRecordNativeError(GetLastError());
                ESGetNativeError();
                theResult = kErrorNativeError;
            }
            SetTextColor(theDC, theColor);
        }
        if (!ExtTextOutA(theDC, (long)theLocation.GetX(), (long)theLocation.GetY(), 0, NULL, inText->GetText(),
                         inText->GetText().GetTCHARLength(), NULL)) {
            ESRecordNativeError(GetLastError());
            ESGetNativeError();
            theResult = kErrorNativeError;
        }
        SelectObject(theDC, theOldFont);
        SetTextColor(theDC, theOldTextColor);
        SetBkColor(theDC, theOldBkColor);
        SetBkMode(theDC, theOldBkMode);
        theFont->Dereference();
    }
    return theResult;
}

// FUNCTION: DX7HR 0x100034e8
// Draws a text clipped to a rectangle: with the font's bitmap font when it has one, else with GDI (aligned, shadowed)
long DX7Rasterizer::Draw(GEText* inText, const GERectangle& inRectangle, GESurface* inTarget)
{
    long theResult = kErrorOK;
    if (inText->GetFont()->GetFontBitmap() != NULL)
        theResult = inText->GetFont()->GetFontBitmap()->Draw(inText, inRectangle, this, 0);
    else {
        RECT theClip;
        theClip.top = UMFloor(inRectangle.GetTop());
        theClip.left = UMFloor(inRectangle.GetLeft());
        theClip.right = UMFloor(inRectangle.GetRight());
        theClip.bottom = UMFloor(inRectangle.GetBottom());
        if (inTarget->GetIsTextLocked() || inTarget->TextLock() == kErrorOK) {
            HDC theDC = inTarget->GetTextDC();
            int theOldBkMode = SetBkMode(theDC, inText->GetIsTransparent() ? TRANSPARENT : OPAQUE);
            COLORREF theOldTextColor = SetTextColor(theDC, RGB((BYTE)(inText->GetFgColor().GetRed() * 255.0f),
                                                               (BYTE)(inText->GetFgColor().GetGreen() * 255.0f),
                                                               (BYTE)(inText->GetFgColor().GetBlue() * 255.0f)));
            COLORREF theOldBkColor = SetBkColor(theDC, RGB((BYTE)(inText->GetBgColor().GetRed() * 255.0f),
                                                           (BYTE)(inText->GetBgColor().GetGreen() * 255.0f),
                                                           (BYTE)(inText->GetBgColor().GetBlue() * 255.0f)));
            GEFont* theFont = inText->GetFont();
            theFont->Reference();
            HGDIOBJ theOldFont = SelectObject(theDC, theFont->GetOSFont());
            GE2DPoint theLocation = inText->GetLocation();
            SIZE theSize;
            if (GetTextExtentPoint32A(theDC, inText->GetText(), inText->GetText().GetLength(), &theSize)) {
                if (inText->GetAlignment() == kTextAlignCenter)
                    theLocation.SetX(theLocation.GetX() - theSize.cx / 2);
                else if (inText->GetAlignment() == kTextAlignRight)
                    theLocation.SetX(theLocation.GetX() - theSize.cx);
            }
            if (theFont->GetFontStyle() & kFontStyleShadow) {
                BYTE theShade = inText->GetFgColor().GetRed() + inText->GetFgColor().GetGreen() +
                                inText->GetFgColor().GetBlue() > 0.1f ? 0 : 255;
                COLORREF theColor = SetTextColor(theDC, RGB(theShade, theShade, theShade));
                if (!ExtTextOutA(theDC, (long)theLocation.GetX() + 1, (long)theLocation.GetY() + 1, ETO_CLIPPED,
                                 &theClip, inText->GetText(), inText->GetText().GetTCHARLength(), NULL)) {
                    ESRecordNativeError(GetLastError());
                    ESGetNativeError();
                    theResult = kErrorNativeError;
                }
                SetTextColor(theDC, theColor);
            }
            if (!ExtTextOutA(theDC, (long)theLocation.GetX(), (long)theLocation.GetY(), ETO_CLIPPED, &theClip,
                             inText->GetText(), inText->GetText().GetTCHARLength(), NULL)) {
                ESRecordNativeError(GetLastError());
                ESGetNativeError();
                theResult = kErrorNativeError;
            }
            SelectObject(theDC, theOldFont);
            SetTextColor(theDC, theOldTextColor);
            SetBkColor(theDC, theOldBkColor);
            SetBkMode(theDC, theOldBkMode);
            theFont->Dereference();
        }
    }
    return theResult;
}

// COMDATs this unit is the first (link order) to instantiate, then the template-static init chain head and atexit
// registrars (GELOD x3, then U2DSparseArray):
// TEMPLATE: DX7HR 0x100037d0
// SYNTHETIC: DX7HR 0x100037e2
// SYNTHETIC: DX7HR 0x1000380a
// SYNTHETIC: DX7HR 0x10003816
// SYNTHETIC: DX7HR 0x10003822
// SYNTHETIC: DX7HR 0x1000382e

// DX7HRTnLDisplay.dll (the T&L twin builds this unit unchanged): the same functions at their twin addresses.
// SYNTHETIC: DX7HRTNL 0x10002b83
// SYNTHETIC: DX7HRTNL 0x10002b8d
// SYNTHETIC: DX7HRTNL 0x10002b9e
// SYNTHETIC: DX7HRTNL 0x10002baa
// SYNTHETIC: DX7HRTNL 0x10002bb5
// SYNTHETIC: DX7HRTNL 0x10002bbf
// SYNTHETIC: DX7HRTNL 0x10002bd0
// SYNTHETIC: DX7HRTNL 0x10002bdc
// SYNTHETIC: DX7HRTNL 0x10002be7
// SYNTHETIC: DX7HRTNL 0x10002bf1
// SYNTHETIC: DX7HRTNL 0x10002c02
// SYNTHETIC: DX7HRTNL 0x10002c0e
// SYNTHETIC: DX7HRTNL 0x10002c19
// SYNTHETIC: DX7HRTNL 0x10002c23
// SYNTHETIC: DX7HRTNL 0x10002c34
// SYNTHETIC: DX7HRTNL 0x10002c40
// SYNTHETIC: DX7HRTNL 0x10002c4b
// SYNTHETIC: DX7HRTNL 0x10002c55
// SYNTHETIC: DX7HRTNL 0x10002c66
// SYNTHETIC: DX7HRTNL 0x10002c72
// SYNTHETIC: DX7HRTNL 0x10002c7d
// SYNTHETIC: DX7HRTNL 0x10002c87
// SYNTHETIC: DX7HRTNL 0x10002c98
// SYNTHETIC: DX7HRTNL 0x10002ca4
// SYNTHETIC: DX7HRTNL 0x10002caf
// SYNTHETIC: DX7HRTNL 0x10002cb9
// SYNTHETIC: DX7HRTNL 0x10002cca
// SYNTHETIC: DX7HRTNL 0x10002cd6
// FUNCTION: DX7HRTNL 0x10002ce1
// FUNCTION: DX7HRTNL 0x10002f07
// FUNCTION: DX7HRTNL 0x100030c1
// FUNCTION: DX7HRTNL 0x10003235
// FUNCTION: DX7HRTNL 0x10003328
// FUNCTION: DX7HRTNL 0x10003567
// SYNTHETIC: DX7HRTNL 0x1000384f
// SYNTHETIC: DX7HRTNL 0x10003877
// SYNTHETIC: DX7HRTNL 0x10003883
// SYNTHETIC: DX7HRTNL 0x1000388f
// SYNTHETIC: DX7HRTNL 0x1000389b
