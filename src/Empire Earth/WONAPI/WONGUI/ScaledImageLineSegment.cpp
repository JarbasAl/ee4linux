// Genuine SDK clipped image-tiling segment, SOURCE_ONLY_NO_NATIVE_ID.
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
void ScaledImageLineSegment::Paint(Graphics &g, int x, int y, ElementPaintInfo &theInfo)
{
	g.PushClipRect(x,y,mWidth,mHeight);
	x+=mHSpace; y+=mVSpace;
	for(int w=0; w<mWidth; w+=mImage->mWidth)
		for(int h=0; h<mHeight; h+=mImage->mHeight)
			g.DrawImage(mImage.get(),x+w,y+h);

	g.PopClipRect();
}
}
