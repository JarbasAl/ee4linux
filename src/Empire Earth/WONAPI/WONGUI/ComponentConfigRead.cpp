// Reconstructed WONGUI configuration readers. Original source grouping provisional.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <cctype>
#include <list>
#include <vector>
#include <map>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"
namespace WONAPI {

// FUNCTION: WON 0x00782db0
bool ComponentConfig::EndOfString()
{
    mStringParser->SkipWhitespace();
    return mStringParser->EndOfString();
}

// FUNCTION: WON 0x00782dd0
void ComponentConfig::SkipComma()
{
    mStringParser->SkipWhitespace();
    if(mStringParser->GetChar(0)==',')
        mStringParser->IncrementPos(1);
}

// FUNCTION: WON 0x00782e00
void ComponentConfig::EnsureComma()
{
    if(!mStringParser->CheckNextChar(','))
        throw ConfigObjectException("Expecting ','");
}

// FUNCTION: WON 0x00783560
int ComponentConfig::ParseExpression(int inLevel)
{
    int value;
    mStringParser->SkipWhitespace();
    if(mStringParser->GetChar(0)=='(')
    {
        mStringParser->IncrementPos(1);
        value=ParseExpression(0);
        mStringParser->SkipWhitespace();
        if(mStringParser->GetChar(0)!=')')
            throw ConfigObjectException("Expecting ')'");
        mStringParser->IncrementPos(1);
    }
    else
        value=GetIntValue();
    for(;;)
    {
        mStringParser->SkipWhitespace();
        unsigned char op=mStringParser->GetChar(0);
        mStringParser->IncrementPos(1);
        if(op=='+' && inLevel<1)
            value+=ParseExpression(1);
        else if(op=='-' && inLevel<1)
            value-=ParseExpression(1);
        else if(op=='*' && inLevel<2)
            value*=ParseExpression(2);
        else if(op=='/' && inLevel<2)
        {
            int divisor=ParseExpression(2);
            if(divisor==0)
                throw ConfigObjectException("Division by 0.");
            value/=divisor;
        }
        else
        {
            mStringParser->IncrementPos(-1);
            return value;
        }
    }
}

// FUNCTION: WON 0x007837e0
int ComponentConfig::ReadInt()
{
    return ParseExpression(0);
}

// FUNCTION: WON 0x007837f0
bool ComponentConfig::Fn3837F0()
{
    bool value;
    if(mStringParser->ReadValue(value))
        return value;
    throw ConfigObjectException("Expecting bool");
}

// FUNCTION: WON 0x00783930
int ComponentConfig::ReadChar()
{
    mStringParser->SkipWhitespace();
    if(mStringParser->EndOfString())
        throw ConfigObjectException("Expecting char");
    int value=mStringParser->GetWChar(0);
    mStringParser->IncrementPos(1);
    return value;
}

// FUNCTION: WON 0x00783a90
GUIString ComponentConfig::ReadFlag(bool* outOn)
{
    mStringParser->SkipWhitespace();
    int next=mStringParser->GetChar(0);
    if(next=='+')
    {
        mStringParser->IncrementPos(1);
        if(outOn!=NULL) *outOn=true;
    }
    else if(next=='-')
    {
        mStringParser->IncrementPos(1);
        if(outOn!=NULL) *outOn=false;
    }
    return Fn3842F0(true);
}

// FUNCTION: WON 0x00783b00
GUIString ComponentConfig::ReadRawString(bool inStopAtWhitespace)
{
    std::wstring value;
    mStringParser->ReadString(value,inStopAtWhitespace);
    return GUIString(value);
}

// FUNCTION: WON 0x007856f0
Direction ComponentConfig::ReadDirection()
{
    GUIString value=Fn3842F0(false);
    if(value.CompareNoCase("Left")==0) return Direction_Left;
    if(value.CompareNoCase("Right")==0) return Direction_Right;
    if(value.CompareNoCase("Up")==0) return Direction_Up;
    if(value.CompareNoCase("Down")==0) return Direction_Down;
    throw ConfigObjectException("Bad Direction");
}

// FUNCTION: WON 0x00785950
HorzAlign ComponentConfig::ReadHorzAlign()
{
    GUIString value=Fn3842F0(false);
    if(value.CompareNoCase("Left")==0) return HorzAlign_Left;
    if(value.CompareNoCase("Center")==0) return HorzAlign_Center;
    if(value.CompareNoCase("Right")==0) return HorzAlign_Right;
    throw ConfigObjectException("Bad HorzAlign");
}

// FUNCTION: WON 0x00785b10
VertAlign ComponentConfig::ReadVertAlign()
{
    GUIString value=Fn3842F0(false);
    if(value.CompareNoCase("Top")==0) return VertAlign_Top;
    if(value.CompareNoCase("Center")==0) return VertAlign_Center;
    if(value.CompareNoCase("Bottom")==0) return VertAlign_Bottom;
    throw ConfigObjectException("Bad VertAlign");
}

// FUNCTION: WON 0x00782f40
int ComponentConfig::GetIntValue()
{
    {
        int value;
        if(mStringParser->ReadValue(value)) return value;
    }
    std::string name;
    while(!mStringParser->EndOfString())
    {
        char next=mStringParser->GetChar(0);
        unsigned int c=next;
        if(c<256 && isspace(c)) break;
        if(next!='@' && next!='_' && next!='.' && !(c<256 && isalnum(c))) break;
        name+=next;
        mStringParser->IncrementPos(1);
    }
    bool isFunction=mStringParser->GetChar(0)=='(';
    if(name[0]=='@')
    {
        // Native compares the letters directly before its redundant toupper(bool).
        if(name.length()>3 && name[1]=='I' && name[2]=='D' && name[3]=='_')
        {
            int controlId;
            if(!mResourceConfigTable->GetControlId(name.substr(4),controlId))
                throw ConfigObjectException("Invalid control id: "+name);
            return controlId;
        }
        return GetIntResource(std::string(name.c_str()+1));
    }
    ComponentConfig* config=this;
    std::string key;
    std::string::size_type dot=name.rfind('.');
    if(dot!=std::string::npos)
    {
        std::string configName=name.substr(0,dot);
        key=name.substr(dot+1);
        config=GetComponentConfig(GUIString(configName),true);
    }
    else key=name;
    StringToUpperCaseInPlace(key);
    int result;
    if(isFunction)
    {
        std::vector<int> params;
        mStringParser->IncrementPos(1);
        if(!mStringParser->CheckNextChar(')'))
        {
            for(;;)
            {
                int param=ReadInt();
                params.push_back(param);
                if(mStringParser->CheckNextChar(')')) break;
                EnsureComma();
                if(mStringParser->CheckNextChar(')')) break;
            }
        }
        config->Virtual30(key,params,result);
        return result;
    }
    if(config->Virtual2C(key,result)) return result;
    throw ConfigObjectException("Invalid named value: "+key);
}

// FUNCTION: WON 0x00784210
int ComponentConfig::GetIntResource(const std::string& inKey)
{
    int value;
    if(mCurResourceConfig.get()!=NULL && mCurResourceConfig->GetInt(inKey,value))
        return value;
    if(mL38!=NULL)
        return static_cast<ComponentConfig*>(mL38)->GetIntResource(inKey);
    throw ConfigObjectException("Int resource not found: "+inKey);
}

// FUNCTION: WON 0x00787390
bool ComponentConfig::Virtual2C(const std::string& inName,int& outValue)
{
    if(inName=="LEFT") outValue=mComponent->Left();
    else if(inName=="RIGHT") outValue=mComponent->Right();
    else if(inName=="TOP") outValue=mComponent->Top();
    else if(inName=="BOTTOM") outValue=mComponent->Bottom();
    else if(inName=="WIDTH") outValue=mComponent->Width();
    else if(inName=="HEIGHT") outValue=mComponent->Height();
    else return false;
    return true;
}

// FUNCTION: WON 0x00787490
bool ComponentConfig::Virtual30(const std::string& inName,const std::vector<int>& inParams,int& outValue)
{
    if(inName=="MAX")
    {
        int value=0;
        for(std::vector<int>::const_iterator i=inParams.begin();i!=inParams.end();++i)
            if(*i>value) value=*i;
        outValue=value;
        return true;
    }
    if(inName=="MIN")
    {
        int value=0;
        for(std::vector<int>::const_iterator i=inParams.begin();i!=inParams.end();++i)
            if(*i<value) value=*i;
        outValue=value;
        return true;
    }
    return false;
}

}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007a1540
// (public: class std::_Ptrit<int,int,int *,int &,int *,int &> __thiscall std::_Ptrit<int,int,int *,int &,int *,int &>::operator+(int)const     [??H?$_Ptr)
// TEMPLATE: WON 0x007a1bc0
// (protected: void __thiscall std::vector<int,class std::allocator<int> >::_Xlen(void)const     [?_Xlen@?$vector@HV?$allocator@H@std@@@std@@IBEXXZ])
// TEMPLATE: WON 0x007a2ce0
// (void __cdecl std::fill(class std::_Ptrit<int,int,int *,int &,int *,int &>,class std::_Ptrit<int,int,int *,int &,int *,int &>,int const &)    [?fill@st)
// TEMPLATE: WON 0x007a1230

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00754b50
// (protected: int * __thiscall std::vector<int,class std::allocator<int> >::_Ucopy(class std::_Ptrit<int,int,int *,int &,int *,int &>,class std::_Ptrit<i)
// TEMPLATE: WON 0x00754b80
// (class std::_Ptrit<int,int,int *,int &,int *,int &> __cdecl std::copy_backward(class std::_Ptrit<int,int,int *,int &,int *,int &>,class std::_Ptrit<int)
// TEMPLATE: WON 0x0079c980
