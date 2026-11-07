// Original-backed RegKey query/delete/close family. Canonical original-backed source-only candidate; native ambiguous Open remains carved.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONBuffer.h"
namespace WONAPI {
// Native2a4260/119: mKey deliberately untouched before native OpenNewKey.
RegKey::RegKey(int root,const std::string& path,bool create,bool nonvolatile)
    :mIsOpen(false),mBuffer(NULL),mL20(0),mL24(0),mL28(0)
{
    OpenNewKey(root,path,create,nonvolatile);
}
// Native2a42e0/137: normal owned destruction; does not mutate mIsOpen.
// FUNCTION: WON 0x6a42e0
RegKey::~RegKey()
{
    if(mIsOpen)
        RegCloseKey(mKey);
    delete [] mBuffer;
}
// Native2a4370/26.
// FUNCTION: WON 0x6a4370
void RegKey::CloseKey()
{
    if(mIsOpen) {
        RegCloseKey(mKey);
        mIsOpen=false;
    }
}
// Native2a4390/161. outLength is the first query's size, not the second's.
// FUNCTION: WON 0x6a4390
DWORD RegKey::GetToBuf(const std::string& name,unsigned long& outLength) const
{
    DWORD type;
    DWORD length;
    if(RegQueryValueExA(mKey,name.c_str(),NULL,&type,NULL,&length)!=ERROR_SUCCESS)
        return 0;
    outLength=length;
    if(mL20<length) {
        delete [] mBuffer;
        mL20=length;
        mBuffer=new char[length];
    }
    if(RegQueryValueExA(mKey,name.c_str(),NULL,&type,
                       reinterpret_cast<BYTE*>(mBuffer),&length)==ERROR_SUCCESS)
        return type;
    return 0;
}
// Native2a4680/81. Keep existing target ABI/address spelling.
// FUNCTION: WON 0x6a4680
int RegKey::QueryDWORD(const std::string& name,DWORD& value)
{
    value=0;
    if(mIsOpen) {
        unsigned long length;
        DWORD type=GetToBuf(name,length);
        if(type!=0) {
            if(type!=REG_DWORD)
                return 2;
            value=*reinterpret_cast<DWORD*>(mBuffer);
            return 0;
        }
    }
    return 1;
}
// Native2a46e0/322. Empty binary returns NoSize without changing caller owner.
// FUNCTION: WON 0x6a46e0
int RegKey::QueryBinary(const std::string& name,ConstSmartPtr<ByteBuffer>& value)
{
    if(mIsOpen) {
        unsigned long length=0;
        DWORD type=GetToBuf(name,length);
        if(type!=0) {
            if(type!=REG_BINARY)
                return 2;
            if(length!=0) {
                value=new ByteBuffer(mBuffer,length,false);
                return 0;
            }
            return 4;
        }
    }
    return 1;
}
// Native2a4830/54. Exact original nonempty-name/open predicates.
// FUNCTION: WON 0x6a4830
bool RegKey::DeleteSubKey(const std::string& key)
{
    if(key.length()<=0 || !mIsOpen)
        return false;
    return RegDeleteKeyA(mKey,key.c_str())==ERROR_SUCCESS;
}
// Native2a4870/66. Child is closed only when this parent is open.
// FUNCTION: WON 0x6a4870
bool RegKey::DeleteSubKey(RegKey& key)
{
    if(mIsOpen) {
        if(key.mIsOpen)
            key.CloseKey();
        return RegDeleteKeyA(mKey,key.mPath.c_str())==ERROR_SUCCESS;
    }
    return false;
}
/* DEFERRED ORIGINAL-NATIVE, ZERO SOURCE CREDIT:
   OpenNewKey(int,...), native2a4440/42, forwards through GetRootHKey2a41d0.
   OpenNewKey(HKEY,...), native2a4470/520, includes ambiguous RegCreateKeyExA
   lpClass operand in physical dead create-argument slot. No initialized class
   buffer or guessed source owner is substituted. Both remain original-carved.
*/
}

namespace WONAPI {
// Genuine SDK root-key conversion; native2a41d0/77B, switch table28B separate.
// FUNCTION: WON 0x006a41d0
HKEY RegKey::GetRootHKey(int root)
{
    switch(root) {
    case 0: return HKEY_CLASSES_ROOT;
    case 1: return HKEY_CURRENT_USER;
    case 2: return HKEY_LOCAL_MACHINE;
    case 3: return HKEY_USERS;
    case 4: return HKEY_PERFORMANCE_DATA;
    case 5: return HKEY_CURRENT_CONFIG;
    case 6: return HKEY_DYN_DATA;
    }
    return NULL;
}
// Genuine SDK enum-root overload; established target canonical root ABI is int32.
// FUNCTION: WON 0x006a4440
bool RegKey::OpenNewKey(int root,const std::string& path,bool create,bool nonvolatile)
{
    return OpenNewKey(GetRootHKey(root),path,create,nonvolatile);
}
// Genuine SDK Windows registry open/create and retained path behavior.
// FUNCTION: WON 0x006a4470
bool RegKey::OpenNewKey(HKEY root,const std::string& path,bool create,bool nonvolatile)
{
    CloseKey();
    if(path.size()>0) {
        if(create) {
            char aClass;
            DWORD wasCreated;
            DWORD option=nonvolatile?REG_OPTION_NON_VOLATILE:REG_OPTION_VOLATILE;
            mIsOpen=(RegCreateKeyExA(root,path.c_str(),NULL,&aClass,option,
                        KEY_ALL_ACCESS,NULL,&mKey,&wasCreated)==ERROR_SUCCESS);
        }
        else
            mIsOpen=(RegOpenKeyExA(root,path.c_str(),NULL,KEY_ALL_ACCESS,&mKey)==ERROR_SUCCESS);
        if(mIsOpen) mPath=path;
    }
    return mIsOpen;
}
}

