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
int MSCheckbox::mCheckboxWidth=13;
int MSCheckbox::mCheckboxHeight=13;
MSCheckbox::CheckboxPainterFunc MSCheckbox::CheckboxPainter=MSCheckbox::DefaultCheckboxPainterFunc;
// FUNCTION: WON 0x6ff430
void MSCheckbox::DefaultCheckboxPainterFunc(Graphics& g,MSCheckbox* button) {
    MSGraphicsUtil::DrawCheck(g,0,0,mCheckboxWidth,mCheckboxHeight,(button->mButtonFlags&0x20)!=0,(button->mButtonFlags&4)!=0);
}
// FUNCTION: WON 0x6ff470
void MSCheckbox::Paint(Graphics& g) {
    Component::Paint(g);
    g.SetSysColor(StandardColor_3DFace);
    if(!mIsTransparent) g.FillRect(0,0,Width(),Height());
    int height=mCheckboxHeight;
    int y=0;
    if(mVAlign==VertAlign_Center) y=(Height()-height-mTopTextPad-mBottomTextPad)/2;
    else if(mVAlign==VertAlign_Bottom) y=Height()-height-mTopTextPad-mBottomTextPad;
    g.Translate(0,mTopTextPad+y);
    CheckboxPainter(g,this);
    g.Translate(0,-y-mTopTextPad);
    DrawButtonText(g,0,0);
}
