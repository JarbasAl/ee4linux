#ifndef EE_WONAPI_WONLOBBYTEXTWRAP_H
#define EE_WONAPI_WONLOBBYTEXTWRAP_H
#include "ee/wonapi/WONLobbyTextSegment.h"
namespace WONAPI {
class LineElement;
class LineBuilder {
public:
    int mMaxAscent,mMaxDescent,mTopAlignHeight,mWidth; //00..0f
    SmartPtr<LineElement> mLine; //10, original-owned complete14 value
    int GetWidth() { return mWidth; }
    void AddSegment(LineSegment*); //30b5d0 genuine SDK line metrics builder
    void StartBuild(LineElement*); //30b560 actual original provider
    void FinishBuild(); //30b790 actual original provider
    int GetHeight() { int h=mMaxAscent+mMaxDescent; return h>=mTopAlignHeight?h:mTopAlignHeight; }
    inline void Clear(); // defined in WONLobbyLineLayout.h after LineElement (MultiLineBuilder 0x70c000 inlines it)
    inline bool IsEmpty(); // defined in WONLobbyLineLayout.h after LineElement (HTMLDisplayGen 0x7384a0 inlines it)
};
// Actual complete80 target builder. Original ctor30bd60/dtor30d3e0 retained;
// target omits later SDK mCurSelIndex, confirmed all list offsets50/5c/68/74.
class MultiLineBuilder {
public:
    typedef std::vector<SmartPtr<LineElement> > LineVector;
    typedef std::list<SegmentInfo> SegmentList;
    typedef std::vector<FloatingSegment> FloatingVector;
    typedef std::list<LineSegment*> LineSegmentList;
    typedef std::list<FloatingSegment> FloatingList;
    LineVector* mLineVector; //00 borrowed
    SegmentList* mSegmentList; //04 borrowed
    int mWidth,mHeight,mSpecifiedWidth; //08,0c,10
    int mLeftMargin,mRightMargin; //14,18
    int mMaxLeftHeight,mMaxRightHeight; //1c,20
    int mLeftIndent,mRightIndent; //24,28
    HorzAlign mAlignment; //2c
    LineBuilder mCurLine; //30..43, retained original-owned current line40
    FloatingVector* mLeftItems; //44 borrowed
    FloatingVector* mRightItems; //48 borrowed
    bool mHaveDeferredItems; //4c naturalpadding4d..4f
    LineSegmentList mDeferredLeftItems,mDeferredRightItems; //50..5b,5c..67 borrowed payload pointers
    FloatingList mCurLeftItems,mCurRightItems; //68..73,74..7f retained floating records
    MultiLineBuilder(); //30bd60 actual original generated constructor
    int GetCurMaxLineWidth() { return mRightMargin-mLeftMargin; }
    LineBuilder* CurLine() { return &mCurLine; }
    void SetLineAlignment(HorzAlign align) { mAlignment=align; } //genuine SDK inline, native callback3099c0 writes2c
    void SetIndentSize(int,int); //30be50 support0
    void AlignCurLine(); //30be70
    bool FlushDeferredItems(); //30bf20
    LineElement* AddLine(bool=false,bool=false); //30c000
    void AddSegmentToCurLine(LineSegment*); //30c150
    void CalcMargins(); //30c1b0
    void AddFloatingSegment(LineSegment*,bool); //30c310
    void AddSegments(SegmentList*,LineVector*,FloatingVector*,FloatingVector*,int); //30c560 actual five arguments/ret14
};
// Complete target24 (native30a67d/30a737 new24 +ctor inline writes14..20).
// Base LineSegment is actual14 with RefCount8 +ints08/0c/10; all32 slots proved.
// Constructor is genuine SDK initialization; borrowed mSegment has NO retain.
// Implicit destruction contains no owned fields and is support0.
class TextPieceLineSegment : public LineSegment {
public:
    TextLineSegment* mSegment; //14 borrowed
    int mStartChar,mEndChar,mWidth; //18,1c,20
    TextPieceLineSegment(TextLineSegment* segment,int start,int end,int width) :
        mSegment(segment),mStartChar(start),mEndChar(end),mWidth(width) { mStartSelIndex=segment->mStartSelIndex; }
    virtual void MouseEnter(int x,int y) { mSegment->MouseEnter(x,y); }
    virtual void MouseExit() { mSegment->MouseExit(); }
    virtual void MouseDown(int x,int y,MouseButton b) { mSegment->MouseDown(x,y,b); }
    virtual void MouseUp(int x,int y,MouseButton b) { mSegment->MouseUp(x,y,b); }
    virtual void MouseMove(int x,int y) { mSegment->MouseMove(x,y); }
    virtual void MouseDrag(int x,int y) { mSegment->MouseDrag(x,y); }
    virtual bool IsInvalid() { return mSegment->IsInvalid(); }
    virtual int GetSelChar(int,int); //30b330 support0 forwarding
    virtual bool GetSelCharPos(int&,int&,int,int); //30a2c0
    virtual bool CanStartSelection() { return mSegment->CanStartSelection(); }
    virtual int GetWidth() { return mWidth; }
    virtual int GetHeight() { return mSegment->GetHeight(); }
    virtual int GetAscent() { return mSegment->GetAscent(); }
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //30b350 support0 forwarding
};
}
#endif
