// C:\EEX\Low-Level Engine\Graphics Engine\GERectangle.cpp
// Unit flags (calibrated, docs/CALIBRATION.md "lle"): -O1 -G6 -MD -GR -DLOWLEVELENGINE_EXPORTS
//
// 0x100246a7..0x10024ea0: the four static UString IDs (one shared guard byte, bits 1/2/4/8), And x2, Load,
// RemoveIntersectingRectangle, Save, then the vector<GERectangle> template COMDATs. The rest of GERectangle is
// header-inline (GEBitmap.cpp emits it first). No U2D keys: the unit includes no U2D header.
#include <windows.h>
#include <vector>

#include "lle/LowLevelEngineCore.h"
#include "lle/UString.h"
#include "lle/GE2DPoint.h"
#include "lle/FSFile.h"
#include "lle/GERectangle.h"

const UString GERectangle::sLeftID("GERectangle-Left");
const UString GERectangle::sTopID("GERectangle-Top");
const UString GERectangle::sRightID("GERectangle-Right");
const UString GERectangle::sBottomID("GERectangle-Bottom");

// FUNCTION: LLE 0x100247ab
// Clips the rectangle to another one; returns false and leaves it unchanged when they do not overlap
bool GERectangle::And(const GERectangle& inRectangle)
{
    bool theResult = !(this->mLeft > inRectangle.mRight || this->mRight < inRectangle.mLeft || this->mTop > inRectangle.mBottom ||
                       this->mBottom < inRectangle.mTop);
    if (theResult) {
        if (this->mLeft < inRectangle.mLeft)
            this->mLeft = inRectangle.mLeft;
        if (this->mRight > inRectangle.mRight)
            this->mRight = inRectangle.mRight;
        if (this->mTop < inRectangle.mTop)
            this->mTop = inRectangle.mTop;
        if (this->mBottom > inRectangle.mBottom)
            this->mBottom = inRectangle.mBottom;
    }
    return theResult;
}

// FUNCTION: LLE 0x10024826
// Writes the overlap of two rectangles to outRectangle; returns false (output untouched) when they do not overlap
bool GERectangle::And(const GERectangle& inRectangle, GERectangle& outRectangle) const
{
    bool theResult = !(this->mLeft > inRectangle.mRight || this->mRight < inRectangle.mLeft || this->mTop > inRectangle.mBottom ||
                       this->mBottom < inRectangle.mTop);
    if (theResult) {
        if (this->mLeft < inRectangle.mLeft)
            outRectangle.mLeft = inRectangle.mLeft;
        else
            outRectangle.mLeft = this->mLeft;
        if (this->mRight > inRectangle.mRight)
            outRectangle.mRight = inRectangle.mRight;
        else
            outRectangle.mRight = this->mRight;
        if (this->mTop < inRectangle.mTop)
            outRectangle.mTop = inRectangle.mTop;
        else
            outRectangle.mTop = this->mTop;
        if (this->mBottom > inRectangle.mBottom)
            outRectangle.mBottom = inRectangle.mBottom;
        else
            outRectangle.mBottom = this->mBottom;
    }
    return theResult;
}

// FUNCTION: LLE 0x100248ba
// Reads the left, top, right and bottom edges from a file; returns the first read error (or not-open)
long GERectangle::Load(FSFile& inFile)
{
    long theResult;
    if (inFile.GetIsOpen()) {
        theResult = inFile.Read(this->mLeft);
        if (theResult == kErrorOK) {
            theResult = inFile.Read(this->mTop);
            if (theResult == kErrorOK) {
                theResult = inFile.Read(this->mRight);
                if (theResult == kErrorOK)
                    theResult = inFile.Read(this->mBottom);
            }
        }
    } else
        theResult = kErrorNotOpen;
    return theResult;
}

// FUNCTION: LLE 0x1002490c
// Splits the rectangle around another one into left/right/top/bottom pieces; the whole rectangle when no edge cuts
void GERectangle::RemoveIntersectingRectangle(const GERectangle& inRectangle,
                                              std::vector<GERectangle>& outRectangles) const
{
    bool theLeftSplit = false;
    bool theRightSplit = false;
    if (inRectangle.mLeft >= this->mLeft && inRectangle.mLeft <= this->mRight) {
        outRectangles.push_back(GERectangle(this->mLeft, this->mTop, inRectangle.mLeft, this->mBottom));
        theLeftSplit = true;
    }
    if (inRectangle.mRight >= this->mLeft && inRectangle.mRight <= this->mRight) {
        outRectangles.push_back(GERectangle(inRectangle.mRight, this->mTop, this->mRight, this->mBottom));
        theRightSplit = true;
    }
    if (inRectangle.mTop >= this->mTop && inRectangle.mTop <= this->mBottom)
        outRectangles.push_back(GERectangle(theLeftSplit == true ? inRectangle.mLeft : this->mLeft, this->mTop,
                                            theRightSplit == true ? inRectangle.mRight : this->mRight, inRectangle.mTop));
    if (inRectangle.mBottom >= this->mTop && inRectangle.mBottom <= this->mBottom)
        outRectangles.push_back(GERectangle(theLeftSplit == true ? inRectangle.mLeft : this->mLeft, inRectangle.mBottom,
                                            theRightSplit == true ? inRectangle.mRight : this->mRight, this->mBottom));
    if (outRectangles.size() == 0 && (this->mLeft < inRectangle.mLeft || this->mRight > inRectangle.mRight ||
                                      this->mTop < inRectangle.mTop || this->mBottom > inRectangle.mBottom))
        outRectangles.push_back(*this);
}

// FUNCTION: LLE 0x10024a82
// Writes the left, top, right and bottom edges to a file; returns the first write error (or not-open)
long GERectangle::Save(FSFile& inFile) const
{
    long theResult;
    if (inFile.GetIsOpen()) {
        theResult = inFile.Write(this->mLeft);
        if (theResult == kErrorOK) {
            theResult = inFile.Write(this->mTop);
            if (theResult == kErrorOK) {
                theResult = inFile.Write(this->mRight);
                if (theResult == kErrorOK)
                    theResult = inFile.Write(this->mBottom);
            }
        }
    } else
        theResult = kErrorNotOpen;
    return theResult;
}

// Compiler-generated for this unit (names in analysis/lle/names.txt):
// SYNTHETIC: LLE 0x10024790
// TEMPLATE: LLE 0x10024e0b
// TEMPLATE: LLE 0x10024e3c
// TEMPLATE: LLE 0x10024e6a

// Compiler-generated, continued:
// SYNTHETIC: LLE 0x100246b1
// SYNTHETIC: LLE 0x100246cd
// SYNTHETIC: LLE 0x100246f2
// SYNTHETIC: LLE 0x1002470e
// SYNTHETIC: LLE 0x10024733
// SYNTHETIC: LLE 0x1002474f
// SYNTHETIC: LLE 0x10024774
// SYNTHETIC: LLE 0x10024784

// Compiler-generated, continued:
// SYNTHETIC: LLE 0x100246a7
// SYNTHETIC: LLE 0x100246c1
// SYNTHETIC: LLE 0x100246e8
// SYNTHETIC: LLE 0x10024702
// SYNTHETIC: LLE 0x10024729
// SYNTHETIC: LLE 0x10024743
// SYNTHETIC: LLE 0x1002476a

// Compiler-generated, continued:
// TEMPLATE: LLE 0x10024ad4
// TEMPLATE: LLE 0x10024b00
// TEMPLATE: LLE 0x10024b50
// TEMPLATE: LLE 0x10024ddf
