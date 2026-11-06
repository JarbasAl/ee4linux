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
            generator.AddDocParam(GUIString(new GUIStringData(mSysMOTDPath)),14);
            generator.AddDocParam(GUIString(new GUIStringData(mGameMOTDPath)),14);
            mSysMOTDPath.erase();
            mGameMOTDPath.erase();
            generator.printf(GUIString(new GUIStringData("<body>%1%<br><br>%2%</body>")));
            mBrowser->Go(generator.GetDocument());
        }
    }
    Container::Paint(inGraphics);
}
// Original3c1050..3c127b,555B. Document type gate, not AsyncOp success/status gate.
// FUNCTION: WON 0x007c1050
void MOTDCtrl::ShowMessage(GetHTTPDocumentOp* inOp)
{
    if(inOp->mDocumentType!=0) return;
    mNeedGenHTML=true;
    mSysMOTDPath=inOp->mDocuments[0];
    mGameMOTDPath=inOp->mDocuments[1];
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
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c0b40
// (public: __thiscall WONAPI::MOTDCtrl::MOTDCtrl(void)    [??0MOTDCtrl@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007c0bc0
// (public: virtual void * __thiscall WONAPI::MOTDCtrl::`scalar deleting destructor'(unsigned int)    [??_GMOTDCtrl@WONAPI@@UAEPAXI@Z])
