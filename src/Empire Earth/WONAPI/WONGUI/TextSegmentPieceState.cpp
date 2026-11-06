// Genuine complete TextSegmentPiece state and borrowed-segment forwarding, support only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
// SUPPORT-ONLY: EXE 0x007113f0
TextSegmentPiece::TextSegmentPiece(TextSegment* segment,int start,int end,int width)
{
    mSeg=segment;
    mWidth=width;
    mHeight=segment->mHeight;
    mAscent=segment->mAscent;
    mStartChar=start;
    mEndChar=end;
}
// FUNCTION: WON 0x00711430
bool TextSegmentPiece::IsLink() { return mSeg->IsLink(); }
// FUNCTION: WON 0x00711440
void TextSegmentPiece::Pressed() { mSeg->Pressed(); }
// FUNCTION: WON 0x00711450
bool TextSegmentPiece::Released() { return mSeg->Released(); }
// FUNCTION: WON 0x00711460
const GUIString& TextSegmentPiece::GetLinkParam() { return mSeg->GetLinkParam(); }
// SUPPORT-ONLY: EXE 0x00711470
TextSegmentBase* TextSegmentPiece::GetActual() { return mSeg; }
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007113f0
// (public: __thiscall WONAPI::TextSegmentPiece::TextSegmentPiece(class WONAPI::TextSegment *,int,int,int)    [??0TextSegmentPiece@WONAPI@@QAE@PAVTextSegm)
