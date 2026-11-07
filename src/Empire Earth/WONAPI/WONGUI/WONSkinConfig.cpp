// Original release skin configuration dispatch, retained templates and image cursors.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyWONButtonConfig.h"
#include "ee/wonapi/WONLobbyWONSkinConfig.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
#include "ee/wonapi/WONLobbyWONListConfig.h"
#include "LostDebug.h"
namespace WONAPI {
// FUNCTION: WON 0x00798f90
bool WONSkinConfig::ReadVert()
{
    GUIString text=Fn3842F0(false);
    return text.CompareNoCase("Vert")==0;
}
// FUNCTION: WON 0x00799010
bool WONSkinConfig::Virtual0C(ConfigParser* parser)
{
    if(!ComponentConfig::Virtual0C(parser))
        return false;
    if(gSkin.get()==NULL)
        gSkin=new WONMSSkin;
    return true;
}
// FUNCTION: WON 0x007990e0
void WONSkinConfig::HandleButtonClickSound()
{
    SmartPtr<Sound> sound=ReadSound();
    gSkin->mPushButton->SetClickSound(sound.get());
    gSkin->mCheckbox->SetClickSound(sound.get());
    gSkin->mRadioButton->SetClickSound(sound.get());
    gSkin->mHeaderButton->SetClickSound(sound.get());
    gSkin->mTabButton->SetClickSound(sound.get());
    gSkin->mCloseButton->SetClickSound(sound.get());
    gSkin->mComboButton->SetClickSound(sound.get());
    WONSkin::ButtonTypeMap::iterator it=gSkin->mButtonTypeMap.begin();
    for(;it!=gSkin->mButtonTypeMap.end();++it)
        it->second->SetClickSound(sound.get());
}
// FUNCTION: WON 0x00799210
bool WONSkinConfig::Virtual24(const std::string &theInstruction)
{
	if(theInstruction=="WINSKIN")
	{

	}
	else if(theInstruction=="ADDBUTTONTYPE")
	{
		GUIString aName = Fn3842F0(false); EnsureComma();
		WONButton* aButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
		gSkin->mButtonTypeMap[aName] = aButton;
	}
	else if(theInstruction=="ADDSIMPLEBUTTONTYPE")
	{
		GUIString aName = Fn3842F0(false); EnsureComma();
		WONSimpleButton* aButton = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
		gSkin->mSimpleButtonTypeMap[aName] = aButton;
	}
	else if(theInstruction=="SETPUSHBUTTON")
		gSkin->mPushButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
	else if(theInstruction=="SETCHECKBOX")
		gSkin->mCheckbox = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
	else if(theInstruction=="SETRADIOBUTTON")
	{
		gSkin->mRadioButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
		gSkin->mRadioButton->Fn2F3370(8,true);
	}
	else if(theInstruction=="SETHEADERBUTTON")
		gSkin->mHeaderButton = SafeConfigGetComponent<WONHeaderButton*>(ReadComponent(),"WONHeaderButton");
	else if(theInstruction=="SETTABBUTTON")
		gSkin->mTabButton = SafeConfigGetComponent<WONButton*>(ReadComponent(),"WONButton");
	else if(theInstruction=="SETARROWBUTTON")
	{
		Direction aDirection = ReadDirection(); EnsureComma();
		gSkin->mArrowButton[aDirection] = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");

	}
	else if(theInstruction=="SETCOMBOBUTTON")
		gSkin->mComboButton = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	else if(theInstruction=="SETCLOSEBUTTON")
		gSkin->mCloseButton = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	else if(theInstruction=="SETSCROLLGUTTER")
	{
		bool isVert = ReadVert(); EnsureComma();
		gSkin->mScrollGutter[isVert?1:0] = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	}
	else if(theInstruction=="SETSCROLLTHUMB")
	{
		bool isVert = ReadVert(); EnsureComma();
		gSkin->mScrollThumb[isVert?1:0] = SafeConfigGetComponent<WONSimpleButton*>(ReadComponent(),"WONSimpleButton");
	}
	else if(theInstruction=="SETSCROLLBAR")
	{
		bool isVert = ReadVert(); EnsureComma();
		gSkin->mScrollbar[isVert?1:0] = SafeConfigGetComponent<WONScrollbar*>(ReadComponent(),"WONScrollbar");
	}
	else if(theInstruction=="SETSCROLLCONTAINER")
		gSkin->mScrollContainer = SafeConfigGetComponent<WONScrollContainer*>(ReadComponent(),"WONScrollContainer");
	else if(theInstruction=="SETLISTCTRL")
		gSkin->mListCtrl = SafeConfigGetComponent<WONListCtrl*>(ReadComponent(),"WONListCtrl");
	else if(theInstruction=="SETTABCTRL")
	{
		gSkin->mTabCtrl = SafeConfigGetComponent<WONTabCtrl*>(ReadComponent(),"WONTabCtrl");
		gSkin->mTabCtrl->mSkin=NULL;
	}
	else if(theInstruction=="SETCOMBOBOX")
		gSkin->mComboBox = SafeConfigGetComponent<WONComboBox*>(ReadComponent(),"WONComboBox");
	else if(theInstruction=="SETPOPUPBACK")
		gSkin->mPopupBack = SafeConfigGetComponent<WONFrameComponent*>(ReadComponent(),"WONPopupBack");
	else if(theInstruction=="SETDIALOGBACK")
		gSkin->mDialogBack = SafeConfigGetComponent<WONFrameComponent*>(ReadComponent(),"WONDialogBack");
	else if(theInstruction=="BUTTONCLICKSOUND")
		HandleButtonClickSound();
	else if(theInstruction=="IMAGECURSOR")
	{
		StandardCursor aType = ReadStandardCursor(); EnsureComma();
		SmartPtr<Image> anImage = ReadImage(false);
		SmartPtr<Cursor> aCursor = new ComponentCursor(new ImageComponent(anImage.get()));
		Cursor::SetStandardCursor(aType,aCursor.get());
	}
	else
		return false;

	return true;
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(1);
}


// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007926a0
// (public: __thiscall WONAPI::ConfigObjectException::ConfigObjectException(class std::basic_string<char,struct std::char_traits<char>,class std::allocato)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0079c9e0
// (public: struct std::pair<class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI:)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0079cae0
// (public: struct std::pair<class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI:)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a1560
// TEMPLATE: WON 0x007a1860
// TEMPLATE: WON 0x007a2ef0
// (void __cdecl std::_Construct(struct std::pair<class WONAPI::GUIString const ,class WONAPI::SmartPtr<class WONAPI::WONButton> > *,struct std::pair<clas)

// Compiler-generated and header-inline functions this unit emits:
}
