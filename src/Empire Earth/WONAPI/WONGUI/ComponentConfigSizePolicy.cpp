// Native desired-size policy and borrowed named-child lookup.
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
// FUNCTION: WON 0x007860b0
void ComponentConfig::SetDesiredSizeIfNecessary()
{
    if(mComponent->Width()>0)
        mHasSetWidth=true;
    if(mComponent->Height()>0)
        mHasSetHeight=true;
    if(mHasSetWidth && mHasSetHeight)
        return;
    int width=0,height=0;
    mComponent->Virtual0C(width,height);
    if(mHasSetWidth) {
        width=mComponent->Width();
        mHasSetWidth=true;
    }
    if(mHasSetHeight) {
        height=mComponent->Height();
        mHasSetHeight=true;
    }
    mComponent->Fn2F4B90(width,height);
}

// FUNCTION: WON 0x00786860
Component* ComponentConfig::Fn386860(const std::string& name)
{
    std::map<std::string,SmartPtr<ComponentConfig>,StringLessNoCase>::iterator i=mConfigMap.find(name);
    if(i==mConfigMap.end())
        return NULL;
    return i->second->mComponent.get();
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007a0780
// (protected: struct std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic)
