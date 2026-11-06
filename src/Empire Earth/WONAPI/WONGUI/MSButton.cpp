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

// FUNCTION: WON 0x006fed20
void MSButton::Paint(Graphics& g) {
    Component::Paint(g);
    bool down=(mButtonFlags&0x20)!=0;
    int offset=down?1:0;
    g.SetSysColor(StandardColor_3DFace);
    if(!mIsTransparent) g.FillRect(0,0,Width(),Height());
    int x=0,y=0,width=Width(),height=Height();
    bool frame2=mUseFrame2;
    if(IsDefaultButton()) {
        g.SetSysColor(StandardColor_3DDarkShadow);
        g.DrawRect(0,0,Width(),Height());
        x++;y++;width-=2;height-=2;
        if(down) frame2=true;
    }
    if(frame2) LobbyDrawFrame(g,x,y,width,height,down);
    else LobbyDraw3DFrame(g,x,y,width,height,down);
    DrawButtonText(g,offset,offset);
}
