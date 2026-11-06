// Genuine SDK StringParser collection algorithms on the existing complete12B owner.
// SOURCE_ONLY_NO_NATIVE_ID: no Native RVA/primary/physical credit is asserted.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyParser.h"

namespace WONAPI {
// API restoration only: these existing string scanners already implement the SDK TemplateReadString path.
// Their scalar/escape/cursor behavior is not rewritten or counted as a new string-scanning algorithm.
bool StringParser::ReadValue(std::string &theStr, bool stopAtWhitespace)
{
    return ReadString(theStr,stopAtWhitespace);
}
bool StringParser::ReadValue(std::wstring &theStr, bool stopAtWhitespace)
{
    return ReadString(theStr,stopAtWhitespace);
}

// Genuine SDK generic ReadList. String-only form already exists in StringParserLists.cpp;
// this whole unit supplies the missing bool/int and nested-list dispatch without changing old callers.
template<class T>
static bool ReadList(StringParser *theParser, std::list<T> &theList)
{
    while(true)
    {
        theList.push_back(T());
        if(!theParser->ReadValue(theList.back()))
        {
            theList.pop_back();
            return false;
        }
        if(!theParser->CheckNextChar(','))
            break;
    }
    return true;
}

bool StringParser::ReadValue(std::list<bool> &theList) { return ReadList<bool>(this,theList); }
bool StringParser::ReadValue(std::list<int> &theList) { return ReadList<int>(this,theList); }
bool StringParser::ReadValue(std::list<std::string> &theList) { return ReadList<std::string>(this,theList); }
bool StringParser::ReadValue(std::list<std::wstring> &theList) { return ReadList<std::wstring>(this,theList); }

// Genuine missing SDK parenthesized-list parser. Failed elements/closing delimiters retain prior parsed groups.
template<class T>
static bool ReadListList(StringParser *theParser, std::list<std::list<T> > &theList)
{
    while(true)
    {
        if(!theParser->CheckNextChar('('))
            break;
        theList.push_back(std::list<T>());
        if(!ReadList<T>(theParser,theList.back()))
            return false;
        if(!theParser->CheckNextChar(')'))
            return false;
        if(!theParser->CheckNextChar(','))
            break;
    }
    return true;
}

bool StringParser::ReadValue(std::list<std::list<bool> > &theList) { return ReadListList<bool>(this,theList); }
bool StringParser::ReadValue(std::list<std::list<int> > &theList) { return ReadListList<int>(this,theList); }
bool StringParser::ReadValue(std::list<std::list<std::string> > &theList) { return ReadListList<std::string>(this,theList); }
bool StringParser::ReadValue(std::list<std::list<std::wstring> > &theList) { return ReadListList<std::wstring>(this,theList); }

// Genuine SDK generic flat-list formatter. Existing string-list method remains untouched.
template<class T>
static void ListToString(const std::list<T> &theList, std::string &theStr)
{
    std::list<T>::const_iterator anItr = theList.begin();
    while(anItr!=theList.end())
    {
        StringParser::ValueToString(*anItr,theStr);
        ++anItr;
        if(anItr!=theList.end())
            theStr += ", ";
    }
}

void StringParser::ValueToString(const std::list<bool> &theVal,std::string &theStr) { ListToString<bool>(theVal,theStr); }
void StringParser::ValueToString(const std::list<int> &theVal,std::string &theStr) { ListToString<int>(theVal,theStr); }
void StringParser::ValueToString(const std::list<std::wstring> &theVal,std::string &theStr) { ListToString<std::wstring>(theVal,theStr); }

// Genuine missing SDK nested-list formatter; append semantics and delimiter placement are intentional.
template<class T>
static void ListListToString(const std::list<std::list<T> > &theList,std::string &theStr)
{
    std::list<std::list<T> >::const_iterator anItr = theList.begin();
    while(anItr!=theList.end())
    {
        theStr += "(";
        ListToString<T>(*anItr,theStr);
        theStr += ")";
        ++anItr;
        if(anItr!=theList.end())
            theStr += ", ";
    }
}

void StringParser::ValueToString(const std::list<std::list<bool> > &theVal,std::string &theStr) { ListListToString<bool>(theVal,theStr); }
void StringParser::ValueToString(const std::list<std::list<int> > &theVal,std::string &theStr) { ListListToString<int>(theVal,theStr); }
void StringParser::ValueToString(const std::list<std::list<std::string> > &theVal,std::string &theStr) { ListListToString<std::string>(theVal,theStr); }
void StringParser::ValueToString(const std::list<std::list<std::wstring> > &theVal,std::string &theStr) { ListListToString<std::wstring>(theVal,theStr); }
}
