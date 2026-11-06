// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;
// Actual native initialized dimensions/painter globals; no sourcequota for storage.
int MSRadioButton::mRadioWidth=13;
int MSRadioButton::mRadioHeight=13;
MSRadioButton::RadioPainterFunc MSRadioButton::RadioPainter=MSRadioButton::DefaultRadioPainterFunc;
// FUNCTION: WON 0x6ff5c0
void MSRadioButton::DefaultRadioPainterFunc(Graphics& g,MSRadioButton* button) {
    MSGraphicsUtil::DrawRadio(g,0,0,mRadioWidth,mRadioHeight,(button->mButtonFlags&0x20)!=0,(button->mButtonFlags&4)!=0);
}
// FUNCTION: WON 0x6ff600
void MSRadioButton::Paint(Graphics& g) {
    Component::Paint(g);
    g.SetSysColor(StandardColor_3DFace);
    if(!mIsTransparent) g.FillRect(0,0,Width(),Height());
    int height=mRadioHeight;
    int y=0;
    if(mVAlign==VertAlign_Center) y=(Height()-height-mTopTextPad-mBottomTextPad)/2;
    else if(mVAlign==VertAlign_Bottom) y=Height()-height-mTopTextPad-mBottomTextPad;
    g.Translate(0,mTopTextPad+y);
    RadioPainter(g,this);
    g.Translate(0,-y-mTopTextPad);
    DrawButtonText(g,0,0);
}
