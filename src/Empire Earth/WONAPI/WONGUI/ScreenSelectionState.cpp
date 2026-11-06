// Native screen transitions and edit-selection state.
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
// FUNCTION: WON 0x00706a70
void ScreenContainer::AddScreen(Component* screen)
{
    screen->SetVisible(false);
    Fn2F6140(screen,3,this,0,0,0,0,0);
    Fn2F6A60(screen);
}

// FUNCTION: WON 0x00706ab0
void ScreenContainer::ShowScreen(Component* screen,bool takeFocus)
{
    if(screen==mCurScreen)
        return;
    if(mCurScreen!=NULL)
        mCurScreen->SetVisible(false);
    screen->SetVisible(true);
    mCurScreen=screen;
    if(screen!=NULL && takeFocus)
        screen->Fn2F5890();
    Invalidate();
}

// FUNCTION: WON 0x006f84b0
void InputBox::Fn2F84B0(bool allowSelection)
{
    if(allowSelection==mAllowSelection)
        return;
    mAllowSelection=allowSelection;
    if(allowSelection)
        SetDefaultCursor(Cursor::GetStandardCursor(StandardCursor_IBeam));
    else
        SetDefaultCursor(NULL);
    mSelecting=false;
    if(!mAllowSelection && mSelStartChar!=mSelEndChar) {
        mSelStartChar=mSelEndChar=0;
        Invalidate();
    }
}
}
