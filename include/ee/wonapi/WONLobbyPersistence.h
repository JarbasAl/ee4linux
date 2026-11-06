#ifndef EE_WONAPI_WONLOBBYPERSISTENCE_H
#define EE_WONAPI_WONLOBBYPERSISTENCE_H
#include <string>
#include <set>
#include <exception>
#include <map>
#include <list>
#include <stdio.h>
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONBlowfish.h"
// Canonical original-backed WONLobby persistence storage and caller contracts.
// Complete typed1164B owner; constructor35bcb0 source admitted with real typed member cleanup.

namespace WONAPI {
// Complete IO classes: original extent, field and lifetime contracts proved.
// FileReader exactsizeof20B(0x14): embedded ConfigParser field18 throughnextstring2c, ctor3b1d20.
// FileWriter exactsizeof4: Initstackslots4 andconstructor2a1e10/dtor2a1e20.
class FileReader {
public:
    bool IsOpen() { return mFile!=NULL || mBuffer!=NULL; }
    FileReader(); //2a0d00
    FileReader(const char* inPath); //2a0d10
    FileReader(const void* theMemDataP, unsigned long theMemDataLen);
    bool OpenText(const char* inPath); //2a0d80 fopen r; name provisional for target revision
    void Open(const void* inData, unsigned long inLength); //2a0dc0
    unsigned long pos(); //2a0e20
    unsigned long length(); //2a0e40
    unsigned long Available() { return length()-pos(); } //genuine SDK FileReader API
    void SetPos(unsigned long inPos); //2a0e90
    int ReadMaxBytes(void* outData, int inMaxBytes); //2a12d0 whole399B
    int TryReadByte(); //2a1780
    void SkipBytes(unsigned long inLength); //2a17c0
    void ReadIntoMemory(); //2a1ac0
    int ReadLength(int inFieldSize); //2a1b80
    void ReadRawString(std::string& outString, int inNumChars); //2a1bc0
    void ReadString(std::string& outString, unsigned char inFieldSize); //2a1d90
    void ReadRawWString(std::wstring& theWStr,int theNumChars);
    void ReadWString(std::wstring& theWStr,unsigned char theLengthFieldSize=2);
    void Rewind(); //2a1dc0
    bool EndOfFile(); //2a1de0
private:
    void Init(); //2a0ce0 actual six zero stores,20B
public:
    ~FileReader(); //2a0d30
    bool Open(const char* inPath); //2a0d40
    void Close(); //2a0de0
    void ReadBytes(void* outData, unsigned long inLength); //2a1460
    unsigned char ReadByte(); //2a1b20
    unsigned short ReadShort(); //2a1b40
    long ReadLong(); //2a1b60
    FILE* mFile; //00
    const char* mBuffer; //04; memory-source pointer (delete[] viaownedflag10)
    unsigned long mLength; //08
    unsigned long mPosition; //0c
    bool mOwnBuffer; //10
    bool mMemoryMode; //11; memoryOpen2a0dd6 sets1/fileOpen clears; naturalpadding12..13
};
// Genuine SDK FILE* RAII owner; explicit constructor/destructor, no Native identity claimed.
struct AutoFile {
    FILE* mFile;
    AutoFile(FILE* file) : mFile(file) {}
    ~AutoFile() { fclose(mFile); }
};
class FileWriter {
public:
    FileWriter(); //2a1e10
    unsigned long pos(); //2a2080
    void SetPos(unsigned long inPos); //2a20a0 whole325B
    void WriteLength(unsigned long inLength, unsigned char inFieldSize); //2a2620
    void WriteRawString(const std::string& inString); //2a2660
    void WriteString(const std::string& inString, unsigned char inFieldSize); //2a2680
    void WriteRawWString(const std::wstring& theWStr);
    void WriteWString(const std::wstring& theWStr,unsigned char theLengthFieldSize=2);
    ~FileWriter(); //2a1e20
    bool Open(const char* inPath, bool inOpenIfReadOnly); //2a1e30 retry after clearing readonly
    void Close(); //2a2060
    void WriteBytes(const void* inData, unsigned long inLength); //2a2400
    void WriteByte(unsigned char inValue); //2a25f0
    void WriteShort(unsigned short inValue); //2a2600
    void WriteLong(long inValue); //2a2610
    FILE* mFile; //00
};

// Both direct bases proved by COL hierarchy (file-rtti.py); only dtor overrides.
class FileReaderException : public std::exception {
public:
    FileReaderException() : std::exception() {} //genuine SDK explicit default constructor;0Native
    FileReaderException(const std::string& inDescription) { mDescription = inDescription; }
    // Actual SDK/native 2a10e0: destructor is implicit; std::string/base cleanup is compiler generated.
    std::string mDescription;       // +08; ctor/throw sites2a1460
};
class FileWriterException : public std::exception {
public:
    FileWriterException(const std::string& inDescription) { mDescription = inDescription; }
    // Actual SDK/native 2a2210: destructor is implicit; std::string/base cleanup is compiler generated.
    std::string mDescription;       // +08; ctor/throw sites2a2400
};
// Original-backed record and call-only declarations; no full private-object lifetime.
// GUIStringLessNoCase actual definition shared from WONLobbyGUI.h via model include.
// Complete nonpolymorphic record,24B. Field names provisional; widths and ownership proved.
struct LobbyUserInfo {
    LobbyUserInfo() : mUserId(0), mValue04(0), mValue08(0), mLastUseTime(0) {} //actual fourzero stores at35f867..870
    long mUserId; //00, fileReadShort zeroextends intoDWORD
    long mValue04; //04, fileReadLong
    long mValue08; //08, fileReadLong
    long mLastUseTime; //0c, fileReadLong; LoginCtrl35c910 time(NULL)
    GUIString mUserName; //10
    GUIString mPassword; //14
};
typedef std::map<GUIString, LobbyUserInfo, GUIStringLessNoCase> LobbyUserInfoMap;
const LobbyUserInfoMap& LobbyFn35CE60(); //null singleton returns actual functionstaticempty map
GUIString LobbyFn35CEC0(const GUIString& inUserName);
LobbyUserInfo* LobbyFn35CF20(const GUIString& inUserName);
LobbyUserInfo* LobbyFn35C910(const GUIString& inUserName, const GUIString& inPassword, bool inExistingOnly);
// Original-backed call-only contracts; implementations remain original-carved until admitted.

// Complete getter/setter bytes: persistence_options.txt. Proposed spellings provisional.
const GUIString& LobbyFn35E2D0(); // persistentTabString58 or gEmptyGUIString fallback
bool LobbyFn35E2F0(); // bool54; fallbackfalse
bool LobbyFn35E300(); //55
bool LobbyFn35E310(); //56
bool LobbyFn35E320(); //5c
bool LobbyFn35E330(); //5e
bool LobbyFn35E340(); //5f
bool LobbyFn35E350(); //5d
bool LobbyFn35E360(); //60
bool LobbyFn35E370(); //61
bool LobbyFn35E380(); //62
bool LobbyFn35E390(); //63
unsigned short LobbyFn35E3A0(); //64; caller3bee7b masksffff; fallback50
unsigned short LobbyFn35E3C0(); //66; port fallback8585
long LobbyFn35E3E0(); //70, numeric IPv4 adapter as565f40; synchronized Root/GUI correction
void LobbyFn35E3F0(unsigned short); //66
void LobbyFn35E410(bool); //54; setsdirty7d only on actualchange
void LobbyFn35E430(bool); //55
void LobbyFn35E450(bool); //56
void LobbyFn35E470(const GUIString&); //58; compares GUIStringData then assigns owns ref
void LobbyFn35E4E0(bool); //5c; also recomputes7c from6c
void LobbyFn35E510(bool); //5e
void LobbyFn35E530(bool); //5f
void LobbyFn35E550(bool); //5d; also recomputes7c from6d
void LobbyFn35E580(bool); //60
void LobbyFn35E5A0(bool); //61
void LobbyFn35E5C0(bool); //62
void LobbyFn35E5E0(bool); //63
void LobbyFn35E600(unsigned short); //64; storedfromCX, callerint speed threshold narrows
void LobbyFn35E620(); //network handle global565f40 ->70; setsdirty7c
void LobbyFn35E660(bool); //flush userprefs via35faf0 whenforced||dirty7c
void LobbyFn35E680(bool); //flushglobalprefs via360770 whenforced||dirty7d
void LobbyFn35E6A0(); //restoredefaults35c720 then dirty7d=1/recomputesdirty7c
const GUIString& LobbyFn35DC40(); //35dc40; preferred server name S74
const GUIString& LobbyFn35DC60(); //35dc60; preferred group name S78

// Friends map exact getter35d270 returnsdata+30 orlocalstatic566300.
typedef std::map<unsigned long, SmartPtr<LobbyFriend> > LobbyFriendMap;
const LobbyFriendMap& LobbyFn35D270();
LobbyFriend* LobbyFn35D3F0(unsigned long inUserId); //35d3f0 borrowed map lookup
void LobbyFn35D2C0(unsigned long inUserId);
void LobbyFn35DB70(const GUIString& inServerName, const GUIString& inGroupName);
void LobbyFn35D6C0(const GUIString& inName);
void LobbyFn35D7F0(const IPAddr& inAddress, const GUIString& inName);
void LobbyFn35D870(const IPAddr& inAddress);

// Exact getter35cfc0 returnsdata+18 orfunctionstaticemptyset566320.
typedef std::set<GUIString, GUIStringLessNoCase> LobbyIgnoredNameSet;
const LobbyIgnoredNameSet& LobbyFn35CFC0();
// Full473B35d460 target-proves canonical name/id allocation and returned actualLobbyFriend50.
LobbyFriend* LobbyFn35D460(const GUIString& inName, unsigned long inUserId);

// Cohesive original-backed call-only persistence additions; implementations remain carved.
// Primary SDK nominal APIs and complete target wrappers independently agree.
typedef std::map<GUIString, long, GUIStringLessNoCase> LobbyLanNameInfoMap;
typedef std::map<IPAddr, GUIString> LobbyDirectConnectMap;
const GUIString& LobbyFn35D640(); //19B, existingname68 oractualemptyGUIString
const LobbyLanNameInfoMap& LobbyFn35D660(); //66B, field114c orfunctionstatic566388
void LobbyFn35D770(); //120B RemoveLanName; exact NoCase comparator328030
void LobbyFn35DF00(); //312B ResetServerListFilters; ordinary copy, no iterator repair
const LobbyDirectConnectMap& LobbyFn35D8E0(); //66B, field1158 orstatic566330
void LobbyFn35CF90(LobbyUserInfo* inUser); //35B, ID14=recordfirstDWORDor0 thenReadUser
// Original record35dd20,24B; Add35baa0/Erase35bc20 prove fieldtypes and sensitivity.
class LobbyOptionSet {
public:
    typedef std::list<GUIString> StringList;
    typedef std::map<GUIString, StringList::iterator> StringMap;
    void Add(const GUIString& inValue); //35baa0
    bool Erase(const GUIString& inValue); //35bc20
    void clear(); //genuine SDK ServerListFilterSet::clear; Source-only NativeID unlocated
    StringMap mIndex; //00
    StringList mValues; //0c
};

// Complete actual1164 storage contract; all declared data fields original-backed.

// This declaration grants no new external allocation-site authority.
struct LobbyReconnectInfo {
    long mLastUseTime;
    int mReconnectId;
    LobbyReconnectInfo() : mLastUseTime(0) {} // Original leaves reconnectId uninitialized.
};
class LobbyPersistentData : public RefCount {
public:
    LobbyPersistentData(); //35bcb0; native currentUser14 deliberately untouched before reset
    // Implicit virtual destructor: original35c2c0 has no own-vptr store.
    std::string GetUserFileName(long inUserId); // 35e6e0; descriptive name
    long FindUnusedUserId();                  // 35e770; descriptive name
    long ReadFileHeader(FileReader& inReader);// 35f6a0; descriptive name
    void WriteFileHeader(FileWriter& outWriter, long inVersion);//35f750
    void Fn35EB30(); //35eb30, user history eviction
    GUIString Fn35EE80(FileReader& inReader); //35ee80, plain byte text
    void Fn35F3D0(FileWriter& outWriter, const GUIString& inText); //35f3d0
    GUIString Fn35F000(FileReader& inReader); //35f000, plain ushort text
    GUIString Fn35F190(FileReader& inReader); //35f190, crypto remains original-carved
    void Fn35F4E0(FileWriter& outWriter, const GUIString& inText); //35f4e0
    void Fn35F530(FileWriter& outWriter, const GUIString& inText); //35f530
    GUIString Fn35C520(); //35c520, CRC helper class remains original-carved
    void Fn35C720(); //35c720
    void Fn35F7B0(); //35f7b0
    void Fn35FAF0(); //35faf0
    void Fn35FC70(); //full2459 includes typedcatch6 and continuation
    void Fn360770(); //full913 includes typedcatch6 and continuation
    void Fn360B10(FileReader& inReader); //648
    void Fn360DA0(FileWriter& outWriter); //241
    void Fn35E7E0(); //35e7e0, prune typed LAN-name history to50
    void Fn35EA40(); //228, existing carved LAN-name pruning owner
    std::set<long> mUserIds;                  // +08; type proved namedinsert35fa52
long mCurrentUserId; //14
std::set<GUIString, GUIStringLessNoCase> mIgnoredNames; //18
std::set<unsigned long> mIgnoredUserIds; //24
std::map<unsigned long, SmartPtr<LobbyFriend> > mFriends; //30
std::map<GUIString, LobbyOptionSet, GUIStringLessNoCase> mDefaultGameOptions; //3c
std::map<GUIString, LobbyOptionSet, GUIStringLessNoCase> mGameOptions; //48
bool mB54, mB55, mB56; //54,55,56; naturalpadding57
GUIString mS58; //58
bool mB5C, mB5D, mB5E, mB5F, mB60, mB61, mB62, mB63;
unsigned short mShort64, mPort66; //64,66
GUIString mS68; //68
bool mB6C, mB6D; //6c,6d; naturalpadding6e/6f
long mNetworkHandle; //70; actual numeric IPv4 adapter, provisional member name
GUIString mS74, mS78; //74,78
bool mGlobalDirty, mUserDirty; //7c,7d; naturalpadding7e/7f

LobbyUserInfoMap mUsers; //80
std::map<IPAddr, LobbyReconnectInfo> mReconnectInfoMap; //8c
Blowfish mEncryptKey; //98,10b4B, ends114c
std::map<GUIString, long, GUIStringLessNoCase> mLanNameInfoMap; //114c
std::map<IPAddr, GUIString> mDirectConnectMap; //1158, ends1164

};
LobbyPersistentData* GetLobbyPersistentData(); //existing native35c900 provider, cdecl borrowed singleton accessor
// Actual reconnect map scalar contracts; archived nominal int matches native record field.
int LobbyFn35DB00(const IPAddr& inAddress); //35db00: null/missing0, existing ID; borrowed input
void LobbyFn35D940(const IPAddr& inAddress, int inReconnectId); //35d940: stores time+ID then Save360770
// Target record is the existing complete24B LobbyOptionSet; archived nominal ServerListFilterSet.
const LobbyOptionSet& LobbyFn35DC80(const GUIString& inColumn);
void LobbyFn35DEA0(const GUIString& inColumn, const GUIString& inValue);
void LobbyFn35DED0(const GUIString& inColumn, const GUIString& inValue);
}

#endif
