// Original ByteBuffer family, owned allocation/copy and offset ownership. SCRATCH.
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
// Native2a0a20/150, strlen includes the terminating byte. NULL input is distinct.
// FUNCTION: WON 0x006a0a20
ByteBuffer::ByteBuffer(const char* text)
{
    if(text==NULL) {
        mData=NULL;
        mDataLen=0;
    } else {
        mDataLen=static_cast<unsigned long>(strlen(text))+1;
        mData=new char[mDataLen];
        memcpy(mData,text,mDataLen);
    }
}
// Native2a0ac0/163. Zero-length copying still allocates new char[0].
// FUNCTION: WON 0x006a0ac0
ByteBuffer::ByteBuffer(const void* data,unsigned long length,bool takeOwnership)
{
    if(takeOwnership) {
        mData=const_cast<char*>(static_cast<const char*>(data));
        mDataLen=length;
    } else {
        mData=new char[length];
        mDataLen=length;
        if(length>0)
            memcpy(mData,data,length);
    }
}
// Native2a0b70/77, actual normal owned-array destruction then RefCount base.
// FUNCTION: WON 0x006a0b70
ByteBuffer::~ByteBuffer()
{
    delete [] mData;
}
// Native2a0bc0/46, genuine complete OffsetByteBuffer20; requires canonical type proof.
// Conservative constructor support0: forwards adoption plus offset/base-pointer storage.
OffsetByteBuffer::OffsetByteBuffer(char* allocation,unsigned long offset,unsigned long length)
    :ByteBuffer(allocation+offset,length,true),mAllocation(allocation)
{
}
// Native2a0c10/93: free original allocation; prevent the base from deleting its view.
// FUNCTION: WON 0x006a0c10
OffsetByteBuffer::~OffsetByteBuffer()
{
    delete [] mAllocation;
    mData=NULL;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a0a00
// (public: virtual void * __thiscall WONAPI::ByteBuffer::`scalar deleting destructor'(unsigned int)    [??_GByteBuffer@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006a0bc0
// (public: __thiscall WONAPI::OffsetByteBuffer::OffsetByteBuffer(char *,unsigned long,unsigned long)    [??0OffsetByteBuffer@WONAPI@@QAE@PADKK@Z])
// TEMPLATE: WON 0x006a0bf0
// (public: virtual void * __thiscall WONAPI::OffsetByteBuffer::`scalar deleting destructor'(unsigned int)    [??_GOffsetByteBuffer@WONAPI@@UAEPAXI@Z])
