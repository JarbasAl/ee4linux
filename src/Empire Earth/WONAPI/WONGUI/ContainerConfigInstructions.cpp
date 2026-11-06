// Native container configuration component/layout instruction algorithms.
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
#include "ee/wonapi/WONLobbyConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00788840
bool ContainerConfig::HandleAdd()
{
	while(true)
	{
		Component *aComp = ReadComponent();
		mContainer->Fn2F6A60(aComp);
		if(EndOfString())
			break;

		EnsureComma();
	}


	return true;
}
// FUNCTION: WON 0x00788870
bool ContainerConfig::HandleAddLayoutPoint()
{
	while(true)
	{
		LayoutPoint *aPoint = SafeConfigGetComponent<LayoutPoint*>(ReadComponent(),"LayoutPoint");
		mContainer->AddLayoutPoint(aPoint);
		if(EndOfString())
			break;

		EnsureComma();
	}


	return true;
}
// FUNCTION: WON 0x007888b0
bool ContainerConfig::HandleLayout(bool chain, bool repeat)
{
	Component *target = ReadComponent(); 
	int instructions = 0;
	Component *ref;

	if(!chain && !repeat)
	{
		EnsureComma();
		while(true)
		{
			GUIString aStr = Fn3842F0(true);
			if(aStr.length()==0)
				break;

			if(aStr.CompareNoCase("SameWidth")==0) instructions |= 0x1;
			else if(aStr.CompareNoCase("SameHeight")==0) instructions |= 0x2;
			else if(aStr.CompareNoCase("Above")==0) instructions |= 0x100;
			else if(aStr.CompareNoCase("Below")==0) instructions |= 0x200;
			else if(aStr.CompareNoCase("Right")==0) instructions |= 0x400;
			else if(aStr.CompareNoCase("Left")==0) instructions |= 0x800;
			else if(aStr.CompareNoCase("SameLeft")==0) instructions |= 0x1000;
			else if(aStr.CompareNoCase("SameRight")==0) instructions |= 0x2000;
			else if(aStr.CompareNoCase("SameTop")==0) instructions |= 0x4000;
			else if(aStr.CompareNoCase("SameBottom")==0) instructions |= 0x8000;
			else if(aStr.CompareNoCase("GrowToRight")==0) instructions |= 0x10000;
			else if(aStr.CompareNoCase("GrowToLeft")==0) instructions |= 0x20000;
			else if(aStr.CompareNoCase("GrowToTop")==0) instructions |= 0x40000;
			else if(aStr.CompareNoCase("GrowToBottom")==0) instructions |= 0x80000;
			else if(aStr.CompareNoCase("SameSize")==0) instructions |= 0x3;	
			else 
				throw ConfigObjectException("Unknown layout instruction: " + (std::string)aStr);
		}
	}
	else
		instructions = mLayoutInstruction;

	if(chain || (repeat && EndOfString()))
	{
		if(mLayoutRef==NULL)
			throw ConfigObjectException("Invalid LayoutChain");

		ref = mLayoutRef;
	}
	else
	{
		EnsureComma();
		ref = ReadComponent();
	}

	if(!EndOfString())
		EnsureComma();

	int i;
	for(i=0; i<4; i++)
	{
		if(EndOfString())
			break;

		mLayoutParams[i] = ReadInt();
		if(!EndOfString())
			EnsureComma();
	}

	if((!chain && !repeat) || i!=0)
	{
		for(; i<4; i++)
			mLayoutParams[i] = 0;
	}

	mContainer->Fn2F6140(target,instructions,ref,mLayoutParams[0],mLayoutParams[1],
		mLayoutParams[2],mLayoutParams[3],mCurLayoutId);

	if(repeat)
		mLayoutRef = ref;
	else
		mLayoutRef = target;

	mLayoutInstruction = instructions;

	return true;
}
// FUNCTION: WON 0x00789c80
bool ContainerConfig::Virtual24(const std::string& theInstruction)
{
	if(theInstruction=="ADD")
		return HandleAdd();
	else if(theInstruction=="ADDLAYOUTPOINT")
		return HandleAddLayoutPoint();
	else if(theInstruction=="NEWLAYOUTPOINT")
		return HandleNewLayoutPoint();
	else if(theInstruction=="LAYOUT")
		return HandleLayout();
	else if(theInstruction=="SETLAYOUTID")
		mCurLayoutId = ReadInt();
	else if(theInstruction=="ENABLELAYOUT")
	{
		bool enable = Fn3837F0();
		while(true)
		{
			EnsureComma();
			int anId = ReadInt();
			mContainer->Fn2F6120(anId,enable);
			if(EndOfString())
				break;
		}
	}
	else if(theInstruction=="RECALCLAYOUT")
		mContainer->Fn2F6290();
	else if(theInstruction=="CHAINLAYOUT")
		return HandleLayout(true);
	else if(theInstruction=="REPEATLAYOUT")
		return HandleLayout(false,true);
	else if(theInstruction=="SAMESIZELAYOUT")
		return HandleSameSizeLayout(true,true);
	else if(theInstruction=="SAMEWIDTHLAYOUT")
		return HandleSameSizeLayout(true,false);
	else if(theInstruction=="SAMEHEIGHTLAYOUT")
		return HandleSameSizeLayout(false,true);
	else if(theInstruction=="CENTERLAYOUT")
		return HandleCenterLayout(true,true);
	else if(theInstruction=="HORZCENTERLAYOUT")
		return HandleCenterLayout(true,false);
	else if(theInstruction=="VERTCENTERLAYOUT")
		return HandleCenterLayout(false,true);
	else if(theInstruction=="HORZSPACEEVENLYLAYOUT")
		return HandleSpaceEvenlyLayout(true);
	else if(theInstruction=="VERTSPACEEVENLYLAYOUT")
		return HandleSpaceEvenlyLayout(false);
	else if(theInstruction=="CLEARRIGHTLAYOUT")
		return HandleClearRightLayout();
	else if(theInstruction=="NOOVERLAPLAYOUT")
		return HandleNoOverlapLayout();
	else if(theInstruction=="SIZEATLEASTLAYOUT")
		return HandleSizeAtLeastLayout();
	else if(theInstruction=="ISOLATETABFOCUS")
		mContainer->SetIsolateTabFocus(Fn3837F0());
	else if(theInstruction=="ONLYCHILDINPUT")
		mContainer->SetOnlyChildInput(Fn3837F0());
	else
		return ComponentConfig::Virtual24(theInstruction);

	return true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a1da0
// (class WONAPI::LayoutPoint * __cdecl WONAPI::SafeConfigGetComponent(class WONAPI::Component *,char const *)    [?SafeConfigGetComponent@WONAPI@@YAPAVLa)
