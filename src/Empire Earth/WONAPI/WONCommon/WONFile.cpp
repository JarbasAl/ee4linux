// C:\EEX\Empire Earth\WONAPI\WONCommon\WONFile.cpp (RECONSTRUCTION).
// Original WONFile family2a7610..2a8106; exact filename/TU boundaries provisional.

// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <sys/stat.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONBuffer.h"
#include "ee/wonapi/WONLobbyResource.h"
namespace WONAPI {
// FUNCTION: WON 0x006a7610
WONFile::WONFile(const std::string& inPath)
{
    SetFilePath(inPath);
}
// FUNCTION: WON 0x006a76c0
void WONFile::Reset()
{
    mPath.erase();
    mS10.erase();
    mFileName.erase();
    mS40.erase();
    mS50.erase();
    mB6B = false;
    mB6C = false;
    mB6D = false;
    mStatValid = false;
    mB6F = false;
    mB6A = false;
}
// FUNCTION: WON 0x006a7840
void WONFile::SetFilePath(const std::string& inPath)
{
    Reset();
    mPath = inPath;
    for (std::string::size_type i = 0; i < mPath.length(); ++i)
        if (mPath[i] == '/')
            mPath[i] = '\\';
}
// FUNCTION: WON 0x006a7960
bool WONFile::Delete(bool inEvenIfReadOnly)
{
    if (inEvenIfReadOnly) {
        if (!mStatValid)
            CalcFileInfo();
        if (mB69)
            SetReadOnly(false);
    }
    return remove(mPath.c_str()) == 0;
}
// FUNCTION: WON 0x006a79b0
bool WONFile::SetReadOnly(bool inReadOnly)
{
    return _chmod(mPath.c_str(), inReadOnly ? 0x100 : 0x180) == 0;
}

// FUNCTION: WON 0x006a79e0
const std::string& WONFile::GetFilePath() { return mPath; }
// FUNCTION: WON 0x006a79f0
void WONFile::CalcFileInfo()
{
    struct _stat info;
    mStatValid = true;
    if (_stat(mPath.c_str(), &info) == 0) {
        mB68 = true;
        mB69 = (info.st_mode & 2) == 0;
        mSize = info.st_size;
        mModifyTime = info.st_mtime;
        mB6A = (info.st_mode & 0x4000) != 0;
    } else {
        mSize = 0;
        mModifyTime = 0;
        mB68 = false;
        mB69 = false;
    }
}
// FUNCTION: WON 0x006a7a60
void WONFile::CalcFileDirectory()
{
    std::string::size_type sep = mPath.rfind('\\');
    if (sep != std::string::npos)
        mS10 = mPath.substr(0, sep);
    mFileName = mPath.substr(sep + 1);
    mB6B = true;
    mB6C = true;
}
// FUNCTION: WON 0x006a7c30
void WONFile::CalcFullPath()
{
    char path[260];
    if (_fullpath(path, mPath.c_str(), sizeof(path)) != NULL)
        mS50 = path;
    mB6F = true;
}
// FUNCTION: WON 0x006a7d80
void WONFile::CalcFileExtension()
{
    GetFileName();
    std::string::size_type dot = mFileName.rfind('.');
    if (dot != std::string::npos) {
        mS40 = mFileName.substr(dot + 1);
        mS30 = mFileName.substr(0, dot);
    } else {
        mS30 = mFileName;
    }
    mB6D = true;
}
// FUNCTION: WON 0x006a7fd0
const std::string& WONFile::GetFileDirectory()
{
    if (!mB6B) CalcFileDirectory();
    return mS10;
}
// FUNCTION: WON 0x006a7ff0
const std::string& WONFile::GetFileName()
{
    if (!mB6C) GetFileDirectory();
    return mFileName;
}
// FUNCTION: WON 0x006a8010
const std::string& WONFile::GetFileExtension()
{
    if (!mB6D) CalcFileExtension();
    return mS40;
}
// FUNCTION: WON 0x006a8030
const std::string& WONFile::GetFullPath()
{
    if (!mB6F) CalcFullPath();
    return mS50;
}
// FUNCTION: WON 0x006a8050
unsigned long WONFile::GetSize()
{
    if (!mStatValid) CalcFileInfo();
    return mSize;
}
// FUNCTION: WON 0x006a8070
long WONFile::GetModifyTime()
{
    if (!mStatValid) CalcFileInfo();
    return mModifyTime;
}
// FUNCTION: WON 0x006a8090
bool WONFile::Exists()
{
    if (!mStatValid) CalcFileInfo();
    return mB68;
}
// FUNCTION: WON 0x006a80b0
bool WONFile::IsDir()
{
    if (!mStatValid) CalcFileInfo();
    return mB6A;
}
// FUNCTION: WON 0x006a80d0
bool WONFile::CreateAsDirectory(bool inUnused)
{
    if (Exists()) return IsDir();
    return CreateDirectoryA(mPath.c_str(), NULL) != 0;
}
}

