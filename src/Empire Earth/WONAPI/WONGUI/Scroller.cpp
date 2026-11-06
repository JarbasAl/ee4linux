// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;
// FUNCTION: WON 0x00705580
void Scroller::SetScrollArea(ScrollArea* area) { mScrollArea=area; }
// FUNCTION: WON 0x007055e0
void Scroller::SetOffsets(int horizontal,int vertical) {
    if(mScrollArea->mHorzOffset==horizontal && mScrollArea->mVertOffset==vertical) return;
    mScrollArea->mHorzOffset=horizontal;
    mScrollArea->mVertOffset=vertical;
    mScrollArea->VirtualCC(true);
}
// FUNCTION: WON 0x00705630
void Scroller::SetVertOffset(int vertical) { SetOffsets(mScrollArea->mHorzOffset,vertical); }
// FUNCTION: WON 0x00705650
void Scroller::SetHorzOffset(int horizontal) { SetOffsets(horizontal,mScrollArea->mVertOffset); }
// FUNCTION: WON 0x00705670
void Scroller::Virtual2C(int x,int y,int button) {
    Container::Virtual2C(x,y,button);
    if(button==0) {
        if(mFocusChild==NULL) mScrollArea->Fn2F5890();
        else Fn2F5890();
    }
}
// FUNCTION: WON 0x007056c0
bool Scroller::Virtual3C(int amount) {
    if(Container::Virtual3C(amount)) return true;
    if(mScrollArea.get()!=NULL) return mScrollArea->Virtual3C(amount);
    return false;
}