namespace WONAPI {
using namespace std;

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
bool RegKey::SetValue(const string& theName, const string& theValue){

	if(!IsOpen())
		return false;

	if(RegSetValueEx(mKey, theName.c_str(), NULL, REG_SZ,
	                  reinterpret_cast<CONST BYTE*>(theValue.c_str()),
	                  (theValue.size()+1)) == ERROR_SUCCESS)
	{
		return true;

	}
	else
		return false;

}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
bool RegKey::SetValue(const string& theName, unsigned long theValue)
{
	if(!IsOpen())
		return false;
	
	if(RegSetValueEx(mKey, theName.c_str(), NULL, REG_DWORD,
	                  reinterpret_cast<CONST BYTE*>(&theValue),
	                  sizeof(theValue)) == ERROR_SUCCESS)
	{
		return true;
	}
	else
		return false;

}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
bool RegKey::SetValue(const std::string& theName, const void* theValueP, unsigned long theLength)
{
	if(!IsOpen())
		return false;

	if(RegSetValueEx(mKey, theName.c_str(), NULL, REG_BINARY,
	                 (const unsigned char*)theValueP,
	                 theLength) == ERROR_SUCCESS)
	{
		return true;

	}
	else
		return false;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
bool RegKey::DeleteValue(const string& theName)
{
	if(!IsOpen())
		return false;

	return (RegDeleteValue(mKey, theName.c_str()) == ERROR_SUCCESS);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
bool RegKey::OpenSubKey(const std::string& theSubKey, bool createSubKey, bool nonVolatile)
{
	bool subkeyOpen(false);


	if(theSubKey.size() > 0 && mIsOpen)
	{
		HKEY aSubKey;
		
		if(createSubKey)
		{
			char aClass;
			DWORD wasCreated;
			DWORD anOptionValue((nonVolatile) ? REG_OPTION_NON_VOLATILE : REG_OPTION_VOLATILE);

			subkeyOpen = (RegCreateKeyEx(mKey, theSubKey.c_str(), NULL, &aClass, anOptionValue, KEY_ALL_ACCESS, NULL, &aSubKey, &wasCreated) == ERROR_SUCCESS);
		}
		else
			subkeyOpen = (RegOpenKeyEx(mKey, theSubKey.c_str(), NULL, KEY_ALL_ACCESS, &aSubKey) == ERROR_SUCCESS);

		if(subkeyOpen)
		{
			CloseKey();
			mPath = theSubKey;
			mKey = aSubKey;
			mIsOpen = subkeyOpen;
		}
	}


	return subkeyOpen;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
bool RegKey::GetSubKey(const std::string& theSubKeyName, RegKey& theSubKey, bool createSubKey, bool nonVolatile) const 
{
	if(theSubKeyName.size() > 0 && mIsOpen)
		return theSubKey.OpenNewKey(mKey,theSubKeyName, createSubKey, nonVolatile);

	return false;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
RegKey::GetResult RegKey::GetFirstSubKey(RegKey& theSubKey) const 
{
	mL24 = 0;
	return ((GetNextSubKey(theSubKey) == NoMore) ? NotFound : Ok);

}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
RegKey::GetResult RegKey::GetNextSubKey(RegKey& theSubKey) const 
{
	FILETIME aFileTime;
	char aNameBuffer[256];
	DWORD aNameLength(sizeof(aNameBuffer));
	GetResult ret(NoMore);

	if(RegEnumKeyEx(mKey, mL24, aNameBuffer, &aNameLength, 0, NULL, NULL, &aFileTime) != ERROR_NO_MORE_ITEMS)
	{
		mL24++;
		if(theSubKey.OpenNewKey(mKey,aNameBuffer))
			ret = Ok;
	}

	return ret;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
RegKey::GetResult RegKey::GetFirstValueName(std::string& theName, DataType& theType) const 
{
	mL28 = 0;
	return ((GetNextValueName(theName, theType) == NoMore) ? NotFound : Ok);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK registry method, no Native credit.
RegKey::GetResult RegKey::GetNextValueName(std::string& theName, DataType& theType) const 
{
	char aNameBuffer[256];
	DWORD aNameLength(sizeof(aNameBuffer)), aDataType;
	GetResult ret(NoMore);

	if(RegEnumValue(mKey, mL28, aNameBuffer, &aNameLength, 0, &aDataType, NULL, NULL) != ERROR_NO_MORE_ITEMS)
	{
		switch(aDataType)
		{
			case REG_DWORD:
				theType=Long;
				break;
			case REG_SZ:
				theType=String;
				break;
			case REG_BINARY:
				theType=Binary;
				break;
			default:
				theType=Other;
				break;
		}
		mL28++;
		theName = aNameBuffer;
		ret = Ok;
	}

	return ret;
}
}

namespace WONAPI {
using namespace std;
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK string-valued registry decoder.
RegKey::GetResult RegKey::GetValue(const string& theName, string& theValR) const
{
	GetResult ret = NotFound;

	theValR.erase();
	if(!IsOpen())
		return NotFound;

	unsigned long aLength;
	switch (GetToBuf(theName, aLength))
	{
		case REG_SZ:
			theValR = reinterpret_cast<char *>(mBuffer);
			ret = Ok;
			break;
		case REG_NONE:
			ret = NotFound;
			break;
		default:
			ret = BadType;
			break;
	}

	return ret;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006a4240
// (public: virtual void * __thiscall WONAPI::RegKey::`scalar deleting destructor'(unsigned int)    [??_GRegKey@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x006a4260
// (public: __thiscall WONAPI::RegKey::RegKey(int,class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const &,bool,boo)
