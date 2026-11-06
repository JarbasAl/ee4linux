// BadUserDialog source-intent, no canonical/compile authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyDialogs.h"
using namespace WONAPI;
// Actual global RTTI BadUserItem, complete50 own tail; constructor remains original external3f5290.
class BadUserItem : public ListItem {
public:
    BadUserItem(const RoutingBadUser& inUser,bool inSimple);
    virtual void Virtual04(Graphics& inGraphics,ListArea* inList);
    virtual void Virtual08(ListArea* inList);
    virtual int Virtual10(const ListItem* inOther,ListArea* inList) const;
    SmartPtr<Font> mFont; //18
    GUIString mStrings[6]; //1c..30
    unsigned long mValues[6]; //34..48; onlyindices1/2/4 initialized and read numerically
    bool mIsSimple; //4c; natural padding to50
};
// FUNCTION: WON 0x7f5290
BadUserItem::BadUserItem(const RoutingBadUser& inUser, bool inSimple)
{
    mIsSimple = inSimple;
    mStrings[0] = GUIString(new GUIStringData(inUser.mName));
    char aBuffer[100];
    sprintf(aBuffer, "%d", inUser.mWONId);
    mStrings[1] = GUIString(new GUIStringData(aBuffer));
    mValues[1] = inUser.mWONId;

    unsigned long aDuration = inUser.mExpirationDiff;
    GUIString aUnit;
    if (aDuration > 86400) {
        aDuration /= 86400;
        aUnit = aDuration > 1 ? ::BadUserDialog_XDays_String : ::BadUserDialog_OneDay_String;
    } else if (aDuration > 3600) {
        aDuration /= 3600;
        aUnit = aDuration > 1 ? ::BadUserDialog_XHours_String : ::BadUserDialog_OneHour_String;
    } else if (aDuration > 60) {
        aDuration /= 60;
        aUnit = aDuration > 1 ? ::BadUserDialog_XMinutes_String : ::BadUserDialog_OneMinute_String;
    } else if (aDuration > 0) {
        aUnit = aDuration > 1 ? ::BadUserDialog_XSeconds_String : ::BadUserDialog_OneSecond_String;
    } else {
        aUnit = ::BadUserDialog_Infinite_String;
    }
    sprintf(aBuffer, "%d", aDuration);
    mValues[2] = inUser.mExpirationDiff;
    mStrings[2] = LobbyFormatString(aUnit, GUIString(new GUIStringData(aBuffer)));
    mStrings[3] = GUIString(new GUIStringData(inUser.mModeratorName));
    sprintf(aBuffer, "%d", inUser.mModeratorWONId);
    mStrings[4] = GUIString(new GUIStringData(aBuffer));
    mValues[4] = inUser.mModeratorWONId;
    mStrings[5] = GUIString(new GUIStringData(inUser.mModeratorComment));
}

// FUNCTION: WON 0x7f5790
void BadUserItem::Virtual08(ListArea* inList)
{
    mFont=inList->mFont;
    mHeight=mFont->GetHeight();
}
// FUNCTION: WON 0x7f5800
void BadUserItem::Virtual04(Graphics& inGraphics,ListArea* inList)
{
    DWORD aColor=inList->GetTextColor();
    if(mB08) aColor=inList->GetSelColor().GetForegroundColor(inGraphics,aColor);
    bool aSelected=mB08;
    int aColumn=0;
    if(!mIsSimple) aColumn=static_cast<MultiListArea*>(inList)->GetColumnPaintContext();
    if((mIsSimple || aColumn==-1) && aSelected) {
        inGraphics.SetColor(inList->GetSelColor().GetBackgroundColor(inGraphics));
        int aWidth=inList->mL12C;
        int aHeight=mFont->GetHeight();
        inGraphics.FillRect(0,0,aWidth,aHeight);
        if(!mIsSimple) return;
    }
    if(aColumn>=0 && aColumn<=5) {
        inGraphics.SetColor(aColor);
        inGraphics.SetFont(mFont.get());
        inGraphics.DrawString(mStrings[aColumn],0,0);
    }
}
// FUNCTION: WON 0x7f58c0
int BadUserItem::Virtual10(const ListItem* inOther,ListArea* inList) const
{
    int aColumn=static_cast<MultiListArea*>(inList)->GetSortColumn();
    if(aColumn<0 || aColumn>5) return this<inOther;
    const BadUserItem* anOther=static_cast<const BadUserItem*>(inOther);
    switch(aColumn) {
    case 1:
    case 2:
    case 4:
        return mValues[aColumn]-anOther->mValues[aColumn];
    default:
        return mStrings[aColumn].mData->Compare(anOther->mStrings[aColumn].mData.get());
    }
}
namespace WONAPI {
// Simple false-state/default ownership initializer support, zero breadth quota.
BadUserDialog::BadUserDialog() : mIsSimple(false) {}
// Ordinary destructor implicit: zero credit, preserve original no own-vptr store.
// FUNCTION: WON 0x7f5a90
void BadUserDialog::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(aConfig,mUserList,"UserList",__FILE__,157);
    SafeGetComponent(aConfig,mSimpleUserList,"SimpleUserList",__FILE__,158);
    SafeGetComponent(aConfig,mUserListBox,"UserListBox",__FILE__,159);
    SafeGetComponent(aConfig,mSimpleUserListBox,"SimpleUserListBox",__FILE__,160);
}
// FUNCTION: WON 0x7f5b20
void BadUserDialog::SetUserList(const RoutingBadUserList& inList,bool inSimple)
{
    mIsSimple=inSimple;
    mUserListBox->SetVisible(!inSimple);
    mSimpleUserListBox->SetVisible(inSimple);
    ListArea* aList=mIsSimple ? mSimpleUserList.get() : mUserList.get();
    aList->SetVisible(true);
    aList->Fn2FAA00();
    aList->Fn2FAA50(true);
    for(RoutingBadUserList::const_iterator anItr=inList.begin();anItr!=inList.end();++anItr)
        aList->InsertItem(new BadUserItem(*anItr,inSimple),-1);
    aList->Fn2FAA10();
}
// FUNCTION: WON 0x7f5c20
void BadUserDialog::GetIds(IdList& outIds)
{
    ListArea* aList=mIsSimple ? mSimpleUserList.get() : mUserList.get();
    outIds.clear();
    aList->RewindSelections();
    while(aList->HasMoreSelections()) {
        BadUserItem* anItem=static_cast<BadUserItem*>(aList->GetNextSelection());
        unsigned long anId=anItem->mValues[1]; //actual scalar snapshot3f5ca5 before list-node allocation
        outIds.push_back(anId);
    }
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f5710
// (public: virtual __thiscall BadUserItem::~BadUserItem(void)    [??1BadUserItem@@UAE@XZ])
// TEMPLATE: WON 0x007fa4a0

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f5960
// (public: virtual void * __thiscall WONAPI::BadUserDialog::`scalar deleting destructor'(unsigned int)    [??_GBadUserDialog@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f5980
// (public: virtual __thiscall WONAPI::BadUserDialog::~BadUserDialog(void)    [??1BadUserDialog@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f56f0
// (public: virtual void * __thiscall BadUserItem::`scalar deleting destructor'(unsigned int)    [??_GBadUserItem@@UAEPAXI@Z])
// TEMPLATE: WON 0x007f5920
// (public: __thiscall WONAPI::BadUserDialog::BadUserDialog(void)    [??0BadUserDialog@WONAPI@@QAE@XZ])
