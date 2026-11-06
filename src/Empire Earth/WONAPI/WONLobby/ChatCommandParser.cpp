// ChatCommandParser original-backed reconstruction, WONLobby.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
// Original body family: RVA355750..357d67, allocation0x54 at362aed..362b06,
// RefCount-derived RTTI/vtable451a9c and matching original EH ownership.
// Original .cpp filename is unconfirmed; this source spelling is provisional.
// Boundary separates the following LobbyCompletion family; generated container
// helpers and canonical GUIString/std::wstring COMDATs carry no forced markers.
// Original quirks retained: single-character remainder truncation, ambiguous
// prefix matching, duration unit validation and persistent INFINITE duration.
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <cwctype>
#include <cctype>
#include <cstdlib>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"
extern unsigned short gEmptyWideString[]; // 0x8e60bc, an empty wide string (zero .bss)
using namespace WONAPI;
namespace WONAPI {
}
// FUNCTION: WON 0x755750
ChatCommandParser::ChatCommandParser()
{
 mB2C=false;
 mAllowColonEmote=false;
 mMatchThreshold=2;
 mDuration=0;
 mPrefix='/';
 mRoomFlags=0;
}

// FUNCTION: WON 0x7559f0
ChatCommandParser::~ChatCommandParser() {}

// FUNCTION: WON 0x755b50
void ChatCommandParser::InitCommands()
{
 AddCommands(::ChatCommandLogic_Whisper_String,2);
 AddCommands(::ChatCommandLogic_Block_String,4);
 AddCommands(::ChatCommandLogic_Emote_String,3);
 AddCommands(::ChatCommandLogic_Ignore_String,5);
 AddCommands(::ChatCommandLogic_Away_String,6);
 AddCommands(::ChatCommandLogic_Invite_String,16);
 AddCommands(::ChatCommandLogic_Uninvite_String,17);
 AddCommands(::ChatCommandLogic_Reply_String,18);
 AddCommands(::ChatCommandLogic_Clear_String,19);
 AddCommands(::ChatCommandLogic_Help_String,20);
 AddCommands(::ChatCommandLogic_ShowTeam_String,21);
 AddCommands(::ChatCommandLogic_Moderator_String,7);
 AddCommands(::ChatCommandLogic_ServerMute_String,8);
 AddCommands(::ChatCommandLogic_ServerBan_String,9);
 AddCommands(::ChatCommandLogic_Mute_String,10);
 AddCommands(::ChatCommandLogic_Ban_String,11);
 AddCommands(::ChatCommandLogic_Unmute_String,12);
 AddCommands(::ChatCommandLogic_ServerUnmute_String,13);
 AddCommands(::ChatCommandLogic_Unban_String,14);
 AddCommands(::ChatCommandLogic_ServerUnban_String,15);
 AddCommands(::ChatCommandLogic_Alert_String,22);
 AddCommands(gEmptyWideString,23);
 AddCommands(gEmptyWideString,24);
 AddCommands(::ChatCommandLogic_Warn_String,25);
}

// FUNCTION: WON 0x7561b0
void ChatCommandParser::SetClientList(LobbyClientList* clients,int roomFlags)
{
 mClients=clients;
 mRoomFlags=roomFlags;
}

// FUNCTION: WON 0x756220
void ChatCommandParser::AddCommand(const GUIString& text,int command)
{
 if(text.empty()) return;
 GUIString key=text;
 mCommands[key]=command;
 std::map<int,std::wstring>::iterator i=mCommandStrings.find(command);
 if(i==mCommandStrings.end())
  mCommandStrings.insert(std::map<int,std::wstring>::value_type(command,text));
}

// FUNCTION: WON 0x7564b0
GUIString ChatCommandParser::GetCommandString(int command)
{
 std::map<int,std::wstring>::iterator i=mCommandStrings.find(command);
 if(i!=mCommandStrings.end()) return GUIString(i->second);
 ChatCommandParser::CommandMap::iterator j=mCommands.begin();
 for(;j!=mCommands.end();++j)
  if(j->second==command) return GUIString(j->first);
 return GUIString("");
}

