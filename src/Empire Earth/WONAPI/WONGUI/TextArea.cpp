// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
using namespace WONAPI;

// FUNCTION: WON 0x00711df0
void TextArea::Reset(bool justDisplay) {
    mLineVector.clear();
    mAnchorMap.clear();
    if(!justDisplay) {
        mSegmentList.clear();
        mSelecting=false;
        mSelStartChar=mSelEndChar=0;
        mBackwardsSel=false;
        mVertOffset=mHorzOffset=0;
        mL0A4=mTotalHeight=0;
        VirtualCC(false);
    }
    mNumLines=0;
    mTopLineVectorPos=0;
    mTotalHeight=mL0A4=0;
    mCurLink=NULL;
    mLastFindSeg=NULL;
    mNeedRecalcWordWrap=false;
    mDelayWordWrapTimeAccum=0;
}

// FUNCTION: WON 0x00711fc0
void TextArea::RecalcWordWrap(bool wasDelayed) {
    Reset(true);
    SegmentList::iterator itr=mSegmentList.begin();
    while(itr!=mSegmentList.end()) {
        AddSegmentPrv(itr->get());
        ++itr;
    }
    if(ShouldScrollOnBottom()) mVertOffset=mTotalHeight;
    if(wasDelayed) VirtualCC(false);
}

// FUNCTION: WON 0x00712030
void TextArea::Virtual08() {
    int oldWidth=mScrollArea.width;
    bool bottom=ShouldScrollOnBottom();
    ScrollArea::Virtual08();
    if(TextFlagSet(TextFlag_WordWrap) && !mSegmentList.empty() && mScrollArea.width!=oldWidth && mHasBeenVisible) {
        if(TextFlagSet(TextFlag_DelayWordWrap) && !mSegmentList.empty() && mSegmentList.back()->mAbsEndChar>5000) {
            mDelayWordWrapTimeAccum=0;
            mNeedRecalcWordWrap=true;
            Fn2F57D0(true);
        } else RecalcWordWrap(false);
    }
    if(bottom) mVertOffset=mTotalHeight+mExtraHeight-mScrollArea.height;
    VirtualCC(false);
}

// FUNCTION: WON 0x007120f0
void TextArea::VirtualCC(bool becauseOfScrollbar) {
    mExtraHeight=0;
    if(TextFlagSet(TextFlag_ScrollFullLine) && mTotalHeight>mScrollArea.height) {
        LineVector::iterator itr=FindLineAt(mTotalHeight-mScrollArea.height);
        if(itr!=mLineVector.end()) {
            TextLine* line=(*itr).get();
            mExtraHeight=line->my+line->mHeight+mScrollArea.height-mTotalHeight;
            if(mExtraHeight<0) mExtraHeight=0;
        }
    }
    ScrollArea::VirtualCC(becauseOfScrollbar);
    FindTopLine();
    if(TextFlagSet(TextFlag_ScrollFullLine)) {
        if(mTopLineVectorPos<mLineVector.size()) {
            TextLine* line=mLineVector[mTopLineVectorPos].get();
            mVertOffset=line->my;
            mDownScrollAmount=line->mHeight;
            if(mTopLineVectorPos>0) {
                line=mLineVector[mTopLineVectorPos-1].get();
                mUpScrollAmount=line->mHeight;
            }
            ScrollArea::VirtualCC(true);
        }
    }
}

// FUNCTION: WON 0x007128c0
bool TextArea::ShouldScrollOnBottom() {
    if(!TextFlagSet(TextFlag_ScrollOnBottom)) return false;
    if(!mUserHasScrolled) return true;
    if(!mLineVector.empty()) {
        TextLine* line=mLineVector.back().get();
        if(line->mHeight==0 && mLineVector.size()>1) line=mLineVector[mLineVector.size()-2].get();
        if(IsLineVisible(line)) return true;
    }
    return false;
}

// FUNCTION: WON 0x00713030
bool TextArea::IsLineVisible(TextLine* line) {
    if(line->my+line->mHeight<mVertOffset) return false;
    int y=line->my;
    if(!TextFlagSet(TextFlag_ShowPartialLines)) y+=line->mHeight;
    return y<=mVertOffset+mScrollArea.height;
}

