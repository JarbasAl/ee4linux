// Scratch whole MOTD consumer. Stack HTMLDocumentGen14 must be GUI-owned/full typed before compile.
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
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyEvents.h"
namespace WONAPI {
// Default constructor124/member-only setup0; ordinary generator dtor150/support0.
MOTDCtrl::MOTDCtrl() : mNeedGenHTML(false) {}
// Original3c0be0..3c0cc7,231B. Own-vptr3c0bff then two strings/browser/base cleanup.
// FUNCTION: WON 0x007c0be0
MOTDCtrl::~MOTDCtrl() {}
// Original3c0cd0..3c0d13,67B.
// FUNCTION: WON 0x007c0cd0
void MOTDCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType==2) {
        SafeGetComponent(static_cast<ComponentConfigInit&>(inInit).mConfig,mBrowser,"Browser",__FILE__,36);
        mBrowser->SetLaunchBrowserOnLink(true);
        Fn2F5360(1);
    }
}
// Original3c0d20..3c0fae,654B. System first/game second; flags14 are native full int.
// FUNCTION: WON 0x007c0d20
void MOTDCtrl::Paint(Graphics& inGraphics)
{
    if(mNeedGenHTML) {
        mNeedGenHTML=false;
        if(!mSysMOTDPath.empty() && !mGameMOTDPath.empty()) {
            HTMLDocumentGen generator(NULL);
            generator.AddDocParam(GUIString(mSysMOTDPath),14);
            generator.AddDocParam(GUIString(mGameMOTDPath),14);
            mSysMOTDPath.erase();
            mGameMOTDPath.erase();
            generator.printf(GUIString("<body>%1%<br><br>%2%</body>"));
            mBrowser->Go(generator.GetDocument());
        }
    }
    Container::Paint(inGraphics);
}
// Original3c1050..3c127b,555B. Document type gate, not AsyncOp success/status gate.
// FUNCTION: WON 0x007c1050
void MOTDCtrl::ShowMessage(GetHTTPDocumentOp* inOp)
{
    if(inOp->GetDocType()!=0) return;
    mNeedGenHTML=true;
    mSysMOTDPath=inOp->GetDocumentPath(HTTPDocOwner_Sys);
    mGameMOTDPath=inOp->GetDocumentPath(HTTPDocOwner_Game);
    Invalidate();
}
// Original3c1280..3c12a2,34B.
// FUNCTION: WON 0x007c1280
void MOTDCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==100024) {
        ShowMessage(static_cast<GotHTTPDocumentEvent*>(inEvent)->mOp.get());
        return;
    }
    Component::HandleComponentEvent(inEvent);
}
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
    if(inPing==0xfffd) image=::LobbyGlobal_Searching_Image.get();
    else if(inPing==0xfffc) { image=::LobbyGlobal_Searching_Image; image=image.get(); }
    else if(inPing==0xffff) image=::ServerListCtrl_PingFailed_Image.get();
    else {
        bars=10-int((static_cast<unsigned long>(inPing)*10)/sLobbyPingCeiling);
        if(bars<1) bars=1;
        else if(bars>10) bars=10;
    }
    int x=0;
    if(inAlignment!=0) {
        int width;
        if(text.length()!=0) width=inGraphics->mFont->GetStringWidth(text,0,text.length());
        else if(image.get()!=NULL) width=image->GetWidth();
        else width=bars*3;
        if(inAlignment==1) x=(inWidth-width)/2;
        else x=inWidth-width;
        if(x<0) x=0;
    }
    if(text.length()!=0) inGraphics->DrawString(text,x,0);
    else if(image.get()!=NULL) {
        int y=(inHeight-image->GetHeight())/2;
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

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c0b40
// (public: __thiscall WONAPI::MOTDCtrl::MOTDCtrl(void)    [??0MOTDCtrl@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007c0bc0
// (public: virtual void * __thiscall WONAPI::MOTDCtrl::`scalar deleting destructor'(unsigned int)    [??_GMOTDCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007c0fb0
// (public: __thiscall WONAPI::HTMLDocumentGen::~HTMLDocumentGen(void))
// TEMPLATE: WON 0x0071b370
// (protected: void __thiscall std::vector<class WONAPI::SmartPtr<class WONAPI::HTMLDocument>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::HT)
