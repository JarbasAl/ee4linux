// Original-backed substantial ping renderer; actual normal object/data review pending.
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
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {
// Actual mutable DWORD4df4cc, initialized5000; native only refreshes when still5000.
// Data ownership/binding must be independently reviewed before promotion.
static unsigned long sLobbyPingCeiling=5000;
// Original3c12b0..3c14d2,546B. Five cdecl args; alignment/width are full int.
// FUNCTION: WON 0x007c12b0
void DrawLobbyPing(Graphics* inGraphics,unsigned short inPing,int inHeight,int inAlignment,int inWidth)
{
    if(inPing==0xfffe) return;
    if(sLobbyPingCeiling==5000) {
        LobbyConfig* config=GetLobbyConfig();
        if(config!=NULL) sLobbyPingCeiling=config->mPingCeiling;
    }
    GUIString text(GetEmptyGUIStringData());
    SmartPtr<Image> image;
    int bars=0;
    if(inPing==0xfffd || inPing==0xfffc) image=::LobbyGlobal_Searching_Image;
    else if(inPing==0xffff) image=::ServerListCtrl_PingFailed_Image;
    else {
        bars=10-int((static_cast<unsigned long>(inPing)*10)/sLobbyPingCeiling);
        if(bars<1) bars=1;
        else if(bars>10) bars=10;
    }
    int x=0;
    if(inAlignment!=0) {
        int width;
        if(text.length()!=0) width=inGraphics->mFont->GetStringWidth(text,0,text.length());
        else if(image.get()!=NULL) width=image->mWidth;
        else width=bars*3;
        if(inAlignment==1) x=(inWidth-width)/2;
        else x=inWidth-width;
        if(x<0) x=0;
    }
    if(text.length()!=0) inGraphics->DrawString(text,x,0);
    else if(image.get()!=NULL) {
        int y=(inHeight-image->mHeight)/2;
        if(y<0) y=0;
        inGraphics->DrawImage(image.get(),x,y);
    } else {
        inGraphics->SetColor(0x10d810);
        int height=inHeight-2;
        while(bars>0) {
            inGraphics->FillRect(x,1,2,height);
            x+=3;
            --bars;
        }
    }
}
}
