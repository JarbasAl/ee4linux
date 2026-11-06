// RootScreen whole-family source draft, no canonical markers or credit yet.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <string>
#include <list>
#include <map>
#include <vector>
#include <windows.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyEvents.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
namespace WONAPI {
// Scratch only; requires proved pointer-only TabCtrl/TabBar contracts.
// 3c4880..3c492c172B. Each call reloads mTabCtrl and current tabbar.
// FUNCTION: WON 0x007c4880
void RootScreen::EnterGameTab(bool enter)
{
    if(enter)
    {
        mTabCtrl->Fn3093D0(mGameTab,true);
        if(mTabCtrl->mBottomTabBar->Fn308280()!=mGameTab)
        {
            mLastJoinTab=mTabCtrl->mBottomTabBar->Fn308280();
            mTabCtrl->Fn309350(mGameTab,true);
        }
    }
    else
    {
        if(mTabCtrl->mBottomTabBar->Fn308280()==mGameTab)
            mTabCtrl->Fn309350(mLastJoinTab,true);
        mTabCtrl->Fn3093D0(mGameTab,false);
    }
}
// 3c4930..3c4945 21B
// FUNCTION: WON 0x007c4930
void RootScreen::ShowLobbyTab(){mTabCtrl->Fn309350(mLobbyTab,true);}
// 3c4950..3c4979 41B
// FUNCTION: WON 0x007c4950
void RootScreen::HandleJoinedGame()
{
    LobbyGame* aGame=GetMyLobbyGame();
    if(aGame!=NULL && aGame->Fn3A6D40(mRoomSpecFlags)) EnterGameTab(true);
}
// 3c4980..3c4988 8B
// FUNCTION: WON 0x007c4980
void RootScreen::HandleLeftGame(){EnterGameTab(false);}
// Original3c46d0..3c4878/424B; function-local banner-height static, no synthetic global.
// FUNCTION: WON 0x007c46d0
void RootScreen::Virtual08()
{
    if(mLowerLayoutPoint.get()!=NULL)
    {
        if(mHideLowerControls)
        {
            mLowerDivider->SetVisible(false);
            mSierraLogo->SetVisible(false);
            mLowerBanner->SetVisible(false);
            mLowerBannerFrame->SetVisible(false);
            mBackButton->SetVisible(false);
            mLowerLayoutPoint->Fn2F4B70(mLowerLayoutPoint->Left(),Height());
            Container::Virtual08();
            return;
        }
        static int aBannerHeight=mLowerBanner->Height();
        mLowerDivider->SetVisible(true);
        mSierraLogo->SetVisible(true);
        mBackButton->SetVisible(true);
        int aLowerHeight=mLowerDivider->Height();
        if(LobbyFn35E310())
        {
            mLowerBannerFrame->SetVisible(true);
            mLowerBanner->SetVisible(true);
            mLowerBanner->Fn2F4B90(mLowerBanner->Width(),aBannerHeight);
            aLowerHeight+=aBannerHeight+2*mLowerBannerFrame->mThickness;
        }
        else
        {
            mLowerBannerFrame->SetVisible(false);
            mLowerBanner->SetVisible(false);
            int aHeight=mBackButton->Height();
            if(mSierraLogo->Height()>aHeight) aHeight=mSierraLogo->Height();
            aLowerHeight+=aHeight;
            mLowerBanner->Fn2F4B90(mLowerBanner->Width(),aHeight);
        }
        mLowerLayoutPoint->Fn2F4B70(mLowerLayoutPoint->Left(),Height()-(mLowerDivider->Height()-2)/2-aLowerHeight);
    }
    Container::Virtual08();
}
// ActualRootScreen dispatch3c4ae0..3c4b66/134B, alwaysComponentfallback aftertabhandler.
// FUNCTION: WON 0x007c4ae0
void RootScreen::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType)
    {
    case 29: HandleTabCtrlTabSelected(inEvent); break;
    case 15:
        if(inEvent->GetControlId()==3) { Fn2F6290(); return; }
        break;
    case 100014: HandleEnteredScreen(); return;
    case 100029: HandleJoinedGame(); return;
    case 100032: HandleLeftGame(); return;
    }
    Component::HandleComponentEvent(inEvent);
}
// Original3c3c00/235B. Fundamental fields initialized in body as actually recorded.
// FUNCTION: WON 0x007c3c00
RootScreen::RootScreen(bool inIsLan)
{
    mIsLan=inIsLan;
    if(inIsLan) mRoomSpecFlags=8;
    else mRoomSpecFlags=4;
    mLobbyTab=mLanTab=mDirectConnectTab=mLastJoinTab=-1;
    mHideLowerControls=false;
    mIsSwat=false;
}
// Original3c3d10/432B; explicit own-vptr followed by real automatic cleanup.
// FUNCTION: WON 0x007c3d10
RootScreen::~RootScreen() {}
// FUNCTION: WON 0x007c3ec0
bool RootScreen::OnDirectConnectTab()
{
    return mIsLan && mTabCtrl->mBottomTabBar->Fn308280()==mDirectConnectTab;
}
// FUNCTION: WON 0x007c3f00
bool RootScreen::OnLanTab()
{
    return mIsLan && mTabCtrl->mBottomTabBar->Fn308280()==mLanTab;
}
// FUNCTION: WON 0x007c4990
void RootScreen::HandleEnteredScreen()
{
    mHelpTab->Fn3BC2F0();
    if(mNewsTab.get()!=NULL) mNewsTab->Fn3BC2F0();
    TabCtrl* aTabs=mTabCtrl.get();
    aTabs->Fn309370(LobbyFn35E2D0(),true);
}
// FUNCTION: WON 0x007c4620
SetRoomSpecFlagsEvent::SetRoomSpecFlagsEvent(int inFlags)
    :LobbyEvent(100054),mFlags(inFlags) {}

