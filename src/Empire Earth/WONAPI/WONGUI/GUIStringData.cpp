// Original-backed GUIStringData algorithms; no new COW/Reserve/resize implementation.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <ctype.h>
#include <wctype.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
namespace WONAPI {
// Original327980/77, real owned UTF16 array and automatic RefCount base destruction.
// FUNCTION: WON 0x00727980
GUIStringData::~GUIStringData()
{
    delete [] mText;
}
// Original3279d0/65: this operation uppercases only byte-range code units, not arbitrary Unicode.
// FUNCTION: WON 0x007279d0
void GUIStringData::toUpper()
{
    int length=mLength;
    for(int i=0;i<length;++i)
        if(mText[i]<256)
            mText[i]=toupper(mText[i]);
}
// Original327d90/409: reserve output once; non-byte-range characters become question marks.
// FUNCTION: WON 0x00727d90
std::string GUIStringData::GetString() const
{
    std::string result;
    result.reserve(mLength);
    for(int i=0;i<mLength;++i)
    {
        if(mText[i]<256) result+=static_cast<char>(mText[i]);
        else result+='?';
    }
    return result;
}

// FUNCTION: WON 0x00727f30
std::wstring GUIStringData::GetWString() const
{
    return std::wstring(mText,mLength);
}
// Original327fa0/139. Equal-prefix different lengths return a sign, not their length difference.
// FUNCTION: WON 0x00727fa0
int GUIStringData::Compare(const GUIStringData* other) const
{
    int count=mLength;
    if(other->mLength<count) count=other->mLength;
    const wchar_t* a=mText;
    const wchar_t* b=other->mText;
    for(int i=0;i<count;++i,++a,++b)
        if(*a!=*b) return *a-*b;
    if(mLength==other->mLength) return 0;
    return mLength<other->mLength?-1:1;
}
// Original328030/177: both character results are16-bit CRT wide-uppercase values.
// FUNCTION: WON 0x00728030
int GUIStringData::CompareNoCase(const GUIStringData* other) const
{
    int count=mLength;
    if(other->mLength<count) count=other->mLength;
    const wchar_t* a=mText;
    const wchar_t* b=other->mText;
    for(int i=0;i<count;++i,++a,++b)
    {
        unsigned short ca=towupper(*a);
        unsigned short cb=towupper(*b);
        if(ca!=cb) return ca-cb;
    }
    if(mLength==other->mLength) return 0;
    return mLength<other->mLength?-1:1;
}
// Original328150/140: offset is full signed int, ASCII bytes widen unsigned; counted wide text wins over early NUL.
// FUNCTION: WON 0x00728150
int GUIStringData::CompareNoCase(const char* other, int offset) const
{
    const wchar_t* a=mText+offset;
    int count=mLength-offset;
    for(int i=0;i<count;++i,++a,++other)
    {
        unsigned short ca=towupper(*a);
        unsigned char byte=*other;
        unsigned short cb=towupper(byte);
        if(cb==0) return 1;
        if(ca!=cb) return ca-cb;
    }
    return *other==0?0:-1;
}
// Original3281e0/146: no length clamp/plus recognition; every source index wraps to16 bits.
// FUNCTION: WON 0x007281e0
int GUIStringData::ToInt(unsigned short offset) const
{
    int value=0;
    while(isspace(mText[offset])) ++offset;
    const wchar_t* text=mText;
    bool negative;
    if(text[offset]=='-') {
        negative=true;
        ++offset;
    } else negative=false;
    for(;;)
    {
        int digit=text[offset++]-'0';
        if(digit<0 || digit>9) break;
        value=value*10+digit;
    }
    return negative?-value:value;
}
// Original328280/51: full int character and signed starting position, no new range guard.
// FUNCTION: WON 0x00728280
int GUIStringData::find(int ch, int start) const
{
    int length=mLength;
    for(int i=start;i<length;++i)
        if(mText[i]==ch) return i;
    return -1;
}
// Original3282c0/113: empty needle matches only while start<receiver length.
// FUNCTION: WON 0x007282c0
int GUIStringData::find(const GUIStringData* other, int start) const
{
    int candidate=start;
    int length=mLength;
    int needleLength=other->mLength;
    const wchar_t* needle=other->mText;
    for(;candidate<length;++candidate)
    {
        int pos=candidate,matched=0;
        while(pos<length && matched<needleLength && mText[pos]==needle[matched])
        {
            ++pos;
            ++matched;
        }
        if(matched==needleLength) return pos-needleLength;
    }
    return -1;
}
// Original328340/194: same exact start/empty-needle semantics, with CRT wide-uppercase comparison.
// FUNCTION: WON 0x00728340
int GUIStringData::findNoCase(const GUIStringData* other, int start) const
{
    int length=mLength;
    int needleLength=other->mLength;
    const wchar_t* needle=other->mText;
    for(int candidate=start;candidate<length;++candidate)
    {
        int pos=candidate,matched=0;
        while(pos<length && matched<needleLength)
        {
            unsigned short a=towupper(mText[pos]);
            unsigned short b=towupper(needle[matched]);
            if(a!=b) break;
            ++pos;
            ++matched;
        }
        if(matched==needleLength) return pos-needleLength;
    }
    return -1;
}
}
namespace WONAPI {
// FUNCTION: WON 0x007280f0
// Genuine SDK case-sensitive narrow comparison. Target preserves final ASCII '0' test.
int GUIStringData::compare(const char *theStr, int theOffset) const
{
    const wchar_t *p1 = mText + theOffset;
    const unsigned char *p2 = (const unsigned char*)theStr;
    int aLength = mLength - theOffset;
    for(int i=0; i<aLength; i++)
    {
        int c1 = *p1;
        int c2 = *p2;
        if(c2==0)
            return 1;
        if(c1!=c2)
            return c1-c2;
        p1++; p2++;
    }
    return *p2=='0'?0:-1;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK executable body; no Native/primary/physical credit.
void GUIStringData::toLowerCase()
{
	int aLength = mLength;
	for(int i=0; i<aLength; i++)
	{
		if(mText[i]<256)
			mText[i] = tolower(mText[i]);
	}	
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007274c0
// (public: virtual void * __thiscall WONAPI::GUIStringData::`scalar deleting destructor'(unsigned int)    [??_GGUIStringData@WONAPI@@UAEPAXI@Z])
