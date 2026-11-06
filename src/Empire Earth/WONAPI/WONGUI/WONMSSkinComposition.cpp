// Original Microsoft-style skin: runtime state images, widget templates and frame composition.
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
#include "ee/wonapi/WONLobbySkinListControls.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
#include "ee/wonapi/WONLobbySkinTab.h"
#include "ee/wonapi/WONLobbyWONInputBox.h"
namespace WONAPI {
// FUNCTION: WON 0x00715d70
WONMSSkin::WONMSSkin()
{
	SmartPtr<Font> aFont = WONAPI::WindowManager::GetDefaultWindowManager()->GetNamedFont(std::string("MSButton"));

	// Push Button
	mPushButton = new WONButton;
	mPushButton->Fn2F3370(0x0001,false);
	mPushButton->Fn2F3370(0x0400,true);
	mPushButton->SetDrawGrayText(true);
	mPushButton->SetButtonState(0,new MSImage(MSImageType_PushButton,0),-1,aFont.get());
	mPushButton->SetButtonState(1,new MSImage(MSImageType_PushButton,MSImageFlag_Down),-1,NULL);
	mPushButton->SetButtonState(4,new MSImage(MSImageType_PushButton,MSImageFlag_Default),-1,NULL);

	// Checkbox

	mCheckbox = new WONButton;
	mCheckbox->SetPushOffsets(0,0);
	mCheckbox->SetHasCheck(true);
	mCheckbox->SetScaleImage(false);
	mCheckbox->SetTextPadding(13+4,2,2,2);
	mCheckbox->SetButtonState(0,new MSImage(MSImageType_Checkbox,0,13,13),-1,aFont.get());
	mCheckbox->SetButtonState(1,new MSImage(MSImageType_Checkbox,MSImageFlag_Down,13,13),-1,NULL);
	mCheckbox->SetButtonState(0,new MSImage(MSImageType_Checkbox,MSImageFlag_Checked,13,13),-1,NULL,true);
	mCheckbox->SetButtonState(1,new MSImage(MSImageType_Checkbox,MSImageFlag_Checked | MSImageFlag_Down,13,13),-1,NULL,true);

	// RadioButton

	mRadioButton = new WONButton;
	mRadioButton->Fn2F3370(0x0008,true);
	mRadioButton->SetPushOffsets(0,0);
	mRadioButton->SetHasCheck(true);
	mRadioButton->SetScaleImage(false);
	mRadioButton->SetTextPadding(13+4,2,2,2);
	mRadioButton->SetButtonState(0,new MSImage(MSImageType_RadioButton,0,13,13),-1,aFont.get());
	mRadioButton->SetButtonState(1,new MSImage(MSImageType_RadioButton,MSImageFlag_Down,13,13),-1,NULL);
	mRadioButton->SetButtonState(0,new MSImage(MSImageType_RadioButton,MSImageFlag_Checked,13,13),-1,NULL,true);
	mRadioButton->SetButtonState(1,new MSImage(MSImageType_RadioButton,MSImageFlag_Checked | MSImageFlag_Down,13,13),-1,NULL,true);

	// HeaderButton
	mHeaderButton = new WONHeaderButton;
	mHeaderButton->SetScaleImage(true);
	mHeaderButton->SetButtonState(0, new MSImage(MSImageType_ListHeader,0),-1,aFont.get());
	mHeaderButton->SetButtonState(1, new MSImage(MSImageType_ListHeader,MSImageFlag_Down),-1,aFont.get());

	// Tab Button
	mTabButton = new WONButton;
	mTabButton->SetScaleImage(true);
	mTabButton->SetHasCheck(true);
	mTabButton->SetPushOffsets(0,0);
	mTabButton->SetButtonState(0, new MSImage(MSImageType_TabButton,0),-1,aFont.get());
	mTabButton->SetButtonState(0, new MSImage(MSImageType_TabButton,MSImageFlag_Checked),-1,aFont.get());

	// Close Button
	mCloseButton = new WONSimpleButton;
	mCloseButton->SetImage(0, new MSImage(MSImageType_CloseButton,0,16,14));
	mCloseButton->SetImage(1, new MSImage(MSImageType_CloseButton,MSImageFlag_Down,16,14));

	// Arrow Buttons
	int i;
	for(i=0; i<4; i++)
	{
		mArrowButton[i] = new WONSimpleButton;
		mArrowButton[i]->SetImage(0,new MSImage(MSImageType_LeftArrow+i,0,15,15));
		mArrowButton[i]->SetImage(1,new MSImage(MSImageType_LeftArrow+i,MSImageFlag_Down,15,15));
		mArrowButton[i]->SetImage(2,new MSImage(MSImageType_LeftArrow+i,MSImageFlag_Disabled,15,15));
	}

	// Scrollbar, ScrollThumb, and ScrollGutter
	for(i=0; i<2; i++)
	{
		mScrollbar[i] = new WONScrollbar;

		mScrollThumb[i] = new WONScrollThumb;
		mScrollThumb[i]->SetImage(0,new MSImage(MSImageType_ScrollThumb,0));
		mScrollThumb[i]->SetScaleImage(true);

		mScrollGutter[i] = new WONSimpleButton;
		mScrollGutter[i]->SetScaleImage(true);
		mScrollGutter[i]->SetImage(0,new MSImage(MSImageType_ScrollGutter,0));
	}

	// ComboBox Button
	mComboButton = new WONSimpleButton;
	mComboButton->SetScaleImage(true);
	mComboButton->SetImage(0,new MSImage(MSImageType_DownArrow,0,16,18));
	mComboButton->SetImage(1,new MSImage(MSImageType_DownArrow,MSImageFlag_Down,16,18));
	mComboButton->SetImage(2,new MSImage(MSImageType_DownArrow,MSImageFlag_Disabled,16,18));

	mPopupBack = new WONFrameComponent;
	mPopupBack->VirtualB0(new MSImage(MSImageType_ScrollThumb,0));

	mDialogBack = new WONFrameComponent;
	mDialogBack->VirtualB0(new MSImage(MSImageType_ScrollThumb,0));

	mInputBox = new WONInputBox;
	mComboBox = new WONComboBox;
	mScrollContainer = new WONScrollContainer;
	mListCtrl = new WONListCtrl;
	mTabCtrl = new WONTabCtrl;
	mTabCtrl->SetFrame(WONFrame(new MSImage(MSImageType_TabCtrlFrame),2));

	mInputBox->SetFrame(WONFrame(new MSImage(MSImageType_Frame3,MSImageFlag_Down),2));
	mComboBox->SetFrame(WONFrame(new MSImage(MSImageType_Frame3,MSImageFlag_Down),2));
	mScrollContainer->SetFrame(WONFrame(new MSImage(MSImageType_Frame3,MSImageFlag_Down),2));
	mScrollContainer->SetCorner(new MSImage(MSImageType_ScrollCorner,0));
	mListCtrl->CopyAttributes(mScrollContainer.get());
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00717100
// (public: virtual __thiscall WONAPI::WONMSSkin::~WONMSSkin(void)    [??1WONMSSkin@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x0071bc10
// (public: __thiscall std::map<class WONAPI::GUIString,class WONAPI::SmartPtr<class WONAPI::WONButton>,struct WONAPI::GUIStringLessNoCase,class std::allo)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007170e0
// (public: virtual void * __thiscall WONAPI::WONMSSkin::`scalar deleting destructor'(unsigned int)    [??_GWONMSSkin@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00725040
// (public: void * __thiscall std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONButton> >::`scalar deleting destructor'(uns)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00722a50
// (public: class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONButton> >,str)
// TEMPLATE: WON 0x00722f70
// (protected: void __thiscall std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WON)
