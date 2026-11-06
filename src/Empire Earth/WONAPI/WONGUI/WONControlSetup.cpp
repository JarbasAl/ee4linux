// Existing complete typed control setup; conservative all-support source completion.
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
#include "ee/wonapi/WONLobbySkinnedContainers.h"
namespace WONAPI {
// Original3033a0/378, existing column-view setup constructor; support0 provisionally.
MultiListArea::MultiListArea(int columns)
{
    mItems.reserve(11000);
    mTotalColumnWidth=0;
    mSortColumn=0;
    mSortColumnPrev=0;
    mListHeader=NULL;
    mColumnBorderWidth=5;
    mDynamicSizeColumn=-1;
    mMaxTotalColumnWidth=0;
    mMouseColItem=NULL;
    mSortColFunc=NULL;
    mAutoGrowColumns=false;
    SetNumColumns(columns);
}
// Original319800/328, two actual child allocations and native list defaults; support0 provisionally.
WONComboBox::WONComboBox()
{
    mDrawFrame=true;
    mButtonInFrame=true;
    mEdit=new InputBox;
    mListArea=new ListArea;
    mListArea->Fn2FB600(2,true);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00703520
// (public: virtual void * __thiscall WONAPI::MultiListArea::`scalar deleting destructor'(unsigned int)    [??_GMultiListArea@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x0070be20
// (public: __thiscall WONAPI::WONFrame::~WONFrame(void)    [??1WONFrame@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00719800
// (public: __thiscall WONAPI::WONComboBox::WONComboBox(void)    [??0WONComboBox@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007033a0
// (public: __thiscall WONAPI::MultiListArea::MultiListArea(int)    [??0MultiListArea@WONAPI@@QAE@H@Z])
// TEMPLATE: WON 0x00703540
// (public: virtual __thiscall WONAPI::MultiListArea::~MultiListArea(void)    [??1MultiListArea@WONAPI@@UAE@XZ])
