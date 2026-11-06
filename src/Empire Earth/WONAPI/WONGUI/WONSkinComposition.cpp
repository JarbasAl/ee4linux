// Actual skin widget composition using original duplicate providers and real owners.
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
#include "ee/wonapi/WONLobbyWONListConfig.h"
#include "ee/wonapi/WONLobbySkinnedContainers.h"
#include "ee/wonapi/WONLobbySkinTab.h"
namespace WONAPI {
// FUNCTION: WON 0x007156a0
WONScrollbar* WONSkin::GetScrollbar(bool vertical)
{
    WONScrollbar* bar=new WONScrollbar(vertical);
    bar->CopyAttributes(mScrollbar[vertical?1:0].get());
    bar->mThumb=GetScrollThumb(vertical);
    bar->mScrollRect=GetScrollGutter(vertical);
    if(vertical) {
        bar->mUpArrow=GetArrowButton(Direction_Up);
        bar->mDownArrow=GetArrowButton(Direction_Down);
        bar->Fn2F4B90(bar->mUpArrow->Width(),bar->Height());
    } else {
        bar->mUpArrow=GetArrowButton(Direction_Left);
        bar->mDownArrow=GetArrowButton(Direction_Right);
        bar->Fn2F4B90(bar->Width(),bar->mUpArrow->Height());
    }
    return bar;
}
// FUNCTION: WON 0x007158a0
WONComboBox* WONSkin::GetComboBox()
{
    WONComboBox* combo=mComboBox->Duplicate();
    Button* arrow=mComboButton->Duplicate();
    arrow->Fn2F4E90(0,0); //actual old-version SetDesiredSize call
    arrow->Fn2F3370(2,false);
    WONScrollContainer* list=GetScrollContainer();
    list->SetScrollbarConditions(ScrollbarCondition_Disable,ScrollbarCondition_Conditional);
    list->SetFrame(WONFrame(new WONMSSkin::MSImage(WONMSSkin::MSImageType_ComboPopupFrame),0));
    combo->mButton=arrow; //genuine SDK inline ComboBox::SetButton
    combo->SetListBox(list);
    return combo;
}
// FUNCTION: WON 0x00715ae0
WONScrollContainer* WONSkin::GetScrollContainer()
{
    WONScrollContainer* container=mScrollContainer->Duplicate();
    container->SetScrollbar(GetScrollbar(true));
    container->SetScrollbar(GetScrollbar(false));
    return container;
}
// FUNCTION: WON 0x00715c10
WONTabCtrl* WONSkin::GetTabCtrl()
{
    WONTabCtrl* tab=mTabCtrl->Duplicate();
    tab->mSkin=this; //genuine SDK inline SetSkin
    tab->SetTabArrows(GetArrowButton(Direction_Left),GetArrowButton(Direction_Right));
    return tab;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00728770
// (public: virtual void * __thiscall WONAPI::WONMSSkin::MSImage::`scalar deleting destructor'(unsigned int)    [??_GMSImage@WONMSSkin@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007159f0
// (public: virtual void __thiscall WONAPI::ProgramImage::SetSize(int,int))
