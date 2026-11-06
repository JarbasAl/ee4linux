// C:\EEX\Empire Earth\WONAPI\WONCommon\FileReader.cpp (RECONSTRUCTION).
// Original bounded FileReader family2a0ce0..2a1e05; exact source filename/TU boundary provisional.
// Full actual20B storage; near/support remain original-carved, source-only no runtimeauthority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <stdio.h>
#include <string.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyPersistence.h"
namespace WONAPI {
// Original support0x2a0ce0/20B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a0ce0
void FileReader::Init()
{
    mFile = NULL;
    mBuffer = NULL;
    mLength = 0;
    mPosition = 0;
    mOwnBuffer = false;
    mMemoryMode = false;
}
// Original support0x2a0d00/12B; simple initialization, accessor or forwarding support, zero breadth quota.
FileReader::FileReader() { Init(); }
// FUNCTION: WON 0x006a0d10
FileReader::FileReader(const char* inPath) { Init(); Open(inPath); }
// FUNCTION: WON 0x006a0d30
FileReader::~FileReader()
{
    if (mFile != NULL)
        fclose(mFile);
}
// FUNCTION: WON 0x006a0d40
bool FileReader::Open(const char* inPath)
{
    Close();
    if (inPath[0] == 0) return false;
    mFile = fopen(inPath, "rb");
    mMemoryMode = false;
    return mFile != NULL;
}
// FUNCTION: WON 0x006a0d80
bool FileReader::OpenText(const char* inPath)
{
    Close();
    if (inPath[0] == 0) return false;
    mFile = fopen(inPath, "r");
    mMemoryMode = false;
    return mFile != NULL;
}
// FUNCTION: WON 0x006a0dc0
void FileReader::Open(const void* inData, unsigned long inLength)
{
    Close();
    mBuffer = (const char*)inData;
    mLength = inLength;
    mMemoryMode = true;
}
// FUNCTION: WON 0x006a0de0
void FileReader::Close()
{
    if (mBuffer != NULL) {
        if (mOwnBuffer) delete[] const_cast<char*>(mBuffer);
        mBuffer = NULL;
        mLength = 0;
        mPosition = 0;
        mOwnBuffer = false;
    }
    if (mFile != NULL) {
        fclose(mFile);
        mFile = NULL;
    }
    mMemoryMode = false;
}
// Original support0x2a0e20/31B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a0e20
unsigned long FileReader::pos()
{
    if (mBuffer != NULL) return mPosition;
    if (mFile != NULL) return ftell(mFile);
    return 0;
}
// FUNCTION: WON 0x006a0e40
unsigned long FileReader::length()
{
    if (!mMemoryMode && mFile != NULL) {
        unsigned long oldPos = pos();
        fseek(mFile, 0, SEEK_END);
        mLength = pos();
        mMemoryMode = true;
        SetPos(oldPos);
    }
    return mLength;
}
// FUNCTION: WON 0x006a0e90
void FileReader::SetPos(unsigned long inPos)
{
    if (mBuffer != NULL) {
        if (inPos >= mLength)
            throw FileReaderException("Attempt to setpos past end of mem buffer.");
        mPosition = inPos;
    } else if (mFile != NULL && fseek(mFile, inPos, SEEK_SET) != 0) {
        throw FileReaderException("Attempt to setpos past end of file.");
    }
}
// FUNCTION: WON 0x006a12d0
int FileReader::ReadMaxBytes(void* outData, int inMaxBytes)
{
    if (mBuffer != NULL) {
        int count = mLength - mPosition;
        if (count > inMaxBytes) count = inMaxBytes;
        memcpy(outData, mBuffer + mPosition, count);
        mPosition += count;
        return count;
    }
    if (mFile == NULL) throw FileReaderException("NULL File.");
    return fread(outData, 1, inMaxBytes, mFile);
}
// FUNCTION: WON 0x006a1460
void FileReader::ReadBytes(void* outData, unsigned long inLength)
{
    if (mBuffer != NULL) {
        if (mPosition + inLength > mLength)
            throw FileReaderException("Attempt to read past end of mem buffer.");
        memcpy(outData, mBuffer + mPosition, inLength);
        mPosition += inLength;
    } else {
        if (mFile == NULL) throw FileReaderException("NULL File.");
        if (fread(outData, 1, inLength, mFile) != inLength)
            throw FileReaderException("Attempt to read past end of file.");
    }
}
// FUNCTION: WON 0x006a1780
int FileReader::TryReadByte()
{
    if (mBuffer != NULL) {
        if (mPosition < mLength)
            return (unsigned char)mBuffer[mPosition++];
    } else if (mFile != NULL) {
        return fgetc(mFile);
    }
    return -1;
}
// FUNCTION: WON 0x006a17c0
void FileReader::SkipBytes(unsigned long inLength)
{
    if (mBuffer != NULL) {
        if (mPosition + inLength > mLength)
            throw FileReaderException("Attempt to read past end of mem buffer.");
        mPosition += inLength;
    } else {
        if (mFile == NULL) throw FileReaderException("NULL File.");
        if (fseek(mFile, inLength, SEEK_CUR) != 0)
            throw FileReaderException("Attempt to read past end of file.");
    }
}
// FUNCTION: WON 0x006a1ac0
void FileReader::ReadIntoMemory()
{
    if (mBuffer == NULL && mFile != NULL) {
        unsigned long size = length();
        if (size > 0) {
            unsigned long oldPos = pos();
            SetPos(0);
            char* data = new char[size];
            int count = ReadMaxBytes(data, size);
            Open(data, count);
            mOwnBuffer = true;
            SetPos(oldPos);
        }
    }
}
// Original support0x2a1b20/19B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a1b20
unsigned char FileReader::ReadByte() { unsigned char value; ReadBytes(&value, 1); return value; }
// Original support0x2a1b40/20B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a1b40
unsigned short FileReader::ReadShort() { unsigned short value; ReadBytes(&value, 2); return value; }
// Original support0x2a1b60/19B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a1b60
long FileReader::ReadLong() { long value; ReadBytes(&value, 4); return value; }
// FUNCTION: WON 0x006a1b80
int FileReader::ReadLength(int inFieldSize)
{
    switch (inFieldSize) {
    case 1: return ReadByte();
    case 2: return ReadShort();
    case 4: return ReadLong();
    default: return 0;
    }
}
// FUNCTION: WON 0x006a1bc0
void FileReader::ReadRawString(std::string& outString, int inNumChars)
{
    outString.erase();
    outString.reserve(inNumChars);
    for (int i = 0; i < inNumChars; ++i)
        outString += (char)ReadByte();
}
// FUNCTION: WON 0x006a1d90
void FileReader::ReadString(std::string& outString, unsigned char inFieldSize)
{
    ReadRawString(outString, ReadLength(inFieldSize));
}
// FUNCTION: WON 0x006a1dc0
void FileReader::Rewind()
{
    if (mBuffer != NULL) mPosition = 0;
    else if (mFile != NULL) rewind(mFile);
}
// Original support0x2a1de0/37B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a1de0
bool FileReader::EndOfFile()
{
    if (mBuffer != NULL) return mPosition >= mLength;
    if (mFile != NULL) return feof(mFile) ? true : false;
    return true;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK memory-reader constructor support.
FileReader::FileReader(const void *theMemDataP, unsigned long theMemDataLen)
{
	Init();
	Open(theMemDataP,theMemDataLen);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a0d00
// (public: __thiscall WONAPI::FileReader::FileReader(void)    [??0FileReader@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006a10b0
// (public: virtual __thiscall std::exception::~exception(void)    [??1exception@std@@UAE@XZ])
// TEMPLATE: WON 0x006a10c0
// (public: virtual void * __thiscall WONAPI::FileReaderException::`scalar deleting destructor'(unsigned int)    [??_GFileReaderException@WONAPI@@UAEPAXI@)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a10e0
// (public: virtual __thiscall WONAPI::FileReaderException::~FileReaderException(void)    [??1FileReaderException@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a1150
// (public: __thiscall WONAPI::FileReaderException::FileReaderException(class WONAPI::FileReaderException const &)    [??0FileReaderException@WONAPI@@QAE@)
