
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
#include "ee/wonapi/WONLobbyCore.h"
namespace WONAPI {
// FUNCTION: WON 0x0075cf90
void LobbyFn35CF90(LobbyUserInfo* user)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL) {
        if(user==NULL) data->mCurrentUserId=0;
        else data->mCurrentUserId=user->mUserId;
        data->Fn35FC70();
    }
}

// FUNCTION: WON 0x0075cfc0
const LobbyIgnoredNameSet& LobbyFn35CFC0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL) return data->mIgnoredNames;
    static LobbyIgnoredNameSet empty;
    return empty;
}

// FUNCTION: WON 0x0075d010
bool LobbyFn35D010(const GUIString& name)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data==NULL) return false;
    LobbyIgnoredNameSet::iterator it=data->mIgnoredNames.find(name);
    return it!=data->mIgnoredNames.end();
}

// FUNCTION: WON 0x0075d090
bool LobbyFn35D090(unsigned long id)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data==NULL) return false;
    std::set<unsigned long>::iterator it=data->mIgnoredUserIds.find(id);
    return it!=data->mIgnoredUserIds.end();
}

// FUNCTION: WON 0x0075d270
const LobbyFriendMap& LobbyFn35D270()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL) return data->mFriends;
    static LobbyFriendMap empty;
    return empty;
}

// FUNCTION: WON 0x0075d3f0
LobbyFriend* LobbyFn35D3F0(unsigned long id)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data==NULL) return NULL;
    LobbyFriendMap::iterator it=data->mFriends.find(id);
    if(it==data->mFriends.end()) return NULL;
    return it->second.get();
}

// FUNCTION: WON 0x0075d640
const GUIString& LobbyFn35D640()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mS68:gEmptyGUIString;
}

// FUNCTION: WON 0x0075dc40
const GUIString& LobbyFn35DC40()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mS74:gEmptyGUIString;
}

// FUNCTION: WON 0x0075dc60
const GUIString& LobbyFn35DC60()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mS78:gEmptyGUIString;
}

// FUNCTION: WON 0x0075e2d0
const GUIString& LobbyFn35E2D0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mS58:gEmptyGUIString;
}

// FUNCTION: WON 0x0075e2f0
bool LobbyFn35E2F0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB54:false;
}

// FUNCTION: WON 0x0075e300
bool LobbyFn35E300()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB55:false;
}

// FUNCTION: WON 0x0075e310
bool LobbyFn35E310()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB56:false;
}

// FUNCTION: WON 0x0075e320
bool LobbyFn35E320()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB5C:false;
}

// FUNCTION: WON 0x0075e330
bool LobbyFn35E330()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB5E:false;
}

// FUNCTION: WON 0x0075e340
bool LobbyFn35E340()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB5F:false;
}

// FUNCTION: WON 0x0075e350
bool LobbyFn35E350()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB5D:false;
}

// FUNCTION: WON 0x0075e360
bool LobbyFn35E360()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB60:false;
}

// FUNCTION: WON 0x0075e370
bool LobbyFn35E370()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB61:false;
}

// FUNCTION: WON 0x0075e380
bool LobbyFn35E380()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB62:false;
}

// FUNCTION: WON 0x0075e390
bool LobbyFn35E390()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mB63:false;
}

// FUNCTION: WON 0x0075e3a0
unsigned short LobbyFn35E3A0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL) return data->mShort64;
    return 50;
}

// FUNCTION: WON 0x0075e3c0
unsigned short LobbyFn35E3C0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL) return data->mPort66;
    return 8585;
}

// FUNCTION: WON 0x0075e3e0
long LobbyFn35E3E0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    return data!=NULL?data->mNetworkHandle:0;
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0076c580
// (public: class std::_Tree<class WONAPI::GUIString,class WONAPI::GUIString,struct std::_Setk<class WONAPI::GUIString,struct WONAPI::GUIStringLessNoCase,)
// TEMPLATE: WON 0x0076ca70
// (protected: void __thiscall std::_Tree<class WONAPI::GUIString,class WONAPI::GUIString,struct std::_Setk<class WONAPI::GUIString,struct WONAPI::GUIStri)
// TEMPLATE: WON 0x0076cab0
// (protected: struct std::_Tree<class WONAPI::GUIString,class WONAPI::GUIString,struct std::_Setk<class WONAPI::GUIString,struct WONAPI::GUIStringLessNoC)
// TEMPLATE: WON 0x0076cb00
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::LobbyFriend> >,struct std::_Mapk<uns)
