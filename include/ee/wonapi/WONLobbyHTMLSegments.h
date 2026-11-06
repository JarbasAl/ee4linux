#ifndef EE_WONAPI_WONLOBBYHTMLSEGMENTS_H
#define EE_WONAPI_WONLOBBYHTMLSEGMENTS_H
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
enum LineSegmentFlags {
    LineSegmentFlag_FloatLeft=1,LineSegmentFlag_FloatRight=2,
    LineSegmentFlag_PreLineBreak=4,LineSegmentFlag_PostLineBreak=8,
    LineSegmentFlag_BreakClearLeft=16,LineSegmentFlag_BreakClearRight=32
};
// Actual complete18 owners. Native allocations construct RefCount4=0 and
// these scalar members only; inherited LineSegment8/0c/10 stay uninitialized.
// Actual implicit cleanup311480->338c70 only destroys the non-owning base.
class LineBreakSegment : public LineSegment {
public:
    int mHeight; //14
    LineBreakSegment(int height):mHeight(height) {}
    virtual int GetHeight() { return mHeight; } //actual3385b0 four bytes
    virtual void GetSelText(GUIString&,ElementSelectionInfo&); //actual309a40
};
class AlignLineSegment : public LineSegment {
public:
    HorzAlign mAlign; //14
    AlignLineSegment(HorzAlign align):mAlign(align) {}
    virtual bool PerformInstructions(MultiLineBuilder*); //actual3099c0 RET4
};
// Actual complete1c owner, same natural non-owning base cleanup.
class MarginLineSegment : public LineSegment {
public:
    int mLeftMargin,mNextLeftMargin; //14,18 actual SDK member names
    MarginLineSegment(int left,int next):mLeftMargin(left),mNextLeftMargin(next) {}
    virtual bool PerformInstructions(MultiLineBuilder*); //actual3099d0 RET4
};
}
#endif
