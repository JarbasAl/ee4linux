#ifndef EE_WONAPI_WONLOBBYLINELAYOUT_H
#define EE_WONAPI_WONLOBBYLINELAYOUT_H
#include "ee/wonapi/WONLobbyTextWrap.h"
namespace WONAPI {
// Actual complete LineElement38 with original constructor30b7c0 and destructor
// 30b8a0 providers; native builder30c087 allocates38. Sole base LineSegment14.
// Genuine default constructor is implemented in TextLayoutConstructors.cpp;
// original external destructor/copy behavior remains, no unrelated allocation authority.
class LineElement : public LineSegment {
public:
    LineElement(); //actual original30b7c0 complete38 constructor provider
    virtual int GetWidth(); //311470 external getter
    virtual int GetHeight(); //31a670 external getter
    virtual int GetAscent(); //30b870 external getter
    virtual LineSegment* GetSegmentAt(int&,int&,bool); //30b920
    virtual void SetViewport(ScrollAreaViewport*,int,int); //30ba00
    virtual void SetSelIndex(); //30bab0
    virtual void GetSelText(GUIString&,ElementSelectionInfo&); //30bb10
    virtual bool GetSelCharPos(int&,int&,int,int); //30bb70
    virtual bool FindStr(ElementFindInfo&); //30bbd0
    virtual int GetEndSelIndex(); //30baf0 external support0 getter
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //30bc40 target has no PaintSelection vslot/prepass
    typedef std::list<SmartPtr<LineSegment> > SegmentList;
    SegmentList mSegmentList; //14..1f retained original-owned list
    bool mIsGlueLine; //20 natural padding21..23
    int mLeftMargin,mBaseline,mWidth,mHeight,my; //24,28,2c,30,34
};
inline bool LineBuilder::IsEmpty() { return mLine.get()?mLine->mSegmentList.empty():true; }
inline void LineBuilder::Clear() { mLine=NULL; }
}
#endif