// FUNCTION: WON 0x756620
const char* ChatCommandParser::GetCommandName(int command)
{
 switch(command) {
 case 2:return "w";case 3:return "em";case 4:return "b";case 5:return "i";
 case 6:return "afk";case 7:return "mod";case 8:return "smute";case 9:return "sban";
 case 10:return "mute";case 11:return "ban";case 12:return "unmute";case 13:return "sunmute";
 case 14:return "unban";case 15:return "sunban";case 16:return "invite";case 17:return "uninvite";
 case 18:return "r";case 19:return "cls";case 20:return "?";case 21:return "team";
 case 22:return "alert";case 23:return "startshutdown";case 24:return "abortshutdown";case 25:return "warn";
 default:return "";
 }
}

// FUNCTION: WON 0x756760
void ChatCommandParser::AddCommands(const std::wstring& names,int command)
{
 StringParser parser(names.c_str());
 std::list<std::wstring> tokens;
 parser.ReadValue(tokens);
 if(!names.empty() && !tokens.empty()) {
  for(std::list<std::wstring>::iterator i=tokens.begin();i!=tokens.end();++i)
   AddCommand(GUIString(*i),command);
 } else AddCommand(GUIString(GetCommandName(command)),command);
}

// FUNCTION: WON 0x756950
static bool CompareNames(const GUIString& a,const GUIString& b,bool& prefix)
{
 prefix=false;
 int na=a.length(),nb=b.length();
 for(int i=0;i<na && i<nb;i++) {
  if(towupper(a[i])!=towupper(b[i])) return false;
 }
 if(na<=nb) prefix=true;
 return na==nb;
}

// FUNCTION: WON 0x7569f0
static void SkipWhitespace(const GUIString& text,int& pos)
{
 while(pos<text.length() && iswctype(text[pos],_SPACE)) ++pos;
}

// FUNCTION: WON 0x756a40
static GUIString GetToken(const GUIString& text,int& pos,bool upper,bool quotes)
{
 SkipWhitespace(text,pos);
 if(pos>=text.length()) return GUIString("");
 GUIString token;
 if(quotes && text[pos]=='"') {
  int end=text.find('"',pos+1);
  if(end>=0) {
   token=text.substr(pos+1,end-pos-1);
   pos=end+1;
   if(upper) token.toUpper();
   return token;
  }
 }
 while(pos<text.length()) {
  wchar_t ch=text[pos];
  if(upper) ch=towupper(ch);
  if(ch==' ') return token;
  token.append(ch);
  ++pos;
 }
 return token;
}

// FUNCTION: WON 0x756c90
static GUIString GetRestOfLine(const GUIString& text,int pos)
{
 SkipWhitespace(text,pos);
 if(pos>=text.length()) return GUIString("");
 GUIString result;
 if(text[pos]=='"') {
  int end=text.find('"',pos+1);
  if(end>=0) { result=text.substr(pos+1,end-pos-1); return result; }
 }
 int end=text.length()-1;
 while(end>pos && text[end]==' ') --end;
 if(end<=pos) return GUIString("");
 return text.substr(pos,end-pos+1);
}

// FUNCTION: WON 0x756e90
LobbyClient* ChatCommandParser::FindClient(const GUIString& name,int* outMatch)
{
 int match=0;
 LobbyClient* found=NULL;
 LobbyClientList* clients=mClients.get();
 if(clients!=NULL) {
  for(LobbyClientMap::iterator i=clients->mClients.begin();i!=clients->mClients.end();++i) {
   LobbyClient* client=i->second.get();
   bool prefix=false;
   if(CompareNames(name,client->mName,prefix)) {
    if(match==3) { match=1;found=NULL;break; }
    match=3;found=client;
   } else if(prefix) {
    if(match==0) { match=2;found=i->second.get(); }
    else if(match==2) { match=1;found=NULL;break; }
   }
  }
 }
 if(match==0 || match==1) found=NULL;
 if(outMatch) *outMatch=match;
 return found;
}

