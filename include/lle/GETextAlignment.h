// lle/GETextAlignment.h (RECONSTRUCTION). Named integer constants; each comment gives the evidence for its name;
// (provisional name) marks an inferred one.
#ifndef EE_GETEXTALIGNMENT_H
#define EE_GETEXTALIGNMENT_H

// GEText / UITextLabel horizontal text alignment
enum GETextAlignment {
    kTextAlignLeft = 0,  // UITextLabel::DrawTextA: x = left edge (provisional name)
    kTextAlignCenter = 1,  // UITextLabel: centred in the rectangle; DX7Rasterizer::Draw: x -= width / 2 (provisional name)
    kTextAlignRight = 2,  // UITextLabel: x = right - width; DX7Rasterizer::Draw: x -= width (provisional name)
};

#endif // EE_GETEXTALIGNMENT_H
