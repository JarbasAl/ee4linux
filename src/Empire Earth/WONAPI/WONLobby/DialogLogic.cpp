// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Original-backed dialog logic; two deferred originals remain unmarked below.
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyDialogs.h"
#include "ee/wonapi/WONLobbyDialogLogic.h"
namespace WONAPI {
// FUNCTION: WON 0x7dc3f0
DialogLogic::DialogLogic() { mPopupWindows = false; mStatusCancelled = false; }
// FUNCTION: WON 0x7dc480
DialogLogic::~DialogLogic() {}
// FUNCTION: WON 0x7dc5a0
void DialogLogic::InitComponent(ComponentConfig* inConfig)
{
    mStatusVisible = false;
    mStatusMode = 0;
    mStatusOp = NULL;
    SafeGetComponent(inConfig, mOkCancelDialog, "OkCancelDialog", __FILE__, 48);
    SafeGetComponent(inConfig, mStatusDialog, "StatusDialog", __FILE__, 49);
    if (WONAPI::WindowManager::GetDefaultWindowManager() != NULL)
        mPopupWindows = WONAPI::WindowManager::GetDefaultWindowManager()->mBA5;
}
// FUNCTION: WON 0x7dc630
void DialogLogic::KillHook()
{
    if (mStatusOp.get() != NULL) {
        mStatusOp->Kill(true);
        mStatusOp = NULL;
    }
    ClearDialogs();
}
// FUNCTION: WON 0x7dc680
void DialogLogic::StatusOpComplete(AsyncOp* inOp)
{
    if (inOp != mStatusOp.get())
        return;
    bool aDone = mStatusMode == 2 || (mStatusMode == 1 && inOp->GetStatus() == 0);
    if (aDone)
        GetLobbyWindow()->Fn32DDB0(-1, NULL, mStatusDialog.get());
    else
        UpdateStatus(inOp->GetStatus());
}
// FUNCTION: WON 0x7dc6d0
void DialogLogic::ClearDialogs()
{
    if (mDialogStack.empty() || GetLobbyRoot() == NULL)
        return;
    bool aChanged = false;
    DialogStack::iterator aBackItr = mDialogStack.end();
    while (aBackItr != mDialogStack.begin()) {
        --aBackItr;
        if (aBackItr->mInModal)
            break;
        Component* aComponent = aBackItr->mComponent.get();
        if (aComponent->mParent != NULL)
            aComponent->mParent->VirtualC4(aComponent);
        if (mPopupWindows)
            aBackItr->mWindow->Close();
        aChanged = true;
    }
    if (aChanged) {
        DialogStack::iterator anItr = mDialogStack.begin();
        while (anItr != mDialogStack.end()) {
            if (!anItr->mInModal)
                mDialogStack.erase(anItr++);
            else
                ++anItr;
        }
    }
}
// FUNCTION: WON 0x7dc790
bool DialogLogic::CheckDialog(Component* inDialog, bool& outSizeChanged)
{
    outSizeChanged = true;
    if (mDialogStack.empty())
        return false;
    DialogInfo& anInfo = mDialogStack.back();
    if (anInfo.mComponent.get() != inDialog)
        return false;
    if (inDialog->Width() == anInfo.mOldRect.width && inDialog->Height() == anInfo.mOldRect.height)
        outSizeChanged = false;
    return true;
}
// FUNCTION: WON 0x7dc9a0
int DialogLogic::DoDialog(Component* inDialog)
{
    bool aSizeChanged = true;
    bool aReuse = CheckDialog(inDialog, aSizeChanged);
    if (!aReuse)
        ClearDialogs();
    SmartPtr<Window> aWindow = GetDialogWindow(aReuse);
    if (aWindow.get() == NULL)
        return -1;
    SmartPtr<Container> aRoot;
    if (mPopupWindows) {
        aWindow->SetClientSize(inDialog->Width(), inDialog->Height());
        aRoot = aWindow->mRoot.get();
    }
    else
        aRoot = GetLobbyRoot();
    if (aRoot.get() == NULL)
        return -1;
    if (aSizeChanged) {
        if (mPopupWindows)
            aWindow->Center(true);
        else
            CenterLayout::Center(aRoot.get(), inDialog, true, true);
    }
    if (!aReuse) {
        aRoot->Fn2F6A60(inDialog);
        mDialogStack.push_back(DialogInfo());
    }
    DialogInfo& anInfo = mDialogStack.back();
    anInfo.mWindow = aWindow.get();
    anInfo.mComponent = inDialog;
    anInfo.mInModal = true;
    anInfo.mOldRect = inDialog->GetBounds();
    if (mPopupWindows)
        aWindow->Show(true);
    int aResult = aWindow->RunDialog(inDialog);
    anInfo.mInModal = false;
    return aResult;
}
// FUNCTION: WON 0x7dcc70
void DialogLogic::RunOpWithStatus(AsyncOp* inOp, unsigned long inUnusedTimeout,
                                const GUIString& inTitle, const GUIString& inStatus,
                                int inStatusMode, unsigned long inTimeout)
{
    if (mStatusVisible) {
        inOp->ForceFinish(100002);
        return;
    }
    mStatusOp = inOp;
    mStatusCancelled = false;
    RunOp(mStatusOp.get(), new LogicCompletion<DialogLogic>(&DialogLogic::StatusOpComplete, this), inTimeout);
    mStatusMode = inStatusMode;
    ShowStatus(inTitle, inStatus, true);
    if (mStatusOp->GetStatus() == 100000) {
        mStatusCancelled = true;
        mStatusOp->Kill(true);
    }
    mStatusOp = NULL;
}
// FUNCTION: WON 0x7dcdb0
void DialogLogic::ShowStatus(const GUIString& inTitle, const GUIString& inStatus, bool inCancel)
{
    if (mStatusVisible || LobbyFn3628B0())
        return;
    mStatusVisible = true;
    mStatusDialog->SetText(inTitle, inStatus, inCancel);
    DoDialog(mStatusDialog.get());
    mStatusDialog->mStartTick = GetTickCount();
    mStatusVisible = false;
}
// FUNCTION: WON 0x7dce10
void DialogLogic::ShowError(const GUIString& inTitle, const GUIString& inStatus)
{
    if (!mStatusVisible) {
        if (LobbyFn35E320())
            LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
        ShowStatus(inTitle, inStatus, false);
    }
}
// FUNCTION: WON 0x7dce50
void DialogLogic::ShowError(const GUIString& inTitle, WONStatus inStatus)
{
    ShowStatus(inTitle, LobbyFn366170(inStatus), false);
}
// FUNCTION: WON 0x7dd050
int DialogLogic::ShowOkCancel(const GUIString& inTitle, const GUIString& inText, bool inPlaySound)
{
    mOkCancelDialog->SetText(inTitle, inText, ::LobbyGlobal_OK_String, ::LobbyGlobal_Cancel_String);
    if (inPlaySound && LobbyFn35E320())
        LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
    return DoDialog(mOkCancelDialog.get());
}
// FUNCTION: WON 0x7dd0a0
int DialogLogic::ShowYesNo(const GUIString& inTitle, const GUIString& inText, bool inPlaySound)
{
    mOkCancelDialog->SetText(inTitle, inText, ::LobbyGlobal_Yes_String, ::LobbyGlobal_No_String);
    if (inPlaySound && LobbyFn35E320())
        LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
    return DoDialog(mOkCancelDialog.get());
}
// FUNCTION: WON 0x7dc7e0
SmartPtr<Window> DialogLogic::GetDialogWindow(bool inReuse)
{
    if (!mPopupWindows)
        return LobbyFn361230();
    if (inReuse && !mDialogStack.empty())
        return mDialogStack.back().mWindow;
    SmartPtr<Window> aWindow(new MSWindow);
    aWindow->mIsOneComponentWindow = true;
    if (!mDialogStack.empty())
        aWindow->SetParent(mDialogStack.back().mWindow.get());
    else
        aWindow->SetParent(LobbyFn361230());
    aWindow->mStyle = 0x22;
    aWindow->Create(WONRectangle(0, 0, 100, 100));
    LobbyFn359080(aWindow->mRoot.get());
    aWindow->mRoot->Fn2F59A0(LobbyFn361210());
    return aWindow;
}
// FUNCTION: WON 0x7dd0f0
void DialogLogic::Reset() { ClearDialogs(); }
}