// FUNCTION: WON 0x756f40
bool ChatCommandParser::ParseCommand(const GUIString& text)
{
 int pos=1;
 GUIString command=GetToken(text,pos,true,false);
 ChatCommandParser::CommandMap::iterator i=mCommands.find(command);
 if(i==mCommands.end()) { mCommand=0;mError=1;return false; }
 mCommand=i->second;
 switch(mCommand) {
 case 18: mTargetName=mInput; break;
 case 5:
  mTargetName=GetRestOfLine(text,pos);
  if(mTargetName.length()==0) goto target_done;
  break;
 case 4: mTargetName=GetRestOfLine(text,pos); break;
 case 12:
 case 13:
  mTargetName=GetRestOfLine(text,pos);
  if(mTargetName.length()==0) goto target_done;
  break;
 case 2:case 8:case 9:case 10:case 11:case 16:case 17:case 25:
  mTargetName=GetToken(text,pos,false,true); break;
 default: goto target_done;
 }
 if(mTargetName.length()==0) { mError=2;return false; }
 {
  int match;
  mTarget=FindClient(mTargetName,&match);
  if(match<mMatchThreshold) {
   mError=match==1?4:3;
   return false;
  }
 }
target_done:
 SkipWhitespace(text,pos);
 switch(mCommand) {
 case 2:case 3:case 22:case 25:
  if(pos>=text.length()) { mError=5;return false; }
  mText=text.substr(pos);
  break;
 case 18:
  mText=text.substr(pos);
  if(mText.length()!=0) mCommand=2;
  break;
 case 16:case 17:
  mText=text.substr(pos);
  mInvitation=mCommand==16;
  break;
 case 8:case 9:case 10:case 11:case 23:
  {
   std::string duration=GetToken(text,pos,true,false);
   if(duration!="INFINITE" && duration.length()>0) {
    char unit=duration[duration.length()-1];
    if(isalpha(unit)) duration=duration.substr(0,duration.length()-1);
    int value=atoi(duration.c_str());
    switch(unit) {
    case 'D': value*=24;
    case 'H': value*=60;
    case 'M': value*=60;
    case 'S': break;
    default: mError=7;return false;
    }
    if(value==0) { mError=7;return false; }
    mDuration=value;
   }
   mText=GetRestOfLine(text,pos);
  }
  break;
 case 12:case 13: mText=GetRestOfLine(text,pos); break;
 }
 return true;
}

// FUNCTION: WON 0x757630
bool ChatCommandParser::HandleInput(const GUIString& text)
{
 mError=0;
 mTarget=NULL;
 mTargetName=GetEmptyGUIStringData();
 if(mClients.get()==NULL) { mError=6;return false; }
 if(text[0]==mPrefix) return ParseCommand(text);
 if(mAllowColonEmote && text[0]==':') {
  mCommand=3;
  mText=text.substr(1);
  int pos=0;SkipWhitespace(mText,pos);
  if(pos==mText.length()) { mError=5;return false; }
 } else {
  mCommand=1;
  int pos=0;SkipWhitespace(text,pos);
  if(pos==text.length()) { mError=5;return false; }
  mText=text;
 }
 return true;
}

// FUNCTION: WON 0x757800
void ChatCommandParser::AddHelpEntry(int command,const GUIString& entry,GUIString& out)
{
 out.append(entry);
 out.append(GUIString("<n>"));
}

