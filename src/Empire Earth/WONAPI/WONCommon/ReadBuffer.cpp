// Native full ReadBuffer12 and ReadBufferException24 families; source only.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {
// FUNCTION: WON 0x006a39f0
ReadBuffer::ReadBuffer(const void* data,unsigned long length)
{ SetData(data,length); }
// FUNCTION: WON 0x006a3a10
void ReadBuffer::SetData(const void* data,unsigned long length)
{
    mData=(const char*)data;
    mL08=length;
    Rewind();
}
// FUNCTION: WON 0x006a41c0
void ReadBuffer::Rewind() {mL00=0;}
// FUNCTION: WON 0x006a3a30
const void* ReadBuffer::ReadBytes(unsigned long amount)
{
    if((unsigned long)mL08-(unsigned long)mL00<amount)
        throw ReadBufferException("Attempt to read past end of buffer.");
    const char* result=mData+mL00;
    mL00+=amount;
    return result;
}
// FUNCTION: WON 0x006a3b40
// (the implicit destructor of ReadBufferException is emitted here, after the throw in ReadBytes)
// FUNCTION: WON 0x006a3d30
bool ReadBuffer::ReadBool() {return ReadByte()!=0;}
// FUNCTION: WON 0x006a3d40
unsigned char ReadBuffer::ReadByte() {return *(const unsigned char*)ReadBytes(1);}
// FUNCTION: WON 0x006a3d50
short ReadBuffer::ReadShort() {return *(const short*)ReadBytes(2);}
// FUNCTION: WON 0x006a3d60
long ReadBuffer::ReadLong() {return *(const long*)ReadBytes(4);}
// FUNCTION: WON 0x006a3d70
unsigned long ReadBuffer::ReadLength(unsigned char fieldSize)
{
    switch(fieldSize) {
    case 1:return (signed char)ReadByte();
    case 2:return ReadShort();
    case 4:return ReadLong();
    default:return 0;
    }
}
// FUNCTION: WON 0x006a3db0
void ReadBuffer::ReadRawString(std::string& text,unsigned long length)
{
    text.erase();
    const char* data=(const char*)ReadBytes(length);
    text.append(data,length);
}
// Original3f40/30 forwarding wrapper, zeroquota.
// FUNCTION: WON 0x006a3f40
void ReadBuffer::ReadString(std::string& text,unsigned char fieldSize)
{ReadRawString(text,ReadLength(fieldSize));}
// FUNCTION: WON 0x006a3f60
void ReadBuffer::ReadRawWString(std::wstring& text,unsigned long length)
{
    text.erase();
    const wchar_t* data=(const wchar_t*)ReadBytes(length*2);
    text.append(data,length);
}
// Original40f0/30 forwarding wrapper, existing canonicalint contract retained, zeroquota.
// FUNCTION: WON 0x006a40f0
void ReadBuffer::ReadWString(std::wstring& text,int fieldSize)
{ReadRawWString(text,ReadLength((unsigned char)fieldSize));}
// FUNCTION: WON 0x006a4110
ByteBufferPtr ReadBuffer::ReadBuf(unsigned char fieldSize)
{
    unsigned long length=ReadLength(fieldSize);
    return new ByteBuffer(ReadBytes(length),length,false);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a3b20
// (public: virtual void * __thiscall WONAPI::ReadBufferException::`scalar deleting destructor'(unsigned int)    [??_GReadBufferException@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x006a40c0
// (_wmemcpy    [_wmemcpy])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a3bb0
// (public: __thiscall WONAPI::ReadBufferException::ReadBufferException(class WONAPI::ReadBufferException const &)    [??0ReadBufferException@WONAPI@@QAE@)
