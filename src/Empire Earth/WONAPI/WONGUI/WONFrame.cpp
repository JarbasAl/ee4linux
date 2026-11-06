// Original skin frame and simple image button behavior on borrowed owners.
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
#include "ee/wonapi/WONLobbySkinFrame.h"
namespace WONAPI {

// FUNCTION: WON 0x007177C0
void WONFrame::Copy(const WONFrame &from)
{
	if(from.mImage.get()==NULL)
		mImage = NULL;
	else
		mImage = from.mImage->Duplicate();

	mLeft = from.mLeft;
	mTop = from.mTop;
	mRight = from.mRight;
	mBottom = from.mBottom;
}

// FUNCTION: WON 0x007178C0
void WONFrame::Paint(Graphics &g, Component *theComponent)
{
	if(mImage.get()==NULL)
		return;

	if(mImage->mWidth!=theComponent->Width() || mImage->mHeight!=theComponent->Height())
		mImage->SetSize(theComponent->Width(),theComponent->Height());

	g.DrawImage(mImage.get(),0,0);
}

// FUNCTION: WON 0x00717910
void WONFrame::Paint(Graphics &g, Component *theComponent, int lp, int tp, int rp, int bp)
{
	if(mImage.get()==NULL)
		return;

	if(mImage->mWidth!=theComponent->Width()-lp-rp || mImage->mHeight!=theComponent->Height()-tp-bp)
		mImage->SetSize(theComponent->Width()-lp-rp,theComponent->Height()-tp-bp);

	g.DrawImage(mImage.get(),lp,tp);
}
}

namespace WONAPI {
// FUNCTION: WON 0x0071a7c0
void WONFrameComponent::VirtualAC(Image* image,int left,int top,int right,int bottom) { mFrame.SetImage(image,left,top,right,bottom); }
// FUNCTION: WON 0x0071a7b0
void WONFrameComponent::VirtualB0(Image* image) { mFrame.SetImage(image); }
// FUNCTION: WON 0x0071a7f0
void WONFrameComponent::VirtualB4(int left,int top,int right,int bottom) { mFrame.mLeft=left; mFrame.mTop=top; mFrame.mRight=right; mFrame.mBottom=bottom; }
// FUNCTION: WON 0x0071a630
void WONFrameComponent::VirtualB8(int value) { mFrame.mLeft=value; }
// FUNCTION: WON 0x0071a640
void WONFrameComponent::VirtualBC(int value) { mFrame.mTop=value; }
// FUNCTION: WON 0x0071a650
void WONFrameComponent::VirtualC0(int value) { mFrame.mRight=value; }
// FUNCTION: WON 0x0071a660
void WONFrameComponent::VirtualC4(int value) { mFrame.mBottom=value; }
// SUPPORT: EXE 0x0071a670; genuine SDK frame component behavior,4B.
int WONFrameComponent::VirtualC8() { return mFrame.mLeft; }
// FUNCTION: WON 0x0071a680
int WONFrameComponent::VirtualCC() { return mFrame.mTop; }
// FUNCTION: WON 0x0071a690
int WONFrameComponent::VirtualD0() { return mFrame.mRight; }
// FUNCTION: WON 0x0071a6a0
int WONFrameComponent::VirtualD4() { return mFrame.mBottom; }
// FUNCTION: WON 0x0071a810
void WONFrameComponent::Paint(Graphics& g) { Component::Paint(g); mFrame.Paint(g,this); }
// Genuine SDK image assignment API; native shared349ab0 already mapped StretchImage, zero new physical credit.
// FUNCTION: WON 0x00717730
void WONFrame::SetImage(Image* image) { mImage=image; }
// SUPPORT: EXE 0x00717730; genuine SDK image and borders assignment, native102B.
void WONFrame::SetImage(Image* image,int left,int top,int right,int bottom)
{
    mImage=image; mLeft=left; mTop=top; mRight=right; mBottom=bottom;
}
}
