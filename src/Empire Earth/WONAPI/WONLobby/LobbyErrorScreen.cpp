// Native error-screen construction using complete original GUI owners.
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
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONLobbyCore.h"
#include "ee/wonapi/WONLobbyScreens.h"
#include "ee/wonapi/WONLobbyMSPainters.h"
namespace WONAPI {
extern SmartPtr<Dialog> gLobbyFailureDialog5662F4;
extern GUIString gLobbyInitError5662FC;
extern bool gLobbyInitFailed56633C;
// FUNCTION: WON 0x00762250
Container* LobbyFn362250()
{
    if(gLobbyData.get()!=NULL) return gLobbyData->mLobbyContainer.get();
    if(gLobbyInitFailed56633C) {
    if(gLobbyFailureDialog5662F4.get()==NULL) {
        SmartPtr<Dialog> dialog=new Dialog;
        dialog->SetEndOnEscape(0);
        SmartPtr<MSDlgBack> background=new MSDlgBack;
        SmartPtr<Label> label=new Label;
        label->SetWrap(true);
        label->SetHorzAlign(HorzAlign_Center);
        label->SetAdjustHeightToFitText(true);
        GUIString text("Lobby Error\n\n");
        text.append(gLobbyInitError5662FC);
        label->SetText(text,true);
        SmartPtr<MSButton> button=new MSButton(GUIString("Ok"));
        button->Fn2F3370(0x100,true);
        button->Fn2F4BC0(100,0);
        button->Fn2F5890();
        dialog->Fn2F6140(background.get(),3,dialog.get(),0,0,0,0,0);
        dialog->Fn2F6140(label.get(),0x1001,dialog.get(),10,0,-20,0,0);
        dialog->AddChildLayout(new CenterLayout(dialog.get(),button.get(),true,false),0);
        dialog->Fn2F6140(button.get(),0x200,label.get(),0,20,0,0,0);
        CenterLayout* vertical=new CenterLayout(dialog.get(),label.get(),false,true);
        vertical->Add(button.get());
        dialog->AddChildLayout(vertical,0);
        dialog->Fn2F6A60(background.get());
        dialog->Fn2F6A60(label.get());
        dialog->Fn2F6A60(button.get());
        gLobbyFailureDialog5662F4=dialog;
    }
    return gLobbyFailureDialog5662F4.get();
    }
    return NULL;
}
}