// FUNCTION: WON 0x00713080
void TextArea::Virtual84(bool visible) {
    ScrollArea::Virtual84(visible);
    if(visible && !mHasBeenVisible) {
        mHasBeenVisible=true;
        if(TextFlagSet(TextFlag_WordWrap) && !mSegmentList.empty()) RecalcWordWrap(true);
    }
}

// FUNCTION: WON 0x007130d0
void TextArea::VirtualD4(Graphics& g) {
    LineVector::iterator lineItr=mLineVector.begin()+mTopLineVectorPos;
    while(lineItr!=mLineVector.end()) {
        TextLine* line=(*lineItr).get();
        if(!IsLineVisible(line)) break;
        TextLine::SegmentList::iterator segItr=line->mSegmentList.begin();
        g.Translate(line->mx,line->my);
        while(segItr!=line->mSegmentList.end()) {
            segItr->get()->Paint(g,this);
            ++segItr;
        }
        g.Translate(-line->mx,-line->my);
        ++lineItr;
    }
}

// FUNCTION: WON 0x007131b0
TextArea::LineVector::iterator TextArea::FindLineAt(int y) {
    mSearchLine->my=y;
    LineVector::difference_type first=0,count=mLineVector.end()-mLineVector.begin();
    while(count>0) {
        LineVector::difference_type step=count/2;
        LineVector::difference_type mid=first+step;
        if(mLineVector[mid]->my<y) { first=mid+1;count-=step+1; }
        else count=step;
    }
    LineVector::iterator itr=mLineVector.begin()+first;
    if(itr==mLineVector.end() || (*itr)->my>y) {
        if(itr!=mLineVector.begin()) --itr;
    }
    return itr;
}

// FUNCTION: WON 0x00713250
void TextArea::FindTopLine() {
    LineVector::iterator itr=FindLineAt(mVertOffset);
    mTopLineVectorPos=itr-mLineVector.begin();
}

// FUNCTION: WON 0x00713290
TextSegmentBase* TextArea::FindSegment(int x,int y,bool lenient) {
    if(mLineVector.empty()) return NULL;
    if(lenient) {
        if(x>=mL0A4 && mL0A4!=0) x=mL0A4-1;
        if(x<0) x=0;
        if(y>=mTotalHeight) y=mTotalHeight-1;
        if(y<0) y=0;
    }
    if(mLastFindSeg!=NULL) {
        TextLine* line=mLastFindSeg->mLine;
        int xpos=x-line->mx;
        if(line->my<=y && line->my+line->mHeight>=y) {
            if(xpos>=mLastFindSeg->mx && xpos<=mLastFindSeg->mx+mLastFindSeg->mWidth) return mLastFindSeg;
        }
    }
    LineVector::iterator lineItr=FindLineAt(y);
    TextLine* line=(*lineItr).get();
    if(!lenient && line->my+line->mHeight<y) return NULL;
    x-=line->mx;
    TextLine::SegmentList::iterator segItr=line->mSegmentList.begin();
    TextSegmentBase* seg=NULL;
    while(segItr!=line->mSegmentList.end()) {
        seg=segItr->get();
        if(x<=seg->mx+seg->mWidth) break;
        ++segItr;
    }
    if(seg!=NULL && !lenient && x<seg->mx) seg=NULL;
    mLastFindSeg=seg;
    return seg;
}

// FUNCTION: WON 0x007133d0
void TextArea::VirtualD8(int x,int y,bool newStart) {
    TextSegmentBase* seg=FindSegment(x,y,true);
    if(seg==NULL) return;
    int selection=seg->CharAt(x-seg->mLine->mx);
    if(newStart) {
        mSelStartChar=mSelEndChar=selection;
        mBackwardsSel=false;
        Invalidate();
        return;
    }
    bool backwards=mBackwardsSel;
    if(backwards) mSelStartChar=selection;
    else mSelEndChar=selection;
    if(mSelStartChar>mSelEndChar) {
        mBackwardsSel=!backwards;
        int old=mSelStartChar;mSelStartChar=mSelEndChar;mSelEndChar=old;
    }
    Invalidate();
}