#include <vector>
#include <direct.h>
#include <ctype.h>
namespace WONAPI {
using namespace std;
// SOURCE_ONLY_NO_NATIVE_ID: exact SDK Windows separator branch; no Native data identity.
const char WONFile::PATH_SEP = '\\';

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK method on unchanged current WONFile70 owner.
bool WONFile::NormalizePath()
{
	if(mPath.empty())
		return false;

	std::string aFilePath;
	bool firstIsSlash = false;
	if(mPath[0]!=PATH_SEP)
		aFilePath = PATH_SEP + mPath;
	else
	{
		firstIsSlash = true;
		aFilePath = mPath;
	}

	typedef std::vector<int> DirPosVec;
	DirPosVec aVec;
	
	int aPos = 0;
	while(aPos < aFilePath.length())
	{
		if(aFilePath[aPos]==PATH_SEP)
		{
			bool hadReplacement = false;
			if(aPos+2<aFilePath.length() && aFilePath[aPos+1]=='.' && aFilePath[aPos+2]=='.')
			{
				if(aPos+3>=aFilePath.length() || aFilePath[aPos+3]==PATH_SEP)
				{
					hadReplacement = true;

					// -----> /../
					if(!aVec.empty())
					{
						int aPrevDirPos = aVec.back();
						aFilePath = aFilePath.substr(0,aPrevDirPos) + aFilePath.substr(aPos+3);
						aPos = aPrevDirPos; 
						aVec.pop_back();
					}
					else
						aPos++;
				}
			}
			else if(aPos+1<aFilePath.length() && aFilePath[aPos+1]=='.')
			{
				// ----> /./
				if(aPos+2>=aFilePath.length() || aFilePath[aPos+2]==PATH_SEP)  
				{
					hadReplacement = true;
					aFilePath = aFilePath.substr(0,aPos) + aFilePath.substr(aPos+2);
				}
			}

			if(!hadReplacement)
			{
				aVec.insert(aVec.end(),(DirPosVec::size_type)1,aPos);
				aPos++;
			}
		}
		else						
			aPos++;
	}

	if(!firstIsSlash)
		mPath = aFilePath.substr(1);
	else
		mPath = aFilePath;

	mS10.erase();
	mB6B = false;
	
	return true;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK method on unchanged current WONFile70 owner.
std::string WONFile::GetRelativePath(const WONFile &theSubFile)
{
	const std::string &aPath1 = GetFileDirectory();
	const std::string &aPath2 = theSubFile.mPath;
	if(aPath2.length() > aPath1.length())
	{
		int i;
		for(i=0; i<aPath1.length(); i++)
		{
			unsigned char char1 = aPath1[i];
			unsigned char char2 = aPath2[i];

			char1 = toupper(char1);
			char2 = toupper(char2);
			if(char1!=char2)
				break;
		}

		if(i==aPath1.length() && aPath2[i]==PATH_SEP) // paths match
			return aPath2.substr(i+1);
	}

	// no match --> return absolute path
	return aPath2;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK method on unchanged current WONFile70 owner.
WONFile WONFile::GetWorkingDirectory()
{
#ifdef _XBOX
	return WONFile();
#else
	char aLocalPath[MAX_PATH];
	_getcwd(aLocalPath, sizeof(aLocalPath));
	return WONFile(aLocalPath);
#endif
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK method on unchanged current WONFile70 owner.
bool WONFile::SetWorkingDirectory()
{
#ifdef _XBOX
	return false;
#else
	GetFileDirectory();
	if(mS10.empty())
		return false;
	else
		return chdir(mS10.c_str())==0;
#endif
}
}
