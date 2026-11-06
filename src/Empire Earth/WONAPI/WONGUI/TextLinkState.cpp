// Genuine TextLink state/lifetime on existing complete RichText48 contract, support only.
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
#include "ee/wonapi/WONLobbyRichText.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x007114f0
TextLink::TextLink()
{
    mDownColor=0xff0000;
    mPressed=false;
}
// FUNCTION: WON 0x00711560
const GUIString& TextLink::GetLinkParam() { return mLinkParam; }
// FUNCTION: WON 0x00711640
void TextLink::Pressed()
{
    mPressed=true;
    mNormalColor=mColor;
    mColor=mDownColor;
}
// FUNCTION: WON 0x00711660
bool TextLink::Released()
{
    if(mPressed) {
        mColor=mNormalColor;
        mPressed=false;
        return true;
    }
    return false;
}
// Genuine SDK shared true provider2f30d0, no new native credit.
bool TextLink::IsLink() { return true; }
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007114f0
// (public: __thiscall WONAPI::TextLink::TextLink(void)    [??0TextLink@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00711590
// (public: virtual __thiscall WONAPI::TextLink::~TextLink(void)    [??1TextLink@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00711570
// (public: virtual void * __thiscall WONAPI::TextLink::`scalar deleting destructor'(unsigned int)    [??_GTextLink@WONAPI@@UAEPAXI@Z])
