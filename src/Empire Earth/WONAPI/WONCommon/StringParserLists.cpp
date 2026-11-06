// Actual static ReadList<T> algorithm from SDK StringParser; no new STL implementation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"

using namespace WONAPI;

// FUNCTION: WON 0x006a9fb0
// FUNCTION: WON 0x006aa0e0
template<class T>
static bool ReadList(StringParser* parser,std::list<T>& values)
{
    while(true)
    {
        values.push_back(T());
        if(!parser->ReadString(values.back(),false))
        {
            values.pop_back();
            return false;
        }
        if(!parser->CheckNextChar(','))
            break;
    }
    return true;
}

// Genuine two original17B forwarding members, support0.
bool StringParser::ReadStrings(std::list<std::string>& values)
{
    return ReadList<std::string>(this,values);
}

bool StringParser::ReadStrings(std::list<std::wstring>& values)
{
    return ReadList<std::wstring>(this,values);
}
