// Native routing behavior; source-only reconstruction.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <wctype.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
// FUNCTION: WON 0x6c3730
static bool RoutingNamesMatch(const std::wstring& query,const std::wstring& name,bool& partial)
{
    partial=false;
    std::wstring::const_iterator q=query.begin(),n=name.begin();
    while(q!=query.end() && n!=name.end()) {
        if(towupper(*q)!=towupper(*n)) return false;
        ++q;
        ++n;
    }
    if(query.length()<=name.length()) partial=true;
    return query.length()==name.length();
}

// FUNCTION: WON 0x6c35a0
SmartRoutingConnection::MatchResult SmartRoutingConnection::FindClient(
    const std::wstring& inName,RoutingClientInfoPtr& outClient)
{
    AutoCrit crit(mDataCrit);
    MatchResult result=MatchResult_NotFound;
    for(RoutingClientMap::iterator i=mClientMap.begin();i!=mClientMap.end();++i) {
        bool partial=false;
        if(RoutingNamesMatch(inName,i->second->mName,partial)) {
            if(result==MatchResult_Exact) return MatchResult_Ambiguous;
            result=MatchResult_Exact;
            outClient=i->second;
        } else if(partial) {
            if(result==MatchResult_NotFound) {
                result=MatchResult_Partial;
                outClient=i->second;
            } else if(result==MatchResult_Partial) result=MatchResult_Ambiguous;
        }
    }
    return result;
}
}

#include <stdio.h>
namespace WONAPI {
// FUNCTION: WON 0x6c3110
void SmartRoutingConnection::LoadIgnoreSet()
{
    AutoCrit crit(mDataCrit);
    if(mIgnoreFilePath.empty()) return;
    FILE* file=fopen(mIgnoreFilePath.c_str(),"rb");
    if(file==NULL) return;
    std::wstring name;
    while(!feof(file)) {
        wchar_t ch=0;
        if(fread(&ch,sizeof(wchar_t),1,file)!=1) break;
        if(ch!=0) name+=ch;
        else {
            mIgnoreSet.insert(name);
            name.erase();
        }
    }
    fclose(file);
}

// FUNCTION: WON 0x6c3460
void SmartRoutingConnection::SaveIgnoreSet()
{
    AutoCrit crit(mDataCrit);
    if(mIgnoreFilePath.empty()) return;
    FILE* file=fopen(mIgnoreFilePath.c_str(),"wb");
    if(file==NULL) return;
    IgnoreSet::iterator i=mIgnoreSet.begin();
    while(i!=mIgnoreSet.end()) {
        fwrite(i->c_str(),sizeof(wchar_t),i->length(),file);
        wchar_t end=0;
        fwrite(&end,sizeof(wchar_t),1,file);
        ++i;
    }
    fclose(file);
}
}

