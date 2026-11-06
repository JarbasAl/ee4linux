// WONGUI InputBox configuration, borrowed control only; no control lifetime source.
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
#include "ee/wonapi/WONLobbyResourceConfig.h"
namespace WONAPI {

// FUNCTION: WON 0x0078cd60
bool InputBoxConfig::Virtual24(const std::string& inInstruction)
{
    if(inInstruction=="TEXT") mInputBox->SetText(Fn3842F0(false),true);
    else if(inInstruction=="PASSWORDCHAR") mInputBox->SetPasswordChar(ReadChar());
    else if(inInstruction=="ALLOWSELECTION") mInputBox->Fn2F84B0(Fn3837F0());
    else if(inInstruction=="MAXCHARS") mInputBox->mMaxChars=ReadInt();
    else if(inInstruction=="CHARWIDTH") mInputBox->SetCharWidth(ReadInt());
    else if(inInstruction=="BACKGROUND") mInputBox->mBackground=ReadBackground();
    else if(inInstruction=="DISABLEDBACKGROUND") mInputBox->mDisabledBackground=ReadBackground();
    else if(inInstruction=="ICON") mInputBox->SetIcon(ReadImage(false).get());
    else if(inInstruction=="TEXTCOLOR") mInputBox->mTextColor=Fn385510();
    else if(inInstruction=="DISABLEDTEXTCOLOR") mInputBox->mDisabledTextColor=Fn385510();
    else if(inInstruction=="SELCOLOR") mInputBox->mSelColor=ReadSelectionColor();
    else if(inInstruction=="SELECTONFOCUS") mInputBox->mSelectOnFocus=Fn3837F0();
    else if(inInstruction=="MAXINPUTHISTORY") mInputBox->mMaxInputHistory=ReadInt();
    else if(inInstruction=="USEDISABLEDCOLORS") mInputBox->mUseDisabledColors=Fn3837F0();
    else if(inInstruction=="CURSORBLINKRATE")
    {
        int on=ReadInt(),off=on;
        if(!EndOfString())
        {
            EnsureComma();
            off=ReadInt();
        }
        mInputBox->SetCursorBlinkRate(on,off);
    }
    else return ComponentConfig::Virtual24(inInstruction);
    return true;
}

}
