// C:\EEX\Empire Earth\WONAPI\WONCommon\StringParser.cpp (RECONSTRUCTION).
// Original coherent scanner family; exact historical TU grouping provisional. Complete12B already canonical.
// Actual API names retained where canonical (ReadString); added method names primary-header correlated.
// Source only; near/helper/table support remains original-carved, without runtime authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <set>
#include <windows.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <list>
#include <vector>
#include <ctype.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "LostDebug.h"
namespace WONAPI {
// FUNCTION: WON 0x006a48c0
unsigned char StringParser::GetChar(int offset)
{
    if(mWide) {
        wchar_t ch=mWideText[mPos+offset];
        return ch<256 ? (unsigned char)ch : (unsigned char)0xff;
    }
    return (unsigned char)(mNarrow+mPos)[offset];
}
// FUNCTION: WON 0x006a4910
wchar_t StringParser::GetWChar(int offset)
{ return mWide ? mWideText[mPos+offset] : (unsigned char)mNarrow[mPos+offset]; }
// FUNCTION: WON 0x006a4900
void StringParser::IncrementPos(int amount) { mPos+=amount; }
// FUNCTION: WON 0x006a4940
bool StringParser::EndOfString()
{
    if(mWide) return mWideText[mPos]==0;
    return mNarrow[mPos]==0;
}
// FUNCTION: WON 0x006a4970
void StringParser::SkipWhitespace()
{
    while(!EndOfString()) {
        if(!isspace(GetChar(0))) break;
        ++mPos;
    }
}
// FUNCTION: WON 0x006a49b0
// consumes leading whitespace even on a mismatch
bool StringParser::CheckNextChar(int ch)
{
    SkipWhitespace();
    if(ch==GetChar(0)) {
        ++mPos;
        return true;
    }
    return false;
}
static int ScannerHexValue(char ch);
static void ScannerAppend(std::string& out,wchar_t ch);
static void ScannerAppend(std::string& out,char ch);
inline bool ScannerReadString(StringParser& p,std::string& out,bool stopAtWhitespace)
{
    out.erase();
    p.SkipWhitespace();
    bool quoted=false;
    bool wideLiteral=false;
    std::string::size_type keepLength=0;
    while(!p.EndOfString()) {
        unsigned char ch=p.GetChar(0);
        wchar_t wideChar;
        if(wideLiteral) wideChar=p.GetWChar(0);
        p.IncrementPos(1);
        if(stopAtWhitespace && isspace(ch) && !quoted) break;
        if(ch=='\\' && quoted) {
            unsigned char escaped=p.GetChar(0);
            p.IncrementPos(1);
            switch(escaped) {
            case 't':wideChar='\t';break;
            case 'n':wideChar='\n';break;
            case 'r':wideChar='\r';break;
            case '"':wideChar='"';break;
            case '$':wideChar='$';break;
            case '\\':wideChar='\\';break;
            case 'x': {
                int maxDigits=wideLiteral?4:2;
                wideChar=0;
                for(int digit=0;digit<maxDigits;++digit) {
                    if(!isxdigit(p.GetChar(0))) break;
                    int val=ScannerHexValue(p.GetChar(0));
                    wideChar=wideChar<<4;
                    wideChar=wideChar|val;
                    p.IncrementPos(1);
                }
                break;
            }
            default:p.IncrementPos(-1);wideChar='\\';break;
            }
            if(wideLiteral) ScannerAppend(out,wideChar);
            else ScannerAppend(out,(char)wideChar);
            keepLength=out.length();
        } else if(!quoted && ch=='L' && p.GetChar(0)=='"') {
            p.IncrementPos(1);
            wideLiteral=true;
            quoted=true;
        } else if(ch=='"') {
            if(quoted) {
                quoted=false;
                if(p.GetChar(0)!='+') break;
                p.IncrementPos(1);
                p.SkipWhitespace();
            } else {
                wideLiteral=false;
                quoted=true;
            }
        } else if(!quoted && (ch==',' || ch==')')) {
            p.IncrementPos(-1);
            break;
        } else {
            if(wideLiteral) ScannerAppend(out,wideChar);
            else ScannerAppend(out,(char)ch);
            if(quoted || !isspace(ch)) keepLength=out.length();
        }
    }
    out.resize(keepLength);
    return true;
}
// FUNCTION: WON 0x006a49f0
bool StringParser::ReadString(std::string& out,bool stopAtWhitespace)
{ return ScannerReadString(*this,out,stopAtWhitespace); }
static int ScannerHexValue(char ch)
{ return isdigit(ch) ? ch-'0' : toupper(ch)-'A'+10; }
// FUNCTION: WON 0x006a4da0
// appends the two bytes of the wide character (not UTF-8)
static void ScannerAppend(std::string& out,wchar_t ch)
{
    wchar_t c=ch;
    out.append(reinterpret_cast<const char*>(&c),2);
}
static void ScannerAppend(std::string& out,char ch) { out+=ch; }
static void ScannerAppend(std::wstring& out,wchar_t ch);
inline bool ScannerReadString(StringParser& p,std::wstring& out,bool stopAtWhitespace)
{
    out.erase();
    p.SkipWhitespace();
    bool quoted=false;
    std::wstring::size_type keepLength=0;
    // LOST-DEBUG: none: exact only
    EE_LOST_DEBUG(2);
    while(!p.EndOfString()) {
        unsigned char ch=p.GetChar(0);
        wchar_t wideChar=p.GetWChar(0);
        p.IncrementPos(1);
        if(stopAtWhitespace && isspace(ch) && !quoted) break;
        if(ch=='\\' && quoted) {
            unsigned char escaped=p.GetChar(0);
            p.IncrementPos(1);
            wchar_t value;
            switch(escaped) {
            case 't':value='\t';break;
            case 'n':value='\n';break;
            case 'r':value='\r';break;
            case '"':value='"';break;
            case '$':value='$';break;
            case '\\':value='\\';break;
            case 'x': {
                value=0;
                for(int digit=0;digit<4;++digit) {
                    if(!isxdigit(p.GetChar(0))) break;
                    int val=ScannerHexValue(p.GetChar(0));
                    value=value<<4;
                    value=value|val;
                    p.IncrementPos(1);
                }
                break;
            }
            default:p.IncrementPos(-1);value='\\';break;
            }
            ScannerAppend(out,value);
            keepLength=out.length();
        } else if(!quoted && ch=='L' && p.GetChar(0)=='"') {
            p.IncrementPos(1);
            quoted=true;
        } else if(ch=='"') {
            if(quoted) {
                quoted=false;
                if(p.GetChar(0)!='+') break;
                p.IncrementPos(1);
                p.SkipWhitespace();
            } else quoted=true;
        } else if(!quoted && (ch==',' || ch==')')) {
            p.IncrementPos(-1);
            break;
        } else {
            ScannerAppend(out,wideChar);
            if(quoted || !isspace(ch)) keepLength=out.length();
        }
    }
    out.resize(keepLength);
    return true;
}
// FUNCTION: WON 0x006a4ff0
bool StringParser::ReadString(std::wstring& out,bool stopAtWhitespace)
{ return ScannerReadString(*this,out,stopAtWhitespace); }
static void ScannerAppend(std::wstring& out,wchar_t ch) { out+=ch; }
// FUNCTION: WON 0x006a53d0
// Original2a53d0/380; success means cursor moved, including '-' or '0x' without digits.
bool StringParser::ReadValue(int& out)
{
    SkipWhitespace();
    int start=mPos;
    int value=0;
    bool negative=false;
    if(GetChar(0)=='-') {negative=true;IncrementPos(1);}
    if(GetChar(0)=='0' && tolower(GetChar(1))=='x') {
        IncrementPos(2);
        while(isxdigit(GetChar(0))) {
            value<<=4;
            char ch=toupper(GetChar(0));
            if(ch>='0' && ch<='9') value+=ch-'0';
            else if(ch>='A' && ch<='F') value+=ch-'A'+10;
            else break;
            IncrementPos(1);
        }
    } else {
        while(isdigit(GetChar(0))) {
            value=value*10+GetChar(0)-'0';
            IncrementPos(1);
        }
    }
    if(negative) value=-value;
    if(mPos!=start) {
        out=value;
        return true;
    }
    return false;
}
// FUNCTION: WON 0x006a5550
// Original2a5550/648. Invalid token is consumed but leaves output unchanged.
bool StringParser::ReadValue(bool& out)
{
    SkipWhitespace();
    std::string token;
    while(!EndOfString()) {
        char ch=toupper(GetChar(0));
        if(!isalnum(ch)) break;
        token+=ch;
        ++mPos;
    }
    if(token=="TRUE" || token=="YES" || token=="ON" || token=="Y" || token=="T" || token=="1") out=true;
    else if(token=="FALSE" || token=="NO" || token=="OFF" || token=="N" || token=="F" || token=="0") out=false;
    else return false;
    return true;
}
// New static formatting family; all old scanner bodies remain unchanged.
// One-argument conversion is the already-proved canonical StringUtil2a6280 contract.
std::string WStringToString(const std::wstring&);
// FUNCTION: WON 0x006a5820
void StringParser::ValueToString(const bool& value,std::string& out)
{
    if(value) out+="True";
    else out+="False";
}
// FUNCTION: WON 0x006a5a70
void StringParser::ValueToString(const int& value,std::string& out)
{
    char buffer[50];
    sprintf(buffer,"%d",value);
    out+=buffer;
}
// FUNCTION: WON 0x006a5c40
void StringParser::ValueToString(const unsigned short& value,std::string& out)
{
    int integer=value;
    ValueToString(integer,out);
}
// FUNCTION: WON 0x006a5c60
void StringParser::ValueToString(const std::string& value,std::string& out) {out+=value;}
// FUNCTION: WON 0x006a5d70
void StringParser::ValueToString(const std::wstring& value,std::string& out) {out+=WStringToString(value);}
// FUNCTION: WON 0x006a5e70
void StringParser::ValueToString(const std::list<std::string>& values,std::string& out)
{
    std::list<std::string>::const_iterator i=values.begin();
    while(i!=values.end()) {
        ValueToString(*i,out);
        i++;
        if(i!=values.end()) out+=", ";
    }
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK cursor-matching algorithm. Native entry unlocated.
bool StringParser::StartsWith(const char *theStr)
{
    int originalPos = mPos;
    const unsigned char *p = (const unsigned char*)theStr;
    while(*p!='\0' && !EndOfString())
    {
        if(*p!=GetChar(0))
        {
            mPos = originalPos;
            return false;
        }
        p++;
        mPos++;
    }
    if(EndOfString() && *p!='\0')
    {
        mPos = originalPos;
        return false;
    }
    return true;
}

// SOURCE_ONLY_NO_NATIVE_ID: exact SDK composition; a successful prefix is not rewound on trailing input.
bool StringParser::Is(const char *theStr)
{
    bool startsWith = StartsWith(theStr);
    return startsWith && EndOfString();
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK whitespace scanner, using existing target12B owner.
void StringParser::SkipNonWhitespace()
{
    while(!EndOfString() && !isspace(GetChar(0)))
        mPos++;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a4d70
// (int __cdecl WONAPI::ScannerHexValue(char)    [?ScannerHexValue@WONAPI@@YAHD@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a4f30
// (void __cdecl WONAPI::ScannerAppend(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > &,char)    [?ScannerAppend@)
// TEMPLATE: WON 0x006a5310
// (void __cdecl WONAPI::ScannerAppend(class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short>)
