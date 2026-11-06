// Original-backed CDKey key validation, machine binding, decryption and registry input.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONBlowfish.h"
#include "ee/wonapi/WONCRC16.h"
namespace WONAPI {

// FUNCTION: WON 0x006e3090
CDKey::CDKey() { mValid = false; }

// FUNCTION: WON 0x006e30c0
void CDKey::Invalidate() { mValid = false; }
// FUNCTION: WON 0x006e30d0
void CDKey::SetProductString(const std::string& product)
{
    Invalidate();
    mProduct = product;
}
// FUNCTION: WON 0x006e3220
bool CDKey::CalcKeyFromBuffer(const char* raw)
{
    mP14 = new ByteBuffer(raw, 8, false);
    CRC16 crc;
    crc.Put(mProduct.data(), mProduct.length());
    crc.Put(raw, 3);
    crc.Put(raw+4, 4);
    char check = static_cast<char>((crc.Get() & 0x0fff) >> 4);
    mValid = check == raw[3];
    return mValid;
}
// FUNCTION: WON 0x006e3320
DWORD CDKey::GetMachineId(bool* success)
{
    if (success != NULL)
        *success = true;
    char path[MAX_PATH];
    char drive[_MAX_DRIVE];
    char directory[_MAX_DIR];
    char filename[_MAX_FNAME];
    char extension[_MAX_EXT];
    GetWindowsDirectoryA(path, MAX_PATH);
    _splitpath(path, drive, directory, filename, extension);
    std::string root = drive;
    if (!root.empty()) {
        if (root[root.length()-1] != '\\')
            root += '\\';
        DWORD serial = 0;
        GetVolumeInformationA(root.c_str(), NULL, 0, &serial, NULL, NULL, NULL, 0);
        return serial;
    }
    if (success != NULL)
        *success = false;
    return 0;
}
// FUNCTION: WON 0x006e35a0
void CDKey::GetSymmetricKey(Blowfish& key, bool useMachine) const
{
    CRC16 crc;
    RawBuffer bytes;
    if (useMachine) {
        bool gotId;
        DWORD machineId = GetMachineId(&gotId);
        if (gotId)
            bytes.append(reinterpret_cast<const unsigned char*>(&machineId), 4);
    }
    crc.Put(mProduct.data(), mProduct.length());
    unsigned short check = crc.Get();
    bytes.append(reinterpret_cast<const unsigned char*>(&check), 2);
    for (int i=0; i<3 && i<mProduct.length(); ++i) {
        char letter = mProduct[i];
        crc.Put(&letter, 1);
        check = crc.Get();
        bytes.append(reinterpret_cast<const unsigned char*>(&check), 2);
    }
    key.SetKey(bytes.data(), bytes.length());
}
// FUNCTION: WON 0x006e3880
bool CDKey::InitEncrypted(const void* encrypted, unsigned long length)
{
    mValid = false;
    mP14 = NULL;
    if (encrypted == NULL)
        return false;
    bool success = false;
    bool useMachine = GetUseNewEncryptMethod();
    Blowfish key;
    GetSymmetricKey(key, useMachine);
    ByteBufferPtr decoded = key.Decrypt(encrypted, length);
    if (decoded.get() != NULL)
        success = CalcKeyFromBuffer(decoded->data());
    if (!success) {
        useMachine = !useMachine;
        GetSymmetricKey(key, useMachine);
        decoded = key.Decrypt(encrypted, length);
        if (decoded.get() != NULL)
            success = CalcKeyFromBuffer(decoded->data());
    }
    if (!useMachine)
        NotifyUsedOldMethodToDecrypt();
    return success;
}
// FUNCTION: WON 0x006e3ac0
bool CDKey::LoadFromRegistry(long root, const char* path)
{
    if (mProduct.empty())
        return false;
    RegKey key(root, std::string(path), false, true);
    if (!key.mIsOpen)
        return false;
    ByteBufferPtr encrypted;
    if (key.QueryBinary(mProduct, encrypted) != 0)
        return false;
    return InitEncrypted(encrypted->data(), encrypted->length());
}
}

