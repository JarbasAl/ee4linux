#ifndef EE_WONAPI_WONLOBBYHTMLELEMENTSEGMENTS_H
#define EE_WONAPI_WONLOBBYHTMLELEMENTSEGMENTS_H
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
// Actual complete24 primitive-only owner, native339d9b allocation.
// Natural implicit base-only cleanup311480 ->338c70.
class HorizontalRuleSegment : public LineSegment {
public:
    int mWidthSpec,mWidth,mHeight; //14,18,1c
    bool mNoShade; //20; natural21..23
    HorizontalRuleSegment():mWidthSpec(0),mWidth(0),mHeight(1),mNoShade(false) {}
    virtual int GetAscent() { return GetHeight()/2; } //actual339e50 virtualheightcall
    virtual int GetHeight() { return mHeight+14; } //339e60
    virtual int GetWidth() { return mWidth; } //339e70
    virtual void SetMaxWidth(int); //309c10
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //309c50
    virtual void GetSelText(GUIString&,ElementSelectionInfo&); //309d20
};
// Actual complete24 primitive-only owner, native33ad7b/33adc2 allocation.
class BulletElement : public LineSegment {
public:
    int mWidth,mHeight,mColor,mType; //14,18,1c,20
    BulletElement(int color=0,int type=-1,int width=5,int height=5)
        :mWidth(width),mHeight(height),mColor(color),mType(type) {}
    virtual int GetWidth() { return mType>=0?0:mWidth+10; } //33aec0
    virtual int GetHeight() { return mHeight+2; } //33aee0
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //30b470
};
// Actual complete24 retained owner, native33adce allocation/inlineconstructor.
// Natural implicit normal33af30 releasesGUIString20 thenFont14 beforebase.
class OrderedListElement : public LineSegment {
public:
    SmartPtr<Font> mFont; //14
    int mColor,mWidth; //18,1c
    GUIString mText; //20
    OrderedListElement(int color,Font* font,const GUIString& text)
        :mFont(font),mColor(color),mText(text)
    {
        mWidth=font->GetStringWidth(mText,0,mText.length()); //genuine SDK one-arg inline expansion
    }
    virtual int GetWidth() { return 0; } //actualfolded2c7b10
    virtual int GetHeight() { return mFont->GetHeight(); } //33aef0
    virtual int GetAscent() { return mFont->GetAscent(); } //33af00
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //30b510
};
}
#endif
