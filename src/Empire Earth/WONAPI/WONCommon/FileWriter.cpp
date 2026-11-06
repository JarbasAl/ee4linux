// C:\EEX\Empire Earth\WONAPI\WONCommon\FileWriter.cpp (RECONSTRUCTION).
// Original bounded FileWriter family2a1e10..2a26a5; nextMD5family2a26b0 excluded.
// Full actual4B storage; filename/TU boundary provisional; source-only no runtimeauthority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <stdio.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyPersistence.h"
namespace WONAPI {
// Original support0x2a1e10/9B; simple initialization, accessor or forwarding support, zero breadth quota.
FileWriter::FileWriter() : mFile(NULL) {}
// Original support0x2a1e20/5B; simple initialization, accessor or forwarding support, zero breadth quota.
FileWriter::~FileWriter() { Close(); }
// FUNCTION: WON 0x006a1e30
bool FileWriter::Open(const char* inPath, bool inOpenIfReadOnly)
{
    Close();
    if (inPath[0] == 0)
        return false;
    mFile = fopen(inPath, "wb");
    if (mFile == NULL && inOpenIfReadOnly) {
        WONFile file(inPath);
        file.SetReadOnly(false);
        mFile = fopen(inPath, "wb");
    }
    return mFile != NULL;
}
// FUNCTION: WON 0x006a2060
void FileWriter::Close()
{
    if (mFile != NULL) {
        fclose(mFile);
        mFile = NULL;
    }
}
// Original support0x2a2080/20B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a2080
unsigned long FileWriter::pos()
{
    return mFile != NULL ? ftell(mFile) : 0;
}
// FUNCTION: WON 0x006a20a0
void FileWriter::SetPos(unsigned long inPos)
{
    if (mFile != NULL && fseek(mFile, inPos, SEEK_SET) != 0)
        throw FileWriterException("Attempt to setpos past end of file.");
}
// FUNCTION: WON 0x006a2400
void FileWriter::WriteBytes(const void* inData, unsigned long inLength)
{
    if (mFile == NULL)
        throw FileWriterException("NULL File.");
    if (fwrite(inData, 1, inLength, mFile) != inLength)
        throw FileWriterException("fwrite failed (disk full)");
}
// Original support0x2a25f0/15B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a25f0
void FileWriter::WriteByte(unsigned char inValue) { WriteBytes(&inValue, 1); }
// Original support0x2a2600/15B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a2600
void FileWriter::WriteShort(unsigned short inValue) { WriteBytes(&inValue, 2); }
// Original support0x2a2610/15B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a2610
void FileWriter::WriteLong(long inValue) { WriteBytes(&inValue, 4); }
// FUNCTION: WON 0x006a2620
void FileWriter::WriteLength(unsigned long inLength, unsigned char inFieldSize)
{
    switch (inFieldSize) {
    case 1: WriteByte((unsigned char)inLength); break;
    case 2: WriteShort((unsigned short)inLength); break;
    case 4: WriteLong((long)inLength); break;
    }
}
// Original support0x2a2660/29B; simple initialization, accessor or forwarding support, zero breadth quota.
// FUNCTION: WON 0x006a2660
void FileWriter::WriteRawString(const std::string& inString)
{
    WriteBytes(inString.c_str(), inString.length());
}
// FUNCTION: WON 0x006a2680
void FileWriter::WriteString(const std::string& inString, unsigned char inFieldSize)
{
    WriteLength(inString.length(), inFieldSize);
    WriteRawString(inString);
}
}

namespace WONAPI {
// Genuine SDK required byte-order helper; no forced emission or new Native claim.
inline bool IsLittleEndian()
{
	short aTest = 1;
	return ((*(char*)&aTest)&1)?true:false;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK wide string serialization.
void FileWriter::WriteRawWString(const std::wstring &theWStr)
{
	if(IsLittleEndian() && sizeof(wchar_t)==2)
		WriteBytes(theWStr.data(),theWStr.length()*2);
	else
	{
		for(int i=0; i<theWStr.length(); i++)
			WriteShort(theWStr[i]);
	}
}

void FileWriter::WriteWString(const std::wstring &theWStr, unsigned char theLengthFieldSize)
{
	WriteLength(theWStr.length(),theLengthFieldSize);
	WriteRawWString(theWStr);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a1e20
// (public: __thiscall WONAPI::FileWriter::~FileWriter(void)    [??1FileWriter@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006a21f0
// (public: virtual void * __thiscall WONAPI::FileWriterException::`scalar deleting destructor'(unsigned int)    [??_GFileWriterException@WONAPI@@UAEPAXI@)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a1e10
// (public: __thiscall WONAPI::FileWriter::FileWriter(void)    [??0FileWriter@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006a2210
// (public: virtual __thiscall WONAPI::FileWriterException::~FileWriterException(void)    [??1FileWriterException@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006a2280
// (public: __thiscall WONAPI::FileWriterException::FileWriterException(class WONAPI::FileWriterException const &)    [??0FileWriterException@WONAPI@@QAE@)
