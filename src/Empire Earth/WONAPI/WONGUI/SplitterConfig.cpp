// Complete original-backed splitter configuration algorithms on original-owned receivers.
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
#include "ee/wonapi/WONLobbySplitterConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00794ed0
Splitter::QuadrantPosition SplitterConfig::ReadQuadrant()
{
	GUIString aStr = Fn3842F0(false);
	if(aStr.CompareNoCase("Left")==0)
		return Splitter::Position_Left;
	else if(aStr.CompareNoCase("Right")==0)
		return Splitter::Position_Right;
	else if(aStr.CompareNoCase("Top")==0)
		return Splitter::Position_Top;
	else if(aStr.CompareNoCase("Bottom")==0)
		return Splitter::Position_Bottom;
	else if(aStr.CompareNoCase("TopLeft")==0)
		return Splitter::Position_TopLeft;
	else if(aStr.CompareNoCase("TopRight")==0)
		return Splitter::Position_TopRight;
	else if(aStr.CompareNoCase("BottomLeft")==0)
		return Splitter::Position_BottomLeft;
	else if(aStr.CompareNoCase("BottomRight")==0)
		return Splitter::Position_BottomRight;
	else
		throw ConfigObjectException("Invalid quadrant: " + (std::string)aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x007951b0
Splitter::SplitterType SplitterConfig::ReadSplitterType()
{
	GUIString aStr = Fn3842F0(false);
	if(aStr.CompareNoCase("LeftRight")==0)
		return Splitter::Splitter_LeftRight;
	else if(aStr.CompareNoCase("UpDown")==0)
		return Splitter::Splitter_UpDown;
	else if(aStr.CompareNoCase("LeftRight_Top")==0)
		return Splitter::Splitter_LeftRight_Top;
	else if(aStr.CompareNoCase("LeftRight_Bottom")==0)
		return Splitter::Splitter_LeftRight_Bottom;
	else if(aStr.CompareNoCase("UpDown_Left")==0)
		return Splitter::Splitter_UpDown_Left;
	else if(aStr.CompareNoCase("UpDown_Right")==0)
		return Splitter::Splitter_UpDown_Right;
	else if(aStr.CompareNoCase("Center")==0)
		return Splitter::Splitter_Center;
	else
		throw ConfigObjectException("Invalid splitter type: " + (std::string)aStr);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FUNCTION: WON 0x00795450
bool SplitterConfig::Virtual24(const std::string& theInstruction)
{
	if(theInstruction=="ADDQUADRANT")
	{
		Component *aComponent = ReadComponent(); EnsureComma();
		Splitter::QuadrantPosition aPosition = ReadQuadrant();
		mSplitter->AddQuadrant(aComponent,aPosition);
	}
	else if(theInstruction=="ADDSPLITTER")
	{
		Component *aComponent = ReadComponent(); EnsureComma();
		Splitter::SplitterType aType = ReadSplitterType();
		mSplitter->AddSplitter(aComponent,aType);
	}
	else if(theInstruction=="ADDINVISIBLESPLITTER")
	{
		Splitter::SplitterType aType = ReadSplitterType();
		int aSize = 10;
		if(!EndOfString())
		{
			EnsureComma();
			aSize = ReadInt();
		}
		mSplitter->AddInvisibleSplitter(aType,aSize);
	}
	else if(theInstruction=="ADDCOMPLEXSPLITTER")
	{
		Container *aContainer= SafeConfigGetComponent<Container*>(ReadComponent(),"Container"); EnsureComma();
		Splitter::SplitterType aType = ReadSplitterType();
		Component *aDragComponent = NULL;
		if(!EndOfString())
		{
			EnsureComma();
			aDragComponent = ReadComponent();
		}
		mSplitter->AddComplexSplitter(aContainer,aType,aDragComponent);
	}
	else if(theInstruction=="MINLEFTRIGHT")
	{
		int minLeft = ReadInt(); EnsureComma();
		int minRight = ReadInt(); 
		mSplitter->SetMinLeftRight(minLeft,minRight);
	}
	else if(theInstruction=="MINTOPBOTTOM")
	{
		int minTop = ReadInt(); EnsureComma();
		int minBottom = ReadInt();
		mSplitter->SetMinTopBottom(minTop,minBottom);
	}
	else if(theInstruction=="SPLITTERPERCENTAGES")
	{
		int leftPercent = ReadInt(); EnsureComma();
		int topPercent = ReadInt();
		mSplitter->SetPercentages(leftPercent,topPercent);
	}
	else if(theInstruction=="MAXQUADRANT")
		mSplitter->MaxQuadrant(ReadQuadrant());
	else if(theInstruction=="UNMAXQUADRAN")
		mSplitter->UnMaxQuadrant();
	else if(theInstruction=="SPLITTERPOS")
	{
		int x = ReadInt(); EnsureComma();
		int y = ReadInt();
		mSplitter->SetSplitterPos(x,y);
	}
	else if(theInstruction=="REGISTERMAXBUTTON")
	{
		Button *aButton = SafeConfigGetComponent<Button*>(ReadComponent(),"Button"); EnsureComma();
		Splitter::QuadrantPosition aPosition = ReadQuadrant();
		mSplitter->RegisterMaxButton(aButton,aPosition);
	}
	else
		return ContainerConfig::Virtual24(theInstruction);

	return true;
}

}

namespace WONAPI {
// SUPPORT: EXE 0x0078f960; genuine SDK SplitterConfig SetComponent, zero primary.
void SplitterConfig::Virtual34(Component* component)
{
    ContainerConfig::Virtual34(component);
    mSplitter=static_cast<Splitter*>(component);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007a1fc0
// (class WONAPI::Container * __cdecl WONAPI::SafeConfigGetComponent(class WONAPI::Component *,char const *)    [?SafeConfigGetComponent@WONAPI@@YAPAVCont)