#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// FUNCTION: WON 0x006bfb00
SmartRoutingConnection::SmartRoutingConnection()
    : mClientItr(NULL), mGroupItr(NULL), mMemberItr(NULL)
{
    mDefaultGroupId = 0;
    mClientId = 0;
    mLastInputError = InputError_None;
    mIgnoreFilePath = WONAPICore::GetDefaultFileDirectory() + "_wonignore.bin";
    mDoColonEmote = true;
    mReadClientCDKeyId = false;
    AddDefaultUserCommandStrings();
    AddDefaultModeratorCommandStrings();
    SetAllowPartialNameMatch(true);
    SetServerWideCommands(false);
}
// FUNCTION: WON 0x006c3970
void SmartRoutingConnection::AddCommandString(const std::wstring& inText, InputCommand inCommand)
{
    AutoCrit crit(mDataCrit);
    mCommandStringMap[WStringToUpperCase(inText)] = inCommand;
}
// FUNCTION: WON 0x6c3a50
void SmartRoutingConnection::AddDefaultUserCommandStrings()
{
    AutoCrit crit(mDataCrit);
    AddCommandString(L"W", InputCommand_Whisper);
    AddCommandString(L"WHISPER", InputCommand_Whisper);
    AddCommandString(L"ME", InputCommand_Emote);
    AddCommandString(L"EMOTE", InputCommand_Emote);
    AddCommandString(L"I", InputCommand_Ignore);
    AddCommandString(L"IGNORE", InputCommand_Ignore);
    AddCommandString(L"AWAY", InputCommand_Away);
    AddCommandString(L"AFK", InputCommand_Away);
}
// FUNCTION: WON 0x6c3d90
void SmartRoutingConnection::AddDefaultModeratorCommandStrings()
{
    AutoCrit crit(mDataCrit);
    AddCommandString(L"MOD", InputCommand_BecomeModerator);
    AddCommandString(L"SMUTE", InputCommand_ServerMute);
    AddCommandString(L"SBAN", InputCommand_ServerBan);
    AddCommandString(L"MUTE", InputCommand_Mute);
    AddCommandString(L"BAN", InputCommand_Ban);
    AddCommandString(L"UNMUTE", InputCommand_Unmute);
    AddCommandString(L"SUNMUTE", InputCommand_ServerUnmute);
}
// FUNCTION: WON 0x006c40a0
void SmartRoutingConnection::SetAllowPartialNameMatch(bool inAllow)
{
    AutoCrit crit(mDataCrit);
    if(inAllow)
        mMatchTolerance = MatchResult_Partial;
    else
        mMatchTolerance = MatchResult_Exact;
}
// FUNCTION: WON 0x006c40f0
void SmartRoutingConnection::SetServerWideCommands(bool inOn)
{
    AutoCrit crit(mDataCrit);
    mServerWideCommands = inOn;
}
}

namespace WONAPI {
// FUNCTION: WON 0x006bfe60
RoutingClientInfoPtr SmartRoutingConnection::Virtual1C()
{
    return new SmartRoutingClientInfo;
}
// FUNCTION: WON 0x006c4120
void SmartRoutingConnection::Virtual08()
{
    mClientId=0;
    mMyClient=NULL;
    RoutingConnection::Virtual08();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bff90
// (public: virtual void * __thiscall WONAPI::SmartRoutingConnection::`scalar deleting destructor'(unsigned int)    [??_GSmartRoutingConnection@WONAPI@@UA)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bfce0
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<struct WONAPI::RoutingClientInfo>,struct std::less<unsigned short>,class std::alloca)
// TEMPLATE: WON 0x006bffb0
// (public: __thiscall std::map<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,enum)
// TEMPLATE: WON 0x006c0070
// (public: __thiscall std::set<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,stru)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006bfda0
// (public: __thiscall std::map<unsigned short,class WONAPI::SmartPtr<struct WONAPI::RoutingGroupInfo>,struct std::less<unsigned short>,class std::allocat)
// TEMPLATE: WON 0x006bff00
// (public: virtual void * __thiscall WONAPI::SmartRoutingClientInfo::`scalar deleting destructor'(unsigned int)    [??_GSmartRoutingClientInfo@WONAPI@@UA)
// TEMPLATE: WON 0x006bff20
// (public: virtual __thiscall WONAPI::SmartRoutingClientInfo::~SmartRoutingClientInfo(void)    [??1SmartRoutingClientInfo@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c0130
// (public: virtual __thiscall WONAPI::SmartRoutingConnection::~SmartRoutingConnection(void)    [??1SmartRoutingConnection@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006c9ae0
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006c9c90
// (protected: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,clas)
// TEMPLATE: WON 0x006cac30
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006cb5a0
// (public: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short>)
// TEMPLATE: WON 0x006cacb0
// (protected: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,stru)
// TEMPLATE: WON 0x006cb4e0
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006c99e0
// (public: struct std::pair<class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned)
// TEMPLATE: WON 0x006ca750
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006cafb0
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,class s)
// TEMPLATE: WON 0x006cbca0
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsi)
// TEMPLATE: WON 0x006c3420
// (_wmemset)