#include <ctype.h>
#include <stdio.h>
#include <string.h>
namespace WONAPI {
using namespace std;
namespace {
const char* SKIPCHAR_MAP = "- \t";
const char* STRINGKEY_MAP = "CVCNCVCNCVCNCVCNNNNN";
const unsigned int STRINGKEY_LEN = 20;
const int DASH_OFFSET = 4;
const unsigned char BETA_MASK = 0x01;
const unsigned char FORCE_MASK = 0x80;

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static void AddBits(char *theBuf, int &theOffset, int theBits, int theNumBits)
{
	int aBytePos = theOffset/8;
	int aBitPos = theOffset%8;

	int aMask = (theBits<<aBitPos)&0xff;
	theBuf[aBytePos] |= aMask;

	int extra = aBitPos + theNumBits - 8;
	if(extra>0)
	{
		aMask = (theBits>>(theNumBits - extra))&0xff;
		theBuf[aBytePos+1] |= aMask;
	}
		
	theOffset+=theNumBits;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static bool ProcessCChar(char *theBuf, int& theOffset, char theChar)
{
	int aVal = 0;

	// Determine mask based on char
	switch (toupper(theChar))
	{
		case 'B':
			aVal = 0;   break;
		case 'C':
			aVal = 1;   break;
		case 'D':
			aVal = 2;   break;
		case 'F':
			aVal = 3;   break;
		case 'G':
			aVal = 4;   break;
		case 'J':
			aVal = 5;   break;
		case 'L':
			aVal = 6;   break;
		case 'M':
			aVal = 7;   break;
		case 'N':
			aVal = 8;   break;
		case 'P':
			aVal = 9;   break;
		case 'R':
			aVal = 10;  break;
		case 'S':
			aVal = 11;  break;
		case 'T':
			aVal = 12;  break;
		case 'W':
			aVal = 13;  break;
		case 'X':
			aVal = 14;  break;
		case 'Z':
			aVal = 15;  break;
		default:
			return false;
	}

	AddBits(theBuf,theOffset,aVal,4);
	return true;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static bool ProcessVChar(char *theBuf, int& theOffset, char theChar)
{
	int aVal = 0;

	// Determine mask based on char
	switch (toupper(theChar))
	{
		case 'A':
			aVal = 0;  break;
		case 'E':
			aVal = 1;  break;
		case 'U':
			aVal = 2;  break;
		case 'Y':
			aVal = 3;  break;
		default:
			return false;
	}

	AddBits(theBuf,theOffset,aVal,2);
	return true;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static bool ProcessNChar(char *theBuf, int& theOffset, char theChar)
{
	if(!isdigit(theChar) || theChar<='1') // has to be '2'-'9'
		return false;

	int aVal = (theChar - '0') - 2;
	AddBits(theBuf,theOffset,aVal,3);
	return true;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static int ValFromBits(const char *theBuf, int &theOffset, int theNumBits)
{
	int aBytePos = theOffset/8;
	int aBitPos = theOffset%8;

	int aVal = ((unsigned char)theBuf[aBytePos]>>aBitPos)&((1<<theNumBits)-1)&0xff;
	
	int extra = theNumBits + aBitPos - 8;
	if(extra>0)
	{
		int extraBits = theBuf[aBytePos+1] & ((1<<extra)-1);
		extraBits <<= theNumBits-extra;
		aVal |= extraBits;
	}

	theOffset+=theNumBits;
	return aVal;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static char BuildCChar(const char *theBuf, int& theOffset) 
{
	char aChar = 0;

	// Determine mask based on char
	switch (ValFromBits(theBuf, theOffset, 4))
	{
	case 0:
		aChar = 'B';   break;
	case 1:
		aChar = 'C';   break;
	case 2:
		aChar = 'D';   break;
	case 3:
		aChar = 'F';   break;
	case 4:
		aChar = 'G';   break;
	case 5:
		aChar = 'J';   break;
	case 6:
		aChar = 'L';   break;
	case 7:
		aChar = 'M';   break;
	case 8:
		aChar = 'N';   break;
	case 9:
		aChar = 'P';   break;
	case 10:
		aChar = 'R';   break;
	case 11:
		aChar = 'S';   break;
	case 12:
		aChar = 'T';   break;
	case 13:
		aChar = 'W';   break;
	case 14:
		aChar = 'X';   break;
	case 15:
		aChar = 'Z';   break;
	}

	return aChar;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static char BuildVChar(const char *theBuf, int& theOffset) 
{
	char aChar = 0;

	// Determine mask based on char
	switch (ValFromBits(theBuf, theOffset, 2))
	{
		case 0:
			aChar = 'A';   break;
		case 1:
			aChar = 'E';   break;
		case 2:
			aChar = 'U';   break;
		case 3:
			aChar = 'Y';   break;
	}

	return aChar;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK key codec helper.
static char BuildNChar(const char *theBuf, int& theOffset)
{
	return ValFromBits(theBuf, theOffset, 3) + '0' + 2;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
CDKey::CDKey(const string& theProduct) : mProduct(theProduct)
{
	mValid = false;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
bool CDKey::Init(const string& theKey)
{
	mValid = false;
	mP14 = NULL;

	// remove skip chars such as dashes
	string aKey;
	int aPos = 0;
	while(aPos < theKey.length()) 
	{
		unsigned char aChar = theKey[aPos];
		if(strchr(SKIPCHAR_MAP,aChar)==NULL) // not a skip char
			aKey+=aChar;

		aPos++;
	}

	// Validate length
	if (aKey.size() != STRINGKEY_LEN)
	{
		mValid = false;
		return false;
	}

	char aBuf[8];
	memset(aBuf,0,8);
	
	int anOffset = 0;
	for (int i=0; i < STRINGKEY_LEN; i++)
	{
		bool aTst = true;
		switch (STRINGKEY_MAP[i])
		{
			case 'C':
				aTst = ProcessCChar(aBuf, anOffset, aKey[i]);  break;
			case 'V':
				aTst = ProcessVChar(aBuf, anOffset, aKey[i]);  break;
			case 'N':
				aTst = ProcessNChar(aBuf, anOffset, aKey[i]);  break;
		}

		if (! aTst)
		{
			mValid = false;
			return false;
		}
	}

	return CalcKeyFromBuffer(aBuf);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
string CDKey::GetString() const
{
	if(mP14.get()==NULL)
		return "";

	const char *aBuf = mP14->data();
	int anOffset = 0;
	string aKey = "";

	for (int i=0; i < STRINGKEY_LEN;)
	{
		switch (STRINGKEY_MAP[i])
		{
			case 'C':
				aKey += BuildCChar(aBuf, anOffset);  break;
			case 'V':
				aKey += BuildVChar(aBuf, anOffset);  break;
			case 'N':
				aKey += BuildNChar(aBuf, anOffset);  break;
		}

		// Add dash every four chars
		if ((((++i) % DASH_OFFSET) == 0) && (i < STRINGKEY_LEN))
			aKey += '-';
	}

	return aKey;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
ByteBufferPtr CDKey::GetEncrypted() const
{

	if(!IsValid())
		return NULL;

	Blowfish aSymKey;
	GetSymmetricKey(aSymKey, GetUseNewEncryptMethod());
	return aSymKey.Encrypt(mP14->data(),mP14->length());
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
bool CDKey::InitRaw(const void* theRawKey)
{
	mValid = false;
	mP14 = NULL;
	return CalcKeyFromBuffer((const char*)theRawKey);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
bool CDKey::SaveToFile(const std::string &theFileName)
{
	if(!IsValid())
		return false;
	
	FILE *aFile = fopen(theFileName.c_str(),"wb");
	if(aFile==NULL)
		return false;

	ByteBufferPtr anEncrypt = GetEncrypted();	
	if(anEncrypt.get()==NULL)
		return false;

	if(fwrite(anEncrypt->data(),1,anEncrypt->length(),aFile)!=anEncrypt->length())
		return false;

	fclose(aFile);
	return true;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
bool CDKey::LoadFromFile(const std::string &theFileName)
{
	FILE *aFile = fopen(theFileName.c_str(),"rb");
	if(aFile==NULL)
		return false;

	char aBuf[2048];
	int aReadLen = fread(aBuf,1,2048,aFile);
	fclose(aFile);
	
	return InitEncrypted(aBuf,aReadLen);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
bool CDKey::IsBeta() const
{
	if(mP14.get()==NULL)
		return false;
	else
		return (*(mP14->data()) & BETA_MASK);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent method; no Native credit.
bool CDKey::IsForceCheck() const
{
	if(mP14->length()<3)
		return false;
	else
		return ((mP14->data())[2] & FORCE_MASK ? true : false);
}
}

namespace WONAPI {
// Genuine SDK default registry path; source-only data, no Native identity.
const char* CDKey::mCDKeyPath = "SOFTWARE\\Sierra\\CDKeys";

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK registry persistence body.
bool CDKey::SaveToRegistry(long theRoot, const char* thePath) 
{
	if(!IsValid())
		return false;

	RegKey aKey(theRoot, thePath, true, true);
	if(!aKey.IsOpen())
		return false;

	ByteBufferPtr anEncrypt = GetEncrypted();
	if(anEncrypt.get()==NULL)
		return false;

	return aKey.SetValue(mProduct,anEncrypt->data(),anEncrypt->length());
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK registry persistence body.
void CDKey::ClearFromRegistry(long theRoot, const char* thePath)
{
	if(mProduct.empty())
		return;

	RegKey aKey(theRoot, thePath, false, true);
	aKey.DeleteValue(mProduct);
}
}
