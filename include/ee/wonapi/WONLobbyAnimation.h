// Complete source owner contract: SDK Animation.h, original ctor335340/335540 and RTTI.
#ifndef EE_WONAPI_WONLOBBYANIMATION_H
#define EE_WONAPI_WONLOBBYANIMATION_H
#include <list>
namespace WONAPI {
class AnimationFrame : public RefCount {
public:
    enum DisposalMethod { DisposalMethod_None, DisposalMethod_Leave, DisposalMethod_RestorBGColor, DisposalMethod_RestorePrevious, DisposalMethod_Max };
    SmartPtr<Image> mImage; //08
    int mDelayTime; //0c
    int mLeft, mTop; //10,14
    DisposalMethod mDisposalMethod; //18; complete1c
    AnimationFrame(Image* = NULL,int = 0);
    void Draw(Graphics&,int,int);
    SmartPtr<AnimationFrame> Duplicate();
};
typedef std::list<SmartPtr<AnimationFrame> > AnimationFrameList;
class Animation : public RefCount {
private:
    AnimationFrameList mFrameList; //08..13
    AnimationFrameList::iterator mFrameItr; //14
    int mNumLoops,mLoopCount; //18,1c
    bool mNeedAccumulate,mCopyFirstImageToAccumulate; //20,21
    SmartPtr<NativeImage> mAccumulateImage; //24
    int mWidth,mHeight; //28,2c; complete30
public:
    Animation(const AnimationFrameList&,int = -1,int = -1);
    void RealizeNative(DisplayContext*);
    void Rewind(bool = true);
    bool NextFrame();
    int GetFrameDelayTime();
    int GetNumFrames() { return (int)mFrameList.size(); } //native count+10 used3488af, helper0
    void Draw(Graphics&,int,int);
    SmartPtr<Animation> Duplicate();
    int GetWidth() { return mWidth; } //actual SDK inline getter, nativeconsumer306e78/7b
    int GetHeight() { return mHeight; } //actual SDK inline getter, no storage/layout change
    void SetNumLoops(int n) { mNumLoops=n; }
    void SetNeedAccumulate(bool b) { mNeedAccumulate=b; }
    void SetCopyFirstImageToAccumulate(bool b) { mCopyFirstImageToAccumulate=b; }
};
}
#endif
