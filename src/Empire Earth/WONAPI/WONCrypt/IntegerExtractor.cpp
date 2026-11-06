// Native ASN.1 integer extractor used by ElGamal key loading.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONIntegerExtractor.h"
namespace WONAPI {
// FUNCTION: WON 0x007d8e10
IntegerExtractor::IntegerExtractor(const void* data,unsigned long size,bool buffered)
    : input((const unsigned char*)data,size),offset(0),mIsValid(true)
{
    if (buffered) {
        if (GetByte()!=(SEQUENCE|CONSTRUCTED)) return;
        if (!LengthDecode()) return;
    }
}
// FUNCTION: WON 0x007d8eb0
unsigned char IntegerExtractor::GetByte()
{
    if (offset<input.length()) return input[offset++];
    mIsValid=false;
    return 0;
}
// FUNCTION: WON 0x007d8fb0
unsigned char IntegerExtractor::PeekByte()
{
    if (offset<input.length()) return input[offset];
    mIsValid=false;
    return 0;
}
// FUNCTION: WON 0x007d90a0
bool IntegerExtractor::LengthDecode()
{
    unsigned char b=GetByte();
    if (b&0x80) {
        int count=b&0x7f;
        if (input.length()-offset>=count) {
            b=GetByte();
            while (b==0 && count>1) {
                b=GetByte();
                --count;
            }
            switch (count) {
            case 0: return false;
            case 1: return mIsValid;
            case 2: GetByte(); return mIsValid;
            }
        }
        return false;
    }
    return mIsValid;
}
// FUNCTION: WON 0x007d9100
bool IntegerExtractor::Decode(int length,BigInteger& value)
{
    if (!mIsValid) return false;
    if (input.length()<=offset) return false;
    // Native observes the leading byte here; signed two's-complement conversion is inactive.
    PeekByte();
    while (length>0 && PeekByte()==0) {
        ++offset;
        --length;
    }
    RawBuffer bytes(length,(unsigned char)0);
    for (int i=0;i<length;++i) bytes[i]=GetByte();
    value.fromBinary(bytes);
    return mIsValid;
}
// FUNCTION: WON 0x007d92a0
bool IntegerExtractor::Extract(BigInteger& value)
{
    if (!mIsValid) return false;
    if (GetByte()!=INTEGER) return false;
    int count;
    if ((PeekByte()&0x80)==0) count=GetByte();
    else {
        int width=GetByte()&0x7f;
        if (width>2) return false;
        count=GetByte();
        if (width>1) count=(count<<8)|GetByte();
    }
    return Decode(count,value);
}
}
