// Native full typed container layout instruction builders.
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
#include "ee/wonapi/WONLobbyLayout.h"
namespace WONAPI {
// FUNCTION: WON 0x00788e20
bool ContainerConfig::HandleSameSizeLayout(bool sameWidth, bool sameHeight)
{	
	SmartPtr<SameSizeLayout> aLayout = new SameSizeLayout(sameWidth, sameHeight);

	while(true)
	{	
		Component *aComp = ReadComponent();
		aLayout->Add(aComp);
		if(EndOfString())
			break;
		else
			EnsureComma();
	}

	mContainer->AddChildLayout(aLayout.get(),mCurLayoutId);
	return true;
}
// FUNCTION: WON 0x00788f30
bool ContainerConfig::HandleCenterLayout(bool horz, bool vert)
{
	Component *aComp = ReadComponent();
	SmartPtr<CenterLayout> aLayout = new CenterLayout(aComp,horz,vert);
	while(!EndOfString())
	{	
		EnsureComma();
		aComp = ReadComponent();
		aLayout->Add(aComp);
	}

	mContainer->AddChildLayout(aLayout.get(),mCurLayoutId);
	return true;
}
// FUNCTION: WON 0x00789080
bool ContainerConfig::HandleClearRightLayout()
{
	Component *aComp = ReadComponent(); EnsureComma();
	int aPad = ReadInt(); EnsureComma();

	SmartPtr<ClearRightLayout> aLayout = new ClearRightLayout(aComp,aPad);
	while(true)
	{
		aComp = ReadComponent();
		aLayout->Add(aComp);
		if(EndOfString())
			break;

		EnsureComma();
	}

	mContainer->AddChildLayout(aLayout.get(),mCurLayoutId);
	return true;
}
// FUNCTION: WON 0x00789280
bool ContainerConfig::HandleSpaceEvenlyLayout(bool horz)
{
	Component *aSpaceBox = ReadComponent(); EnsureComma();
	int aLeftPad = ReadInt(); EnsureComma();
	int aRightPad = ReadInt(); EnsureComma();

	SmartPtr<SpaceEvenlyLayout> aLayout = new SpaceEvenlyLayout(aSpaceBox,aLeftPad,aRightPad,horz);
	while(!EndOfString())
	{
		Component *aComponent = ReadComponent();
		aLayout->Add(aComponent);
		if(EndOfString())
			break;

		EnsureComma();
	}

	mContainer->AddChildLayout(aLayout.get(),mCurLayoutId);
	return true;
}
// FUNCTION: WON 0x007894b0
bool ContainerConfig::HandleNoOverlapLayout()
{
	Component *target = ReadComponent();
	EnsureComma();

	int instructions = 0;
	while(true)
	{
		GUIString aStr = Fn3842F0(true);
		if(aStr.length()==0)
			break;

		if(aStr.CompareNoCase("Above")==0) instructions |= 0x100;
		else if(aStr.CompareNoCase("Below")==0) instructions |= 0x200;
		else if(aStr.CompareNoCase("Right")==0) instructions |= 0x400;
		else if(aStr.CompareNoCase("Left")==0) instructions |= 0x800;
		else 
			throw ConfigObjectException("Unknown no overlap instruction: " + (std::string)aStr);
	}

	EnsureComma();
	Component *ref = ReadComponent(); EnsureComma();
	int horzPad, vertPad;
	horzPad = ReadInt(); EnsureComma();
	vertPad = ReadInt(); 

	SmartPtr<NoOverlapLayout> aLayout = new NoOverlapLayout(target,instructions,ref,horzPad,vertPad);
	while(!EndOfString())
	{
		EnsureComma();
		Component *aComponent = ReadComponent();
		aLayout->Add(aComponent);
	}

	mContainer->AddChildLayout(aLayout.get());

	return true;
}
// FUNCTION: WON 0x00789860
bool ContainerConfig::HandleSizeAtLeastLayout()
{
	Component *target = ReadComponent(); EnsureComma();
	int aHorzPad = ReadInt(); EnsureComma();
	int aVertPad = ReadInt(); 

	SmartPtr<SizeAtLeastLayout> aLayout = new SizeAtLeastLayout(target,aHorzPad, aVertPad);
	while(!EndOfString())
	{
		EnsureComma();
		aLayout->Add(ReadComponent());		
	}
	mContainer->AddChildLayout(aLayout.get());
	return true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00789200
// (public: virtual __thiscall WONAPI::CenterLayout::~CenterLayout(void)    [??1CenterLayout@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007891e0
// (public: virtual void * __thiscall WONAPI::CenterLayout::`scalar deleting destructor'(unsigned int)    [??_GCenterLayout@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00789410
// (public: virtual void * __thiscall WONAPI::SpaceEvenlyLayout::`scalar deleting destructor'(unsigned int)    [??_GSpaceEvenlyLayout@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007897c0
// (public: virtual void * __thiscall WONAPI::NoOverlapLayout::`scalar deleting destructor'(unsigned int)    [??_GNoOverlapLayout@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007899d0
// (public: virtual void * __thiscall WONAPI::SizeAtLeastLayout::`scalar deleting destructor'(unsigned int)    [??_GSizeAtLeastLayout@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00789430
// (public: virtual __thiscall WONAPI::SpaceEvenlyLayout::~SpaceEvenlyLayout(void)    [??1SpaceEvenlyLayout@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007897e0
// (public: virtual __thiscall WONAPI::NoOverlapLayout::~NoOverlapLayout(void)    [??1NoOverlapLayout@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x007899f0
// (public: virtual __thiscall WONAPI::SizeAtLeastLayout::~SizeAtLeastLayout(void)    [??1SizeAtLeastLayout@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a2cc0
// (void __cdecl std::_Construct(class WONAPI::Component * *,class WONAPI::Component * const &)    [?_Construct@std@@YAXPAPAVComponent@WONAPI@@ABQAV23@@Z])