// complete observable status update. Native also default-constructs/destroys an empty
// std::wstring with no use in this body; its unexplained source lifetime is recorded debt,
// not fabricated as an unused local. No helper/EH byte-shape claim.
namespace WONAPI {
// FUNCTION: WON 0x007dced0
void DialogLogic::UpdateStatus(WONStatus inStatus)
{
    WONRectangle anOldBounds = mStatusDialog->GetBounds();
    int anOldWidth = anOldBounds.Width();
    int anOldHeight = anOldBounds.Height();
    std::wstring aText; // constructed and destroyed; no statement reads it
    mStatusDialog->SetStatus(LobbyFn366170(inStatus), false);
    if (inStatus != 0 && LobbyFn35E320())
        LobbyFn35B9A0(::LobbyGlobal_Error_Sound.get());
    if (anOldWidth == mStatusDialog->Width() && anOldHeight == mStatusDialog->Height())
        return;
    if (mPopupWindows) {
        Window* aWindow = mStatusDialog->Fn2F5940();
        if (aWindow == NULL)
            return;
        aWindow->SetClientSize(mStatusDialog->Width(), mStatusDialog->Height());
        aWindow->Center(true);
    }
    else {
        Container* aRoot = GetLobbyRoot();
        if (aRoot != NULL) {
            CenterLayout::Center(aRoot, mStatusDialog.get(), true, true);
            WONRectangle aBounds(mStatusDialog->GetBounds());
            aRoot->Virtual60(aBounds);
        }
    }
}
}
// TEMPLATE: WON 0x007f47c0

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007dc460
// (public: virtual void * __thiscall WONAPI::DialogLogic::`scalar deleting destructor'(unsigned int)    [??_GDialogLogic@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f4750
// (void __cdecl std::_Construct(struct WONAPI::DialogLogic::DialogInfo *,struct WONAPI::DialogLogic::DialogInfo const &)    [?_Construct@std@@YAXPAUDialo)
// TEMPLATE: WON 0x007f33d0
// TEMPLATE: WON 0x007f3550

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1300
// (public: virtual void __thiscall WONAPI::LogicCompletion<class WONAPI::DialogLogic>::Run(class WONAPI::AsyncOp *)    [?Run@?$LogicCompletion@VDialogLog)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007dcbf0
// (public: __thiscall WONAPI::DialogLogic::DialogInfo::~DialogInfo(void)    [??1DialogInfo@DialogLogic@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f12a0
// (public: __thiscall std::list<struct WONAPI::DialogLogic::DialogInfo,class std::allocator<struct WONAPI::DialogLogic::DialogInfo> >::~list<struct WONAP)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1410
// (public: virtual void * __thiscall WONAPI::LogicCompletion<class WONAPI::DialogLogic>::`scalar deleting destructor'(unsigned int)    [??_G?$LogicComple)
// TEMPLATE: WON 0x007f14b0
// (public: virtual __thiscall WONAPI::LogicCompletion<class WONAPI::DialogLogic>::~LogicCompletion<class WONAPI::DialogLogic>(void)    [??1?$LogicComplet)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007f1690
// (public: void __thiscall std::list<struct WONAPI::DialogLogic::DialogInfo,class std::allocator<struct WONAPI::DialogLogic::DialogInfo> >::_Insert(class)
// TEMPLATE: WON 0x007f1f50
// (protected: void __thiscall std::list<struct WONAPI::DialogLogic::DialogInfo,class std::allocator<struct WONAPI::DialogLogic::DialogInfo> >::_Incsize(u)
