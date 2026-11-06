// Genuine constructor completion for already accepted text/layout value owners.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <vector>
#include <list>
#include <deque>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
#include "ee/wonapi/WONLobbyLineLayout.h"
#include "ee/wonapi/WONLobbyTableElement.h"
namespace WONAPI {
// TextSegmentBase() and TextLine() live in TextAreaInstructions.cpp / TextAreaLines.cpp (the original object
// defines them before TextArea's `new` sites).
// SUPPORT-ONLY: EXE 0x00710c40
TextSegment::TextSegment()
{
    mColor=0;
}

// SUPPORT-ONLY: EXE 0x0070d7f0
TableElement::TableElement()
{
    mCurCell=NULL;
    mCellSpacing=2;
    mCellPadding=1;
    mBorder=0;
    mBGColor=-1;
    mMinWidth=mMaxWidth=0;
    mWidthSpec=0;
}
}

namespace WONAPI {
// Genuine SDK explicit destructor; native own-vptr45038c precedes ownedlist20
// cleanup, then RefCount cleanup. This is not an authored implicit lifetime.
// SUPPORT-ONLY: EXE 0x00711730 (130 native bytes)
TextLine::~TextLine() {}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0070d7f0
// (public: __thiscall WONAPI::TableElement::TableElement(void)    [??0TableElement@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00710c40
// (public: __thiscall WONAPI::TextSegment::TextSegment(void)    [??0TextSegment@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x00710ce0
// (public: virtual __thiscall WONAPI::TextSegment::~TextSegment(void)    [??1TextSegment@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x00711730
// (public: virtual __thiscall WONAPI::TextLine::~TextLine(void)    [??1TextLine@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x0071b6f0
// (public: __thiscall std::vector<struct WONAPI::TableElement::ColumnInfo,class std::allocator<struct WONAPI::TableElement::ColumnInfo> >::~vector<struct)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0070d8c0
// (public: virtual void * __thiscall WONAPI::TableElement::`scalar deleting destructor'(unsigned int)    [??_GTableElement@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00710cc0
// (public: virtual void * __thiscall WONAPI::TextSegment::`scalar deleting destructor'(unsigned int)    [??_GTextSegment@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0070d8a0
// (public: virtual int __thiscall WONAPI::TableElement::GetWidth(void)    [?GetWidth@TableElement@WONAPI@@UAEHXZ])
// TEMPLATE: WON 0x0070d8b0
// (public: virtual int __thiscall WONAPI::TableElement::GetHeight(void)    [?GetHeight@TableElement@WONAPI@@UAEHXZ])
