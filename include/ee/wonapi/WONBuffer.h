// Canonical WON byte/read/write buffers. Requires the ordinary LLE/windows foundation context.
// Pure declaration extraction: existing contracts are preserved; target storage is ReadBuffer0c,
// WriteBuffer18, ByteBuffer10, ReadBufferException18. No GUI/Core/Root header dependency.
#ifndef EE_WONAPI_WONBUFFER_H
#define EE_WONAPI_WONBUFFER_H
#include <exception>
#include <string>
#include "ee/EEWonNet.h"
namespace WONAPI {

// a reference-counted byte buffer (+8 data, +0xc length)
class ByteBuffer : public RefCount {
public:
    explicit ByteBuffer(const char* inText); // 0x2a0a20, native148B including NUL
    ByteBuffer(const void* inData, unsigned long inLength, bool inTakeOwnership);    // 0x2a0ac0
    virtual ~ByteBuffer();                 // 0x2a0b70, actual77B delete[] owner8 then RefCount
    const char* data() const { return this ? mData : NULL; }
    unsigned long length() const { return this ? mDataLen : 0; }

    char* mData;                            // +0x08
    unsigned long mDataLen;                 // +0x0c
};

inline AuthLoginCommunityData::AuthLoginCommunityData() {}
typedef ConstSmartPtr<ByteBuffer> ByteBufferPtr;

// Complete actual 20-byte offset buffer: owns the original allocation at +10.
// Mutable allocation parameter is a bounded native ownership-contract spelling.
class OffsetByteBuffer : public ByteBuffer {
public:
    OffsetByteBuffer(char* inAllocation, unsigned long inOffset, unsigned long inLength); //2a0bc0/46
    virtual ~OffsetByteBuffer(); //2a0c10/93, delete original then clear base data
    char* mAllocation; //10, sole additional actual array owner
};

class ReadBuffer {
public:
    const char* data() const { return mData; }
    int pos() { return mL00; }
    void SetPos(int thePos) { mL00=thePos; }
    int Available() { return length()-pos(); }
    unsigned long ReadLength(unsigned char); //2a3d70, native signedbyte/short extension toDWORD
    void ReadRawString(std::string&,unsigned long); //2a3db0
    void ReadRawWString(std::wstring&,unsigned long); //2a3f60
    bool ReadBool();                        // 0x2a3d30
    unsigned char ReadByte();               // 0x2a3d40
    short ReadShort();                      // 0x2a3d50
    long ReadLong();                        // 0x2a3d60
    void ReadWString(std::wstring& outText, int inLengthSize);    // 0x2a40f0

    // (SendFileOp's file window: a 0xc-byte ReadBuffer over its buffer; names PROVISIONAL)
    ReadBuffer(const void* inData, unsigned long inLength);   // 0x2a39f0
    void SetData(const void* inData, unsigned long inLength);  // 0x2a3a10 (rewinds)
    const void* ReadBytes(unsigned long inLength);      // 0x2a3a30 (throws past the end)
    void Rewind();                         // 0x2a41c0, resets read position only
    void ReadString(std::string& outText, unsigned char inLengthSize = 2); // 0x2a3f40
    ByteBufferPtr ReadBuf(unsigned char inLengthSize); // 0x2a4110, native owning return
    unsigned long length() const { return mL08; } // actual public SDK width; no field migration

    // (EELobbyGame 0x292c7e: `[+8] - [+0] > 0`, bytes left to read; names PROVISIONAL)
    long mL00;                              // +0x00 the read position
    const char* mData;                      // +0x04
    long mL08;                              // +0x08 the length
};

class WriteBuffer {
public:
    void AppendRawString(const std::string& theStr) { AppendString(theStr,0); }
    char* data() { return mData; }
    unsigned long length() const { return mLength; }
    unsigned long capacity() const { return mCapacity; }
    void Reset(); //2a8110/31, destroys allocation and clears storage
    void Release(); //2a8130/13, relinquishes pointers then resets
    void CheckWrite(unsigned long); //2a81b0/45
    void SetSize(unsigned long); //2a8240/22
    void AppendLength(unsigned long, unsigned char); //2a8310/59
    void SetBytes(unsigned long, const void*, unsigned long); //2a8410/40
    void Fn2A8260(unsigned long); //2a8260, full27B, grows then advances actual length
    void AppendString(const std::string&, unsigned char = 2); //2a8350, full54B

    // (EELobbyGame 0x293437/0x2934a0: a 0x18-byte local, ctor (0, true), the dtor called directly)
    WriteBuffer(unsigned char inLengthFieldSize, bool inLengthIncludesField);   // 0x2a8140 (vtable 0x84c930)
    virtual ~WriteBuffer();                 // 0x2a8190 (delete[] of +0x14)
    ConstSmartPtr<ByteBuffer> ToByteBuffer(bool inRelease);    // 0x2a8480
    void AppendByte(unsigned char inByte);  // 0x2a82c0
    void AppendBool(bool inValue);          // 0x2a82d0
    void AppendShort(short inValue);        // 0x2a82f0
    void SetShort(unsigned long inPos, short inValue); // 0x2a8460
    void AppendLong(long inValue);          // 0x2a8300
    void Reserve(unsigned long inNumBytes); // 0x2a81e0, actual91B
    void ReserveChunk(unsigned long theNumBytesToReserve,unsigned long theMaxSize=0);
    void AppendBuffer(const ByteBuffer* inBuffer, unsigned char inLengthFieldSize = 0); // 0x2a83d0
    void AppendBytes(const void* inData, unsigned long inLength); // 0x2a8280 (LobbyGame summary writes six address bytes)
    void AppendWString(const std::wstring& inText, int inLengthSize);    // 0x2a8390
    void SetByte(unsigned long inPos, unsigned char inByte);   // 0x2a8440

    unsigned char mLengthFieldSize;         // +0x04
    bool mLengthIncludesField;              // +0x05; natural alignment +06/+07
    unsigned long mCapacity;                // +0x08
    unsigned long mLength;                  // +0x0c
    char* mData;                           // +0x10; allocation +6 after growth
    char* mAllocation;                     // +0x14; delete[] owner
};

// RTTI: ReadBufferException : std::exception. Original dtor 0x2a3b40 destroys string +8.
class ReadBufferException : public std::exception {
public:
    ReadBufferException() {} //actual default constructor retained, support0
    ReadBufferException(const std::string& text) {mMessage=text;} //actual2a3a30 throw owner24
    std::string mMessage;                   // +0x08; total size 0x18
};

}
#endif
