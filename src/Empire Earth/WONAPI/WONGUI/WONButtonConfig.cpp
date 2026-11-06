// Actual WON simple/rich button state parsing and configuration dispatch.
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
#include "ee/wonapi/WONLobbyWONButtonConfig.h"
namespace WONAPI {

// FUNCTION: WON 0x007971c0
void WONSimpleButtonConfig::HandleButtonState()
{
	GUIString aStateStr = Fn3842F0(false);
	EnsureComma();

	int aState = 0;
	if(aStateStr.CompareNoCase("Normal")==0) aState = 0;
	else if(aStateStr.CompareNoCase("Pressed")==0) aState = 1;
	else if(aStateStr.CompareNoCase("Disabled")==0) aState = 2;
	else if(aStateStr.CompareNoCase("Over")==0) aState = 3;
	else
		throw ConfigObjectException("Invalid button state: " + (std::string)aStateStr);

	SmartPtr<Image> anImage = ReadImage(false);
	mButton->SetImage(aState,anImage.get());
}

// FUNCTION: WON 0x007973f0
void WONSimpleButtonConfig::HandleButtonStates()
{	
	for(int i=0; i<4; i++)
	{
		mButton->SetImage(i,ReadImage(false).get()); 
		if(EndOfString())
			return;

		EnsureComma();
	}
}

// FUNCTION: WON 0x007974e0
bool WONSimpleButtonConfig::Virtual24(const std::string &theInstruction)
{
	if(theInstruction=="SCALEIMAGE")
		mButton->SetScaleImage(Fn3837F0());
	else if(theInstruction=="TYPE")
	{
		GUIString aType = Fn3842F0(false);
		if(!gSkin->CopySimpleButtonType(aType,mButton))
			throw ConfigObjectException("Type not found: " + std::string(aType));
	}
	else if(theInstruction=="BUTTONSTATE")
		HandleButtonState();
	else if(theInstruction=="BUTTONSTATES")
		HandleButtonStates();
	else if(theInstruction=="CHECKMOUSEBUTTONDOWN")
		mButton->SetCheckMouseButtonDown(Fn3837F0());
	else
		return ButtonConfig::Virtual24(theInstruction);

	return true;
}

// FUNCTION: WON 0x00797740
void WONButtonConfig::HandleSetButtonState()
{
	GUIString aStateStr = Fn3842F0(false);
	EnsureComma();

	int aState = 0;
	bool checked = false;
	if(aStateStr.CompareNoCase("Normal")==0) aState = 0;
	else if(aStateStr.CompareNoCase("Pressed")==0) aState = 1;
	else if(aStateStr.CompareNoCase("Disabled")==0) aState = 2;
	else if(aStateStr.CompareNoCase("Over")==0) aState = 3;
	else if(aStateStr.CompareNoCase("Default")==0) aState = 4;
	else if(aStateStr.CompareNoCase("CheckedNormal")==0) { aState = 0; checked = true; }
	else if(aStateStr.CompareNoCase("CheckedPressed")==0) { aState = 1; checked = true; }
	else if(aStateStr.CompareNoCase("CheckedDisabled")==0) { aState = 2; checked = true; }
	else if(aStateStr.CompareNoCase("CheckedOver")==0) { aState = 3; checked = true; }
	else if(aStateStr.CompareNoCase("CheckedDefault")==0) { aState = 4; checked = true; }
	else
		throw ConfigObjectException("Invalid button state: " + (std::string)aStateStr);

	SmartPtr<Image> anImage = ReadImage(false);
	int aColor = -1;
	SmartPtr<Font> aFont;
	if(!EndOfString())
	{
		EnsureComma();
		aColor = Fn385510();
		if(!EndOfString())
		{
			EnsureComma();
			aFont = ReadFont();
		}
	}

	mButton->SetButtonState(aState,anImage.get(),aColor,aFont.get(),checked);
}

// FUNCTION: WON 0x00797aa0
bool WONButtonConfig::Virtual24(const std::string& theInstruction)
{
	if(theInstruction=="FONT")
		mButton->SetFont(ReadFont().get());
	else if(theInstruction=="TYPE")
	{
		GUIString aType = Fn3842F0(false);
		if(!gSkin->CopyButtonType(aType,mButton))
			throw ConfigObjectException("Type not found: " + std::string(aType));
	}
	else if(theInstruction=="TEXTCOLOR")
		mButton->SetTextColor(Fn385510());
	else if(theInstruction=="DEFAULTTEXTCOLOR")
		mButton->SetDefaultTextColor(Fn385510());
	else if(theInstruction=="VERTALIGN")
		mButton->SetVertAlign(ReadVertAlign());
	else if(theInstruction=="HORZALIGN")
		mButton->SetHorzAlign(ReadHorzAlign());
	else if(theInstruction=="TRANSPARENT")
		mButton->SetTransparent(Fn3837F0());
	else if(theInstruction=="ADJUSTHEIGHTTOFITTEXT")
		mButton->SetAdjustHeightToFitText(Fn3837F0());
	else if(theInstruction=="WRAP")
		mButton->SetWrapText(Fn3837F0());
	else if(theInstruction=="DRAWGRAYTEXT")
		mButton->SetDrawGrayText(Fn3837F0());
	else if(theInstruction=="TEXTPADDING")
	{
		int left = ReadInt(); EnsureComma();
		int top = ReadInt(); EnsureComma();
		int right = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mButton->SetTextPadding(left,top,right,bottom);
	}
	else if(theInstruction=="FOCUSPADDING")
	{
		int left = ReadInt(); EnsureComma();
		int top = ReadInt(); EnsureComma();
		int right = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mButton->SetFocusPadding(left,top,right,bottom);
	}
	else if(theInstruction=="HORZTEXTPAD")
	{
		int left = ReadInt(); 
		int right = mButton->GetRightTextPad();
		if(!EndOfString())
		{
			EnsureComma(); 
			right = ReadInt();
		}
		mButton->SetHorzTextPad(left,right);
	}
	else if(theInstruction=="INCREMENTHORZTEXTPAD")
	{
/*		int left = ReadInt(); EnsureComma();
		int right = ReadInt();
		mButton->IncrementHorzTextPad(left,right);
*/	}
	else if(theInstruction=="INCREMENTVERTEXTPAD")
	{
/*		int top = ReadInt(); EnsureComma();
		int bottom = ReadInt();
		mButton->IncrementVertTextPad(top,bottom);
*/	}
	else if(theInstruction=="BUTTONSTATE")
		HandleSetButtonState();
	else if(theInstruction=="SCALEIMAGE")
		mButton->SetScaleImage(Fn3837F0());
	else if(theInstruction=="DRAWFOCUSRECT")
		mButton->SetFocusStyle(Fn3837F0()?1:0);
	else
		return ButtonConfig::Virtual24(theInstruction);

	return true;
}
}

namespace WONAPI {
// FUNCTION: WON 0x00796a20
void WONSimpleButtonConfig::Virtual34(Component* component) { mButton=static_cast<WONSimpleButton*>(component); ButtonConfig::Virtual34(component); }
// (0x00796a20: ICF-folded with WONSimpleButtonConfig::Virtual34 above)
void WONButtonConfig::Virtual34(Component* component) { mButton=static_cast<WONButton*>(component); ButtonConfig::Virtual34(component); }
}
