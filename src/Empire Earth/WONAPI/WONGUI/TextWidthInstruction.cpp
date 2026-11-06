// Genuine SDK width instruction; SOURCE_ONLY_NO_NATIVE_ID.
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
#include "ee/wonapi/WONLobbyTextAreaConfig.h"
namespace WONAPI {
void TextWidthInstruction::PerformInstructions(TextArea *theTextArea)
{
	if(mLine->mWidth < mLineShouldBeAtLeastThisWidth)
		mWidth = mLineShouldBeAtLeastThisWidth - mLine->mWidth;
}
}