// FUNCTION: WON 0x00713480
bool TextArea::Virtual44(int) {
    if(mNeedRecalcWordWrap) {
        mDelayWordWrapTimeAccum++;
        if(mDelayWordWrapTimeAccum>1) {
            int chars=0;
            if(!mSegmentList.empty()) chars=mSegmentList.back()->mAbsEndChar;
            if(mDelayWordWrapTimeAccum>=10 || mDelayWordWrapTimeAccum>=chars/16000) RecalcWordWrap(true);
        }
    }
    if(!mSelecting) return mNeedRecalcWordWrap;
    int x,y;GetMousePos(x,y);
    if(x>=0 && x<=Width() && y>=0 && y<=Height()) return true;
    if(y>Height()) {
        int delta=y-Height();
        if(TextFlagSet(TextFlag_ScrollFullLine) && delta<mDownScrollAmount) delta=mDownScrollAmount;
        mVertOffset+=delta;
    } else if(y<0) {
        int delta=-y;
        if(TextFlagSet(TextFlag_ScrollFullLine) && delta<mUpScrollAmount) delta=mUpScrollAmount;
        mVertOffset-=delta;
    }
    if(x>Width()) mHorzOffset+=x-Width();
    else if(x<0) mHorzOffset+=x;
    VirtualCC(false);
    ScrollTranslate(x,y);
    VirtualD8(x,y,false);
    return true;
}

// FUNCTION: WON 0x007135e0
void TextArea::Virtual2C(int x,int y,int button) {
    ScrollArea::Virtual2C(x,y,button);
    if(button!=0) return;
    ScrollTranslate(x,y);
    Fn2F5890();
    if(mCurLink!=NULL) {
        mCurLink->Pressed();
        Invalidate();
        mSelecting=false;
        return;
    }
    if(TextFlagSet(TextFlag_AllowSelection)) {
        mSelecting=true;
        Fn2F57D0(true);
        VirtualD8(x,y,(GetKeyMod()&1)==0);
    }
}

// FUNCTION: WON 0x007138f0
void TextArea::Virtual28() {
    ScrollArea::Virtual28();
    if(mCurLink!=NULL) {
        mCurLink->Released();
        mCurLink=NULL;
        Invalidate();
    }
    mSelecting=false;
}

// FUNCTION: WON 0x00713930
void TextArea::Virtual34(int x,int y) {
    ScrollArea::Virtual34(x,y);
    ScrollTranslate(x,y);
    TextSegmentBase* seg=FindSegment(x,y,false);
    if(seg==mCurLink) return;
    if(seg!=NULL && seg->IsLink()) {
        mCurLink=seg;
        mParent->Virtual78(Cursor::GetStandardCursor(StandardCursor_Hand));
    } else {
        mCurLink=NULL;
        mParent->Virtual78(TextFlagSet(TextFlag_AllowSelection)?Cursor::GetStandardCursor(StandardCursor_IBeam):(Cursor*)NULL);
    }
}

// FUNCTION: WON 0x007139f0
void TextArea::Virtual24(int x,int y) {
    ScrollArea::Virtual24(x,y);
    mParent->Virtual78(TextFlagSet(TextFlag_AllowSelection)?Cursor::GetStandardCursor(StandardCursor_IBeam):(Cursor*)NULL);
    Virtual34(x,y);
}

// FUNCTION: WON 0x00713a40
void TextArea::Virtual38(int x,int y) {
    ScrollArea::Virtual38(x,y);
    ScrollTranslate(x,y);
    if(mSelecting) VirtualD8(x,y,false);
}

// FUNCTION: WON 0x00713a90
bool TextArea::Virtual54(int key) {
    if(ScrollArea::Virtual54(key)) return true;
    switch(key) {
    case VK_RIGHT:mHorzOffset+=10;break;
    case VK_LEFT:mHorzOffset-=10;break;
    case VK_DOWN:mVertOffset+=mDownScrollAmount;break;
    case VK_UP:mVertOffset-=mUpScrollAmount;break;
    case VK_NEXT:mVertOffset+=mScrollArea.height;break;
    case VK_PRIOR:mVertOffset-=mScrollArea.height;break;
    default:return false;
    }
    mUserHasScrolled=true;
    VirtualCC(false);
    return true;
}

