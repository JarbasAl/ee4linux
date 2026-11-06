// Native component configuration lookup, mapping and instruction dispatch.
// Complete six-method source cohort; external genuine APIs remain Root D header prerequisites.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/WONPerfTimer.h"
namespace WONAPI {
// FUNCTION: WON 0x00786150
bool ComponentConfig::Virtual04(ConfigParser* parser,const std::string& type)
{
    if (ConfigObject::Virtual04(parser,type)) return true;
    if (type=="MODIFY") return HandleModifyGroup(*parser);
    if (type=="RESOURCE") return HandleResourceGroup(*parser);
    SmartPtr<ComponentConfig> config;
    SmartPtr<Component> component;
    if (!mResourceConfigTable->AllocateConfig(type,config,component)) return false;
    config->Virtual34(component.get());
    bool success=ParseChild(*parser,config.get(),false,true);
    if (success && !config->mS18.empty()) {
        config->mL38=this;
        mConfigMap[config->mS18]=config;
    }
    return success;
}
// FUNCTION: WON 0x00786380
ComponentConfig* ComponentConfig::GetComponentConfig(const GUIString& path,bool fail)
{
    int dot=path.find('.',0);
    GUIString name,rest;
    if (dot>0) {
        name=path.substr(0,(unsigned short)dot);
        rest=path.substr((unsigned short)(dot+1));
    } else name=path;
    ComponentConfig* config=NULL;
    if (name.CompareNoCase("this")==0) config=this;
    else if (name.CompareNoCase("parent")==0) config=static_cast<ComponentConfig*>(mL38);
    else {
        std::map<std::string,SmartPtr<ComponentConfig>,StringLessNoCase>::iterator i=mConfigMap.find(name);
        if (i!=mConfigMap.end()) config=i->second.get();
        else if (name[0]=='_') {
            std::string predefined=name.substr(1);
            StringToUpperCaseInPlace(predefined);
            Component* component=NULL;
            if (Virtual28(predefined,config,component) && config!=NULL && component!=NULL) {
                mConfigMap[(std::string)name]=config;
                config->Virtual34(component);
                config->mL38=this;
            }
        }
    }
    if (config==NULL) {
        if (fail) throw ConfigObjectException(std::string("Component not found: ")+(std::string)name);
        return NULL;
    }
    if (rest.length()==0) return config;
    return config->GetComponentConfig(rest,true);
}
// FUNCTION: WON 0x007868d0
ComponentConfig* ComponentConfig::ReadComponentConfig()
{
    GUIString name=Fn3842F0(false);
    return GetComponentConfig(name,true);
}
// FUNCTION: WON 0x00786960
bool ComponentConfig::HandleMapComponent()
{
    ComponentConfig* config=ReadComponentConfig();
    if (!EndOfString()) {
        EnsureComma();
        GUIString name=Fn3842F0(false);
        int dot=name.rFind('.',-1);
        if (dot>=0) {
            GUIString component=name.substr(0,(unsigned short)dot);
            name=name.substr((unsigned short)(dot+1));
            ComponentConfig* target=GetComponentConfig(component,true);
            if (name.length()==0) name=GUIString(config->mS18);
            target->mConfigMap[(std::string)name]=config;
        }
        mConfigMap[(std::string)name]=config;
    } else config->mConfigMap[mS18]=this;
    return true;
}
// SUPPORT-ONLY: EXE 0x00786c30 (thin named map insertion helper, zeroquota)
void ComponentConfig::AddChildToConfigMap(const GUIString& name,ComponentConfig* config)
{
    if (name.length()!=0) mConfigMap[(std::string)name]=config;
}
// FUNCTION: WON 0x00786d80
bool ComponentConfig::HandleModifyGroup(ConfigParser& parser)
{
    std::string name,inherited;
    if (!ReadGroupStart(parser,name,inherited)) return true;
    ComponentConfig* config=GetComponentConfig(GUIString(name),true);
    config->ParseGroup(parser,true,false,false,false);
    return true;
}
// FUNCTION: WON 0x007870a0
bool ComponentConfig::HandleModify()
{
    ComponentConfig* config=ReadComponentConfig();
    EnsureComma();
    std::string instruction=Fn3842F0(false);
    SkipComma();
    StringToUpperCaseInPlace(instruction);
    if (!config->BaseHandleInstruction(*mConfigParser,instruction,*mStringParser))
        mConfigParser->AddWarning("Unrecognized instruction: "+instruction);
    return true;
}
// FUNCTION: WON 0x00787370
bool ComponentConfig::BaseHandleInstruction(ConfigParser& parser,const std::string& key,StringParser& value)
{
    mConfigParser=&parser;
    mStringParser=&value;
    return Virtual24(key);
}
}

