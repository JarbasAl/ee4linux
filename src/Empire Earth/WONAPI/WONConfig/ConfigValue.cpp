// Native user-authored ConfigValue template behaviors; source only.
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
#include "ee/wonapi/WONConfigValue.h"
namespace WONAPI {
// Genuine ConfigValue<T>::CopyValueHook instantiations:
// bool3b3f70/56, ushort3b4000/58, string3b41a0/371, wstring3b4480/389, stringlist3b47b0/270.
// FUNCTION: WON 0x007b3f70
// FUNCTION: WON 0x007b4000
// FUNCTION: WON 0x007b41a0
// FUNCTION: WON 0x007b4480
// FUNCTION: WON 0x007b47b0
template<class T> bool ConfigValue<T>::CopyValueHook(ConfigValueBase* source)
{
    ConfigValue<T>* value=dynamic_cast<ConfigValue<T>*>(source);
    if(value!=NULL) {
        mVal=value->mVal;
        return true;
    }
    return false;
}
// Tiny scalar default stores and parser/format forwarding methods are support0.
template<class T> void SimpleConfigValue<T>::InitializeDefaultHook() {mVal=mDefVal;}
template<class T> bool SimpleConfigValue<T>::ReadValue(StringParser& parser) {return parser.ReadValue(mVal);}
template<> bool SimpleConfigValue<unsigned short>::ReadValue(StringParser& parser)
{
    int value;
    bool result=parser.ReadValue(value);
    mVal=(unsigned short)value;
    return result;
}
template<class T> void SimpleConfigValue<T>::WriteValue(std::string& out) {StringParser::ValueToString(mVal,out);}
// Genuine ConfigStringValue<char/wchar_t>::InitializeDefaultHook3b4040/284 and3b4320/276.
// FUNCTION: WON 0x007b4040
// FUNCTION: WON 0x007b4320
template<class T> void ConfigStringValue<T>::InitializeDefaultHook()
{
    if(mDefVal!=NULL) mVal=mDefVal;
    else mVal.erase();
}
template<class T> bool ConfigStringValue<T>::ReadValue(StringParser& parser) {return parser.ReadString(mVal,false);}
template<class T> void ConfigStringValue<T>::WriteValue(std::string& out) {StringParser::ValueToString(mVal,out);}
// Genuine ConfigListValue<list<string> >::InitializeDefaultHook3b4610/154.
// FUNCTION: WON 0x007b4610
template<class T> void ConfigListValue<T>::InitializeDefaultHook()
{
    mVal.clear();
    if(mDefStr!=NULL) {
        StringParser parser(mDefStr);
        parser.mWide=mIsWString;
        parser.ReadValue(mVal);
    }
}
// Genuine ConfigListValue<list<string> >::ReadValue3b46b0/213.
// FUNCTION: WON 0x007b46b0
template<class T> bool ConfigListValue<T>::ReadValue(StringParser& parser)
{
    if(!mAdditive) mVal.clear();
    return parser.ReadValue(mVal);
}
template<class T> void ConfigListValue<T>::WriteValue(std::string& out) {StringParser::ValueToString(mVal,out);}
// FUNCTION: WON 0x007b1b40
void ConfigObject::KeyAdd(const char* key,bool& value,bool initial) {RegisterKeyValue(key,new SimpleConfigValue<bool>(value,initial));}
// FUNCTION: WON 0x007b1ba0
void ConfigObject::KeyAdd(const char* key,unsigned short& value,unsigned short initial) {RegisterKeyValue(key,new SimpleConfigValue<unsigned short>(value,initial));}
// FUNCTION: WON 0x007b1c00
void ConfigObject::KeyAdd(const char* key,std::string& value,const char* initial) {RegisterKeyValue(key,new ConfigStringValue<char>(value,initial));}
// FUNCTION: WON 0x007b1c60
void ConfigObject::KeyAdd(const char* key,std::wstring& value,const wchar_t* initial) {RegisterKeyValue(key,new ConfigStringValue<wchar_t>(value,initial));}
// FUNCTION: WON 0x007b1cc0
void ConfigObject::KeyAdd(const char* key,std::list<std::string>& value,const char* initial,bool additive) {RegisterKeyValue(key,new ConfigListValue<std::list<std::string> >(value,initial,additive));}
}
namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK typed registration paths; no implicit-body or template-emission credit.
void ConfigObject::RegisterInt64Value(const char *theKey, __int64 &theInt64, __int64 theDefVal)
{
	RegisterKeyValue(theKey, new SimpleConfigValue<__int64>(theInt64,theDefVal));
}

