// TOUDialog original-backed source intent; pending GUI reference/HTML generator contracts.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
// Simple false-state/default-owning initialization support, zero breadth quota.
TOUDialog::TOUDialog() : mNeedGenHTML(false) {}
// Actual ordinary destructor implicit: owned widgets followed by Dialog cleanup, zero quota.
// Shared13B delegating Paint support, zero substantive credit.
void TOUDialog::Paint(Graphics& inGraphics) { Container::Paint(inGraphics); }
// FUNCTION: WON 0x7f9910
void TOUDialog::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(aConfig,mBrowser,"Browser",__FILE__,30);
    SafeGetComponent(aConfig,mAgreeCheck,"AgreeCheck",__FILE__,31);
    SafeGetComponent(aConfig,mAcceptButton,"AcceptButton",__FILE__,32);
    SafeGetComponent(aConfig,mRejectButton,"RejectButton",__FILE__,33);
    mAcceptButton->mControlId=1;
    mRejectButton->mControlId=-1;
    Reset();
    Fn2F5360(1);
}
// FUNCTION: WON 0x7f99c0
void TOUDialog::Reset()
{
    if(mNeedGenHTML) {
        mNeedGenHTML=false;
        HTMLDocumentGen aGenerator(NULL);
        aGenerator.AddDocParam(GUIString(new GUIStringData(LobbyFn35B630())),6);
        aGenerator.AddDocParam(GUIString(new GUIStringData(LobbyFn35B650())),6);
        aGenerator.printf(GUIString(new GUIStringData("<body>%1%<br><br>%2%</body>")));
        mBrowser->Go(aGenerator.GetDocument());
    }
    mBrowser->GetHTMLView()->SetOffsets(0,0);
    mAgreeCheck->SetCheck(false,false);
    mAcceptButton->SetEnabled(false);
}
// FUNCTION: WON 0x7f9b90
void TOUDialog::HandleHTTPDocEvent(GotHTTPDocumentEvent* inEvent)
{
    if(inEvent->mOp->mDocumentType==HTTPDocType_TOU) mNeedGenHTML=true;
}
// FUNCTION: WON 0x7f9bb0
void TOUDialog::HandleComponentEvent(ComponentEvent* inEvent)
{
    if(inEvent->mType==100024) {
        HandleHTTPDocEvent(static_cast<GotHTTPDocumentEvent*>(inEvent));
        return;
    }
    if(inEvent->mType==15 && inEvent->mComponent==mAgreeCheck.get()) {
        mAcceptButton->SetEnabled(mAgreeCheck->mChecked!=0);
        return;
    }
    Dialog::HandleComponentEvent(inEvent);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f97e0
// (public: virtual void * __thiscall WONAPI::TOUDialog::`scalar deleting destructor'(unsigned int)    [??_GTOUDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f9800
// (public: virtual __thiscall WONAPI::TOUDialog::~TOUDialog(void)    [??1TOUDialog@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f97a0
// (public: __thiscall WONAPI::TOUDialog::TOUDialog(void)    [??0TOUDialog@WONAPI@@QAE@XZ])