// Original ComponentConfig.cpp file-static state, original-owned data566b30/566b34.
// Symbolic references only: this split source does not allocate duplicate profiling state.
extern bool gShowParseTime;
extern int gLinesParsed;
namespace WONAPI {
// FUNCTION: WON 0x00787520
bool ComponentConfig::Virtual24(const std::string& key)
{
    if (key=="MODIFY") return HandleModify();
    if (key=="MAPCOMPONENT") return HandleMapComponent();
    if (key=="CONTROLID") {
        Component* component=mComponent.get();
        component->mControlId=ReadInt();
    } else if (key=="LEFT") {
        Component* component=mComponent.get();
        int value=ReadInt();
        component->Fn2F4B70(value,component->Top());
    } else if (key=="TOP") {
        Component* component=mComponent.get();
        int value=ReadInt();
        component->Fn2F4B70(component->Left(),value);
    } else if (key=="WIDTH") {
        Component* component=mComponent.get();
        mHasSetWidth=true;
        int value=ReadInt();
        component->Fn2F4B90(value,component->Height());
    } else if (key=="HEIGHT") {
        Component* component=mComponent.get();
        mHasSetHeight=true;
        int value=ReadInt();
        component->Fn2F4B90(component->Width(),value);
    } else if (key=="POS") {
        int x=ReadInt(); EnsureComma();
        int y=ReadInt();
        mComponent->Fn2F4B70(x,y);
    } else if (key=="SIZE") {
        mHasSetWidth=mHasSetHeight=true;
        int width=ReadInt(); EnsureComma();
        int height=ReadInt();
        mComponent->Fn2F4B90(width,height);
    } else if (key=="TRANSLUCENTCOLOR") {
        int color=Fn385510();
        if (EndOfString()) mComponent->SetTranslucentColor(color,4);
        else {
            EnsureComma();
            mComponent->SetTranslucentColor(color,ReadInt());
        }
    } else if (key=="SIZEATLEAST") {
        SetDesiredSizeIfNecessary();
        int width=ReadInt(); EnsureComma();
        int height=ReadInt();
        mHasSetWidth=width>0;
        mHasSetHeight=height>0;
        mComponent->Fn2F4BC0(width,height);
    } else if (key=="SETDESIREDSIZE") {
        int width=ReadInt(); EnsureComma();
        int height=ReadInt();
        mComponent->Fn2F4E90(width,height);
    } else if (key=="SETDESIREDWIDTH") {
        Component* component=mComponent.get();
        int width=ReadInt();
        component->SetDesiredWidth(width);
    } else if (key=="SETDESIREDHEIGHT") {
        Component* component=mComponent.get();
        int height=ReadInt();
        component->SetDesiredHeight(height);
    } else if (key=="POSSIZE") {
        mHasSetWidth=mHasSetHeight=true;
        int x=ReadInt(); EnsureComma();
        int y=ReadInt(); EnsureComma();
        int width=ReadInt(); EnsureComma();
        int height=ReadInt();
        mComponent->Fn2F4BF0(x,y,width,height);
    } else if (key=="ENABLE") {
        mComponent->SetEnabled(Fn3837F0());
    } else if (key=="SETVISIBLE") {
        mComponent->SetVisible(Fn3837F0());
    } else if (key=="REQUESTFOCUS") {
        if (Fn3837F0()) mComponent->Fn2F5890();
    } else if (key=="DEFAULTCURSOR") {
        mComponent->SetDefaultCursor(ReadCursor());
    } else if (key=="COMPONENTFLAGS") {
        while (true) {
            bool add=true;
            GUIString name=ReadFlag(&add);
            if (name.length()==0) break;
            int flag;
            if (name.CompareNoCase("Invisible")==0) flag=0x1;
            else if (name.CompareNoCase("WantFocus")==0) flag=0x8;
            else if (name.CompareNoCase("ParentDrag")==0) flag=0x20;
            else if (name.CompareNoCase("NoParentCursor")==0) flag=0x40;
            else if (name.CompareNoCase("Disabled")==0) flag=0x80;
            else if (name.CompareNoCase("GrabBG")==0) flag=0x100;
            else if (name.CompareNoCase("Translucent")==0) flag=0x200;
            else if (name.CompareNoCase("NoInput")==0) flag=0x800;
            else if (name.CompareNoCase("AlwaysOnTop")==0) flag=0x1000;
            else if (name.CompareNoCase("WantTabFocus")==0) flag=0x4000;
            else if (name.CompareNoCase("Clip")==0) flag=0x2000;
            else if (name.CompareNoCase("DebugBG")==0) flag=0x20000;
            else throw ConfigObjectException("Unknown component flag: "+(std::string)name);
            mComponent->SetComponentFlags(flag,add);
        }
    } else if (key=="DECODEIMAGES") {
        Window* window=Window::GetDefaultWindow();
        window->SetSkipDecodeImages(!Fn3837F0());
    } else if (key=="SHOWPARSETIME") {
        gShowParseTime=Fn3837F0();
    } else if (key=="STARTPARSETIMER") {
        ConfigParser::ResetLineCount();
        PerfTimer::GlobalStart(NULL);
    } else if (key=="STOPPARSETIMER") {
        PerfTimer::GlobalStop(NULL);
        gLinesParsed=ConfigParser::GetLineCount();
    } else return false;
    return true;
}
}

namespace WONAPI {
// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK direct-child configuration lookups, borrowed results.
Component* ComponentConfig::GetChildComponent(const std::string& theName)
{
    std::map<std::string,SmartPtr<ComponentConfig>,StringLessNoCase>::iterator anItr=mConfigMap.find(theName);
    if(anItr==mConfigMap.end())
        return NULL;
    return anItr->second->mComponent.get();
}
ComponentConfig* ComponentConfig::GetChildConfig(const std::string& theName)
{
    std::map<std::string,SmartPtr<ComponentConfig>,StringLessNoCase>::iterator anItr=mConfigMap.find(theName);
    if(anItr!=mConfigMap.end())
        return anItr->second.get();
    else
        return NULL;
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007a1b60
// (public: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std:)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0079ba60
// (public: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_str)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0079b8b0
// (public: struct std::pair<class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<cla)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x0079b820
// (public: __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::SmartPtr<cla)
// TEMPLATE: WON 0x0079d140
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_)
