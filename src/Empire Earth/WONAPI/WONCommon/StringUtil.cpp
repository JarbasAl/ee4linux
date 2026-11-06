// C:\EEX\Empire Earth\WONAPI\WONCommon\StringUtil.cpp (RECONSTRUCTION).
// Original conversion family; exact historical TU grouping provisional. No new object layout.
// Source-only, helpers/library support and other near code remain original-carved; no runtime authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyConfig.h"
namespace WONAPI {
// Actual565ef0 zero-filled cache, data0quota. No invalidation/thread policy invented.
static int gStringUtilDefaultCodePage=0;
// FUNCTION: WON 0x006a63e0
// the code page is the CRT LC_CTYPE locale's suffix (not GetACP/GetLocaleInfo)
static int StringUtilLocaleCodePage()
{
    const char* locale=setlocale(LC_CTYPE,NULL);
    if(locale==NULL) return 0;
    const char* dot=strchr(locale,'.');
    if(dot==NULL) return 0;
    return atoi(dot+1);
}
// Actual2a6570/20, lazy cache helper0; zero result causes retry on the next call.
static int StringUtilDefaultCodePage()
{
    if(gStringUtilDefaultCodePage==0) gStringUtilDefaultCodePage=StringUtilLocaleCodePage();
    return gStringUtilDefaultCodePage;
}
// Actual2a6250/48, native fixed-capacity ACP helper0. Initial terminator/failure behavior deliberate.
static wchar_t* StringUtilToWideBuffer(wchar_t* out,const char* in,int count)
{
    if(out==NULL) return NULL;
    *out=0;
    if(in!=NULL) MultiByteToWideChar(0,0,in,count,out,count);
    return out;
}
// Actual2a63a0/50, native fixed-capacity ACP helper0, not a byte truncation loop.
static char* StringUtilToNarrowBuffer(char* out,const wchar_t* in,int count)
{
    if(out==NULL) return NULL;
    *out=0;
    if(in!=NULL) WideCharToMultiByte(0,0,in,count,out,count,NULL,NULL);
    return out;
}
// FUNCTION: WON 0x006a6130
// Original2a6130/286. Raw array deliberately unguarded across output string construction.
std::wstring StringToWString(const std::string& in)
{
    int length=in.length();
    if(length==0) return std::wstring();
    wchar_t* buffer=new wchar_t[length+1];
    StringUtilToWideBuffer(buffer,in.c_str(),length);
    buffer[length]=0;
    std::wstring result(buffer);
    delete [] buffer;
    return result;
}
// FUNCTION: WON 0x006a6280
// Original2a6280/287; fixed-size ACP conversion can fail/partially mutate, result code ignored as original.
std::string WStringToString(const std::wstring& in)
{
    int length=in.length();
    if(length==0) return std::string();
    char* buffer=new char[length+1];
    StringUtilToNarrowBuffer(buffer,in.c_str(),length);
    buffer[length]=0;
    std::string result(buffer);
    delete [] buffer;
    return result;
}
// FUNCTION: WON 0x006a6410
// Original2a6410/352. Length-based conversion preserves embedded NULs, unlike the one-argument overload.
std::string WStringToString(const std::wstring& in,int codePage)
{
    if(codePage<0) codePage=StringUtilDefaultCodePage();
    int count=WideCharToMultiByte(codePage,0,in.c_str(),in.length(),NULL,0,NULL,NULL);
    std::string result;
    result.reserve(count+1);
    result.resize(count);
    WideCharToMultiByte(codePage,0,in.c_str(),in.length(),const_cast<char*>(result.data()),result.length(),NULL,NULL);
    return result;
}
// FUNCTION: WON 0x006a6590
// Original2a6590/350. Both API calls occur even for zero/failure output length.
std::wstring StringToWString(const std::string& in,int codePage)
{
    if(codePage<0) codePage=StringUtilDefaultCodePage();
    int count=MultiByteToWideChar(codePage,0,in.c_str(),in.length(),NULL,0);
    std::wstring result;
    result.reserve(count+1);
    result.resize(count);
    MultiByteToWideChar(codePage,0,in.c_str(),in.length(),const_cast<wchar_t*>(result.data()),result.length());
    return result;
}
}

