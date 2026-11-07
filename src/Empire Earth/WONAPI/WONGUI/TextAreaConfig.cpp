// Native TextArea configuration instruction handling; borrowed original-owned receiver.
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
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x0078f860
void TextAreaConfig::Virtual34(Component* theComponent)
{
    ScrollAreaConfig::Virtual34(theComponent);
    mTextArea = (TextArea*)theComponent;
}
// FUNCTION: WON 0x00795990
bool TextAreaConfig::Virtual24(const std::string& key)
{
    if(key=="TEXTFLAGS") {
        while(true) {
            bool add=true;
            GUIString flag=ReadFlag(&add);
            if(flag.length()==0) break;
            int mask;
            if(flag.CompareNoCase("WordWrap")==0) mask=1;
            else if(flag.CompareNoCase("AllowSelection")==0) mask=2;
            else if(flag.CompareNoCase("ScrollOnBottom")==0) mask=4;
            else if(flag.CompareNoCase("ShowPartialLines")==0) mask=8;
            else if(flag.CompareNoCase("ScrollFullLine")==0) mask=0x10;
            else if(flag.CompareNoCase("DelayWordWrap")==0) mask=0x20;
            else throw ConfigObjectException("Unknown text flag: "+(std::string)flag);
            mTextArea->SetTextFlags(mask,add);
        }
    } else if(key=="TEXT") {
        mTextArea->AddSegment(Fn3842F0(false),false);
    } else if(key=="FORMATTEDTEXT") {
        mTextArea->AddFormatedText(Fn3842F0(false),false);
    } else if(key=="FONT") {
        mTextArea->SetFont(ReadFont());
    } else if(key=="TEXTCOLOR") {
        mTextArea->mTextColor=Fn385510();
    } else if(key=="SELCOLOR") {
        mTextArea->mSelColor=ReadSelectionColor();
    } else if(key=="LINKFONT") {
        mTextArea->SetLinkFont(ReadFont());
    } else if(key=="MAXCHARS") {
        mTextArea->SetMaxChars(ReadInt());
    } else if(key=="MAXLINES") {
        mTextArea->SetMaxLines(ReadInt());
    } else if(key=="WRAPINDENTSIZE") {
        mTextArea->SetWrapIndentSize(ReadInt());
    } else if(key=="HORZALIGN") {
        mTextArea->SetLineAlignment(ReadHorzAlign());
    } else return ScrollAreaConfig::Virtual24(key);
    return true;
}
}
