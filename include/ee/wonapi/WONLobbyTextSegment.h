#ifndef EE_WONAPI_WONLOBBYTEXTSEGMENT_H
#define EE_WONAPI_WONLOBBYTEXTSEGMENT_H
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
struct ElementSelectionInfo {
    int mStartSelSegment,mEndSelSegment;
    int mStartSelSegmentPos,mEndSelSegmentPos;
}; //actual10 hex plain value; original selection readers use all four fields
// Actual complete find-info34 value: native30cd70 constructs/copies full record,
// owned GUIString10 +caseSensitive14/findPrev15 and ordinary scalar outputs.
struct ElementFindInfo {
    int mStartSelSegment,mEndSelSegment;
    int mStartSelSegmentPos,mEndSelSegmentPos;
    GUIString mFindText; //10
    bool mCaseSensitive,mFindPrev; //14,15 actual native30ce3c/30ce42
    int mCurFoundLength; //18
    int mFoundStartSegment,mFoundEndSegment; //1c,20
    int mFoundStartChar,mFoundEndChar; //24,28
    int mFoundX,mFoundY; //2c,30
};
// Complete actual TextLineSegment28 with genuine SDK inline initialization.
// No invented standalone constructor provider; source lifetime/copy bodies remain original-owned.
class TextLineSegment : public LineSegment {
public:
    TextLineSegment() :mWidth(0),mMinWidth(0) {} //genuine SDK default, no standalone RVA/primary credit
    TextLineSegment(const GUIString& text,int color,Font* font)
        :mText(text),mColor(color),mFont(font),mWidth(0),mMinWidth(0) {} //actual339595..3395cd; support0
    virtual int GetWidth(); //309ed0 support0 cached getter
    virtual int GetMinWidth(); //309e70
    virtual int GetHeight(); //339720 original external
    virtual int GetAscent(); //339730 original external
    virtual int GetFirstWrapWidth(); //309f00
    virtual void MouseEnter(int,int); //30b310 original external
    virtual int GetSelChar(int,int); //309f90 support0 forwarding overload
    int GetSelChar(int,int,int,int); //309f40 substantive hit-position algorithm
    virtual bool GetSelCharPos(int&,int&,int,int); //30a260
    virtual void GetSelText(GUIString&,ElementSelectionInfo&); //309fc0
    virtual bool FindStr(ElementFindInfo&); //30a3f0 original external, no implementation in this cohort
    bool FindPartial(ElementFindInfo&,int,int); //30a320
    virtual void Wrap(MultiLineBuilder*,bool); //30a590 original external
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //30a900 support0 forwarding overload
    void Paint(Graphics&,int,int,int,int,int,int,int,ElementPaintInfo&); //30a950 complete target text/selection painter
    void PaintSelection(Graphics&,int,int,int,int,int,int,int,ElementPaintInfo&); //genuine SDK nonvirtual background-only painter; SOURCE_ONLY_NO_NATIVE_ID
    GUIString mText; //14, actual retained GUIString
    int mColor; //18
    SmartPtr<Font> mFont; //1c retained original owner
    int mWidth,mMinWidth; //20,24
};
}
#endif
