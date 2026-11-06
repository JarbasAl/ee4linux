// Complete original-backed GUI values; source-only, no runtime substitution.
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
namespace WONAPI {
// Genuine current Font virtualdefaults, allsupport0/sharednative0newcredit.
void Font::Virtual04(DisplayContext*) {}
// FUNCTION: WON 0x00726a20
void Font::Virtual08(DisplayContext*,const GUIString&,unsigned short,unsigned short,int,int) {}
// FUNCTION: WON 0x00726a30
int Font::GetStringWidth(const GUIString&,unsigned short,unsigned short) { return 0; }
int Font::Virtual10(int) { return 0; }
int Font::GetHeight() { return 0; }
int Font::GetAscent() { return 0; }
int Font::GetDescent() { return 0; }
}
