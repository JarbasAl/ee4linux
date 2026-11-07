#pragma once
#include "ee/wonapi/WONLobbyConfig.h"
// Actual SDK ConfigValue template hierarchy; native12/16/20/24-byte owners and five slots.
// Requires RefCount/StringParser/ConfigValueBase through the existing project include order.
namespace WONAPI {
template<class T> class ConfigValue : public ConfigValueBase {
protected:
    T& mVal; //0c, borrowed target; completeConfigValue16
    virtual bool CopyValueHook(ConfigValueBase*);
public:
    ConfigValue(T& value):mVal(value) {}
};
template<class T> class SimpleConfigValue : public ConfigValue<T> {
protected:
    T mDefVal; //10, complete20 for bool andunsignedshort
    virtual void InitializeDefaultHook();
public:
    SimpleConfigValue(T& value,T initial):ConfigValue<T>(value),mDefVal(initial) {}
    virtual bool ReadValue(StringParser&);
    virtual void WriteValue(std::string&);
};
template<class T> class ConfigStringValue : public ConfigValue<std::basic_string<T> > {
protected:
    const T* mDefVal; //10, borrowed; complete20
    virtual void InitializeDefaultHook();
public:
    ConfigStringValue(std::basic_string<T>& value,const T* initial):ConfigValue<std::basic_string<T> >(value),mDefVal(initial) {}
    virtual bool ReadValue(StringParser&);
    virtual void WriteValue(std::string&);
};
template<class T> class ConfigListValue : public ConfigValue<T> {
protected:
    const char* mDefStr; //10, borrowed narrow/wide character view
    bool mIsWString; //14
    bool mAdditive; //15, complete24
    virtual void InitializeDefaultHook();
public:
    ConfigListValue(T& value,const char* initial,bool additive):ConfigValue<T>(value),mDefStr(initial),mIsWString(false),mAdditive(additive) {}
    ConfigListValue(T& value,const wchar_t* initial,bool additive):ConfigValue<T>(value),mDefStr((const char*)initial),mIsWString(true),mAdditive(additive) {}
    virtual bool ReadValue(StringParser&);
    virtual void WriteValue(std::string&);
};
}