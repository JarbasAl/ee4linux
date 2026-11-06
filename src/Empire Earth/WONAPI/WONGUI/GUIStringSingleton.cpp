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
// Original pooled UTF16 provider, already retained by current EEWonNet.cpp;
// declaration only, no new global allocation or constant-value assumption.
extern unsigned short gEmptyWideString[];
namespace WONAPI {
// FUNCTION: WON 0x00728450
GUIStringData* GetEmptyGUIStringData()
{
    static SmartPtr<GUIStringData> aNullStr=new GUIStringData(gEmptyWideString);
    return aNullStr.get();
}
}
