
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyParser.h"
namespace WONAPI {

// SUPPORT-ONLY: native2a5a70 scalar formatting adapter.
void StringParser::ValueToString(const int& inValue, std::string& outText)
{
    char aBuffer[50];
    sprintf(aBuffer, "%d", inValue);
    outText += aBuffer;
}

// SUPPORT-ONLY: native2a5c40 widens unsigned16 and delegates genuine int overload.
void StringParser::ValueToString(const unsigned short& inValue, std::string& outText)
{
    int aValue = inValue;
    ValueToString(aValue, outText);
}

// SUPPORT-ONLY: native2a5c60 string append adapter.
void StringParser::ValueToString(const std::string& inValue, std::string& outText)
{
    outText += inValue;
}

// SUPPORT-ONLY: native2a5d70 delegates genuine narrow conversion then appends.
void StringParser::ValueToString(const std::wstring& inValue, std::string& outText)
{
    outText += WStringToString(inValue);
}

}
