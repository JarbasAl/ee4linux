// Genuine complete Container setup; no receiver allocation, all support0.
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
// FUNCTION: WON 0x006f5b70
Container::Container()
{
    mMouseChild=NULL;
    mFocusChild=NULL;
    mCurrentCursor=NULL;
    mChildNeedsTimer=false;
    mMouseWheelGoesToFocusChild=mMouseWheelGoesToFocusChildDef;
    mOnlyChildInput=false;
    mIsolateTabFocus=false;
    mRemovingAllChildren=false;
    mHasUsedDialogKeys=false;
    mModalCount=0;
    mNextFocusId=0;
    SetComponentFlags(8,true);
}
}
