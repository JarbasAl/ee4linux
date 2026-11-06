// Native configuration object and parsing family; source only.
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
namespace WONAPI {
class ConfigDefineScope {
    ConfigParser& mParser;
public:
    ConfigDefineScope(ConfigParser& parser):mParser(parser) {mParser.PushDefineStack();}
    ~ConfigDefineScope() {mParser.PopDefineStack();}
};
// FUNCTION: WON 0x007aea90
ConfigObject::ConfigObject() { Rewind(); mL38=NULL; }
// FUNCTION: WON 0x007aec60
void ConfigObject::Virtual18(bool resetName)
{
    Clear();
    if(resetName) {mS18.erase();mS28.erase();}
    KeyValMap::iterator i=mKeyMap.begin();
    while(i!=mKeyMap.end()) {
        ConfigValueBase* value=i->second.get();
        value->InitializeDefaultHook();
        value->mIsDefault=true;
        ++i;
    }
    KeyGroupMap::iterator j=mM48.begin();
    while(j!=mM48.end()) {
        if(j->second.mL04!=NULL) *j->second.mL04=NULL;
        ++j;
    }
}
// FUNCTION: WON 0x007aedf0
bool ConfigObject::QueryValue(KeyValMap::const_iterator itr,std::string& outValue)
{
    ConfigValueBase* value=itr->second.get();
    value->WriteValue(outValue);
    if(value->mIsDefault) outValue+="  [DEF]";
    return true;
}
// FUNCTION: WON 0x007aefd0
bool ConfigObject::Virtual14(const std::string& key,std::string& outValue)
{
    KeyValMap::const_iterator itr=mKeyMap.find(key);
    if(itr!=mKeyMap.end())
        return QueryValue(itr,outValue);
    return false;
}
// FUNCTION: WON 0x007af040
void ConfigObject::Virtual1C(std::string& outValues)
{
    KeyValMap::const_iterator itr=mKeyMap.begin();
    while(itr!=mKeyMap.end()) {
        outValues+=itr->first;
        outValues+=": ";
        QueryValue(itr,outValues);
        outValues+="\n";
        ++itr;
    }
}
// FUNCTION: WON 0x007af280
int ConfigObject::Virtual20(const std::string& key,StringParser& parser)
{
    KeyValMap::iterator itr=mKeyMap.find(key);
    if(itr!=mKeyMap.end()) {
        ConfigValueBase* value=itr->second.get();
        if(value->ReadValue(parser)) {
            value->mIsDefault=false;
            return 0;
        }
        return 2;
    }
    return 1;
}
// FUNCTION: WON 0x007af310
void ConfigObject::Clear() {mL08.clear();Rewind();}
// Original3af370/17 is pure iterator initialization, support0.
// FUNCTION: WON 0x007af370
void ConfigObject::Rewind() {mL14=mL08.begin();}
// FUNCTION: WON 0x007af390
void ConfigObject::AddChild(ConfigObject* child)
{
    child->mL38=this;
    mL08.push_back(child);
}
// FUNCTION: WON 0x007af4c0
bool ConfigObject::ReadGroupStart(ConfigParser& parser,std::string& name,std::string& inherited)
{
    std::string token;
    if(!parser.ReadToken(token,false,false)) return false;
    if(token!="{") {
        StringParser stringParser(token.c_str());
        if(token!=":") {
            stringParser.ReadString(name,false);
            if(!parser.ReadToken(token,false,false)) return false;
        }
        if(token==":") {
            std::string base;
            if(!parser.ReadToken(base,false,false)) return false;
            stringParser.Set(base.c_str());
            stringParser.ReadString(inherited,false);
            if(!parser.ReadToken(token,false,false)) return false;
        }
    }
    if(token!="{") {parser.SetError("Expecting '{' but got "+token,false);return false;}
    return true;
}
// Default parser hooks3099f0/5 share a real always-true native function, zeroquota.
bool ConfigObject::Virtual0C(ConfigParser*) {return true;}
bool ConfigObject::Virtual10(ConfigParser*) {return true;}
// FUNCTION: WON 0x007af7a0
bool ConfigObject::ParseTopLevel(ConfigParser& parser,const char* filePath)
{
    if(filePath!=NULL && !parser.OpenFile(std::string(filePath))) {
        std::string error=std::string("File not found: ")+filePath;
        parser.SetError(error,false);
        return false;
    }
    ConfigDefineScope scope(parser);
    Virtual18(true);
    return ParseGroup(parser,false,false,true,true);
}
// FUNCTION: WON 0x007afa60
bool ConfigObject::ParseGroup(ConfigParser& parser,bool checkBraces,bool skip,bool prePost,bool readStart)
{
    try {
        if(checkBraces && readStart && !ReadGroupStart(parser,mS18,mS28)) return false;
        if(prePost && !skip && !Virtual0C(&parser)) return false;
        while(parser.mAbortReason.empty() && !parser.mStopped) {
            std::string token;
            if(!parser.ReadToken(token,false,false)) {
                if(checkBraces) {parser.SetError("Expecting '}'",false);return false;}
                break;
            }
            StringToUpperCaseInPlace(token);
            if(token=="}") {
                if(!checkBraces) {parser.SetError("Unexpected '}'",false);return false;}
                break;
            }
            if(token[token.length()-1]==':') {
                token.erase(token.length()-1);
                std::string value;
                std::wstring wideValue;
                StringParser valueParser("");
                if(parser.mIsUnicode) {
                    if(!parser.ReadToken(wideValue,true,skip)) return false;
                    valueParser.mText.mWide=wideValue.c_str();
                    valueParser.mPos=0;
                    valueParser.mWide=true;
                } else {
                    if(!parser.ReadToken(value,true,skip)) return false;
                    valueParser.mText.mNarrow=value.c_str();
                    valueParser.mPos=0;
                    valueParser.mWide=false;
                }
                if(!skip && !Virtual08(&parser,token,valueParser))
                    parser.AddWarning("Unexpected key/val: "+token+" "+value);
            } else {
                if(!skip) {
                    if(!Virtual04(&parser,token)) {
                        parser.AddWarning("Unexpected child group: "+token);
                        SkipGroup(parser,true,true);
                    }
                } else SkipGroup(parser,true,true);
            }
        }
        if(prePost && !skip && !Virtual10(&parser)) return false;
    } catch(const ConfigObjectException& exception) {
        if(exception.mFlag) parser.SetError(exception.mText,false);
        else parser.AddWarning(exception.mText);
    }
    return parser.mAbortReason.empty();
}
// FUNCTION: WON 0x007b0000
bool ConfigObject::SkipGroup(ConfigParser& parser,bool checkBraces,bool readStart)
{
    SmartPtr<ConfigObject> child=new ConfigObject;
    return child->ParseGroup(parser,checkBraces,true,false,readStart);
}
// FUNCTION: WON 0x007b06f0
bool ConfigObject::ParseChild(ConfigParser& parser,ConfigObject* child,bool addChild,bool readStart)
{
    ConfigDefineScope scope(parser);
    child->mL38=this;
    child->Virtual18(readStart);
    if(!child->ParseGroup(parser,true,false,true,readStart)) return false;
    if(addChild) AddChild(child);
    return true;
}
// FUNCTION: WON 0x007b07a0
bool ConfigObject::HandleChildGroup(ConfigParser& parser,KeyGroupMap::iterator& itr)
{
    ConfigDefineScope scope(parser);
    ConfigObjectEntry& group=itr->second;
    SmartPtr<ConfigObject> child;
    if(group.mL00!=NULL) child=group.mL00();
    else child=group.mObject.get();
    child->Virtual18(true);
    child->mL38=this;
    if(!child->ParseGroup(parser,true,false,true,true)) return false;
    if(group.mL04!=NULL) *group.mL04=child;
    else if(group.mObject.get()==NULL) AddChild(child.get());
    return true;
}
// FUNCTION: WON 0x007b1120
bool ConfigObject::HandleKeyVal(ConfigParser& parser,StringParser& value,KeyValMap::iterator& itr)
{
    ConfigValueBase* config=itr->second.get();
    config->mIsDefault=false;
    if(!config->ReadValue(value)) parser.AddWarning("Failed to parse value for key: "+itr->first);
    return true;
}
// FUNCTION: WON 0x007b18e0
void ConfigObject::RegisterKeyValue(const char* key,ConfigValueBase* value)
{
    mKeyMap.insert(KeyValMap::value_type(std::string(key),value));
}
// FUNCTION: WON 0x007b00b0
bool ConfigObject::IncludeFile(ConfigParser& parser,const std::string& path,bool required)
{
    std::string file=path;
    std::string::size_type slash=file.find_last_of("/");
    if(slash==std::string::npos) slash=file.find_last_of('\\');
    if(parser.mIgnorePathOnIncludeFile && slash!=std::string::npos)
        file=file.substr(slash+1,file.length()-slash-1);
    file=parser.mFilePath+file;
    ConfigParser included;
    included.SetParent(&parser);
    if(included.OpenFile(file)) {
        bool result=ParseGroup(included,false,false,false,true);
        parser.CopyWarnings(included);
        if(!result) {parser.SetError(included.GetAbortReason(),false);return false;}
        if(included.mStopped) parser.mStopped=true;
    } else if(required) {parser.SetError("File not found: "+file,false);return false;}
    return true;
}
// FUNCTION: WON 0x007b03c0
bool ConfigObject::IncludeGroup(ConfigParser& parser,const std::string& name,bool required)
{
    ConfigParserState defined;
    if(!parser.GetGroupDefine(name,defined)) {
        if(required) parser.AddWarning("IncludeGroup not found: "+name);
        return true;
    }
    ConfigParserState saved;
    parser.GetState(saved);
    parser.SetState(defined);
    bool result=ParseGroup(parser,true,false,false,false);
    parser.SetState(saved);
    if(!parser.mAbortReason.empty()) parser.SetError("IncludeGroup Error",true);
    return result;
}

// FUNCTION: WON 0x007b0910
bool ConfigObject::Virtual04(ConfigParser* parser,const std::string& instruction)
{
    if(instruction=="GROUPDEFINE") {
        std::string name,inherited;
        if(!ReadGroupStart(*parser,name,inherited)) return false;
        parser->AddGroupDefine(name,NULL);
        return SkipGroup(*parser,true,false);
    }
    if(instruction=="IFDEFINED") {
        std::string name,expected;
        if(!ReadGroupStart(*parser,name,expected)) return true;
        std::string actual;
        bool selected=false;
        if(parser->GetDefine(name,actual) && (expected.empty() || StringCompareNoCase(expected,actual)==0)) {
            selected=true;
            ParseGroup(*parser,true,false,false,false);
        } else SkipGroup(*parser,true,false);
        std::string token;
        parser->ReadToken(token,false,false);
        if(_stricmp(token.c_str(),"else")!=0) {
            parser->UngetStr(token);
            return true;
        }
        parser->ReadToken(token,false,false);
        if(token!="{") {
            parser->SetError("Expecting '{'",false);
            return true;
        }
        if(selected) SkipGroup(*parser,true,false);
        else ParseGroup(*parser,true,false,false,false);
        return true;
    }
    if(instruction=="IFNOTDEFINED") {
        std::string name,expected;
        if(!ReadGroupStart(*parser,name,expected)) return true;
        std::string actual;
        bool selected=false;
        if(parser->GetDefine(name,actual) && (expected.empty() || StringCompareNoCase(expected,actual)==0)) {
            selected=true;
            SkipGroup(*parser,true,false);
        } else ParseGroup(*parser,true,false,false,false);
        std::string token;
        parser->ReadToken(token,false,false);
        if(_stricmp(token.c_str(),"else")!=0) {
            parser->UngetStr(token);
            return true;
        }
        parser->ReadToken(token,false,false);
        if(token!="{") {
            parser->SetError("Expecting '{'",false);
            return true;
        }
        if(selected) ParseGroup(*parser,true,false,false,false);
        else SkipGroup(*parser,true,false);
        return true;
    }
    KeyGroupMap::iterator itr=mM48.find(instruction);
    if(itr==mM48.end()) return false;
    return HandleChildGroup(*parser,itr);
}
// FUNCTION: WON 0x007b11d0
bool ConfigObject::Virtual08(ConfigParser* parser,const std::string& instruction,StringParser& value)
{
    if(instruction=="DEFINE" || instruction=="REDEFINE") {
        std::string name;
        if(!value.ReadString(name,true)) {parser->AddWarning("Invalid define.");return true;}
        value.SkipWhitespace();
        if(value.mWide) {
            std::wstring rest=value.mText.mWide+value.mPos;
            parser->AddDefine(name,WStringToString(rest),instruction=="REDEFINE");
        } else {
            std::string rest=value.mText.mNarrow+value.mPos;
            parser->AddDefine(name,rest,instruction=="REDEFINE");
        }
        return true;
    }
    if(instruction=="INCLUDEGROUP") {
        std::string name;
        value.ReadString(name,false);
        bool required=true;
        if(value.CheckNextChar(',')) {
            std::string optional;
            if(!value.ReadString(optional,false))
                parser->AddWarning("Expecting string: 'optional' after include group, include is required.");
            else if(StringCompareNoCase(optional,std::string("optional"))!=0)
                parser->AddWarning("Expecting string: optional after include group, include is required.");
            else required=false;
        }
        return IncludeGroup(*parser,name,required);
    }
    if(instruction=="INCLUDEFILE") {
        std::string path;
        value.ReadString(path,false);
        bool required=true;
        if(value.CheckNextChar(',')) {
            std::string optional;
            if(!value.ReadString(optional,false))
                parser->AddWarning("Expecting string: 'optional' after include file, include is required.");
            else if(StringCompareNoCase(optional,std::string("optional"))!=0)
                parser->AddWarning("Expecting string: optional after include file, include is required.");
            else required=false;
        }
        return IncludeFile(*parser,path,required);
    }
    KeyValMap::iterator itr=mKeyMap.find(instruction);
    if(itr==mKeyMap.end()) return false;
    return HandleKeyVal(*parser,value,itr);
}

}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: same-snapshot SDK configuration merge/copy and group APIs.
void ConfigObject::CopyKeyValsFrom(ConfigObject* theCopyFrom)
{
    KeyValMap& aMap1 = mKeyMap;
    KeyValMap& aMap2 = theCopyFrom->mKeyMap;
    KeyValMap::iterator anItr1 = aMap1.begin();
    KeyValMap::iterator anItr2 = aMap2.begin();
    while(anItr1!=aMap1.end() && anItr2!=aMap2.end())
    {
        int aComp = StringCompareNoCase(anItr1->first,anItr2->first);
        if(aComp<0)
            ++anItr1;
        else if(aComp>0)
            ++anItr2;
        else
        {
            ConfigValueBase* aVal1 = anItr1->second.get();
            ConfigValueBase* aVal2 = anItr2->second.get();
            aVal1->CopyValue(aVal2);
            ++anItr1;
            ++anItr2;
        }
    }
}
ConfigObject* ConfigObject::NextChild()
{
    if(mL14==mL08.end())
        return NULL;
    return (*mL14++).get();
}
bool ConfigObject::HasMoreChildren()
{
    return mL14!=mL08.end();
}
void ConfigObject::RegisterKeyGroup(const char* theKey, ConfigObjectFactory theFactory, SmartPtr<ConfigObject>* theGroup)
{
    if(mM48.insert(KeyGroupMap::value_type(theKey,ConfigObjectEntry(theFactory,theGroup))).second==false)
    {
        // Insert failed: preserve the existing registration, as the original SDK does.
    }
}
void ConfigObject::RegisterKeyGroup(const char* theKey, ConfigObject* theGroup)
{
    if(mM48.insert(KeyGroupMap::value_type(theKey,ConfigObjectEntry(theGroup))).second==false)
    {
        // Insert failed: preserve the existing registration, as the original SDK does.
    }
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007aeb80
// (public: virtual void * __thiscall WONAPI::ConfigObject::`scalar deleting destructor'(unsigned int)    [??_GConfigObject@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007afa50
// (public: __thiscall WONAPI::ConfigDefineScope::~ConfigDefineScope(void)    [??1ConfigDefineScope@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007b0660
// (public: __thiscall WONAPI::ConfigParserState::~ConfigParserState(void)    [??1ConfigParserState@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007aeba0
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct WONAPI::ConfigObjectEntry,s)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007b5ca0
// (protected: struct std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic)
// TEMPLATE: WON 0x007b4f00
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_)
// TEMPLATE: WON 0x007b6b00
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WON)
