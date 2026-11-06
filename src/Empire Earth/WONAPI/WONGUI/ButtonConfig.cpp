// Reconstructed WONGUI button configuration; original TU grouping provisional.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbySound.h"
namespace WONAPI {

// FUNCTION: WON 0x00781880
void ButtonConfig::HandleButtonFlags()
{
    while(true)
    {
        bool on=true;
        GUIString flag=ReadFlag(&on);
        if(flag.length()==0) break;
        int value;
        if(flag.CompareNoCase("InvalidateOnMouseOver")==0) value=1;
        else if(flag.CompareNoCase("RepeatFire")==0) value=2;
        else if(flag.CompareNoCase("Checked")==0) value=4;
        else if(flag.CompareNoCase("Radio")==0) value=8;
        else if(flag.CompareNoCase("ActivateDown")==0) value=0x40;
        else if(flag.CompareNoCase("WantDefault")==0) value=0x400;
        else if(flag.CompareNoCase("EndDialog")==0) value=0x100;
        else if(flag.CompareNoCase("NoActivate")==0) value=0x200;
        else if(flag.CompareNoCase("WantDefault")==0) value=0x400;
        else throw ConfigObjectException("Unknown button flag"+(std::string)flag);
        mButton->Fn2F3370(value,on);
    }
}

// FUNCTION: WON 0x00781d00
void ButtonConfig::HandleFitText()
{
    int width=mButton->Width();
    int height=mButton->Height();
    GUIString oldText=mButton->GetText();
    for(;;)
    {
        GUIString text=Fn3842F0(false);
        mButton->SetText(text,true);
        int desiredWidth=0,desiredHeight=0;
        mButton->Virtual0C(desiredWidth,desiredHeight);
        if(desiredWidth>width) width=desiredWidth;
        if(desiredHeight>height) height=desiredHeight;
        if(EndOfString()) break;
        EnsureComma();
    }
    mButton->SetText(oldText,true);
    mButton->Fn2F4B90(width,height);
}

// FUNCTION: WON 0x00781e80
bool ButtonConfig::Virtual24(const std::string& inInstruction)
{
    if(inInstruction=="BUTTONFLAGS") HandleButtonFlags();
    else if(inInstruction=="TEXT") mButton->SetText(Fn3842F0(false),true);
    else if(inInstruction=="SETCHECK") mButton->SetCheck(Fn3837F0(),false);
    else if(inInstruction=="CLICKSOUND") mButton->SetClickSound(ReadSound().get());
    else if(inInstruction=="FITTEXT") HandleFitText();
    else return ComponentConfig::Virtual24(inInstruction);
    return true;
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00781b10
// (public: virtual __thiscall WONAPI::ConfigObjectException::~ConfigObjectException(void)    [??1ConfigObjectException@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00781af0
// (public: virtual void * __thiscall WONAPI::ConfigObjectException::`scalar deleting destructor'(unsigned int)    [??_GConfigObjectException@WONAPI@@UAEP)
