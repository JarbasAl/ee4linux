// Genuine GUIStringData storage resize; support-only complete source.
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
// FUNCTION: WON 0x0070a1d0
void GUIStringData::resize(int length,bool grow)
{
    Reserve(length,grow);
    mLength=(unsigned short)length;
    mText[mLength]=0;
}
}
