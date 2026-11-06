// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK algorithms, no Native/primary/physical claim.
ComponentScrollArea::ComponentScrollArea(Component *theComponent)
{
	mComponent = theComponent;
	GetViewport()->Fn2F6A60(mComponent.get());
	mVertCenter = true;
	mHorzCenter = true;
	UpdateComponentSize();
}

void ComponentScrollArea::Virtual08()
{
	ScrollArea::Virtual08();
	PositionComponent();
}

void ComponentScrollArea::PositionComponent()
{
	int x = 0, y = 0;
	if(mHorzCenter)
	{
		if(mComponent->Width() < mScrollArea.width)
			x = (mScrollArea.width - mComponent->Width())/2;
	}

	if(mVertCenter)
	{
		if(mComponent->Height() < mScrollArea.height)
			y = (mScrollArea.height - mComponent->Height())/2;
	}

	mComponent->Fn2F4B70(x,y);
	Invalidate();
}

void ComponentScrollArea::SetComponentSize(int theWidth, int theHeight)
{
	SetScrollDimension(theWidth,theHeight);
	mComponent->Fn2F4B90(theWidth,theHeight);
	PositionComponent();
}

void ComponentScrollArea::UpdateComponentSize()
{
	SetComponentSize(mComponent->Width(), mComponent->Height());
}
}