// FUNCTION: WON 0x00713b60
void TextArea::CopySelection() {
    GUIString text;
    SegmentList::iterator itr=mSegmentList.begin();
    while(itr!=mSegmentList.end()) {
        itr->get()->GetSelText(this,text);
        ++itr;
    }
    Clipboard::Set(text);
}

// FUNCTION: WON 0x00713c20
bool TextArea::Virtual50(int key) {
    if(ScrollArea::Virtual50(key)) return true;
    if(key==3) { CopySelection();return true; }
    return false;
}

// FUNCTION: WON 0x00713ec0
void TextArea::AddFormatedText(const GUIString& text,bool lineBreak) {
    int start=0,next=0;
    while(start<text.length()) {
        int end=start;
        while(end<text.length()) {
            int ch=text[end];
            if(ch=='\r') {
                if(end+1<text.length() && text[end+1]=='\n') next=end+2;
                else next=end+1;
                break;
            } else if(ch=='\n') { next=end+1;break; }
            end++;
        }
        bool foundReturn=true;
        if(end>=text.length()) { next=text.length();foundReturn=lineBreak; }
        AddSegment(text.substr((unsigned short)start,(unsigned short)(end-start)),foundReturn);
        start=next;
    }
}

// FUNCTION: WON 0x00713ff0
void TextArea::SetTextFlags(int flags,bool on) {
    bool wordWrap=TextFlagSet(TextFlag_WordWrap);
    if(on) mTextFlags|=flags;else mTextFlags&=~flags;
    if(!wordWrap && TextFlagSet(TextFlag_WordWrap)) Virtual08();
    if(!TextFlagSet(TextFlag_AllowSelection) && mSelStartChar!=mSelEndChar) {
        mSelecting=false;
        mSelStartChar=mSelEndChar=0;
        Invalidate();
    }
}

// FUNCTION: WON 0x00714090
void TextArea::SetMaxChars(int chars) {
    mMaxChars=chars;
    EnforceMaxChars();
    VirtualCC(false);
}
// FUNCTION: WON 0x007140c0
void TextArea::SetMaxLines(int lines) {
    mMaxLines=lines;
    EnforceMaxLines();
    VirtualCC(false);
}

