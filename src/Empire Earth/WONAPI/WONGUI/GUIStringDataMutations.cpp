// Typed GUIStringData mutation algorithms; existing protected data algorithms remain separate.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <string.h>
#include <wchar.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
namespace WONAPI {
// Native3274b0/14 support-only actual initialization helper.
// FUNCTION: WON 0x007274b0
void GUIStringData::Init() { mLength=mCapacity=0; mText=NULL; }
// Native capacity3274e0/273 throughRET4 at3275ee; complete constructor support0.
GUIStringData::GUIStringData(unsigned short capacity) { Init(); resize(capacity,false); resize(0,false); }
// Nativecopy327600/225, natural member initialization/copy support0.
GUIStringData::GUIStringData(const GUIStringData& other)
{ Init(); resize(other.mLength,false); memcpy(mText,other.mText,sizeof(wchar_t)*(mLength+1)); }
// Nativesubset3276f0/231 support0; no invented source-range validation.
GUIStringData::GUIStringData(const GUIStringData* other,unsigned short offset,unsigned short length)
{ Init(); resize(length,false); memcpy(mText,other->mText+offset,sizeof(wchar_t)*length); }
// Native3277e0/96,327840/94,3278a0/103,327910/103 true thin constructors support0.
GUIStringData::GUIStringData(const char* text) { Init(); AssignStr(text,strlen(text)); }
GUIStringData::GUIStringData(const wchar_t* text) { Init(); AssignWStr(text,wcslen(text)); }
GUIStringData::GUIStringData(const std::string& text) { Init(); AssignStr(text.c_str(),text.length()); }
GUIStringData::GUIStringData(const std::wstring& text) { Init(); AssignWStr(text.c_str(),text.length()); }
// FUNCTION: WON 0x00727a20
void GUIStringData::AssignStr(const char* text,int length)
{
    resize(length,false);
    for(int i=0;i<length;++i) mText[i]=(unsigned char)text[i];
}
// FUNCTION: WON 0x00727ac0
void GUIStringData::AssignWStr(const wchar_t* text,int length)
{
    resize(length,false);
    mLength=length;
    for(int i=0;i<length;++i) mText[i]=text[i];
}
// FUNCTION: WON 0x00727b60
void GUIStringData::Fill(int character,unsigned short offset,unsigned short length)
{
    int end=offset+length;
    resize(end,false);
    for(int i=offset;i<end;++i) mText[i]=character;
}
// FUNCTION: WON 0x00727c10
void GUIStringData::assign(const GUIStringData* text,unsigned short offset,unsigned short length)
{
    Reserve(length,false);
    resize(0,false);
    append(text,offset,length);
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; no Native/primary/physical credit.
GUIStringData::GUIStringData(const char *theStr, unsigned short theLen)
{
	Init();
	AssignStr(theStr,theLen);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; no Native/primary/physical credit.
GUIStringData::GUIStringData(const wchar_t *theStr, unsigned short theLen)
{
	Init();
	AssignWStr(theStr,theLen);
}
}

#include <string.h>
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK missing text edit/search API.
void GUIStringData::erase(int theStartChar, int theLength)
{
//	wstring aStr = GetWString();
//	aStr.erase(theStartChar,theLength);
//	AssignWStr(aStr.c_str(),aStr.length());
//	return;
	
	if (theStartChar>mLength || theStartChar < 0)
		return;

	int aEndChar = theStartChar+theLength;
	if(theLength==0 || aEndChar>mLength)
		aEndChar = mLength;

	if(aEndChar!=mLength)
		memmove(mText+theStartChar,mText+aEndChar,sizeof(wchar_t)*(mLength-aEndChar));
	resize(mLength-(aEndChar-theStartChar),false);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK missing text edit/search API.
void GUIStringData::insert(int theStartChar, const GUIStringData *theStr)
{
//	wstring aStr1 = GetWString();
//	wstring aStr2 = theStr->GetWString();
//	aStr1.insert(theStartChar,aStr2);
//	AssignWStr(aStr1.c_str(),aStr1.length());
//	return;

	if(theStartChar>mLength)
		return;

	int aOldLen = mLength;

	resize(mLength+theStr->mLength,false);
	if(theStartChar!=aOldLen)
		memmove(mText+theStartChar+theStr->mLength,mText+theStartChar,sizeof(wchar_t)*(aOldLen-theStartChar));
	memcpy(mText+theStartChar,theStr->mText,sizeof(wchar_t)*theStr->mLength);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK missing text edit/search API.
int GUIStringData::setAt(int thePos, int theChar)
{
	if (thePos < mLength)
		mText[thePos] = (unsigned short)theChar;
	return theChar;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007277e0
// (public: __thiscall WONAPI::GUIStringData::GUIStringData(char const *)    [??0GUIStringData@WONAPI@@QAE@PBD@Z])
// TEMPLATE: WON 0x00727840
// (public: __thiscall WONAPI::GUIStringData::GUIStringData(unsigned short const *)    [??0GUIStringData@WONAPI@@QAE@PBG@Z])
// TEMPLATE: WON 0x007278a0
// (public: __thiscall WONAPI::GUIStringData::GUIStringData(class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const )
// TEMPLATE: WON 0x00727910
// (public: __thiscall WONAPI::GUIStringData::GUIStringData(class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allo)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00727600
// (public: __thiscall WONAPI::GUIStringData::GUIStringData(class WONAPI::GUIStringData const &)    [??0GUIStringData@WONAPI@@QAE@ABV01@@Z])
// TEMPLATE: WON 0x007276f0
// (public: __thiscall WONAPI::GUIStringData::GUIStringData(class WONAPI::GUIStringData const *,unsigned short,unsigned short)    [??0GUIStringData@WONAPI)