// FUNCTION: WON 0x7579c0
GUIString ChatCommandParser::GetHelp(unsigned short clientId)
{
 bool lan=(mRoomFlags>>3)&1;
 bool isIgnored=false,isAdmin=false,isModerator=false,isCaptain=false;
 if(mClients.get()!=NULL) {
  LobbyClient* client=mClients->FindClient(clientId);
  if(client!=NULL) {
   isAdmin=client->Fn3A54D0();
   isModerator=client->Fn3A54E0((mRoomFlags>>1)&1);
   isCaptain=client->Fn3A54E0(1);
   isIgnored=client->Fn3A54C0();
  }
 }
 GUIString out=::ChatCommandLogic_HelpHeader_String;
 out.append(GUIString("<n>"));
 AddHelpEntry(20,::ChatCommandLogic_HelpHelp_String,out);
 AddHelpEntry(2,::ChatCommandLogic_WhisperHelp_String,out);
 AddHelpEntry(18,::ChatCommandLogic_ReplyHelp_String,out);
 AddHelpEntry(3,::ChatCommandLogic_EmoteHelp_String,out);
 AddHelpEntry(5,::ChatCommandLogic_IgnoreHelp_String,out);
 AddHelpEntry(19,::ChatCommandLogic_ClearHelp_String,out);
 if(mRoomFlags&2) AddHelpEntry(21,::ChatCommandLogic_ShowTeamHelp_String,out);
 if(!lan) {
  AddHelpEntry(4,::ChatCommandLogic_BlockHelp_String,out);
  AddHelpEntry(6,::ChatCommandLogic_AwayHelp_String,out);
  if(isCaptain) {
   AddHelpEntry(16,::ChatCommandLogic_InviteHelp_String,out);
   AddHelpEntry(17,::ChatCommandLogic_UninviteHelp_String,out);
  }
  if(isModerator||isAdmin) {
   AddHelpEntry(10,::ChatCommandLogic_MuteHelp_String,out);
   AddHelpEntry(12,::ChatCommandLogic_UnmuteHelp_String,out);
   AddHelpEntry(11,::ChatCommandLogic_BanHelp_String,out);
   AddHelpEntry(14,::ChatCommandLogic_UnbanHelp_String,out);
   if(isAdmin) {
    AddHelpEntry(7,::ChatCommandLogic_ModeratorHelp_String,out);
    AddHelpEntry(8,::ChatCommandLogic_ServerMuteHelp_String,out);
    AddHelpEntry(13,::ChatCommandLogic_ServerUnmuteHelp_String,out);
    AddHelpEntry(9,::ChatCommandLogic_ServerBanHelp_String,out);
    AddHelpEntry(15,::ChatCommandLogic_ServerUnbanHelp_String,out);
    AddHelpEntry(25,::ChatCommandLogic_WarnHelp_String,out);
   }
  }
  if(isIgnored) AddHelpEntry(22,::ChatCommandLogic_AlertHelp_String,out);
 }
 return out;
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00755850
// (public: virtual void * __thiscall WONAPI::ChatCommandParser::`scalar deleting destructor'(unsigned int)    [??_GChatCommandParser@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00756190
// (public: __thiscall WONAPI::GUIString::operator class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<uns)
// TEMPLATE: WON 0x00756470
// (public: __thiscall std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned s)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00755870
// (public: __thiscall std::map<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,int,)
// TEMPLATE: WON 0x00755930
// (public: __thiscall std::map<int,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00768ab0
// (public: __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > con)
// TEMPLATE: WON 0x00768de0
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x00768e90
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x00769040
// (public: class std::_Tree<int,struct std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::all)
// TEMPLATE: WON 0x007690f0
// (protected: class std::_Tree<int,struct std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::)
// TEMPLATE: WON 0x0076b7f0
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x0076b830
// (protected: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,stru)
// TEMPLATE: WON 0x0076c010
// (protected: void __thiscall std::_Tree<int,struct std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,c)
// TEMPLATE: WON 0x0076c050
// (protected: struct std::_Tree<int,struct std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std:)
// TEMPLATE: WON 0x00770010
// (public: void * __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short)
// TEMPLATE: WON 0x00770030
// (public: void * __thiscall std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<uns)
// TEMPLATE: WON 0x0076b310
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x0076bb30
// (public: class std::_Tree<int,struct std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::all)
// TEMPLATE: WON 0x0076f890
// (void __cdecl std::_Construct(struct std::pair<int const ,class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::all)
// TEMPLATE: WON 0x0076fc30
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsi)
// TEMPLATE: WON 0x00768ce0
// (public: struct std::pair<class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned)
