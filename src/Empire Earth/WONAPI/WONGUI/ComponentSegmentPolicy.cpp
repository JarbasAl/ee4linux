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
// FUNCTION: WON 0x007098f0
void ComponentLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	bool selected = DrawSelection1(g,x,y,theInfo);

	mComponent->Virtual68();
	g.Translate(x,y);
	mComponent->Paint(g);
	g.Translate(-x,-y);
	if(selected)
		DrawSelection2(g,x,y);
}

// FUNCTION: WON 0x00709960
void ComponentLineSegment::SetViewport(ScrollAreaViewport *theViewport, int x, int y)
{
	LineSegment::SetViewport(theViewport,x,y);
	if(mComponent->ComponentFlagSet(0x100))
		mComponent->SetComponentFlags(0x100,false);

	mComponent->mParent=theViewport;	
	if(theViewport!=NULL)
	{
		mComponent->Fn2F4B70(x,y);
		mComponent->Virtual6C();	
	}
}
}