// Original3c3f40..3c461a/1754B. Every widget owned member and temporary typed.
// FUNCTION: WON 0x007c3f40
void RootScreen::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* aConfig=static_cast<ComponentConfigInit&>(inInit).mConfig;
    mIsSwat=aConfig->Fn386860("SWATUJDEF")!=NULL;
    SafeGetComponent(aConfig,mTabCtrl,"TabCtrl",__FILE__,79);
    SafeGetComponent(aConfig,mLowerLayoutPoint,"LowerLayoutPoint",__FILE__,80);
    SafeGetComponent(aConfig,mLowerDivider,"LowerDivider",__FILE__,81);
    SafeGetComponent(aConfig,mSierraLogo,"SierraLogo",__FILE__,82);
    SafeGetComponent(aConfig,mLowerBanner,"LowerBanner",__FILE__,83);
    SafeGetComponent(aConfig,mLowerBannerFrame,"LowerBannerFrame",__FILE__,84);
    SafeGetComponent(aConfig,mBackButton,"BackButton",__FILE__,85);
    SafeGetComponent(aConfig,mHelpTab,"HelpTab",__FILE__,86);
    SmartPtr<Container> aTab;
    SafeGetComponent(aConfig,aTab,"GameTab",__FILE__,89);
    mGameTab=mTabCtrl->Fn309390(aTab.get());
    mTabCtrl->Fn3093D0(mGameTab,false);
    if(!mIsLan)
    {
        SafeGetComponent(aConfig,mNewsTab,"NewsTab",__FILE__,95);
        SafeGetComponent(aConfig,aTab,"LobbyTab",__FILE__,96);
        mLobbyTab=mTabCtrl->Fn309390(aTab.get());
        Fn2F52D0(new SetRoomSpecFlagsEvent(4));
    }
    else
    {
        SafeGetComponent(aConfig,aTab,"LanTab",__FILE__,101);
        mLanTab=mTabCtrl->Fn309390(aTab.get());
        SafeGetComponent(aConfig,aTab,"DirectConnectTab",__FILE__,102);
        mDirectConnectTab=mTabCtrl->Fn309390(aTab.get());
        Fn2F52D0(new SetRoomSpecFlagsEvent(8));
    }
    Fn2F5360(1);
}
// Original3c49d0..3c4ad6/262B; shared static prior window-height record.
// FUNCTION: WON 0x007c49d0
void RootScreen::HandleTabCtrlTabSelected(ComponentEvent* inEvent)
{
    if(inEvent->mComponent!=mTabCtrl.get()) return;
    if(mIsSwat)
    {
        static bool aResized=false;
        static int aOldHeight;
        Window* aWindow=GetRoot();
        WONRectangle aWindowRect;
        WONRectangle aClientRect;
        aWindow->GetRect(aWindowRect,false);
        aWindow->GetRect(aClientRect,true);
        int aWindowHeight=aWindowRect.height;
        int aClientHeight=aClientRect.height;
        if(mTabCtrl->mBottomTabBar->Fn308280()==2)
        {
            mHideLowerControls=true;
            aOldHeight=aWindowRect.height;
            aResized=true;
            aWindow->SetPosSize(aWindowRect.x,aWindowRect.y,aWindowRect.width,
                aWindowHeight-aClientHeight+150);
        }
        else if(aResized)
        {
            mHideLowerControls=false;
            aResized=false;
            aWindow->SetPosSize(aWindowRect.x,aWindowRect.y,aWindowRect.width,aOldHeight);
        }
        Fn2F6290();
    }
    LobbyFn35E680(false);
}

}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c3cf0
// (public: virtual void * __thiscall WONAPI::RootScreen::`scalar deleting destructor'(unsigned int)    [??_GRootScreen@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007c4650
// (public: virtual void * __thiscall WONAPI::SetRoomSpecFlagsEvent::`scalar deleting destructor'(unsigned int)    [??_GSetRoomSpecFlagsEvent@WONAPI@@UAEP)
// TEMPLATE: WON 0x007c4670
// (public: virtual __thiscall WONAPI::SetRoomSpecFlagsEvent::~SetRoomSpecFlagsEvent(void)    [??1SetRoomSpecFlagsEvent@WONAPI@@UAE@XZ])
