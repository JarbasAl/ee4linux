
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
#include "ee/wonapi/WONLobbyMSConfig.h"
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyMSBorderPainters.h"
#include "LostDebug.h"
// Private dependency draft: genuine static declarations and missing config
// class contracts must be supplied by Root before canonical use.
namespace WONAPI {
// Genuine SDK component allocation providers for this existing MS registration table.
// FUNCTION: WON 0x0078e480
Component* MSButtonConfig::ButtonFactory() { return new MSButton; }
// FUNCTION: WON 0x0078e540
Component* MSButtonConfig::RadioFactory() { return new MSRadioButton; }
// FUNCTION: WON 0x0078e660
Component* MSInputBoxConfig::CompFactory() { return new MSInputBox; }
// FUNCTION: WON 0x0078e710
Component* MSComboBoxConfig::CompFactory() { return new MSComboBox; }
// FUNCTION: WON 0x0078e7f0
Component* MSArrowButtonConfig::CompFactory() { return new MSArrowButton; }
// FUNCTION: WON 0x0078ec00
Component* MSScrollbarConfig::CompFactory() { return new MSScrollbar; }
// FUNCTION: WON 0x0078ed70
Component* MSDlgBackConfig::CompFactory() { return new MSDlgBack; }
// FUNCTION: WON 0x0078ee10
Component* MS3DFrameConfig::CompFactory() { return new MS3DFrame; }
// FUNCTION: WON 0x0078f040
Component* MSTabCtrlConfig::CompFactory() { return new MSTabCtrl; }
// FUNCTION: WON 0x0078f120
Component* MSScrollContainerConfig::CompFactory() { return new MSScrollContainer; }

// Genuine remaining SDK callbacks closing this existing registration table.
// FUNCTION: WON 0x0078efe0
ComponentConfig* MSEtchedFrameGroupConfig::CfgFactory() { return new MSEtchedFrameGroupConfig; }
// FUNCTION: WON 0x0078f180
ComponentConfig* MSScrollContainerConfig::CfgFactory() { return new MSScrollContainerConfig; }
// FUNCTION: WON 0x0078f240
ComponentConfig* MSHeaderControlConfig::CfgFactory() { return new MSHeaderControlConfig; }
// FUNCTION: WON 0x0078f320
ComponentConfig* MSListCtrlConfig::CfgFactory() { return new MSListCtrlConfig; }

// Genuine SDK allocation callbacks closing this existing registration table.
// FUNCTION: WON 0x0078e6c0
ComponentConfig* MSInputBoxConfig::CfgFactory() { return new MSInputBoxConfig; }
// FUNCTION: WON 0x0078ec60
ComponentConfig* MSScrollbarConfig::CfgFactory() { return new MSScrollbarConfig; }
// FUNCTION: WON 0x0078e850
ComponentConfig* MSArrowButtonConfig::CfgFactory() { return new MSArrowButtonConfig; }
// FUNCTION: WON 0x0078e910
ComponentConfig* MSCloseButtonConfig::CfgFactory() { return new MSCloseButtonConfig; }
// FUNCTION: WON 0x0078ed20
ComponentConfig* MSSeperatorConfig::CfgFactory() { return new MSSeperatorConfig; }
// FUNCTION: WON 0x0078edc0
ComponentConfig* MSDlgBackConfig::CfgFactory() { return new MSDlgBackConfig; }
// FUNCTION: WON 0x0078ee70
ComponentConfig* MS3DFrameConfig::CfgFactory() { return new MS3DFrameConfig; }
// FUNCTION: WON 0x0078ef20
ComponentConfig* MSEtchedFrameConfig::CfgFactory() { return new MSEtchedFrameConfig; }

// FUNCTION: WON 0x0078dcb0
void MSComponentConfig_Init(ResourceConfigTable *theTable)
{
	theTable->AddAllocator("MSButton",MSButtonConfig::CfgFactory,MSButtonConfig::ButtonFactory);
	theTable->AddAllocator("MSCheckbox",MSButtonConfig::CfgFactory,MSButtonConfig::CheckboxFactory);
	theTable->AddAllocator("MSRadioButton",MSButtonConfig::CfgFactory,MSButtonConfig::RadioFactory);
	theTable->AddAllocator("MSLabel",MSButtonConfig::CfgFactory,MSButtonConfig::LabelFactory);
	theTable->AddAllocator("MSInputBox",MSInputBoxConfig::CfgFactory,MSInputBoxConfig::CompFactory);
	theTable->AddAllocator("MSComboBox",MSComboBoxConfig::CfgFactory,MSComboBoxConfig::CompFactory);
	theTable->AddAllocator("MSArrowButton",MSArrowButtonConfig::CfgFactory,MSArrowButtonConfig::CompFactory);
	theTable->AddAllocator("MSCloseButton",MSCloseButtonConfig::CfgFactory,MSCloseButtonConfig::CompFactory);
	theTable->AddAllocator("MSTabButton",MSTabButtonConfig::CfgFactory,MSTabButtonConfig::CompFactory);
	theTable->AddAllocator("MSHeaderButton",MSHeaderButtonConfig::CfgFactory,MSHeaderButtonConfig::CompFactory);
	theTable->AddAllocator("MSScrollbar",MSScrollbarConfig::CfgFactory,MSScrollbarConfig::CompFactory);
	theTable->AddAllocator("MSScrollContainer",MSScrollContainerConfig::CfgFactory,MSScrollContainerConfig::CompFactory);
	theTable->AddAllocator("MSSeperator",MSSeperatorConfig::CfgFactory,MSSeperatorConfig::CompFactory);
	theTable->AddAllocator("MS3DFrame",MS3DFrameConfig::CfgFactory,MS3DFrameConfig::CompFactory);
	theTable->AddAllocator("MSEtchedFrame",MSEtchedFrameConfig::CfgFactory,MSEtchedFrameConfig::CompFactory);
	theTable->AddAllocator("MSEtchedFrameGroup",MSEtchedFrameGroupConfig::CfgFactory,MSEtchedFrameGroupConfig::CompFactory);
	theTable->AddAllocator("MSDlgBack",MSDlgBackConfig::CfgFactory,MSDlgBackConfig::CompFactory);
	theTable->AddAllocator("MSTabCtrl",MSTabCtrlConfig::CfgFactory,MSTabCtrlConfig::CompFactory);
	theTable->AddAllocator("MSListCtrl",MSListCtrlConfig::CfgFactory,MSListCtrlConfig::CompFactory);
	theTable->AddAllocator("MSHeaderControl",MSHeaderControlConfig::CfgFactory,MSHeaderControlConfig::CompFactory);
	// LOST-DEBUG: none: exact only (one site after the last string temp, as WONGUIConfig_Init)
	EE_LOST_DEBUG(1);
}

}

