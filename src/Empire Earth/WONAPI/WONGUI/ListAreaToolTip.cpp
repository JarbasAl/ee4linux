// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <deque>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
using namespace WONAPI;
// FUNCTION: WON 0x006fb7b0
ListAreaToolTip::ListAreaToolTip(ListArea* area) {
    mListArea=area;
    mItem=NULL;
}
// FUNCTION: WON 0x006fb8d0
void ListAreaToolTip::Paint(Graphics& g) {
    Component::Paint(g);
    if(mItem.get()==NULL) return;
    ColorScheme* scheme=g.mColorScheme;
    if(GetOwnColorScheme()==NULL) g.mColorScheme=mListArea->GetColorScheme();
    int x=mListArea->mLeftPad,y=-mItem->mTop+2;
    g.SetSysColor(StandardColor_ToolTipBack);
    g.FillRect(0,0,Width(),Height());
    g.PushClipRect(0,2,Width(),Height()-4);
    g.Translate(x,y);
    mListArea->VirtualD4(g);
    g.Translate(-x,-y);
    g.PopClipRect();
    g.SetSysColor(StandardColor_ToolTipText);
    g.DrawRect(0,0,Width(),Height());
    g.mColorScheme=scheme;
}
// FUNCTION: WON 0x006fb9b0
void ListAreaToolTip::DoTip(ListItem* item) {
    if(mItem.get()==item && mItem.get()!=NULL && mParent!=NULL) return;
    EndTip();
    mItem=item;
    if(mItem.get()==NULL) return;
    int x=0,y=mListArea->mTopPad-mListArea->mVertOffset;
    mListArea->Fn2F56D0(x,y);
    y+=mItem->mTop-2;
    PopupParams params(this,0x6247,mListArea,x,y);
    int width=mItem->mL10+mListArea->mLeftPad+mListArea->mRightPad;
    if(width<mListArea->Width()) width=mListArea->Width();
    Fn2F4B90(width,mItem->mHeight+4);
    mListArea->GetRoot()->OpenPopup(params);
}
// FUNCTION: WON 0x006fbab0
void ListAreaToolTip::EndTip() {
    if(mParent!=NULL) mListArea->GetRoot()->ClosePopup(this);
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006fb840
// (public: virtual void * __thiscall WONAPI::ListAreaToolTip::`scalar deleting destructor'(unsigned int)    [??_GListAreaToolTip@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006fb860
// (ListAreaToolTip implicit destructor)
