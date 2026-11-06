// Verbatim established ConfigObject54/ConfigValueBase12 declarations moved from GUI.
// No fields, slots, inline bodies or owner semantics altered.
#ifndef EE_WONAPI_WONCONFIGOBJECT_H
#define EE_WONAPI_WONCONFIGOBJECT_H
#include <string>
#include <list>
#include <map>
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONStringCompare.h"
namespace WONAPI {
class ConfigParser;
class StringParser;
class ConfigObject;
typedef ConfigObject* (*ConfigObjectFactory)();
struct ConfigObjectEntry {                  // established complete0c; genuine SDK GroupType constructors below
    ConfigObjectEntry(ConfigObjectFactory=NULL,SmartPtr<ConfigObject>* =NULL);
    ConfigObjectEntry(ConfigObject*);
    ConfigObjectFactory mL00;
    SmartPtr<ConfigObject>* mL04;
    SmartPtr<ConfigObject> mObject;             // +0x08 (released by the node dtor 0x296a8f)
};
// Full12-byte abstract base, nativeInitialize/Read/Write/default flag accessed byConfigObject.
class ConfigValueBase : public RefCount {
public:
    virtual void InitializeDefaultHook()=0; //slot04
    virtual bool CopyValueHook(ConfigValueBase*)=0; //slot08
    virtual bool ReadValue(StringParser&)=0; //slot0c
    virtual void WriteValue(std::string&)=0; //slot10
    void CopyValue(ConfigValueBase* source) { if(CopyValueHook(source)) mIsDefault=source->mIsDefault; } //genuine SDK inline; preserves defaultflag only after successful typedcopy
    bool mIsDefault; //08; native3aed47/3b1149/3af2de, natural09..0b padding
};
typedef SmartPtr<ConfigValueBase> ConfigValueBasePtr;

// a configuration object (0x54 bytes: LobbyConfig's members start at +0x54). Layout from its implicit dtor 0x294e83
//; member names PROVISIONAL.
class ConfigObject : public RefCount {
public:
    typedef std::list<SmartPtr<ConfigObject> > ChildList;
    typedef std::map<std::string,ConfigValueBasePtr,StringLessNoCase> KeyValMap;
    typedef std::map<std::string,ConfigObjectEntry,StringLessNoCase> KeyGroupMap;
    ConfigObject();                        // 0x3aea90
    // Original ConfigObject vtable has these eight slots after its implicit virtual destructor.
    virtual bool Virtual04(ConfigParser* inParser, const std::string& inInstruction);
    virtual bool Virtual08(ConfigParser* inParser, const std::string& inKey, StringParser& inValue);
    virtual bool Virtual0C(ConfigParser* inParser);
    virtual bool Virtual10(ConfigParser* inParser);
    virtual bool Virtual14(const std::string& inKey, std::string& outValue);
    virtual void Virtual18(bool inFlag);
    virtual void Virtual1C(std::string& outValues);
    virtual int Virtual20(const std::string& inKey, StringParser& inValue);
    void KeyAdd(const char* inKey, unsigned short& ioValue, unsigned short inDefault);   // 0x3b1ba0
    void KeyAdd(const char* inKey, bool& ioValue, bool inDefault); // 0x3b1b40
    void KeyAdd(const char* inKey, std::string& ioValue, const char* inDefault); // 0x3b1c00
    void KeyAdd(const char* inKey, std::wstring& ioValue, const wchar_t* inDefault); // 0x3b1c60
    void KeyAdd(const char* inKey, std::list<std::string>& ioValue, const char* inDefault, bool inFlag); // 0x3b1cc0

    void CopyKeyValsFrom(ConfigObject*); //genuine SDK ordered case-insensitive merge/copy, SOURCE_ONLY_NO_NATIVE_ID
    ConfigObject* NextChild();
    bool HasMoreChildren();
    void RegisterKeyGroup(const char*,ConfigObjectFactory,SmartPtr<ConfigObject>* =NULL);
    void RegisterKeyGroup(const char*,ConfigObject*);
    void Clear(); //3af310
    void Rewind(); //3af370
    void AddChild(ConfigObject*); //3af390
    bool ReadGroupStart(ConfigParser&,std::string&,std::string&); //3af4c0
    bool ParseTopLevel(ConfigParser&,const char*); //3af7a0
    bool ParseGroup(ConfigParser&,bool=true,bool=false,bool=true,bool=true); //3afa60
    bool SkipGroup(ConfigParser&,bool=true,bool=true); //3b0000
    bool IncludeFile(ConfigParser&,const std::string&,bool=true); //3b00b0
    bool IncludeGroup(ConfigParser&,const std::string&,bool=true); //3b03c0
    bool ParseChild(ConfigParser&,ConfigObject*,bool=true,bool=true); //3b06f0
    bool HandleChildGroup(ConfigParser&,KeyGroupMap::iterator&); //3b07a0
    bool HandleKeyVal(ConfigParser&,StringParser&,KeyValMap::iterator&); //3b1120
    bool QueryValue(KeyValMap::const_iterator,std::string&); //3aedf0
    void RegisterKeyValue(const char*,ConfigValueBase*); //3b18e0
    void RegisterInt64Value(const char *theKey, __int64 &theInt64, __int64 theDefVal = 0);
    void RegisterIntValue(const char *theKey, int &theInt, int theDefVal = 0);
    void RegisterLongValue(const char *theKey, long &theLong, long theDefVal = 0);
    void RegisterShortValue(const char *theKey, short &theShort, short theDefVal = 0);
    void RegisterByteValue(const char *theKey, char &theByte, char theDefVal = 0);
    void RegisterUInt64Value(const char *theKey, unsigned __int64 &theInt64, unsigned __int64 theDefVal = 0);
    void RegisterBoolListValue(const char *theKey, std::list<bool> &theBoolList, const char *theDefVal = NULL, bool additive = false);
    void RegisterIntListValue(const char *theKey, std::list<int> &theIntList, const char *theDefVal = NULL, bool additive = false);
    void RegisterWStringListValue(const char *theKey, std::list<std::wstring> &theStringList, const wchar_t *theDefVal = NULL, bool additive = false);
    void RegisterBoolListListValue(const char *theKey, std::list<std::list<bool> > &theBoolListList, const char *theDefVal = NULL, bool additive = false);
    void RegisterIntListListValue(const char *theKey, std::list<std::list<int> > &theIntListList, const char *theDefVal = NULL, bool additive = false);
    void RegisterStringListListValue(const char *theKey, std::list<std::list<std::string> > &theStringListList, const char *theDefVal = NULL, bool additive = false);
    void RegisterWStringListListValue(const char *theKey, std::list<std::list<std::wstring> > &theStringListList, const wchar_t *theDefVal = NULL, bool additive = false);

    ChildList mL08;    // +0x08 (dtor ICF 0xd17ea, list<SmartPtr<..>>)
    ChildList::iterator mL14;                              // +0x14
    std::string mS18;                       // +0x18
    std::string mS28;                       // +0x28
    ConfigObject* mL38;                              // +0x38
    KeyValMap mKeyMap;     // +0x3c (KeyAdd 0x3b18e0 inserts at +0x3c; node value 0x14)
    KeyGroupMap mM48;         // +0x48 (node value 0x1c: SmartPtr at value +0x18)
};

inline ConfigObjectEntry::ConfigObjectEntry(ConfigObjectFactory factory,SmartPtr<ConfigObject>* group)
    : mL00(factory),mL04(group) {}
inline ConfigObjectEntry::ConfigObjectEntry(ConfigObject* group)
    : mL00(NULL),mL04(NULL),mObject(group) {}
}
#endif
