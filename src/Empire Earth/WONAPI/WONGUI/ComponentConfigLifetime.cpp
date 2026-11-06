
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
// FUNCTION: WON 0x007825b0
ComponentConfig::ComponentConfig()
{
    mHasSetWidth=false;
    mHasSetHeight=false;
    mHaveInitedComponents=false;
}
// FUNCTION: WON 0x00782690
ComponentConfig::~ComponentConfig()
{
}
// FUNCTION: WON 0x00787f90
ContainerConfig::ContainerConfig()
{
    mLayoutRef=NULL;
    mCurLayoutId=0;
}
}

namespace WONAPI {
// Genuine original SDK class-static object definitions; data only, zero authored CPP function bodies.
SmartPtr<ResourceConfigTable> ComponentConfig::mResourceConfigTable;
std::string ComponentConfig::mResourceFolder;
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00782670
// (public: virtual void * __thiscall WONAPI::ComponentConfig::`scalar deleting destructor'(unsigned int)    [??_GComponentConfig@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0079b9b0
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)
// TEMPLATE: WON 0x0079d100
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class s)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a32e0
// (public: void * __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::Smart)

// TEMPLATE: WON 0x007824c0
// TEMPLATE: WON 0x007824d0
// TEMPLATE: WON 0x007824e0
// TEMPLATE: WON 0x007824f0
// TEMPLATE: WON 0x00782510
// TEMPLATE: WON 0x00782520
// TEMPLATE: WON 0x00782540
// TEMPLATE: WON 0x00782550
