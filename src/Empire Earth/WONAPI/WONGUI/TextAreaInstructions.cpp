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
using namespace WONAPI;

// Actual TextArea instruction append operations; complete finite owners only.
// Defined before LineBreak in the original object: VC6 then knows the ctor cannot throw and omits the
// delete-on-throw guard around `new TextSegmentBase` (0x712690, 0x712730).
// FUNCTION: WON 0x00710a30
TextSegmentBase::TextSegmentBase()
{
    mLine=NULL;
    mAbsStartChar=mAbsEndChar=0;
    mWidth=mHeight=mAscent=0;
    mx=my=0;
    mLineBreak=false;
}
// FUNCTION: WON 0x00712690
void TextArea::LineBreak(int height) {
    SmartPtr<TextSegmentBase> segment = new TextSegmentBase;
    segment->mHeight = segment->mAscent = height;
    segment->mLineBreak = true;
    AddSegment(segment.get());
}

// FUNCTION: WON 0x00712730
void TextArea::SetLineAlignment(HorzAlign alignment) {
    if(alignment != mLineAlign) {
        SmartPtr<TextAlignInstruction> instruction = new TextAlignInstruction;
        instruction->mAlign = alignment;
        AddSegment(instruction.get());
    }
}

namespace WONAPI {
// Genuine SDK explicit instruction callback; current complete30 instruction owner.
// FUNCTION: WON 0x00711680
void TextAlignInstruction::PerformInstructions(TextArea* area)
{
    area->mLineAlign=mAlign;
}
}
