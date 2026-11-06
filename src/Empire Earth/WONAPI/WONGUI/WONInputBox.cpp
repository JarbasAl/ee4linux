// Actual skinned input owner and theme duplication; all methods support0.
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
#include "ee/wonapi/WONLobbyWONInputBox.h"
namespace WONAPI {
// Native319580/81, genuine empty SDK ctor with automatic real owners; support0.
WONInputBox::WONInputBox()
{
}
// Native319770/107, default control plus deep theme frame; support0.
// FUNCTION: WON 0x00719770
WONInputBox* WONInputBox::Duplicate()
{
    WONInputBox* input=new WONInputBox;
    input->SetFrame(mFrame);
    return input;
}
// Native3197e0 forwards base content then actual frame paint; support0.
// FUNCTION: WON 0x007197e0
void WONInputBox::Paint(Graphics& graphics)
{
    InputBox::Paint(graphics);
    mFrame.Paint(graphics,this);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00719580
// (public: __thiscall WONAPI::WONInputBox::WONInputBox(void)    [??0WONInputBox@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007195e0
// (public: virtual void * __thiscall WONAPI::WONInputBox::`scalar deleting destructor'(unsigned int)    [??_GWONInputBox@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00719600
// (public: virtual __thiscall WONAPI::WONInputBox::~WONInputBox(void)    [??1WONInputBox@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0071c300
// (public: void __thiscall std::deque<class WONAPI::GUIString,class std::allocator<class WONAPI::GUIString> >::pop_back(void)    [?pop_back@?$deque@VGUIS)
