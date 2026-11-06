// Complete native10B ProgramImage owner; Image prefix0c plus callback0c.
#ifndef EE_WONAPI_WONLOBBYIMAGEHELPERS_H
#define EE_WONAPI_WONLOBBYIMAGEHELPERS_H
namespace WONAPI {
// Genuine SDK ImagePiece, complete Image0c +ownedImageSP0c +left10/top14 =18.
class ImagePiece : public Image {
private:
    SmartPtr<Image> mImage;
    int mLeft,mTop;
protected:
    virtual ~ImagePiece();
public:
    ImagePiece(Image*,int,int,int,int);
    virtual void Draw(DisplayContext*,int,int,int,int,int,int);
};
class ProgramImage : public Image {
protected:
    typedef void (*PaintFunc)(Graphics&,int,int);
    PaintFunc mPaintFunc;
    virtual void Paint(Graphics&); //3287c0, slot14
public:
    ProgramImage(int = 0,int = 0,PaintFunc = NULL); //328790
    ProgramImage(PaintFunc); //genuine separate SDK callback-only constructor
    virtual SmartPtr<Image> Duplicate(); //3286d0
    virtual void Draw(DisplayContext*,int,int,int,int,int,int); //3287f0
    virtual void SetSize(int w,int h) { mWidth=(unsigned short)w; mHeight=(unsigned short)h; }
};
}
#endif
