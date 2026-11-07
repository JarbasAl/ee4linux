// Genuine ResourceConfigTable20 query/glue/factory operations; original global maps only.
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
#include "ee/wonapi/WONLobbyResourceConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00793e50
ResourceConfig* ResourceConfigTable::SafeGetConfig(const std::string &theName)
{
	ResourceMap::iterator anItr = mResourceMap.find(theName);
	if(anItr!=mResourceMap.end())
		return anItr->second.get();
	else
		throw ConfigObjectException("Resource group not found: " + theName);
}
// FUNCTION: WON 0x00793f40
void ResourceConfigTable::RegisterGlueResourceFunc(GlueResourceFunc theFunc)
{
	mGlueResourceFuncSet.insert(theFunc);
}
// FUNCTION: WON 0x00793fe0
void ResourceConfigTable::SafeGlueResources()
{
	GlueResourceFuncSet::const_iterator anItr = mGlueResourceFuncSet.begin();
	while(true)
	{
		if(anItr==mGlueResourceFuncSet.end())
			break;
		(*anItr)(this);
		++anItr;
	}
}
// FUNCTION: WON 0x00794040
void ResourceConfigTable::AddAllocator(const std::string &theName, ComponentConfigFactory theConfigFactory, ComponentFactory theComponentFactory)
{
	ConfigAllocatorMap::iterator anItr = mConfigAllocatorMap.insert(ConfigAllocatorMap::value_type(theName,ConfigAllocator(NULL,NULL))).first;
	ConfigAllocator &anAlloc = anItr->second;
	if(anAlloc.mConfigFactory==NULL)
	{
		anAlloc.mConfigFactory = theConfigFactory;
		anAlloc.mComponentFactory = theComponentFactory;
	}
}
// FUNCTION: WON 0x007941d0
void ResourceConfigTable::RegisterControlId(const std::string &theName, int theVal)
{
	mControlIdMap[theName] = theVal;
}
// FUNCTION: WON 0x007941f0
bool ResourceConfigTable::AllocateConfig(const std::string &theName, SmartPtr<ComponentConfig> &theConfig, SmartPtr<Component> &theComponent)
{
	ConfigAllocatorMap::iterator anItr = mConfigAllocatorMap.find(theName);
	if(anItr==mConfigAllocatorMap.end())
		return false;

	ConfigAllocator &anAlloc = anItr->second;
	theConfig = anAlloc.mConfigFactory();
	theComponent = anAlloc.mComponentFactory();	
	return true;
}
// FUNCTION: WON 0x007942f0
bool ResourceConfigTable::GetControlId(const std::string &theName, int &theVal)
{
	ControlIdMap::iterator anItr = mControlIdMap.find(theName);
	if(anItr==mControlIdMap.end())
		return false;
	
	theVal = anItr->second;
	return true;	
}

// Genuine original SDK class-static object definitions; data only, zero authored CPP function bodies.
ResourceConfigTable::ControlIdMap ResourceConfigTable::mControlIdMap;
ResourceConfigTable::ConfigAllocatorMap ResourceConfigTable::mConfigAllocatorMap;


// SOURCE_ONLY_NO_NATIVE_ID: same-snapshot SDK compatible resource API completion.
ResourceConfig* ResourceConfigTable::GetConfig(const std::string &theName)
{
	ResourceMap::iterator anItr = mResourceMap.find(theName);
	if(anItr!=mResourceMap.end())
		return anItr->second;
	else
		return NULL;
}

void ResourceConfigTable::ClearGlueResourceFuncs()
{
	mGlueResourceFuncSet.clear();
}

void ResourceConfigTable::ClearAllocators()
{
	mConfigAllocatorMap.clear();
}

void ResourceConfigTable::ClearControlIds()
{
	mControlIdMap.clear();
}

void ResourceConfigTable::ClearResources()
{
	mResourceMap.clear();
}

void ResourceConfigTable::ClearAll()
{
	ClearGlueResourceFuncs();
	ClearAllocators();
	ClearControlIds();
	ClearResources();
}