#include <ctype.h>
#include <wctype.h>
// Next original StringUtil algorithms; no new standard-library implementation.
namespace WONAPI {
// Native2a66f0/342: signed-char CRT input and counted length, not NUL traversal.
// FUNCTION: WON 0x6a66f0
std::string StringToLowerCase(const std::string& text)
{
    std::string result;
    result.reserve(text.length());
    for(std::string::size_type i=0;i<text.length();++i)
        result+=static_cast<char>(tolower(text[i]));
    return result;
}
// Native2a6850/323: original upper-case conversion only; no invented lower variant.
// FUNCTION: WON 0x6a6850
std::wstring WStringToUpperCase(const std::wstring& text)
{
    std::wstring result;
    result.reserve(text.length());
    for(std::wstring::size_type i=0;i<text.length();++i)
        result+=static_cast<wchar_t>(towupper(text[i]));
    return result;
}
// Native2a69a0/268: both indexed accesses are mutable and preserve COW/freeze behavior.
// FUNCTION: WON 0x6a69a0
void StringToUpperCaseInPlace(std::string& text)
{
    for(std::string::size_type i=0;i<text.length();++i)
        text[i]=static_cast<char>(toupper(text[i]));
}
// Native2a6ab0/176: mismatch branch deliberately calls toupper again.
// FUNCTION: WON 0x6a6ab0
int StringCompareNoCase(const std::string& first,const std::string& second)
{
    std::string::const_iterator a=first.begin();
    std::string::const_iterator b=second.begin();
    while(a!=first.end() && b!=second.end()) {
        if(toupper(*a)!=toupper(*b))
            return toupper(*a)<toupper(*b)?-1:1;
        ++a;
        ++b;
    }
    if(first.length()==second.length())
        return 0;
    return first.length()<second.length()?-1:1;
}
// Native2a6b60/163: towupper results compared at actual unsigned16-bit width.
// FUNCTION: WON 0x6a6b60
int WStringCompareNoCase(const std::wstring& first,const std::wstring& second)
{
    std::wstring::const_iterator a=first.begin();
    std::wstring::const_iterator b=second.begin();
    while(a!=first.end() && b!=second.end()) {
        if(static_cast<wchar_t>(towupper(*a))!=static_cast<wchar_t>(towupper(*b)))
            return static_cast<wchar_t>(towupper(*a))<static_cast<wchar_t>(towupper(*b))?-1:1;
        ++a;
        ++b;
    }
    if(first.length()==second.length())
        return 0;
    return first.length()<second.length()?-1:1;
}
}

namespace WONAPI {
// SUPPORT-ONLY: genuine SDK CPP WONAPI::GetEmptyStr; actual native69B, zero primary.
const std::string& GetEmptyStr()
{
    static std::string anEmptyStr;
    return anEmptyStr;
}
}

namespace WONAPI {
// FUNCTION: WON 0x006a6090
const std::wstring& GetEmptyWStr()
{
	static std::wstring anEmptyWStr;
	return anEmptyWStr;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK case transformation loops; no Native identity claimed.
std::string StringToUpperCase(const std::string& theStr)
{
	std::string aNewStr;
	aNewStr.reserve(theStr.length());
	for(int i=0; i<theStr.length(); i++)
		aNewStr+=toupper(theStr[i]);

	return aNewStr;
}

std::wstring WStringToLowerCase(const std::wstring& theStr)
{
	std::wstring aNewStr;
	aNewStr.reserve(theStr.length());
	for(int i=0; i<theStr.length(); i++)
		aNewStr+=towlower(theStr[i]);

	return aNewStr;
}

void StringToLowerCaseInPlace(std::string &theStr)
{
	for(int i=0; i<theStr.length(); i++)
		theStr[i] = tolower(theStr[i]);
}

void WStringToUpperCaseInPlace(std::wstring &theStr)
{
	for(int i=0; i<theStr.length(); i++)
		theStr[i] = towupper(theStr[i]);
}

void WStringToLowerCaseInPlace(std::wstring &theStr)
{
	for(int i=0; i<theStr.length(); i++)
		theStr[i] = towlower(theStr[i]);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a6250
// (unsigned short * __cdecl WONAPI::StringUtilToWideBuffer(unsigned short *,char const *,int)    [?StringUtilToWideBuffer@WONAPI@@YAPAGPAGPBDH@Z])
// TEMPLATE: WON 0x006a63a0
// (char * __cdecl WONAPI::StringUtilToNarrowBuffer(char *,unsigned short const *,int)    [?StringUtilToNarrowBuffer@WONAPI@@YAPADPADPBGH@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a6040
// (_$E73    [_$E73])
// TEMPLATE: WON 0x006a60e0
// (_$E75    [_$E75])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a6570
// (int __cdecl WONAPI::StringUtilDefaultCodePage(void)    [?StringUtilDefaultCodePage@WONAPI@@YAHXZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a5ff0
// (class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const & __cdecl WONAPI::GetEmptyStr(void)    [?GetEmptyStr@WON)