// Genuine original MSComponentConfig.h inline factory APIs; support0.
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
#include "ee/wonapi/WONLobbyMSConfig.h"
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyMSRemainingPainters.h"
namespace WONAPI {
// FUNCTION: WON 0x0078eaa0
Component* MSHeaderButtonConfig::CompFactory() { return new MSHeaderButton; }
// FUNCTION: WON 0x0078f1e0
Component* MSHeaderControlConfig::CompFactory() { return new MSHeaderControl; }
// FUNCTION: WON 0x0078f2c0
Component* MSListCtrlConfig::CompFactory() { return new MSListCtrl; }
}

namespace WONAPI {
// FUNCTION: WON 0x0078ef80
Component* MSEtchedFrameGroupConfig::CompFactory() { return new MSEtchedFrameGroup; }
}

namespace WONAPI {
// FUNCTION: WON 0x0078e600
ComponentConfig* MSButtonConfig::CfgFactory() { return new MSButtonConfig; }
// FUNCTION: WON 0x0078e4e0
Component* MSButtonConfig::CheckboxFactory() { return new MSCheckbox; }
}

namespace WONAPI {
// FUNCTION: WON 0x0078e960
Component* MSTabButtonConfig::CompFactory() { return new MSTabButton; }
// FUNCTION: WON 0x0078e5a0
Component* MSButtonConfig::LabelFactory() { return new MSLabel; }
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0078e8b0
// (public: static class WONAPI::Component * __cdecl WONAPI::MSCloseButtonConfig::CompFactory(void)    [?CompFactory@MSCloseButtonConfig@WONAPI@@SAPAVComp)
// TEMPLATE: WON 0x0078ecc0
// (public: static class WONAPI::Component * __cdecl WONAPI::MSSeperatorConfig::CompFactory(void)    [?CompFactory@MSSeperatorConfig@WONAPI@@SAPAVComponen)
// TEMPLATE: WON 0x0078eec0
// (public: static class WONAPI::Component * __cdecl WONAPI::MSEtchedFrameConfig::CompFactory(void)    [?CompFactory@MSEtchedFrameConfig@WONAPI@@SAPAVComp)
// TEMPLATE: WON 0x0078f0a0
// (public: static class WONAPI::ComponentConfig * __cdecl WONAPI::MSTabCtrlConfig::CfgFactory(void)    [?CfgFactory@MSTabCtrlConfig@WONAPI@@SAPAVComponen)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0078ea40
// (public: static class WONAPI::ComponentConfig * __cdecl WONAPI::MSTabButtonConfig::CfgFactory(void)    [?CfgFactory@MSTabButtonConfig@WONAPI@@SAPAVComp)
// TEMPLATE: WON 0x0078eb80
// (public: static class WONAPI::ComponentConfig * __cdecl WONAPI::MSHeaderButtonConfig::CfgFactory(void)    [?CfgFactory@MSHeaderButtonConfig@WONAPI@@SAP)
