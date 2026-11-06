// Complete original-backed GUI values; source-only, no runtime substitution.
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
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONStringLocalize.h"
namespace WONAPI {
// Actual original SDK static-overload behavior through the established canonical
// global cdecl call-view names. No new API, provider, or wrapper layer is added.
// FUNCTION: WON 0x0074a100
GUIString LobbyFormatString(const GUIString& format,const GUIString& first)
{
    const GUIString* subs[]={&first};
    return StringLocalize::GetStr(format,subs,1);
}
// FUNCTION: WON 0x0074a130
GUIString LobbyFn34A130(const GUIString& format,const GUIString& first,const GUIString& second)
{
    const GUIString* subs[]={&first,&second};
    return StringLocalize::GetStr(format,subs,2);
}
// FUNCTION: WON 0x0074a170
GUIString LobbyFn34A170(const GUIString& format,const GUIString& first,const GUIString& second,const GUIString& third)
{
    const GUIString* subs[]={&first,&second,&third};
    return StringLocalize::GetStr(format,subs,3);
}
}
