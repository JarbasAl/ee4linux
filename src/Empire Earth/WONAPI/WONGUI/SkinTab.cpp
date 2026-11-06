// Skinned tab frame ownership and standard tab setup.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbySkinTab.h"
#include "ee/wonapi/WONLobbyMSPainters.h"
namespace WONAPI {
// FUNCTION: WON 0x0071AAA0
void WONTabCtrl::SetFrame(const WONFrame &theFrame)
{
    mFrame = theFrame;
    SmartPtr<ImageComponent> aFrame = new ImageComponent;
    aFrame->mStretch = true;
    aFrame->VirtualB0(mFrame.mImage.get());
    // CLI_SameLeft | CLI_SameTop | CLI_SameSize = 0x5003.
    Fn2F6140(aFrame.get(),0x5003,mContainerLayout.get(),0,0,0,0,0);
    Fn2F6A60(aFrame.get());
    if(!mIsTransparent)
    {
        mBack = new MSDlgBack;
        Fn2F6140(mBack.get(),0x5003,aFrame.get(),mFrame.mLeft,mFrame.mTop,
            -mFrame.mRight-mFrame.mLeft,-mFrame.mTop-mFrame.mBottom,0);
        Fn2F6A60(mBack.get());
    }
}
// Support0: conditional RemoveChild forwarding, native31ac50/30; does not store flag.
// FUNCTION: WON 0x0071ac50
void WONTabCtrl::SetTransparent(bool isTransparent)
{
    if(mBack.get()!=NULL && isTransparent)
        VirtualC4(mBack.get());
}
// FUNCTION: WON 0x0071AC70
void WONTabCtrl::AddStandardTab(const GUIString &theText,Container *theContainer)
{
    SmartPtr<WONButton> aButton = mSkin->GetTabButton();
    aButton->SetText(theText,true);
    aButton->SetDesiredWidth(0);
    AddTab(aButton.get(),theContainer);
}
}
