// Genuine complete HTML image-segment ownership and dimension queries, support only.
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
// FUNCTION: WON 0x0073aa30
ImageLineSegment::ImageLineSegment(Image* image)
    :mImage(image),mHSpace(5),mVSpace(0),mBorder(0),mBorderColor(0),mAlign(0) {}
// FUNCTION: WON 0x0073aa80
int ImageLineSegment::GetWidth() { return mImage->mWidth+2*mHSpace+2*mBorder; }
// FUNCTION: WON 0x0073aaa0
int ImageLineSegment::GetHeight() { return mImage->mHeight+2*mVSpace+2*mBorder; }
// FUNCTION: WON 0x0073aac0
char ImageLineSegment::GetVertAlign() { return mAlign; }
}

// FUNCTION: WON 0x0073aad0
// (??_GImageLineSegment@WONAPI@@UAEPAXI@Z)