// Private next borrowed TextArea methods. Requires genuine H declarations, no receiver allocation/lifecycle.
// FUNCTION: WON 0x00712260
void TextArea::AddSeg(TextSegmentBase* segment) {
    TextLine* line=mLineVector.back().get();
    bool adjust=false;
    int baseline=segment->mAscent;
    if(baseline>line->mBaseline) { line->mBaseline=baseline;adjust=true; }
    int descent=segment->mHeight-segment->mAscent;
    if(descent>line->mDescent) line->mDescent=descent;
    if(segment->mHeight>line->mHeight) {
        mTotalHeight-=line->mHeight;
        line->mHeight=segment->mHeight;
        mTotalHeight+=line->mHeight;
        adjust=true;
    }
    if(line->mDescent+line->mBaseline>line->mHeight) {
        mTotalHeight-=line->mHeight;
        line->mHeight=line->mBaseline+line->mDescent;
        mTotalHeight+=line->mHeight;
        adjust=true;
    }
    if(adjust) {
        TextLine::SegmentList::iterator itr=line->mSegmentList.begin();
        while(itr!=line->mSegmentList.end()) {
            TextSegmentBase* seg=itr->get();
            seg->my=line->mBaseline-seg->mAscent;
            ++itr;
        }
    }
    segment->mx=line->mWidth;
    segment->my=line->mBaseline-segment->mAscent;
    segment->mLine=line;
    line->mSegmentList.push_back(SmartPtr<TextSegmentBase>(segment));
    line->mWidth+=segment->mWidth;
    AlignLine(line,mLineAlign);
}
// FUNCTION: WON 0x007124a0
void TextArea::AlignLine(TextLine* line,HorzAlign align) {
    int x=0;
    switch(align) {
    case HorzAlign_Center:x=(mScrollArea.width-line->mWidth)/2;break;
    case HorzAlign_Right:x=mScrollArea.width-line->mWidth;break;
    }
    if(x<=0) x=0;
    line->mx=x;
}
// FUNCTION: WON 0x00712980
void TextArea::AddSegment(TextSegmentBase* segment) {
    if(!mSegmentList.empty()) segment->mAbsStartChar=mSegmentList.back()->mAbsEndChar;
    segment->mAbsEndChar=segment->mAbsStartChar+segment->GetLength();
    bool bottom=ShouldScrollOnBottom();
    mSegmentList.push_back(SmartPtr<TextSegmentBase>(segment));
    segment->CalcDimensions();
    AddSegmentPrv(segment);
    EnforceMaxChars();
    EnforceMaxLines();
    if(bottom) mVertOffset=mTotalHeight+mExtraHeight-mScrollArea.height;
    VirtualCC(false);
}
// FUNCTION: WON 0x00712b60
void TextArea::EnforceInvariants() {
    mL0A4=0;
    mTotalHeight=0;
    mNumLines=0;
    if(mLineVector.empty() || mSegmentList.empty()) return;
    int deltaPos=mLineVector.front()->my;
    LineVector::iterator lineItr=mLineVector.begin();
    while(lineItr!=mLineVector.end()) {
        TextLine* line=(*lineItr).get();
        if(line->mWidth>mL0A4) mL0A4=line->mWidth;
        line->my-=deltaPos;
        mTotalHeight+=line->mHeight;
        ++lineItr;
    }
    int deltaChar=mSegmentList.front()->mAbsStartChar;
    SegmentList::iterator itr=mSegmentList.begin();
    while(itr!=mSegmentList.end()) {
        TextSegmentBase* seg=itr->get();
        seg->mAbsStartChar-=deltaChar;
        seg->mAbsEndChar-=deltaChar;
        if(seg->mLineBreak) mNumLines++;
        ++itr;
    }
    mSelStartChar-=deltaChar;
    mSelEndChar-=deltaChar;
    if(mSelStartChar<0) mSelStartChar=0;
    if(mSelEndChar<0) mSelEndChar=0;
    mVertOffset-=deltaPos;
}
// FUNCTION: WON 0x00712cd0
void TextArea::RemoveSegmentsUpTo(TextSegmentBase* segment) {
    TextLine* findLine=segment->mLine;
    mLastFindSeg=NULL;
    bool found=false;
    LineVector::iterator lineItr=mLineVector.begin();
    while(lineItr!=mLineVector.end()) {
        TextLine* line=(*lineItr).get();
        if(line==findLine) found=true;
        if(found && !line->mSegmentList.empty()) {
            TextSegmentBase* seg=line->mSegmentList.back().get();
            if(seg->GetActual()!=segment) {
                segment->mLine=line;
                break;
            }
        }
        ++lineItr;
    }
    mLineVector.erase(mLineVector.begin(),lineItr);
    TextSegmentBase* stop=NULL;
    if(!mLineVector.empty()) {
        TextLine* line=mLineVector.front().get();
        if(!line->mSegmentList.empty()) stop=line->mSegmentList.front()->GetActual();
    }
    SegmentList::iterator itr=mSegmentList.begin();
    while(itr!=mSegmentList.end()) {
        TextSegmentBase* seg=itr->get();
        if(seg==stop) break;
        if(seg==mCurLink) mCurLink=NULL;
        ++itr;
    }
    mSegmentList.erase(mSegmentList.begin(),itr);
    EnforceInvariants();
    VirtualCC(false);
}
// FUNCTION: WON 0x00712f70
void TextArea::EnforceMaxChars() {
    if(mMaxChars<=0 || mSegmentList.empty()) return;
    TextSegmentBase* seg=mSegmentList.back().get();
    int toRemove=seg->mAbsEndChar-mMaxChars;
    if(toRemove<=0) return;
    SegmentList::iterator itr=mSegmentList.begin();
    while(itr!=mSegmentList.end()) {
        seg=itr->get();
        if(seg->mAbsEndChar+1>=toRemove) break;
        ++itr;
    }
    RemoveSegmentsUpTo(seg);
}
// FUNCTION: WON 0x00712fd0
void TextArea::EnforceMaxLines() {
    if(mMaxLines<=0 || mSegmentList.empty()) return;
    TextSegmentBase* seg=mSegmentList.back().get();
    int toRemove=mNumLines-mMaxLines;
    if(toRemove<=0) return;
    int count=0;
    SegmentList::iterator itr=mSegmentList.begin();
    while(itr!=mSegmentList.end()) {
        seg=itr->get();
        if(seg->mLineBreak) count++;
        if(count>=toRemove) break;
        ++itr;
    }
    RemoveSegmentsUpTo(seg);
}

