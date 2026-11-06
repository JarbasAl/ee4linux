// Actual WON skin/control registration and its genuine file-static callbacks.
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
#include "ee/wonapi/WONLobbySkinListControls.h"
#include "ee/wonapi/WONLobbyWONInputBox.h"
#include "ee/wonapi/WONLobbyWONButtonConfig.h"
#include "ee/wonapi/WONLobbyWONFrameConfig.h"
#include "ee/wonapi/WONLobbyWONFrameValueConfig.h"
#include "ee/wonapi/WONLobbyWONSkinConfig.h"
#include "LostDebug.h"
using namespace WONAPI;
SmartPtr<WONSkin> gSkin;

// FUNCTION: WON 0x00796d70
static Component* PushButtonFactory() { return gSkin->GetPushButton(); }

// FUNCTION: WON 0x00796d80
static Component* RadioButtonFactory() { return gSkin->GetRadioButton(); }

// FUNCTION: WON 0x00796d90
static Component* CheckboxFactory() { return gSkin->GetCheckbox(); }

// FUNCTION: WON 0x00796da0
static Component* TabButtonFactory() { return gSkin->GetTabButton(); }

// FUNCTION: WON 0x00796db0
static Component* HeaderButtonFactory() { return gSkin->GetHeaderButton(); }

// FUNCTION: WON 0x00796dc0
static Component* CloseButtonFactory() { return gSkin->GetCloseButton(); }

// FUNCTION: WON 0x00796dd0
static Component* InputBoxFactory() { return gSkin->GetInputBox(); }

// FUNCTION: WON 0x00796de0
static Component* ComboBoxFactory() { return gSkin->GetComboBox(); }

// FUNCTION: WON 0x00796df0
static Component* ScrollContainerFactory() { return gSkin->GetScrollContainer(); }

// FUNCTION: WON 0x00796e00
static Component* ListCtrlFactory() { return gSkin->GetListCtrl(); }

// FUNCTION: WON 0x00796e10
static Component* TabCtrlFactory() { return gSkin->GetTabCtrl(); }

// FUNCTION: WON 0x00796e20
static Component* WONFrameComponentFactory() { return new WONFrameComponent; }

// FUNCTION: WON 0x00796e90
static Component* PopupBackFactory() { return gSkin->GetPopupBack(); }

// FUNCTION: WON 0x00796ea0
static Component* DialogBackFactory() { return gSkin->GetDialogBack(); }

// FUNCTION: WON 0x00796eb0
static Component* WONButtonSkinFactory() { return new WONButton; }

// FUNCTION: WON 0x00796f10
static Component* WONHeaderButtonSkinFactory() { return new WONHeaderButton; }

// FUNCTION: WON 0x00796f70
static Component* WONSimpleButtonSkinFactory() { return new WONSimpleButton; }

// FUNCTION: WON 0x00796fd0
static Component* WONScrollContainerSkinFactory() { return new WONScrollContainer; }

// FUNCTION: WON 0x00797030
static Component* WONListCtrlSkinFactory() { return new WONListCtrl; }

// FUNCTION: WON 0x00797090
static Component* WONTabCtrlSkinFactory() { return new WONTabCtrl(gSkin.get()); }

// FUNCTION: WON 0x00797100
static Component* WONComboBoxSkinFactory() { return new WONComboBox; }

// FUNCTION: WON 0x00797160
static Component* WONScrollbarSkinFactory() { return new WONScrollbar; }

