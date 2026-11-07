// WONLobby model contracts proved from original RTTI, allocations, callers and lifetime paths.
#ifndef EE_WONAPI_WONLOBBYMODEL_H
#define EE_WONAPI_WONLOBBYMODEL_H
#include <set>
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {

// sizeof0x50, only virtual slot destructor. No unknown gaps.
class LobbyFriend : public RefCount {
public:
    LobbyFriend(const GUIString&, unsigned long);
    // Implicit destructor: original3a6380 has no own-vptr store.
    void Fn3A6440(const IPAddr&, const GUIString&, unsigned char);
    void Fn3A65B0();
    void Fn3A66B0(LobbyClient*);
    void Fn3A66F0(bool);
    void Fn3A6700(unsigned long);
    void Fn3A6710(unsigned long);
    GUIString mName;             //08
    unsigned long GetUserId() { return mUserId; }
    unsigned long mUserId;       //0c
    IPAddr mAddress;             //10
    GUIString mLocation;         //38
    unsigned long mLastSearch;   //3c provisional meaning: ctor zero; reset preserves
    unsigned char mFlags;       //40
    void SetLastLocate(unsigned long inTime) { mLastLocate = inTime; }
    unsigned long mLastLocate;   //44
    unsigned long mSearchStart;  //48
    bool mAnonymous;             //4c
public:
    unsigned long GetSearchStart() const { return mSearchStart; }
    bool GetAnonymous() const { return mAnonymous; }
public:
    const GUIString& GetName() const { return mName; }
    unsigned char GetFlags() const { return mFlags; }
};

// Narrow canonical LobbyGroup packet; name/member semantics provisional.
// RTTI LobbyGroup : RefCount, vtable452c4c single dtor, new0x20 at3a9ca5.
class LobbyGroup : public RefCount {
public:
    LobbyGroup();
    virtual ~LobbyGroup();
    void Fn3A8C30(RoutingGroupInfo*, LobbyClientList*);
    void Fn3A8D50(const RoutingMemberMap&, LobbyClientList*);
    void Fn3A8E40(LobbyClient*, unsigned char);
    SmartPtr<LobbyClient> Fn3A8EB0(unsigned short);
    LobbyClient* Fn3A8F80(unsigned short, LobbyClient**);
    LobbyClient* Fn3A8FF0();
    LobbyClient* Fn3A9010(unsigned short);
    bool Fn3A9020();
    bool Fn3A9030();
    bool Fn3A9040();
    bool Fn3A9050();
    bool Fn3A9060();
    bool Fn3A9070();
    GUIString mName;                   //08
    unsigned short GetId() { return mId; }
    unsigned short mId;               //0c
    unsigned short mCaptainId;        //0e
    unsigned long mNumClients;        //10
    unsigned long mMaxClients;        //14
    unsigned long mFlags;             //18
    SmartPtr<LobbyClientList> mClients;//1c
};

typedef std::set<std::string, StringLessNoCase> LobbyServerAddressSet;
typedef std::map<unsigned short, SmartPtr<LobbyGroup> > LobbyGroupMap;
LobbyFriend* LobbyFn35D3F0(unsigned long inUserId); // carved persistence friend lookup

// Original allocation 0x68; RTTI LobbyServer : RefCount. Connection target is reference-only here.
class LobbyServer : public RefCount {
public:
    LobbyServer();                         // 0x3a91c0
    void Fn3A96A0(); //3a96a0, original-carved disconnect; actual QueueSocket ancestry
    virtual ~LobbyServer();                // 0x3a9500
    bool Fn3A9490(const std::string& inName);
    void Fn3A9640(RoutingConnection* inConnection);
    void Fn3A9700(DirEntity* inEntity);     // original carved; DirEntity metadata closure remains open
    void Fn3A9AD0(const RoutingClientMap& inClients);
    void Fn3A9AF0();
    void Fn3A9B60();
    void Fn3A9BE0(const RoutingGroupMap& inGroups, bool inFlag);
    LobbyClient* Fn3A9F50(RoutingClientInfo* inInfo);
    void Fn3AA000(LobbyClient* inClient, unsigned char inFlags, LobbyGroup* inGroup);
    LobbyGroup* Fn3AA030(RoutingGroupInfo* inInfo);
    SmartPtr<LobbyGroup> Fn3AA170(unsigned short inId);
    SmartPtr<LobbyClient> Fn3AA280(unsigned short inId);
    SmartPtr<LobbyClient> Fn3AA2B0(unsigned short inId, LobbyGroup* inGroup);
    LobbyClient* GetClient(int inId);       // 0x3aa370 (int: EELobbyGame 0x692c7e zero-extends the id it passes)
    LobbyClient* Fn3AA380(unsigned long inId);
    LobbyClient* Fn3AA390();
    LobbyGroup* Fn3AA3B0(int inId);
    int Fn3AA410();
    bool Fn3AA430();
    void Fn3AA440();

    GUIString mName;                       // +0x08
    LobbyServerAddressSet mAddresses;      // +0x0c
    IPAddr mAddress;                       // +0x18
    int mNumChatClients;                   // +0x40
    int mNumClients;                       // +0x44
    unsigned long mLastPing;               // +0x48
    bool mB4C;                             // +0x4c
    LobbyGroupMap mGroups;                 // +0x50
    SmartPtr<LobbyClientList> mClients;    // +0x5c
    SmartPtr<RoutingConnection> mConnection; // +0x60; do not allocate/sizeof/dynamic_cast/override its unclosed family
    unsigned short mMyClientId;            // +0x64
public:
    unsigned long GetLastPing() const { return mLastPing; }
    unsigned short GetMyClientId() const { return mMyClientId; }
};

typedef std::map<IPAddr, SmartPtr<LobbyServer> > LobbyServerMap;
class LobbyServerList : public RefCount {
public:
    LobbyServerList();
    virtual ~LobbyServerList();
    void Fn3AA670(const DirEntityList& inEntities);
    LobbyServer* Fn3AA980(const IPAddr& inAddress);
    LobbyServerMap mServers;               // +0x08; allocation 0x14, node 0x3c
};
typedef std::map<unsigned short,SmartPtr<LobbyGame> > LobbyIdGameMap;
typedef std::map<IPAddr,SmartPtr<LobbyGame> > LobbyAddrGameMap;
// RTTI452c44/COL47aca0/TD4ded08; actual size2c from3e6e80 allocation.
class LobbyGameList : public RefCount {
public:
    LobbyGameList(int inType);
    virtual ~LobbyGameList();
    void Clear();
    void AddGame(LobbyGame*);
    LobbyGame* GetOrCreateGame(unsigned short,bool*);
    LobbyGame* GetOrCreateGame(const IPAddr&,bool*);
    LobbyGame* GetGame(unsigned short);
    LobbyGame* GetGame(const IPAddr&);
    SmartPtr<LobbyGame> RemoveGame(unsigned short);
    SmartPtr<LobbyGame> RemoveGame(const IPAddr&);
    void Rewind();
    bool HasMore();
    LobbyGame* Next();
    bool IsEmpty();
    bool RemoveStaleGames(long);
    long GetType() const { return mType; }
    long mType;                            // +08
    LobbyIdGameMap mIdGames;                // +0c
    LobbyAddrGameMap mAddrGames;            // +18
    LobbyIdGameMap::iterator mIdItr;        // +24
    LobbyAddrGameMap::iterator mAddrItr;    // +28
};


}
#endif
