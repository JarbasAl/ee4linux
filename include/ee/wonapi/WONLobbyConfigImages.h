#ifndef EE_WONAPI_WONLOBBYCONFIGIMAGES_H
#define EE_WONAPI_WONLOBBYCONFIGIMAGES_H
#include "ee/wonapi/WONLobbyStretchImage.h"
namespace WONAPI {

// Complete actual primitive filter owners. SDK fields corroborated by native
// config allocations1c/14/18/14 and writes, both real virtual slots0/4.
// Genuine SDK ImageFilter.cpp family, full source owner; no claimed native identity.
class MultiImageFilter : public ImageFilter {
protected:
    typedef std::list<SmartPtr<ImageFilter> > FilterList;
    FilterList mFilterList;
public:
    MultiImageFilter() {}
    MultiImageFilter* Add(ImageFilter*);
    virtual SmartPtr<RawImage> Filter(RawImage*);
};

class ScaleImageFilter : public ImageFilter {
public:
    int mWidth,mHeight; //08/0c
    int mPercentWidth,mPercentHeight; //10/14
    bool mIsPercent; //18 naturalpad, total1c
    ScaleImageFilter(int percent=100) { SetPercentSize(percent,percent); }
    ScaleImageFilter(int w,int h) { SetAbsSize(w,h); }
    void SetAbsSize(int,int); //348b80
    void SetPercentSize(int,int); //348b60
    bool CheckSize(Image*); //348ba0 actual SDK nonvirtual helper
    virtual SmartPtr<RawImage> Filter(RawImage*); //348c30 native external
};
class TintImageFilter : public ImageFilter {
public:
    int mRed,mGreen,mBlue; //08/0c/10 total14
    TintImageFilter(int r,int g,int b) : mRed(r),mGreen(g),mBlue(b) {}
    virtual SmartPtr<RawImage> Filter(RawImage*); //348dd0 native external
};
class CropImageFilter : public ImageFilter {
public:
    int mx,my,mWidth,mHeight; //08/0c/10/14 total18
    CropImageFilter(int x=0,int y=0,int w=0,int h=0) : mx(x),my(y),mWidth(w),mHeight(h) {}
    void SetPos(int x,int y) { mx=x;my=y; }
    void SetSize(int w,int h) { mWidth=w;mHeight=h; }
    int GetX() { return mx; }
    int GetY() { return my; }
    int GetWidth() { return mWidth; }
    int GetHeight() { return mHeight; }
    virtual SmartPtr<RawImage> Filter(RawImage*); //348a60 native external
};
class TrimImageFilter : public CropImageFilter {
public:
    TrimImageFilter() {}
    virtual SmartPtr<RawImage> Filter(RawImage*);
};

class TransparentImageFilter : public ImageFilter {
public:
    enum PixelSample { UpperLeft,UpperRight,LowerLeft,LowerRight };
    DWORD mTransparentColor; //08
    PixelSample mPixelSample; //0c
    bool mUseSample; //10 naturalpad,total14
    TransparentImageFilter(DWORD color) { mUseSample=false;mTransparentColor=color; }
    TransparentImageFilter(PixelSample sample) { mUseSample=true;mPixelSample=sample; }
    void SetPixelSample(PixelSample theSample) { mUseSample = true; mPixelSample = theSample; }
    void SetTransparentColor(DWORD theColor) { mUseSample = false; mTransparentColor = theColor; }
    virtual SmartPtr<RawImage> Filter(RawImage*); //349110 native external
};

// Complete actual external-lifetime configuration owners. New ImageFilterConfig
// allocation calls nativeconstructor38c260 and actualnormaldestruction38c2b0;
// completebaseImageConfig7c +RawSP7c/bool80 =>84 bytes, no opaque owner stand-in.
class ImageConfig : public ComponentConfig {
public:
    static ComponentConfig* CfgFactory(); //38b130 genuine SDK static API, original external provider only
    static Component* CompFactory(); //2c7b10 genuine SDK static API, original external provider only
    ImageConfig(); //38b2b0 native external
    // Genuine SDK implicit virtual destructor: automatic owned members and base cleanup.
    virtual bool Virtual10(ConfigParser*); //3099f0 true leaf; native external only
    virtual bool Virtual24(const std::string&); //38b3e0
    virtual bool Virtual2C(const std::string&,int&); //38b370
    virtual Image* GetImage() { return mImage.get(); } //38b2d0 slot38, support0
    virtual RawImage* GetRawImage() { return NULL; } //2c7b10 slot3c, genuinebase behavior/support0
    SmartPtr<Image> mImage; //78, total7c
};
// Complete actual80 external-base owners; raw convenience7c aliases inherited ImageSP78.
class StretchImageConfig : public ImageConfig {
public:
    static ComponentConfig* CfgFactory(); //38b190 genuine SDK static API, original external provider only
    StretchImageConfig(); //38b4e0
    // Genuine SDK implicit virtual destructor: automatic owned members and base cleanup.
    virtual bool Virtual24(const std::string&); //38b630
    StretchImage* mStretchImage; //7c, raw alias, full80
};
class StretchCenterImageConfig : public ImageConfig {
public:
    static ComponentConfig* CfgFactory(); //38b1f0 genuine SDK static API, original external provider only
    StretchCenterImageConfig(); //38bf90
    // Genuine SDK implicit virtual destructor: automatic owned members and base cleanup.
    virtual bool Virtual24(const std::string&); //38c0e0
    StretchCenterImage* mStretchCenterImage; //7c raw alias, full80
};
class ImageFilterConfig : public ImageConfig {
public:
    static ComponentConfig* CfgFactory(); //38b250 genuine SDK static API, original external provider only
    ImageFilterConfig(); //38c260 native external
    // Genuine SDK implicit virtual destructor: automatic owned members and base cleanup.
    virtual bool Virtual24(const std::string&); //38c780
    virtual Image* GetImage(); //38cc80 slot38
    virtual RawImage* GetRawImage() { return mRawImage.get(); } //38c280 slot3c, support0
    SmartPtr<RawImage> HandleFilter(); //38c340
    void SetRawImage(RawImage*); //38c6e0
    SmartPtr<RawImage> mRawImage; //7c
    bool mImageIsRawImage; //80 naturalpad, total84
};
}
#endif
