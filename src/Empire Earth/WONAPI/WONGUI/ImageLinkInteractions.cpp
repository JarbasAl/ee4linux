// Native retained-owner HTML segment interaction policy.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyHTMLImageSegments.h"
namespace WONAPI {
// FUNCTION: WON 0x0070af30
void ImageLinkLineSegment::LinkDown()
{
	mBorderColor = mDownColor;
	mTableContext->Invalidate();
}

// FUNCTION: WON 0x0070af50
void ImageLinkLineSegment::LinkUp()
{
	if(mBorderColor!=mNormalColor)
	{
		mBorderColor = mNormalColor;
		mTableContext->Invalidate();
	}
}

// FUNCTION: WON 0x0070af70
void ImageLinkLineSegment::MouseEnter(int x, int y)
{
	if(mImageMap.get()!=NULL)
	{
		mCurLinkParam = NULL;
		MouseMove(x,y);
		return;
	}

	mTableContext->Virtual78(Cursor::GetStandardCursor(StandardCursor_Hand));
	mTableContext->Fn2F53F0(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,mCurLinkParam.get()),false);
}

// FUNCTION: WON 0x0070b080
void ImageLinkLineSegment::MouseMove(int x, int y)
{
	if(mImageMap.get()==NULL)
		return;

	TextLinkParam *aParam = mImageMap->GetLink(x,y);
	if(aParam==NULL)
		aParam = mAnchorLinkParam.get();

	if(aParam!=mCurLinkParam.get())
	{
		mCurLinkParam = aParam;
		mTableContext->Virtual78(aParam!=NULL ? Cursor::GetStandardCursor(StandardCursor_Hand) : NULL);
		mTableContext->Fn2F53F0(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,mCurLinkParam.get()),false);
	}

}

// FUNCTION: WON 0x0070b1b0
void ImageLinkLineSegment::MouseDown(int x, int y, MouseButton theButton)
{
	if(mCurLinkParam.get()==NULL)
		return;

	if(theButton==MouseButton_Left)
		mCurLinkParam->LinkDown();
}

// FUNCTION: WON 0x0070b1d0
void ImageLinkLineSegment::MouseUp(int x, int y, MouseButton theButton)
{
	if(mCurLinkParam.get()==NULL)
		return;

	if(theButton==MouseButton_Left)
	{
		if(mCurLinkParam->LinkUp())
			mTableContext->Fn2F53F0(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkActivated,mCurLinkParam.get()),false);
	}
}

// FUNCTION: WON 0x0070b280
void ImageLinkLineSegment::MouseExit()
{
	if(mCurLinkParam.get()==NULL)
		return;

	mCurLinkParam->LinkUp();
	mTableContext->Fn2F53F0(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,NULL),false);
}
// SUPPORT: EXE 0x0070a7b0; genuine class-specific false default, shared physical native provider.
bool ImageLinkLineSegment::CanStartSelection() { return false; }
}
