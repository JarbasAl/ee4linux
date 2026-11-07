// WONLobby configuration dependencies proved from original RTTI, registry calls and singleton writers.
#ifndef EE_WONAPI_WONLOBBYCONFIG_H
#define EE_WONAPI_WONLOBBYCONFIG_H
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"
#include <vector>
#include "ee/wonapi/WONStringCompare.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyLogic.h"
namespace WONAPI {

// Complete target reference geometry: ConfigParser0x88=136B, state0x28=40B.
// Native ctor3b1d20/dtor3b1e10 and all field readers corroborate the complete136 owner.
// Full construction/destruction and stack use are authorized by Round5 native lifetime proof; Unicode5d remains intentionally uninitialized in ctor.
struct ConfigParserState {
    long mFilePos; //00
    std::string mFileNameAndPath; //04
    int mLineNum; //14
    std::wstring mBackStr; //18; end28hex =40B
};
class ConfigParser {
private:
    ConfigParser(const ConfigParser&);
    ConfigParser& operator=(const ConfigParser&);
protected:
    static int mLineCount; //actual data566b54, no storage in the object
public:
    ConfigParser(); //native3b1d20, complete136 constructor reviewed Round5
    static void ResetLineCount() { mLineCount=0; } //actual SDK inline/global566b54, support0
    static int GetLineCount() { return mLineCount; } //actual SDK inline, support0
    virtual ~ConfigParser(); //native3b1e10, completeCloseFile+naturalownedcleanup; sole actual virtual slot
    bool ReadToken(std::string&, bool inToEndLine=false, bool inSkip=false); //3b3d70
    bool ReadToken(std::wstring&, bool inToEndLine=false, bool inSkip=false); //3b3d50
    void SetParent(ConfigParser*);
    void SetError(const std::string&, bool); //preserve canonical native3b1fb0 spelling
    void AddWarning(const std::string&);
    void CopyWarnings(ConfigParser&);
    const std::string& GetAbortReason();
    bool IsStopped() { return mStopped; }
    void Stop() { mStopped=true; }
    bool GetIgnorePathOnIncludeFile() { return mIgnorePathOnIncludeFile; }
    bool EndOfFile();
    int GetChar();
    int PeekChar();
    void UngetChar(int);
    void UngetStr(const std::string&);
    void UngetStr(const std::wstring&);
    void SkipWhitespace();
    void SkipLine();
    void SkipBlockComment();
    bool OpenFile(const std::string&);
    void CloseFile();
    void Reset();
    void PushDefineStack();
    void PopDefineStack();
    bool AddDefine(const std::string&, const std::string&, bool);
    bool GetDefine(const std::string&, std::string&);
    bool GetDefine(const std::string&, std::wstring&);
    bool AddGroupDefine(const std::string&, ConfigParserState*);
    bool GetGroupDefine(const std::string&, ConfigParserState&);
    void GetState(ConfigParserState&);
    bool SetState(const ConfigParserState&);
    struct DefineMapContainer; //actual nested pointee; complete definition follows this class
    typedef std::vector<SmartPtr<DefineMapContainer> > DefineStack;
    DefineStack mDefineStack; //04..14
    ConfigParser* mParent; //14
    FileReader mFileReader; //18..2c actual20B
    std::wstring mBackStr; //2c
    std::string mAbortReason; //3c
    std::string mWarnings; //4c
    bool mWarningsAsErrors; //5c
    bool mIsUnicode; //5d; actual ctor leaves uninitialized
    int mLineNum; //60; natural padding5e/5f
    std::string mFilePath; //64
    std::string mFileNameAndPath; //74
    bool mIgnorePathOnIncludeFile; //84
    bool mStopped; //85; natural padding86/87, end0x88=136B
};

// Target nested type32B, independently proved; archived SDK DefineStruct belongs another revision.
// Natural implicit construction/destruction supports only the new define operations; no full Parser lifetime authority.
struct ConfigParser::DefineMapContainer : public RefCount {
    typedef std::map<std::string, std::string, StringLessNoCase> DefineMap;
    typedef std::map<std::string, ConfigParserState, StringLessNoCase> GroupDefineMap;
    DefineMap mDefineMap; //08,12B; sentinel30hex
    GroupDefineMap mGroupDefineMap; //14,12B; sentinel48hex; end20hex
};

// RegKey RTTI and three-slot virtual family, actual stack object extent 0x2c.
// Out-of-line constructor/destructor remain original-owned; no new vtable implementation here.
class RegKey {
public:
    enum GetResult { Ok, NotFound, BadType, NoMore, NoSize };
    enum DataType { Long, String, Binary, Other };
    GetResult GetValue(const std::string&,std::string&) const;
    bool OpenSubKey(const std::string&, bool = false, bool = true);
    bool GetSubKey(const std::string&, RegKey&, bool = false, bool = true) const;
    GetResult GetFirstSubKey(RegKey&) const;
    GetResult GetNextSubKey(RegKey&) const;
    GetResult GetFirstValueName(std::string&, DataType&) const;
    GetResult GetNextValueName(std::string&, DataType&) const;
    bool SetValue(const std::string&, const std::string&);
    bool SetValue(const std::string&, unsigned long);
    bool SetValue(const std::string&, const void*, unsigned long);
    bool DeleteValue(const std::string&);
    RegKey(int inRoot, const std::string& inPath, bool inCreate, bool inNonvolatile); // 0x2a4260
    virtual ~RegKey();                      // 0x2a42e0
    virtual bool DeleteSubKey(const std::string& inKey); // 0x2a4830
    virtual bool DeleteSubKey(RegKey& inKey); // 0x2a4870 (unused overload qualifier provisional)
    int QueryDWORD(const std::string& inName, DWORD& outValue); // 0x2a4680
    bool OpenNewKey(int inRoot, const std::string& inPath, bool inCreate, bool inNonvolatile); //native2a4440, unresolved HKEY create implementation stays carved
    void CloseKey(); //native2a4370
    DWORD GetToBuf(const std::string& inName, unsigned long& outLength) const; //native2a4390
    int QueryBinary(const std::string& inName, ConstSmartPtr<ByteBuffer>& outValue); //native2a46e0
    bool IsOpen() const { return mIsOpen; } //genuine SDK inline bool08; no HKEY validity inference
    const std::string& GetLeafName(void) const { return mPath; } //genuine SDK borrowed owned-string0c accessor
    HKEY mKey;                              // +0x04
    bool mIsOpen;                           // +0x08
    std::string mPath;                      // +0x0c
    mutable char* mBuffer;                          // +0x1c
    mutable DWORD mL20;                             // registry query/enum buffer management
    mutable DWORD mL24;
    mutable DWORD mL28;
private:
    HKEY GetRootHKey(int inRoot); //genuine SDK private root conversion, native2a41d0
    bool OpenNewKey(HKEY inRoot,const std::string& inPath,bool inCreate = false,bool inNonvolatile = true); //genuine SDK private handle overload, native2a4470
};

class LobbyPersistentData;
class LobbyContainer;
class AccountLogic; class DialogLogic; class InitLogic; class RoutingLogic;
class StagingLogic; class LanLogic; class PingLogic; class WindowEventBase;
class ChatCommandParser;
enum LobbyStatus; enum LobbyRunMode; enum LobbyGameType;
// Complete actual118 owner: native3628c0/3630a0 and allocation361fb0.
// Construction/destruction/protocol providers remain external until separately authored.
// Target revision omits the later SDK BigFileList after mInitError; no opaque/gap fields.
class WONLobbyPrv : public RefCount {
public:
    WONLobbyPrv(); //3628c0 external1979, owns all genuine members below
    virtual ~WONLobbyPrv(); //3630a0 actual604, scalar36308030
    bool Init(ResourceConfigTable*,void (*)(ComponentConfig*)); //365ac0, physical extent pending tail audit
    void InitAuth(const std::wstring&,const char* = NULL); //366010 actual RET8
    void Destroy(); //3660f0, original provider
    SmartPtr<LobbyPersistentData> mPersistentData; //08
    SmartPtr<LobbyConfig> mConfig; //0c, canonical existing field spelling retained
    SmartPtr<LobbyContainer> mLobbyContainer; //10
    GUIString mInitError; //14
    SmartPtr<AccountLogic> mAccountLogic; //18
    SmartPtr<DialogLogic> mDialogLogic; //1c
    SmartPtr<InitLogic> mInitLogic; //20
    SmartPtr<RoutingLogic> mRoutingLogic; //24
    SmartPtr<StagingLogic> mStagingLogic; //28
    SmartPtr<LanLogic> mLanLogic; //2c
    SmartPtr<PingLogic> mPingLogic; //30
    LobbyLogicMgr mLogicMgr; //34..40, actual listowner
    SmartPtr<UDPManager> mUDPManager; //40
    SmartPtr<WindowEventBase> mTimerEvent; //44
    DWORD mLastTimerTick; //48, ctor leaves untouched
    LobbyStatus mLobbyStatus; //4c
    LobbyRunMode mRunMode; //50
    bool mSuspended; //54, natural padding55..57
    SmartPtr<ServerContext> mAccountServers; //58
    SmartPtr<ServerContext> mAuthServers; //5c
    SmartPtr<ServerContext> mDirServers; //60
    SmartPtr<ServerContext> mRoomDirServers; //64
    SmartPtr<ServerContext> mFirewallDetectServers; //68
    SmartPtr<ServerContext> mPatchServers; //6c
    WONStatus mVersionStatus; //70
    SmartPtr<AuthContext> mAuthContext; //74
    CDKey mCDKey; //78..94, complete1c
    std::wstring mLoginCommunity; //94..a4
    bool mHasSetHashFileList; //a4, natural paddinga5..a7
    typedef std::list<std::string> HashFileList;
    HashFileList mHashFileList; //a8..b4
    unsigned short mProductId; //b4
    unsigned short mLanProductId; //b6
    unsigned short mLobbyPort; //b8, natural paddingba..bb
    time_t mSysTOUTime,mGameTOUTime; //bc,c0
    bool mBehindFirewall,mFirewallTestDone; //c4,c5, natural paddingc6..c7
    std::string mSysTOUPath,mGameTOUPath; //c8,d8
    IPAddr mPublicAddr; //e8..110, complete28
    SmartPtr<ChatCommandParser> mChatCommandParser; //110
    LobbyGameType mCurGameType; //114..118
};
extern SmartPtr<WONLobbyPrv> gLobbyData;     // 0x56630c; assignment/shutdown prove intrusive SmartPtr
// LobbyConfig::mDefaultFileName owns original566310; see complete declaration in WONLobbyGUI.h.
extern bool gLobbyUseLocalIP56634D;          // 0x56634d; distinct KeyAdd target from config member +de
void StringToUpperCaseInPlace(std::string&); //existing2a69a0; StringUtil.cpp
std::string WStringToString(const std::wstring&); //existing2a6280; StringUtil.cpp
std::wstring StringToWString(const std::string& inString); // 0x2a6130
}
#endif