// FUNCTION: WON 0x00713680
void TextArea::Virtual30(int x,int y,int button) {
    ScrollArea::Virtual30(x,y,button);
    if(button!=0) return;
    ScrollTranslate(x,y);
    TextSegmentBase* seg=FindSegment(x,y,false);
    bool pressed=false;
    if(mCurLink!=NULL) pressed=mCurLink->Released();
    if(seg==mCurLink && pressed) {
        mLinkParam=mCurLink->GetLinkParam();
        Fn2F53F0(new TextLinkActivatedEvent(this,mCurLink->GetActual()),false);
    }
    if(mCurLink!=NULL) {
        if(seg==NULL || !seg->IsLink()) {
            mCurLink=NULL;
            mParent->Virtual78(TextFlagSet(TextFlag_AllowSelection)?Cursor::GetStandardCursor(StandardCursor_IBeam):(Cursor*)NULL);
        } else mCurLink=seg;
        Invalidate();
    }
    mSelecting=false;
}
/* HISTORICAL_REJECTED_SOURCE: EXE 0x00713860
Preserved unused source for future models. Pinned SDK declares no event destructor;
the target135B destructor is genuinely implicit. This former authored141B body
adds an own-vptr store absent from native. Active class declaration now permits
natural implicit SP20/senderSP10/RefCount cleanup; no replacement body is authored.
// Actual owner cleanup: linkSP20 then ComponentEvent senderSP10/base. Lifecycle support0.
TextLinkActivatedEvent::~TextLinkActivatedEvent() {}
*/

// FUNCTION: WON 0x007127d0
void TextArea::AddSegmentPrv(TextSegmentBase* segment) {
    if(mLineVector.empty()) AddLine();
    TextLine* line=mLineVector.back().get();
    segment->mLine=line;
    segment->PerformInstructions(this);
    if(Width()<=0 || line->mWidth+segment->mWidth<=mScrollArea.width || !TextFlagSet(TextFlag_WordWrap) || !mHasBeenVisible) {
        AddSeg(segment);
        if(line->mWidth>mL0A4 && !TextFlagSet(TextFlag_WordWrap)) mL0A4=line->mWidth;
    } else segment->Wrap(this,line);
    if(segment->mLineBreak) {
        mNumLines++;
        AddLine();
    }
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00713840
// (public: virtual void * __thiscall WONAPI::TextLinkActivatedEvent::`scalar deleting destructor'(unsigned int)    [??_GTextLinkActivatedEvent@WONAPI@@UA)
// TEMPLATE: WON 0x0071b800
// (public: class std::deque<class WONAPI::SmartPtr<class WONAPI::TextLine>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::TextLine> > >::itera)
// TEMPLATE: WON 0x0071b810
// (public: class std::deque<class WONAPI::SmartPtr<class WONAPI::TextLine>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::TextLine> > >::itera)
// TEMPLATE: WON 0x0071b830
// (public: class std::deque<class WONAPI::SmartPtr<class WONAPI::TextLine>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::TextLine> > >::itera)
// TEMPLATE: WON 0x0071b850
// (public: bool __thiscall std::deque<class WONAPI::SmartPtr<class WONAPI::TextLine>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::TextLine> )
// TEMPLATE: WON 0x0071d4e0
// (public: class std::deque<class WONAPI::SmartPtr<class WONAPI::TextLine>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::TextLine> > >::itera)
// TEMPLATE: WON 0x00723de0
// (class std::deque<class WONAPI::SmartPtr<class WONAPI::TextLine>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::TextLine> > >::iterator __cd)
// TEMPLATE: WON 0x00723eb0
// (class std::deque<class WONAPI::SmartPtr<class WONAPI::TextLine>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::TextLine> > >::iterator __cd)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00713860
// (public: virtual __thiscall WONAPI::TextLinkActivatedEvent::~TextLinkActivatedEvent(void)    [??1TextLinkActivatedEvent@WONAPI@@UAE@XZ])
