#ifndef EE_WONAPI_WONLOBBYHTMLIMAGESEGMENTS_H
#define EE_WONAPI_WONLOBBYHTMLIMAGESEGMENTS_H
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
// Actual complete18 owner, native allocation33a5ca and constructor stores33a5de.
// Genuine inline SDK ctor; implicit cleanup33a9d0 releases Component14.
class ComponentLineSegment : public LineSegment {
public:
    SmartPtr<Component> mComponent; //14
    ComponentLineSegment(Component* component):mComponent(component) {}
    virtual int GetWidth();
    virtual int GetHeight();
    virtual void MouseEnter(int,int);
    virtual void MouseExit();
    virtual void MouseDown(int,int,MouseButton);
    virtual void MouseUp(int,int,MouseButton);
    virtual void MouseMove(int,int);
    virtual void MouseDrag(int,int);
    virtual bool IsInvalid();
    virtual bool CanStartSelection();
    virtual bool AllowModalInput();
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&);
    virtual void SetViewport(ScrollAreaViewport*,int,int);
};
// Actual complete2c owner, native33aa30/76 ctor, implicit cleanup33aaf0.
class ImageLineSegment : public LineSegment {
public:
    SmartPtr<Image> mImage; //14
    int mHSpace,mVSpace,mBorder,mBorderColor; //18,1c,20,24
    char mAlign; //28; natural29..2b
    ImageLineSegment(Image* =NULL); //33aa30 actualexternal constructor RET4
    virtual int GetWidth(); //33aa80
    virtual int GetHeight(); //33aaa0
    virtual char GetVertAlign(); //33aac0
    virtual void Paint(Graphics&,int,int,ElementPaintInfo&); //30b390
};
// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK derived owner, two int fields.
// Existing base2c and its virtual contract are unchanged; no Native allocation/VT claim.
class ScaledImageLineSegment : public ImageLineSegment
{
public:
	int mWidth, mHeight;

public:
	ScaledImageLineSegment(Image *theImage, int theWidth, int theHeight) : ImageLineSegment(theImage), mWidth(theWidth), mHeight(theHeight) { }

	virtual int GetWidth() { return mWidth + 2*mHSpace; }
	virtual int GetHeight() { return mHeight + 2*mVSpace; }
	virtual void Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo);
};
typedef SmartPtr<ScaledImageLineSegment> ScaledImageLineSegmentPtr;
// Actual complete40 owner, native constructor inlined at33a690/33a743.
// Implicit cleanup33ab70 releases3c,38,34 then inherited14, in that order.
class ImageLinkLineSegment : public ImageLineSegment {
public:
    DWORD mNormalColor,mDownColor; //2c,30
    SmartPtr<TextLinkParam> mCurLinkParam,mAnchorLinkParam; //34,38
    SmartPtr<ImageMap> mImageMap; //3c
    ImageLinkLineSegment(TextLinkParam* param,int color,int down,Image* image)
        :ImageLineSegment(image),mNormalColor(color),mDownColor(down),
         mCurLinkParam(param),mAnchorLinkParam(param)
    {
        if(param!=NULL) param->mLinkList.push_back(this); //genuine SDK AddLink inline operation
    }
    virtual void LinkDown();
    virtual void LinkUp();
    virtual void MouseEnter(int,int);
    virtual void MouseMove(int,int);
    virtual void MouseDown(int,int,MouseButton);
    virtual void MouseUp(int,int,MouseButton);
    virtual void MouseExit();
    virtual bool CanStartSelection();
};
}
#endif
