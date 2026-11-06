// HTML select-option text collection and whitespace normalization.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <ctype.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyHTMLDisplayGen.h"
namespace WONAPI {
// FUNCTION: WON 0x0073b6e0
void HTMLDisplayGen::HandleOption(HTMLTag* tag)
{
    if(mCurCombo.get()==NULL || tag->IsEnd())
        return;
    GUIString option;
    bool lastWasSpace=true;
    while(true) {
        HTMLTag* next=GetNextTag();
        if(next==NULL)
            break;
        if(next->GetType()==HTMLTagType_Option) {
            if(next->IsStart())
                --mTagListItr;
            break;
        }
        else if(next->GetType()==HTMLTagType_Text) {
            const GUIString& text=((HTMLTextTag*)next)->GetText();
            for(int i=0;i<text.length();++i) {
                unsigned int c=text[i];
                if(c<256 && isspace(c)) {
                    if(!lastWasSpace) {
                        option.append(' ');
                        lastWasSpace=true;
                    }
                }
                else {
                    option.append(c);
                    lastWasSpace=false;
                }
            }
        }
        else {
            --mTagListItr;
            break;
        }
    }
    mCurCombo->AddString(option,-1);
    int width=mCurCombo->mListArea->GetTotalWidth();
    mCurCombo->Fn2F4B90(width+20,mCurCombo->Height());
}
}
