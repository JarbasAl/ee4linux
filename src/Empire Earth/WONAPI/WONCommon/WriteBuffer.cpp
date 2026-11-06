// Original-backed complete WriteBuffer24 serializer family. SCRATCH, no new header subtree.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <string.h>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {
// FUNCTION: WON 0x006a8140
WriteBuffer::WriteBuffer(unsigned char lengthSize,bool includesLength)
{
    mAllocation=NULL;
    mData=NULL;
    mLengthFieldSize=lengthSize;
    mLengthIncludesField=includesLength;
    Reset();
}
// Native2a8190/17, normal owned-array deletion, not implicit/scalar quota.
// FUNCTION: WON 0x006a8190
WriteBuffer::~WriteBuffer()
{
    delete [] mAllocation;
}
// Native2a8110/31, real owned reset.
// FUNCTION: WON 0x006a8110
void WriteBuffer::Reset()
{
    delete [] mAllocation;
    mAllocation=NULL;
    mData=NULL;
    mLength=0;
    mCapacity=0;
}
// Native2a8130/13, conservative forwarding/state-transfer support0.
// FUNCTION: WON 0x006a8130
void WriteBuffer::Release()
{
    mAllocation=NULL;
    mData=NULL;
    Reset();
}
// Native2a81b0/45. Initial capacity check is unsigned; growth loop is signed32.
// FUNCTION: WON 0x006a81b0
void WriteBuffer::CheckWrite(unsigned long length)
{
    unsigned long needed=mLength+length;
    if(needed>mCapacity) {
        long capacity=static_cast<long>(mCapacity);
        if(capacity<32)
            capacity=32;
        while(capacity<static_cast<long>(needed))
            capacity <<= 1;
        Reserve(static_cast<unsigned long>(capacity));
    }
}
// Native2a81e0/91. Six prefix bytes reserved, existing payload copied before old free.
// FUNCTION: WON 0x006a81e0
void WriteBuffer::Reserve(unsigned long capacity)
{
    if(mCapacity>=capacity)
        return;
    char* allocation=new char[capacity+6];
    char* data=allocation+6;
    memcpy(data,mData,mLength);
    delete [] mAllocation;
    mAllocation=allocation;
    mData=data;
    mCapacity=capacity;
}
// Native2a8240/22, setter/forwarding support0.
// FUNCTION: WON 0x006a8240
void WriteBuffer::SetSize(unsigned long size)
{
    Reserve(size);
    mLength=size;
}
// Native2a8260/27, cursor/growth forwarding support0; canonical address spelling retained.
// FUNCTION: WON 0x006a8260
void WriteBuffer::Fn2A8260(unsigned long length)
{
    CheckWrite(length);
    mLength+=length;
}
// Native2a8280/59. No zero-length shortcut or source-alias repair.
// FUNCTION: WON 0x006a8280
void WriteBuffer::AppendBytes(const void* bytes,unsigned long length)
{
    CheckWrite(length);
    memcpy(mData+mLength,bytes,length);
    mLength+=length;
}
// Native2a82c0/15, primitive serialization forwarding support0.
// FUNCTION: WON 0x006a82c0
void WriteBuffer::AppendByte(unsigned char value) { AppendBytes(&value,1); }
// Native2a82d0/28, bool normalization and primitive forwarding support0.
// FUNCTION: WON 0x006a82d0
void WriteBuffer::AppendBool(bool value)
{
    unsigned char byte=value?1:0;
    AppendBytes(&byte,1);
}
// Native2a82f0/15, primitive forwarding support0.
// FUNCTION: WON 0x006a82f0
void WriteBuffer::AppendShort(short value) { AppendBytes(&value,2); }
// Native2a8300/15, primitive forwarding support0.
// FUNCTION: WON 0x006a8300
void WriteBuffer::AppendLong(long value) { AppendBytes(&value,4); }
// Native2a8310/59: only 1/2/4 length fields; other values emit nothing.
// FUNCTION: WON 0x006a8310
void WriteBuffer::AppendLength(unsigned long length,unsigned char size)
{
    switch(size) {
    case 1: AppendByte(static_cast<unsigned char>(length)); break;
    case 2: AppendShort(static_cast<short>(length)); break;
    case 4: AppendLong(static_cast<long>(length)); break;
    }
}
// Native2a8350/54, counted narrow bytes and independent current-length read after prefix.
// FUNCTION: WON 0x006a8350
void WriteBuffer::AppendString(const std::string& text,unsigned char size)
{
    AppendLength(text.length(),size);
    AppendBytes(text.c_str(),text.length());
}
// Native2a8390/57, prefix is wide character count, payload is raw two-byte units.
// FUNCTION: WON 0x006a8390
void WriteBuffer::AppendWString(const std::wstring& text,int size)
{
    AppendLength(text.length(),static_cast<unsigned char>(size));
    AppendBytes(text.c_str(),text.length()*2);
}
// Native2a83d0/59, NULL emits only optional zero length and no append call.
// FUNCTION: WON 0x006a83d0
void WriteBuffer::AppendBuffer(const ByteBuffer* buffer,unsigned char size)
{
    unsigned long length=buffer!=NULL?buffer->length():0;
    AppendLength(length,size);
    if(buffer!=NULL && length>0)
        AppendBytes(buffer->data(),length);
}
// Native2a8410/40: unchecked backpatch, primitive copying support0.
// FUNCTION: WON 0x006a8410
void WriteBuffer::SetBytes(unsigned long position,const void* bytes,unsigned long length)
{
    memcpy(mData+position,bytes,length);
}
// Native2a8440/20, primitive support0.
// FUNCTION: WON 0x006a8440
void WriteBuffer::SetByte(unsigned long position,unsigned char value) { SetBytes(position,&value,1); }
// Native2a8460/20, primitive support0.
// FUNCTION: WON 0x006a8460
void WriteBuffer::SetShort(unsigned long position,short value) { SetBytes(position,&value,2); }
// Native2a8480/349, length-prefix construction and real typed allocation/ownership transfer.
// FUNCTION: WON 0x006a8480
SmartPtr<ByteBuffer> WriteBuffer::ToByteBuffer(bool release)
{
    unsigned long value=mLength;
    unsigned long length=mLength;
    char* data=mData;
    switch(mLengthFieldSize) {
    case 1:
        if(mLengthIncludesField) ++value;
        --data;
        ++length;
        *data=static_cast<char>(value);
        break;
    case 2:
        if(value<0xffff) {
            if(mLengthIncludesField) value+=2;
            data-=2;
            length+=2;
            unsigned short word=static_cast<unsigned short>(value);
            memcpy(data,&word,2);
        } else {
            data-=6;
            length+=6;
            unsigned short marker=0xffff;
            memcpy(data,&marker,2);
            if(mLengthIncludesField) value+=4;
            memcpy(data+2,&value,4);
        }
        break;
    case 4:
        if(mLengthIncludesField) value+=4;
        data-=4;
        length+=4;
        memcpy(data,&value,4);
        break;
    }
    if(release) {
        char* allocation=mAllocation;
        Release();
        return new OffsetByteBuffer(allocation,static_cast<unsigned long>(data-allocation),length);
    }
    return new ByteBuffer(data,length,false);
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK chunk growth algorithm, unchanged owner and Reserve.
void WriteBuffer::ReserveChunk(unsigned long theNumBytesToReserve, unsigned long theMaxSize)
{
	int aTotalLen = theNumBytesToReserve;
	if(aTotalLen>mCapacity)
	{
		int aSize = mCapacity;
		if(aSize<32)
			aSize = 32;

		while(aSize<aTotalLen)
			aSize<<=1;

		if(theMaxSize>=theNumBytesToReserve && aSize>theMaxSize)
			aSize = theMaxSize;

		Reserve(aSize);
	}
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a8170
// (public: virtual void * __thiscall WONAPI::WriteBuffer::`scalar deleting destructor'(unsigned int)    [??_GWriteBuffer@WONAPI@@UAEPAXI@Z])