void ConfigObject::RegisterIntValue(const char *theKey, int &theInt, int theDefVal)
{
	RegisterKeyValue(theKey, new SimpleConfigValue<int>(theInt,theDefVal));
}

void ConfigObject::RegisterLongValue(const char *theKey, long &theLong, long theDefVal)
{
	RegisterKeyValue(theKey, new SimpleConfigValue<long>(theLong, theDefVal));
}

void ConfigObject::RegisterShortValue(const char *theKey, short &theShort, short theDefVal)
{
	RegisterKeyValue(theKey, new SimpleConfigValue<short>(theShort, theDefVal));
}

void ConfigObject::RegisterByteValue(const char *theKey, char &theByte, char theDefVal)
{
	RegisterKeyValue(theKey, new SimpleConfigValue<char>(theByte,theDefVal));
}

void ConfigObject::RegisterUInt64Value(const char *theKey, unsigned __int64 &theInt64, unsigned __int64 theDefVal)
{
	RegisterKeyValue(theKey, new SimpleConfigValue<unsigned __int64>(theInt64,theDefVal));
}

void ConfigObject::RegisterBoolListValue(const char *theKey, std::list<bool> &theBoolList, const char *theDefVal, bool additive)
{
	RegisterKeyValue(theKey, new ConfigListValue<std::list<bool> >(theBoolList, theDefVal, additive));
}

void ConfigObject::RegisterIntListValue(const char *theKey, std::list<int> &theIntList, const char *theDefVal, bool additive)
{
	RegisterKeyValue(theKey, new ConfigListValue<std::list<int> >(theIntList, theDefVal, additive));
}

void ConfigObject::RegisterWStringListValue(const char *theKey, std::list<std::wstring> &theStringList, const wchar_t *theDefVal, bool additive)
{
	RegisterKeyValue(theKey, new ConfigListValue<std::list<std::wstring> >(theStringList, theDefVal, additive));
}

void ConfigObject::RegisterBoolListListValue(const char *theKey, std::list<std::list<bool> > &theBoolListList, const char *theDefVal, bool additive)
{
	RegisterKeyValue(theKey, new ConfigListValue<std::list<std::list<bool> > >(theBoolListList, theDefVal, additive));
}

void ConfigObject::RegisterIntListListValue(const char *theKey, std::list<std::list<int> > &theIntListList, const char *theDefVal, bool additive)
{
	RegisterKeyValue(theKey, new ConfigListValue<std::list<std::list<int> > >(theIntListList, theDefVal, additive));
}

void ConfigObject::RegisterStringListListValue(const char *theKey, std::list<std::list<std::string> > &theStringListList, const char *theDefVal, bool additive)
{
	RegisterKeyValue(theKey, new ConfigListValue<std::list<std::list<std::string> > >(theStringListList, theDefVal, additive));
}

void ConfigObject::RegisterWStringListListValue(const char *theKey, std::list<std::list<std::wstring> > &theStringListList, const wchar_t *theDefVal, bool additive)
{
	RegisterKeyValue(theKey, new ConfigListValue<std::list<std::list<std::wstring> > >(theStringListList, theDefVal, additive));
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007b4790
// (public: virtual void __thiscall WONAPI::ConfigListValue<class std::list<class std::basic_string<char,struct std::char_traits<char>,class std::allocato)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006aa250
// (public: __thiscall std::list<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,cla)
// TEMPLATE: WON 0x006aa710
// (public: void * __thiscall std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >::`scalar del)
// TEMPLATE: WON 0x006aa760
// (public: void * __thiscall std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >::`scalar deleting destructor'(unsigned int)