namespace WONAPI {
// Genuine remaining SDK callbacks closing this existing registration table.
// FUNCTION: WON 0x00796b50
ComponentConfig* WONFrameComponentConfig::CfgFactory() { return new WONFrameComponentConfig; }
// FUNCTION: WON 0x00796bc0
ComponentConfig* WONScrollbarConfig::CfgFactory() { return new WONScrollbarConfig; }
// FUNCTION: WON 0x00796c20
ComponentConfig* WONScrollContainerConfig::CfgFactory() { return new WONScrollContainerConfig; }
// FUNCTION: WON 0x00796ca0
ComponentConfig* WONListCtrlConfig::CfgFactory() { return new WONListCtrlConfig; }
// FUNCTION: WON 0x00796d20
ComponentConfig* WONSkinConfig::CfgFactory() { return new WONSkinConfig; }

// Genuine SDK allocation callbacks closing this existing registration table.
// FUNCTION: WON 0x007969c0
ComponentConfig* WONSimpleButtonConfig::CfgFactory() { return new WONSimpleButtonConfig; }
// FUNCTION: WON 0x00796a30
ComponentConfig* WONButtonConfig::CfgFactory() { return new WONButtonConfig; }

// FUNCTION: WON 0x00795fa0
void WONControlsConfig_Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("WONSkin",WONSkinConfig::CfgFactory,ComponentConfig::CompFactory);
	theTable->AddAllocator("WONButtonSkin",WONButtonConfig::CfgFactory,WONButtonSkinFactory);
	theTable->AddAllocator("WONScrollbarSkin",WONScrollbarConfig::CfgFactory,WONScrollbarSkinFactory);
	theTable->AddAllocator("WONHeaderButtonSkin",WONButtonConfig::CfgFactory,WONHeaderButtonSkinFactory);
	theTable->AddAllocator("WONSimpleButtonSkin",WONSimpleButtonConfig::CfgFactory,WONSimpleButtonSkinFactory);
	theTable->AddAllocator("WONScrollContainerSkin",WONScrollContainerConfig::CfgFactory,WONScrollContainerSkinFactory);
	theTable->AddAllocator("WONListCtrlSkin",WONListCtrlConfig::CfgFactory,WONListCtrlSkinFactory);
	theTable->AddAllocator("WONTabCtrlSkin",WONTabCtrlConfig::CfgFactory,WONTabCtrlSkinFactory);
	theTable->AddAllocator("WONComboBoxSkin",WONComboBoxConfig::CfgFactory,WONComboBoxSkinFactory);

	theTable->AddAllocator("WONButton",WONButtonConfig::CfgFactory,PushButtonFactory);
	theTable->AddAllocator("WONCheckbox",WONButtonConfig::CfgFactory,CheckboxFactory);
	theTable->AddAllocator("WONRadioButton",WONButtonConfig::CfgFactory,RadioButtonFactory);
	theTable->AddAllocator("WONTabButton",WONButtonConfig::CfgFactory,TabButtonFactory);
	theTable->AddAllocator("WONHeaderButton",WONButtonConfig::CfgFactory,HeaderButtonFactory);
	theTable->AddAllocator("WONCloseButton",WONButtonConfig::CfgFactory,CloseButtonFactory);
	theTable->AddAllocator("WONPopupBack",WONFrameComponentConfig::CfgFactory,PopupBackFactory);
	theTable->AddAllocator("WONDialogBack",WONFrameComponentConfig::CfgFactory,DialogBackFactory);

	theTable->AddAllocator("WONInputBox",InputBoxConfig::CfgFactory,InputBoxFactory);
	theTable->AddAllocator("WONComboBox",WONComboBoxConfig::CfgFactory,ComboBoxFactory);
	theTable->AddAllocator("WONTabCtrl",WONTabCtrlConfig::CfgFactory,TabCtrlFactory);
	theTable->AddAllocator("WONScrollContainer",WONScrollContainerConfig::CfgFactory,ScrollContainerFactory);
	theTable->AddAllocator("WONListCtrl",WONListCtrlConfig::CfgFactory,ListCtrlFactory);
	theTable->AddAllocator("WONFrameComponent",WONFrameComponentConfig::CfgFactory,WONFrameComponentFactory);
	// LOST-DEBUG: none: exact only (one site after the last string temp, as WONGUIConfig_Init)
	EE_LOST_DEBUG(1);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00796af0
// (public: static class WONAPI::ComponentConfig * __cdecl WONAPI::WONTabCtrlConfig::CfgFactory(void)    [?CfgFactory@WONTabCtrlConfig@WONAPI@@SAPAVCompon)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00796c80
// (public: virtual void __thiscall WONAPI::WONScrollContainerConfig::Virtual34(class WONAPI::Component *)    [?Virtual34@WONScrollContainerConfig@WONAPI@)

// TEMPLATE: WON 0x00795f70
// TEMPLATE: WON 0x00795f80
// TEMPLATE: WON 0x00795f60
// TEMPLATE: WON 0x00795f90
// TEMPLATE: WON 0x00796a90
