// Genuine line builder retained-receiver state setup/finalization, support only.
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
#include "ee/wonapi/WONLobbyLineLayout.h"
namespace WONAPI {
// FUNCTION: WON 0x0070b560
void LineBuilder::StartBuild(LineElement* element)
{
    mLine=element;
    mMaxAscent=mMaxDescent=mTopAlignHeight=mWidth=0;
}
// FUNCTION: WON 0x0070b790
void LineBuilder::FinishBuild()
{
    mLine->mWidth=mWidth;
    mLine->mHeight=GetHeight();
    mLine->mBaseline=mMaxAscent;
}
}
