// Genuine SDK default skin selectors and frame-copy factories.
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
#include "ee/wonapi/WONLobbyMSSkinImage.h"
#include "ee/wonapi/WONLobbyWONInputBox.h"
#include "ee/wonapi/WONLobbySkinListControls.h"
namespace WONAPI {
// FUNCTION: WON 0x00715540
WONSimpleButton* WONSkin::GetCloseButton()
{
    return mCloseButton->Duplicate();
}
// FUNCTION: WON 0x00715890
WONInputBox* WONSkin::GetInputBox()
{
    return mInputBox->Duplicate();
}
// FUNCTION: WON 0x007154f0
WONButton* WONSkin::GetPushButton()
{
    return mPushButton->Duplicate();
}
// FUNCTION: WON 0x00715500
WONButton* WONSkin::GetCheckbox()
{
    return mCheckbox->Duplicate();
}
// FUNCTION: WON 0x00715510
WONButton* WONSkin::GetRadioButton()
{
    return mRadioButton->Duplicate();
}
// FUNCTION: WON 0x00715520
WONHeaderButton* WONSkin::GetHeaderButton()
{
    return mHeaderButton->Duplicate();
}
// FUNCTION: WON 0x00715580
WONFrameComponent* WONSkin::GetPopupBack()
{
    WONFrameComponent* component=new WONFrameComponent(mPopupBack->GetFrame());
    return component;
}
// FUNCTION: WON 0x007155f0
WONFrameComponent* WONSkin::GetDialogBack()
{
    WONFrameComponent* component=new WONFrameComponent(mDialogBack->GetFrame());
    return component;
}
}

#include "ee/wonapi/WONLobbySkinTab.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x007152d0
WONSkin::~WONSkin() {}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007154d0
// (public: virtual void * __thiscall WONAPI::WONSkin::`scalar deleting destructor'(unsigned int)    [??_GWONSkin@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00725020
// (public: void * __thiscall std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONButton> >::`scalar deleting destructor'(uns)
// TEMPLATE: WON 0x0071bb50
// (public: __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONButton> )
// TEMPLATE: WON 0x0071bc60
// (public: __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONSimpleBu)
// TEMPLATE: WON 0x00722fb0
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONSimpleButton>)
// TEMPLATE: WON 0x007234d0
// (protected: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WON)
// TEMPLATE: WON 0x00725060
// (public: __thiscall std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONButton> >::~pair<class WONAPI::GUIString const ,cl)
// TEMPLATE: WON 0x007250e0
// (public: __thiscall std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONSimpleButton> >::~pair<class WONAPI::GUIString con)