// Compiler-generated and header-inline functions this unit emits:
// FUNCTION: WON 0x00794360
bool ScrollAreaConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="BACKGROUND") mScrollArea->SetBackground(ReadBackground());
    else if(instruction=="BORDERPADDING")
    {
        int left=ReadInt();EnsureComma();
        int top=ReadInt();EnsureComma();
        int right=ReadInt();EnsureComma();
        int bottom=ReadInt();
        mScrollArea->SetBorderPadding(left,top,right,bottom);
    }
    else return ContainerConfig::Virtual24(instruction);
    return true;
}
// FUNCTION: WON 0x00794540
bool ScrollerConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SCROLLAREA")
    {
        ScrollArea* area=dynamic_cast<ScrollArea*>(ReadComponent());
        if(area==NULL) throw ConfigObjectException("Expecting ScrollArea");
        mScroller->SetScrollArea(area);
        return true;
    }
    return ContainerConfig::Virtual24(instruction);
}
// FUNCTION: WON 0x007946e0
ScrollbarCondition ScrollbarScrollerConfig::GetScrollbarCondition()
{
    GUIString condition=Fn3842F0(false);
    if(condition.CompareNoCase("DISABLE")==0) return ScrollbarCondition_Disable;
    if(condition.CompareNoCase("ENABLE")==0) return ScrollbarCondition_Enable;
    if(condition.CompareNoCase("CONDITIONAL")==0) return ScrollbarCondition_Conditional;
    throw ConfigObjectException("Invalid scrollbar condition: "+(std::string)condition);
}
// FUNCTION: WON 0x007948a0
bool ScrollbarScrollerConfig::Virtual24(const std::string& instruction)
{
    if(instruction=="SCROLLBARCONDITIONS")
    {
        ScrollbarCondition horz=GetScrollbarCondition();EnsureComma();
        ScrollbarCondition vert=GetScrollbarCondition();
        mScrollbarScroller->SetScrollbarConditions(horz,vert);
        return true;
    }
    return ScrollerConfig::Virtual24(instruction);
}


// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK button-scroller configuration policy.
ScrollButtonCondition ButtonScrollerConfig::GetScrollButtonCondition()
{
	GUIString aStr = Fn3842F0(false);
	if(aStr.CompareNoCase("DISABLE")==0)
		return ScrollButtonCondition_Disable;
	else if(aStr.CompareNoCase("ENABLE")==0)
		return ScrollButtonCondition_Enable;
	else if(aStr.CompareNoCase("CONDITIONAL")==0)
		return ScrollButtonCondition_Conditional;
	else if(aStr.CompareNoCase("PARTIALCONDITIONAL")==0)
		return ScrollButtonCondition_PartialConditional;
	else
		throw ConfigObjectException("Invalid scrollbar condition: " + (std::string)aStr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
bool ButtonScrollerConfig::Virtual24(const std::string &theInstruction)
{
	if(theInstruction=="SCROLLBARCONDITIONS")
	{
		ScrollButtonCondition horz, vert;
		horz = GetScrollButtonCondition(); EnsureComma();
		vert = GetScrollButtonCondition();
		mButtonScroller->SetScrollButtonConditions(horz,vert);
	}
	else
		return ScrollerConfig::Virtual24(theInstruction);
	
	return true;
}

///////////////////////////////////////////////////////////////////////////////
// TEMPLATE: WON 0x00790520
// TEMPLATE: WON 0x00790530
// (_$E71    [_$E71])
// TEMPLATE: WON 0x00790540
// TEMPLATE: WON 0x00790550
// TEMPLATE: WON 0x00790630
// TEMPLATE: WON 0x00790640
// (_$E77    [_$E77])
// TEMPLATE: WON 0x00790650
// TEMPLATE: WON 0x00790660
// TEMPLATE: WON 0x00793dc0
// (public: __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::SmartPtr<cla)
// TEMPLATE: WON 0x0079b610
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,int,struct WONAPI::StringLessNoCas)
// TEMPLATE: WON 0x0079b660
// TEMPLATE: WON 0x0079b7d0
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct WONAPI::ResourceConfigTable)
// (0x0079ba60 _Tree::lower_bound: no longer emitted out of line since the derived map; caller inlines it)
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x0079c2d0
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_)
// TEMPLATE: WON 0x0079c680
// (protected: class std::_Tree<void (__cdecl*)(class WONAPI::ResourceConfigTable *),void (__cdecl*)(class WONAPI::ResourceConfigTable *),struct std::_Set)
// TEMPLATE: WON 0x0079cbe0
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x0079fa00
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x0079ff20
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class s)
// TEMPLATE: WON 0x0079ff60
// TEMPLATE: WON 0x007a0440
// TEMPLATE: WON 0x007a0480
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_)
// TEMPLATE: WON 0x007a07d0
// TEMPLATE: WON 0x007a0cb0
// TEMPLATE: WON 0x007a0cf0
// (protected: struct std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic)
// TEMPLATE: WON 0x007a0d40
// (public: class std::_Tree<void (__cdecl*)(class WONAPI::ResourceConfigTable *),void (__cdecl*)(class WONAPI::ResourceConfigTable *),struct std::_Setk<v)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a11f0
// (protected: void __thiscall std::_Tree<void (__cdecl*)(class WONAPI::ResourceConfigTable *),void (__cdecl*)(class WONAPI::ResourceConfigTable *),struct)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a1d50
// (public: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std:)
// TEMPLATE: WON 0x007a2b50
// TEMPLATE: WON 0x007a2d80
// TEMPLATE: WON 0x007a3380
// (public: void * __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::Smart)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a4cb0
// (public: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std:)

// Compiler-generated and header-inline functions this unit emits:
}
