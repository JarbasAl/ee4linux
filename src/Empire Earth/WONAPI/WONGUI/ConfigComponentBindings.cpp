// Native predefined child configuration dispatch; no runtime authority.
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
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00782d50
void ComponentConfig::Virtual34(Component* component)
{
    mComponent=component;
}

// SUPPORT-ONLY: EXE 0x0078f8d0 (22B native shared SetComponent contract; zero primary)
void ButtonConfig::Virtual34(Component* component)
{
    ComponentConfig::Virtual34(component);
    mButton=(Button*)component;
}

// SUPPORT-ONLY: EXE 0x0078f8d0 (22B native shared SetComponent contract; zero primary)
void ContainerConfig::Virtual34(Component* component)
{
    ComponentConfig::Virtual34(component);
    mContainer=(Container*)component;
}

// SUPPORT-ONLY: EXE 0x0078f960 (25B native shared SetComponent contract; zero primary)
void DialogConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mDialog=(Dialog*)component;
}

// SUPPORT-ONLY: EXE 0x0078f8d0 (22B native shared SetComponent contract; zero primary)
void InputBoxConfig::Virtual34(Component* component)
{
    ComponentConfig::Virtual34(component);
    mInputBox=(InputBox*)component;
}

// SUPPORT-ONLY: EXE 0x0078f8d0 (22B native shared SetComponent contract; zero primary)
void LabelConfig::Virtual34(Component* component)
{
    ComponentConfig::Virtual34(component);
    mLabel=(Label*)component;
}

// SUPPORT-ONLY: EXE 0x0078f960 (25B native shared SetComponent contract; zero primary)
void ScrollAreaConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mScrollArea=(ScrollArea*)component;
}

// SUPPORT-ONLY: EXE 0x0078f860 (25B native shared SetComponent contract; zero primary)
void ListAreaConfig::Virtual34(Component* component)
{
    ScrollAreaConfig::Virtual34(component);
    mListArea=(ListArea*)component;
}

// FUNCTION: WON 0x0078fd10
void MultiListAreaConfig::Virtual34(Component* component)
{
    ListAreaConfig::Virtual34(component);
    mMultiListArea=(MultiListArea*)component;
}

// SUPPORT-ONLY: EXE 0x0078f960 (25B native shared SetComponent contract; zero primary)
void ScrollbarConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mScrollbar=(Scrollbar*)component;
}

// FUNCTION: WON 0x0079b5f0
void TreeAreaConfig::Virtual34(Component* component)
{
    mTreeArea=(TreeArea*)component;
    ListAreaConfig::Virtual34(component);
}

// SUPPORT-ONLY: EXE 0x0078f960 (25B native shared SetComponent contract; zero primary)
void ComboBoxConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mComboBox=(ComboBox*)component;
}

// FUNCTION: WON 0x0078e7d0
void MSComboBoxConfig::Virtual34(Component* component)
{
    mMSComboBox=(MSComboBox*)component;
    ComboBoxConfig::Virtual34(component);
}

// SUPPORT-ONLY: EXE 0x0078f860 (25B native shared SetComponent contract; zero primary)
void MSScrollbarConfig::Virtual34(Component* component)
{
    ScrollbarConfig::Virtual34(component);
    mMSScrollbar=(MSScrollbar*)component;
}

// SUPPORT-ONLY: EXE 0x0078f960 (25B native shared SetComponent contract; zero primary)
void MSEtchedFrameGroupConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mFrame=(MSEtchedFrameGroup*)component;
}

// FUNCTION: WON 0x0078f100
void MSTabCtrlConfig::Virtual34(Component* component)
{
    TabCtrlConfig::Virtual34(component);
    mMSTabCtrl=(MSTabCtrl*)component;
}

// FUNCTION: WON 0x0078fd10
void MSScrollContainerConfig::Virtual34(Component* component)
{
    ScrollbarScrollerConfig::Virtual34(component);
    mScrollContainer=(MSScrollContainer*)component;
}

// FUNCTION: WON 0x0078f2a0
void MSHeaderControlConfig::Virtual34(Component* component)
{
    mMSHeader=(MSHeaderControl*)component;
    MultiListHeaderConfig::Virtual34(component);
}

// FUNCTION: WON 0x0078f380
void MSListCtrlConfig::Virtual34(Component* component)
{
    mListCtrl=(MSListCtrl*)component;
    MSScrollContainerConfig::Virtual34(component);
}

// FUNCTION: WON 0x0078f100
void WONScrollbarConfig::Virtual34(Component* component)
{
    ScrollbarConfig::Virtual34(component);
    mScrollbar=(WONScrollbar*)component;
}

// FUNCTION: WON 0x00798f70
void WONHeaderControlConfig::Virtual34(Component* component)
{
    MultiListHeaderConfig::Virtual34(component);
    mHeader=(WONHeaderControl*)component;
}

// FUNCTION: WON 0x00796d00
void WONListCtrlConfig::Virtual34(Component* component)
{
    WONScrollContainerConfig::Virtual34(component);
    mListCtrl=(WONListCtrl*)component;
}

}
