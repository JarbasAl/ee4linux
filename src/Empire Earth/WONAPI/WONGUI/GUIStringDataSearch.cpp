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
namespace WONAPI {
// FUNCTION: WON 0x00728410
int GUIStringData::rFind(int character,int start) const
{
    if(start==-1) start=mLength-1;
    for(int i=start;i>=0;--i)
        if(mText[i]==character) return i;
    return -1;
}
}

#include <string.h>
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK missing text edit/search API.
int GUIStringData::findOneOf(const GUIStringData *theStr, int theStartPos) const
{
	int aFirstFound = -1;

	for (int i=0; i<theStr->mLength; i++)
	{
		int aFound = find(theStr->mText[i], theStartPos);
		if (aFound!=-1)
			if (aFirstFound==-1 || aFound<aFirstFound)
				aFirstFound = aFound;
	}

	return aFirstFound;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK missing text edit/search API.
int GUIStringData::rFind(const GUIStringData* theStr, int theStartPos) const
{
	std::wstring sSrc = GetWString();
	std::wstring sFind = theStr->GetWString();

	return sSrc.rfind(sFind, theStartPos);
}
}
