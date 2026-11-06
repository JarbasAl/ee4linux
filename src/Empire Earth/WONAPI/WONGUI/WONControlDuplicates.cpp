
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
#include "ee/wonapi/WONLobbyMSSkinImage.h"
#include "ee/wonapi/WONLobbySkinnedContainers.h"
#include "ee/wonapi/WONLobbySkinTab.h"
namespace WONAPI {
// FUNCTION: WON 0x00717ae0
WONSimpleButton* WONSimpleButton::Duplicate()
{
    WONSimpleButton* button=new WONSimpleButton;
    for(int i=0;i<4;++i) button->mImages[i]=mImages[i];
    button->mScaleImage=mScaleImage;
    button->mCheckMouseButtonDown=mCheckMouseButtonDown;
    button->CopyAttributes(this);
    button->Fn2F4B90(Width(),Height());
    return button;
}
// FUNCTION: WON 0x007199e0
WONComboBox* WONComboBox::Duplicate()
{
    WONComboBox* combo=new WONComboBox;
    combo->SetFrame(mFrame);
    combo->mDrawFrame=mDrawFrame;
    combo->mButtonInFrame=mButtonInFrame;
    return combo;
}
// FUNCTION: WON 0x00719f00
WONScrollContainer* WONScrollContainer::Duplicate()
{
    WONScrollContainer* container=new WONScrollContainer;
    container->CopyAttributes(this);
    return container;
}
// FUNCTION: WON 0x0071ad20
WONTabCtrl* WONTabCtrl::Duplicate()
{
    WONTabCtrl* tab=new WONTabCtrl(mSkin.get());
    tab->CopyAttributes(this);
    tab->SetTransparent(mIsTransparent);
    tab->SetFrame(mFrame);
    return tab;
}
}
