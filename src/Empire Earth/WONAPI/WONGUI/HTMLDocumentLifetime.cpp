// Genuine authored virtual lifetime; automatic complete-owner cleanup, support only.
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
#include "ee/wonapi/WONLobbyHTMLTag.h"
#include "ee/wonapi/WONLobbyAnimation.h"
namespace WONAPI {
// FUNCTION: WON 0x0073c9e0
HTMLDocument::~HTMLDocument() {}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable API body, zero Native/primary/physical credit.
void HTMLDocument::Rewind(bool beginAtBodyStart)
{
	if(beginAtBodyStart)
		mTagItr = GetBodyStart();
	else
		mTagItr = mTagList.begin();
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable API body, zero Native/primary/physical credit.
HTMLTag* HTMLDocument::GetNextTag(bool stopAtBodyEnd)
{
	if(stopAtBodyEnd)
	{
		if(mTagItr==mBodyEnd)
			return NULL;
	}
	else 
	{
		if(mTagItr==mTagList.end())
			return NULL;
	}

	return *mTagItr++;
}
}
