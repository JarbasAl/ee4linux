// Native ComboBox configuration dispatch using complete original-owned ComboBox controls.
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
#include "ee/wonapi/WONLobbyComboBoxConfig.h"
#include "ee/wonapi/WONLobbyResourceConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x00782050
ComboBoxConfig::ComboBoxConfig()
{
}

// FUNCTION: WON 0x007821b0
bool ComboBoxConfig::Virtual24(const std::string& key)
{
    if(key=="LISTBOXWIDTH") {
        int width=ReadInt(); mComboBox->SetListBoxWidth(width);
    } else if(key=="LISTBOXHEIGHT") {
        int height=ReadInt(); mComboBox->SetListBoxHeight(height);
    } else if(key=="LISTBOXSIZE") {
        int width=ReadInt();
        int height=ReadInt();
        mComboBox->SetListBoxDimensions(width,height);
    } else if(key=="TEXT") {
        mComboBox->SetText(Fn3842F0(false),true);
    } else if(key=="TEXTITEM") {
        while(!EndOfString()) {
            mComboBox->AddString(Fn3842F0(false),-1);
            if(!EndOfString()) EnsureComma();
        }
    } else if(key=="SELITEM") {
        mComboBox->SetSelItem(ReadInt(),true);
    } else if(key=="NOTYPE") {
        mComboBox->SetNoType(Fn3837F0());
    } else if(key=="MAXCHARS") {
        InputBox* input=mComboBox->mEdit.get();
        input->mMaxChars=ReadInt();
    } else if(key=="SHOWBUTTONWHENDISABLED") {
        bool show=Fn3837F0(); mComboBox->mShowButtonWhenDisabled=show;
    } else return ContainerConfig::Virtual24(key);
    return true;
}
// FUNCTION: WON 0x00782070
bool ComboBoxConfig::Virtual28(const std::string& name,ComponentConfig*& config,Component*& component)
{
    if(name=="LISTAREA") {
        config=ListAreaConfig::CfgFactory();
        component=mComboBox->mListArea.get();
    } else if(name=="INPUTBOX") {
        config=InputBoxConfig::CfgFactory();
        component=mComboBox->mEdit.get();
    } else return ContainerConfig::Virtual28(name,config,component);
    return true;
}

}
