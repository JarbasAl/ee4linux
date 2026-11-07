// Native complete ImageFilter8/StretchImageFilter24 and StretchImage30 owners.
#ifndef EE_WONAPI_WONLOBBYSTRETCHIMAGE_H
#define EE_WONAPI_WONLOBBYSTRETCHIMAGE_H
#include "ee/wonapi/WONLobbyImageHelpers.h"
namespace WONAPI {
class ImageFilter : public RefCount {
public:
    virtual SmartPtr<RawImage> Filter(RawImage *theImage) { return theImage; } //genuine SDK owned identity default, native slot4
};
class StretchImageFilter : public ImageFilter {
public:
    int mWidth,mHeight; //08,0c
    int mLeftWidth,mRightWidth,mTopHeight,mBottomHeight; //10..1c
    bool mTileHorz,mTileVert,mSkipCenter; //20..22, complete24
    StretchImageFilter(); //349540
    virtual SmartPtr<RawImage> Filter(RawImage*); //349570 external real pixel algorithm until authored cohort
    void SetSize(int w,int h) { mWidth=w; mHeight=h; }
    void SetBorders(int l,int t,int r,int b) { mLeftWidth=l; mRightWidth=r; mTopHeight=t; mBottomHeight=b; }
    void SetTile(bool h,bool v) { mTileHorz=h; mTileVert=v; }
    void SetSkipCenter(bool b) { mSkipCenter=b; }
};
class StretchImage : public ProgramImage {
private:
    SmartPtr<RawImage> mImage; //10
    SmartPtr<NativeImage> mCachedImage; //14
    bool mNeedRecalc; //18
    int mLeftWidth,mRightWidth,mTopHeight,mBottomHeight; //1c..28
    bool mTileHorz,mTileVert,mSkipCenter; //2c..2e, complete30
    void CalcImage(DisplayContext*); //349810
protected:
    virtual ~StretchImage(); //349740
public:
    StretchImage(RawImage*,int,int,int,int); //349650
    int GetLeft() { return mLeftWidth; } //genuine SDK inline, actual1c
    int GetRight() { return mRightWidth; } //genuine SDK inline, actual20
    int GetTop() { return mTopHeight; } //genuine SDK inline, actual24
    int GetBottom() { return mBottomHeight; } //genuine SDK inline, actual28
    virtual void Paint(Graphics&); //3499a0
    virtual SmartPtr<Image> Duplicate(); //349b80
    virtual SmartPtr<NativeImage> GetNative(DisplayContext*); //349c60
    virtual void SetSize(int,int); //3497e0
    void SetImage(RawImage*); //349ab0
    void SetBorders(int,int,int,int); //349b10
    void CopyAttributes(StretchImage*); //349b30
    void SetTile(bool h,bool v) { mTileHorz=h; mTileVert=v; }
    void SetSkipCenter(bool b) { mSkipCenter=b; }
};
class StretchCenterImage : public ProgramImage {
protected:
    SmartPtr<Image> mStretchImage,mCenterImage; //10,14 complete18
public:
    StretchCenterImage(Image* stretch=NULL,Image* center=NULL) : mStretchImage(stretch),mCenterImage(center) {}
    void SetStretchImage(Image* image) { mStretchImage=image; } //actual SDK inline, native38c13a
    void SetCenterImage(Image* image) { mCenterImage=image; } //actual SDK inline, native38c1dd
    virtual void Paint(Graphics&); //349cb0
    virtual SmartPtr<Image> Duplicate(); //349d40
    virtual void SetSize(int,int); //349d20
};
}
#endif
