// Scratch whole browser consumer; BrowserComponent remains GUI-owned reference only.
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
#include "ee/wonapi/WONLobbyControlConfig.h"
#include "ee/wonapi/WONLobbyScreens.h"
namespace WONAPI {
// Original3bbdc0..3bbebe,254B. Home assignment follows default-owned members.
// FUNCTION: WON 0x007bbdc0
GameBrowserCtrl::GameBrowserCtrl(const GUIString& inHome)
{
    mHomeLocation=inHome;
    mIsReset=true;
}

// Original3bc0b0..3bc244,404B.
// FUNCTION: WON 0x007bc0b0
void GameBrowserCtrl::InitComponent(ComponentInit& inInit)
{
    if(inInit.mType!=2) return;
    ComponentConfig* config=static_cast<ComponentConfigInit&>(inInit).mConfig;
    SafeGetComponent(config,mBrowser,"Browser",__FILE__,27);
    SafeGetComponent(config,mBackButton,"BackButton",__FILE__,28);
    SafeGetComponent(config,mForwardButton,"ForwardButton",__FILE__,29);
    SafeGetComponent(config,mStopButton,"StopButton",__FILE__,30);
    SafeGetComponent(config,mHomeButton,"HomeButton",__FILE__,31);
    SafeGetComponent(config,mRefreshButton,"RefreshButton",__FILE__,32);
    SafeGetComponent(config,mFindInput,"FindInput",__FILE__,33);
    SafeGetComponent(config,mFindNextButton,"FindNextButton",__FILE__,34);
    SafeGetComponent(config,mFindPrevButton,"FindPrevButton",__FILE__,35);
    SafeGetComponent(config,mCaseSensitiveCheck,"CaseSensitiveCheck",__FILE__,36);
    SafeGetComponent(config,mStatusLabel,"StatusLabel",__FILE__,37);
    mBackButton->mControlId=2;
    mForwardButton->mControlId=3;
    mStopButton->mControlId=5;
    mRefreshButton->mControlId=1;
    mHomeButton->mControlId=4;
    mFindNextButton->mControlId=7;
    mFindPrevButton->mControlId=8;
    EnableButtons();
}
// Original3bc250..3bc2ad,93B. Button receiver snapshot precedes browser predicate.
// FUNCTION: WON 0x007bc250
void GameBrowserCtrl::EnableButtons()
{
    Button* button=mBackButton.get();
    button->SetEnabled(mBrowser->BackButtonEnabled());
    button=mForwardButton.get();
    button->SetEnabled(mBrowser->ForwardButtonEnabled());
    BrowserComponent* browser=mBrowser.get();
    Button* stop=mStopButton.get();
    stop->SetEnabled(browser->StopButtonEnabled());
}
// Original3bc2b0..3bc2e4,52B. Browser/input receiver snapshots before GetText.
// FUNCTION: WON 0x007bc2b0
void GameBrowserCtrl::DoFind(bool inNext)
{
    Button* check=mCaseSensitiveCheck.get();
    InputBox* input=mFindInput.get();
    BrowserComponent* browser=mBrowser.get();
    browser->FindStr(input->GetText(),inNext,check->mChecked!=0);
}
// Original3bc2f0..3bc307,23B.
// FUNCTION: WON 0x007bc2f0
void GameBrowserCtrl::Fn3BC2F0()
{
    mBrowser->Reset();
    mIsReset=true;
}
// Original3bc310..3bc327,23B.
// FUNCTION: WON 0x007bc310
void GameBrowserCtrl::GoHomeIfReset()
{
    if(mIsReset) {
        mIsReset=false;
        GoHome();
    }
}
// Original3bc330..3bc349,25B.
// FUNCTION: WON 0x007bc330
void GameBrowserCtrl::GoHome()
{
    mBrowser->Go(mHomeLocation,false,0,-1);
}
// Original3bc350..3bc408,184B; separate owned32B switch table excluded.
// FUNCTION: WON 0x007bc350
void GameBrowserCtrl::HandleComponentEvent(ComponentEvent* inEvent)
{
    switch(inEvent->mType) {
    case 33: EnableButtons(); return;
    case 31: mStatusLabel->SetText(mBrowser->GetCurStatus(),true); return;
    case 15:
        switch(inEvent->GetControlId()) {
        case 2: mBrowser->Back(); break;
        case 3: mBrowser->Forward(); break;
        case 5: mBrowser->Stop(); break;
        case 1: mBrowser->Refresh(); break;
        case 4: GoHome(); break;
        case 7: DoFind(true); break;
        case 8: DoFind(false); break;
        }
        return;
    }
}
// Original3bc430..3bc448,24B.
// FUNCTION: WON 0x007bc430
void GameBrowserCtrl::Paint(Graphics& inGraphics)
{
    GoHomeIfReset();
    Container::Paint(inGraphics);
}
}

namespace WONAPI {
// FUNCTION: WON 0x00765460
ComponentConfig* GameBrowserCtrlConfig::CfgFactory()
{
    return new GameBrowserCtrlConfig;
}
}

#include "ee/wonapi/WONURL.h"
namespace WONAPI {
// FUNCTION: WON 0x007bc450
bool GameBrowserCtrlConfig::Virtual24(const std::string& instruction)
{
    if(ContainerConfig::Virtual24(instruction))
        return true;
    if(instruction=="HOMELOCATION")
    {
        std::string path=Fn3842F0(false);
        URL url=path;
        if(url.GetType()==URLType_File)
            path=ComponentConfig::GetResourceFile(path);
        mBrowser->SetHomeLocation(path);
    }
    else
        return false;
    return true;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bc6b0
// (public: __thiscall WONAPI::URL::~URL(void)    [??1URL@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007cdd20

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bbec0
// (public: virtual void * __thiscall WONAPI::GameBrowserCtrl::`scalar deleting destructor'(unsigned int)    [??_GGameBrowserCtrl@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007bbee0
// (public: virtual __thiscall WONAPI::GameBrowserCtrl::~GameBrowserCtrl(void)    [??1GameBrowserCtrl@WONAPI@@UAE@XZ])
