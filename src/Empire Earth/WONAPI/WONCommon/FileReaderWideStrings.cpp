// Genuine same-SDK wide memory/file string reader on unchanged FileReader14 target.
// SOURCE_ONLY_NO_NATIVE_ID; no Native/primary/physical identity asserted.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyPersistence.h"
namespace WONAPI {
void FileReader::ReadRawWString(std::wstring &theWStr, int theNumChars)
{
	theWStr.erase();
	theWStr.reserve(theNumChars);
	for(int i=0; i<theNumChars; i++)
		theWStr += (wchar_t)ReadShort();
}

void FileReader::ReadWString(std::wstring &theWStr, unsigned char theLengthFieldSize)
{
	ReadRawWString(theWStr,ReadLength(theLengthFieldSize));
}
}
