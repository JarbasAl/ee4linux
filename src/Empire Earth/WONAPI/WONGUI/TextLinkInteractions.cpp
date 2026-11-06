// Actual text hyperlink interaction and original retained event ownership.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextLinkSegment.h"
namespace WONAPI {
// FUNCTION: WON 0x0070abc0
void TextLinkLineSegment::LinkDown()
{
	mColor = mDownColor;
	mTableContext->Invalidate();
}


// FUNCTION: WON 0x0070abe0
void TextLinkLineSegment::LinkUp()
{
	if(mColor!=mNormalColor)
	{
		mColor = mNormalColor;
		mTableContext->Invalidate();
	}
}


// FUNCTION: WON 0x70ac00
void TextLinkLineSegment::MouseEnter(int x, int y)
{
	mTableContext->Virtual78(Cursor::GetStandardCursor(StandardCursor_Hand));
	mTableContext->Fn2F53F0(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,mLinkParam.get()),false);
}


// FUNCTION: WON 0x0070ad70
void TextLinkLineSegment::MouseDown(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left)
		mLinkParam->LinkDown();
}


// FUNCTION: WON 0x70ad90
void TextLinkLineSegment::MouseUp(int x, int y, MouseButton theButton)
{
	if(theButton==MouseButton_Left)
	{
		if(mLinkParam->LinkUp())
			mTableContext->Fn2F53F0(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkActivated,mLinkParam.get()),false);
	}
}


// FUNCTION: WON 0x70ae40
void TextLinkLineSegment::MouseExit()
{
	mLinkParam->LinkUp();
	mTableContext->Fn2F53F0(new HyperLinkEvent(mTableContext,ComponentEvent_HyperLinkOver,NULL),false);
}

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0070acc0
// (public: virtual void * __thiscall WONAPI::HyperLinkEvent::`scalar deleting destructor'(unsigned int)    [??_GHyperLinkEvent@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0070ace0
// (public: virtual __thiscall WONAPI::HyperLinkEvent::~HyperLinkEvent(void)    [??1HyperLinkEvent@WONAPI@@UAE@XZ])
