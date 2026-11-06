// Complete original-backed Component source completion; all selected methods support0.
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
namespace WONAPI {
// ChildLayoutManager SDKimplicit destructor source is deferred; current proven native2f5da0 remains call-only.
// FUNCTION: WON 0x006f5e80
Container::~Container()
{
    for(std::list<SmartPtr<Component> >::iterator child=mChildList.begin();
        child!=mChildList.end();++child)
        (*child)->mParent=NULL;
}
}

// Compiler-generated and header-inline functions this unit emits:

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006f5cc0
// (public: virtual void * __thiscall WONAPI::Container::`scalar deleting destructor'(unsigned int)    [??_GContainer@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071adb0
// (public: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::Component>,class WONAPI::SmartPtr<class WONAPI::Component>,struct std::_Setk<class WONAP)
// TEMPLATE: WON 0x0071bd50
// (public: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::Component>,class WONAPI::SmartPtr<class WONAPI::Component>,struct std::_Setk<class WONAP)
// TEMPLATE: WON 0x006f5ce0
// (public: __thiscall std::set<class WONAPI::SmartPtr<class WONAPI::Component>,struct WONAPI::SmartPtr<class WONAPI::Component>::Comp,class std::allocato)
// TEMPLATE: WON 0x006f5da0
// (public: virtual __thiscall WONAPI::ChildLayoutManager::~ChildLayoutManager(void))
