// Source-only reconstructed WONGUI Animation; no runtime substitution authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyAnimation.h"

using namespace WONAPI;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735340
AnimationFrame::AnimationFrame(Image *theImage, int theDelayTime)
{
	mImage = theImage;
	mDelayTime = theDelayTime;
	mDisposalMethod = DisposalMethod_None;
	mLeft = mTop = 0;
}

// FUNCTION: WON 0x007353e0
// (??_GAnimationFrame@WONAPI@@UAEPAXI@Z)

// FUNCTION: WON 0x00735400
// (??1AnimationFrame@WONAPI@@UAE@XZ)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735460
SmartPtr<AnimationFrame> AnimationFrame::Duplicate()
{
	AnimationFrame *aFrame = new AnimationFrame(mImage.get(),mDelayTime);
	aFrame->mDisposalMethod = mDisposalMethod;
	aFrame->mLeft = mLeft;
	aFrame->mTop = mTop;
	return aFrame;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735510
void AnimationFrame::Draw(Graphics &g, int x, int y)
{
	g.DrawImage(mImage.get(),x+mLeft,y+mTop);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735540
Animation::Animation(const AnimationFrameList &theList,  int theWidth, int theHeight) : mFrameList(theList)
{
	if(theWidth==-1 || theHeight==-1)
	{
		if(!theList.empty())
		{
			Image *anImage = theList.front()->mImage.get();
			mWidth = anImage->mWidth;
			mHeight = anImage->mHeight;
		}
		else
			mWidth = mHeight = 0;
	}
	else
	{
		mWidth = theWidth;
		mHeight = theHeight;
	}

	mFrameItr = mFrameList.begin();
	mNumLoops = 0;
	mLoopCount = 0;

	mNeedAccumulate = false;
	mCopyFirstImageToAccumulate = false;
}

// FUNCTION: WON 0x00735660
// (??_GAnimation@WONAPI@@UAEPAXI@Z)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735730
void Animation::RealizeNative(DisplayContext *theContext)
{
	if(mFrameList.empty())
		return;

	if(dynamic_cast<RawImage*>(mFrameList.front()->mImage.get())!=NULL)
	{
		for(AnimationFrameList::iterator anItr = mFrameList.begin(); anItr!=mFrameList.end(); ++anItr)
		{
			AnimationFrame *aFrame = anItr->get();
			aFrame->mImage = ((RawImage*)aFrame->mImage.get())->GetNative(theContext).get();
		}
	}

	if(mNeedAccumulate && !mCopyFirstImageToAccumulate && mAccumulateImage.get()==NULL)
		mAccumulateImage = theContext->CreateImage(mWidth,mHeight);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007358f0
void Animation::Rewind(bool resetLoopCount)
{
	if(resetLoopCount)
		mLoopCount = 0;

	mFrameItr = mFrameList.begin();

	if(mCopyFirstImageToAccumulate && !mFrameList.empty())
		mAccumulateImage = (NativeImage*)mFrameList.front()->mImage->Duplicate().get();
	else if(mAccumulateImage.get()!=NULL)
	{
		AnimationFrame *aFrame = mFrameItr->get();
		aFrame->Draw(mAccumulateImage->GetGraphics(),0,0);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735a10
bool Animation::NextFrame()
{
	if(mFrameList.size()==1 || ++mFrameItr==mFrameList.end())
	{
		if(mNumLoops>0 && ++mLoopCount>=mNumLoops)
			return false;
		else
			Rewind(false);
	}

	if(mAccumulateImage.get()!=NULL)
	{
		AnimationFrame *aFrame = mFrameItr->get();
		aFrame->Draw(mAccumulateImage->GetGraphics(),0,0);
	}	

	return true;
}
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735a70
int Animation::GetFrameDelayTime()
{
	if(mFrameItr==mFrameList.end())
		return 0;

	AnimationFrame *aFrame = mFrameItr->get();
	return aFrame->mDelayTime;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735a90
void Animation::Draw(Graphics &g, int x, int y)
{
	if(mAccumulateImage.get()!=NULL)
		g.DrawImage(mAccumulateImage.get(),x,y);
	else
	{
		if(mFrameItr!=mFrameList.end())
		{
			AnimationFrame *aFrame = mFrameItr->get();
			aFrame->Draw(g,x,y);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00735ae0
SmartPtr<Animation> Animation::Duplicate()
{
	SmartPtr<Animation> anAnim = new Animation(mFrameList,mWidth,mHeight);
	for(AnimationFrameList::iterator anItr = anAnim->mFrameList.begin(); anItr!=anAnim->mFrameList.end(); ++anItr)
		*anItr = (*anItr)->Duplicate();

	anAnim->mNumLoops = mNumLoops;
	anAnim->mLoopCount = mLoopCount;
	anAnim->mNeedAccumulate = mNeedAccumulate;
	anAnim->mCopyFirstImageToAccumulate = mCopyFirstImageToAccumulate;
	if(mAccumulateImage.get()!=NULL)
		anAnim->mAccumulateImage = (NativeImage*)mAccumulateImage->Duplicate().get();

	return anAnim;
}
