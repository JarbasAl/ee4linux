// Genuine SDK GUIString substring value factories; support0.
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
namespace WONAPI {
// FUNCTION: WON 0x006f8b90
GUIString GUIString::substr(unsigned short offset,unsigned short count) const
{
    return GUIString(new GUIStringData(mData.get(),offset,count));
}
// FUNCTION: WON 0x00739c80
GUIString GUIString::substr(unsigned short offset) const
{
    return GUIString(new GUIStringData(mData.get(),offset,length()-offset));
}
}
