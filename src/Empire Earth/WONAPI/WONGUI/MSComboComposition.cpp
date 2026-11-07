// Actual MSComboBox child composition and input/list policy; native2ffb20.
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
#include "ee/wonapi/WONLobbyMSExtraConfig.h"
namespace WONAPI {
// FUNCTION: WON 0x006ffb20
MSComboBox::MSComboBox()
{
    mDrawFrame=true;
    mButton=ArrowButtonFactory(Direction_Down);
    if(mButton->Width()==0) mButton->Fn2F4B90(16,mButton->Height());
    mEdit=new InputBox;
    mListArea=new ListArea;
    mListArea->SetFont(GetNamedFont("MSButton"));
    mListArea->Fn2FB600(2,true);
    mEdit->SetFont(GetNamedFont("MSButton"));
    SmartPtr<MSScrollContainer> listBox=new MSScrollContainer(mListArea.get());
    listBox->SetScrollbarConditions(ScrollbarCondition_Disable,ScrollbarCondition_Conditional);
    listBox->SetDraw3DFrame(false);
    mListBox=listBox.get();
    mButton->Fn2F3370(2,false);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ffde0
// (public: virtual __thiscall WONAPI::MSComboBox::~MSComboBox(void)    [??1MSComboBox@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ffdc0
// (public: virtual void * __thiscall WONAPI::MSComboBox::`scalar deleting destructor'(unsigned int)    [??_GMSComboBox@WONAPI@@UAEPAXI@Z])
