
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
// FUNCTION: WON 0x0075cde0
LobbyUserInfo* LobbyFn35CDE0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data==NULL || data->mCurrentUserId==0)
        return NULL;
    for(LobbyUserInfoMap::iterator it=data->mUsers.begin();it!=data->mUsers.end();++it)
    {
        LobbyUserInfo& user=it->second;
        if(user.mUserId==data->mCurrentUserId)
            return &user;
    }
    return NULL;
}
// FUNCTION: WON 0x0075ce60
const LobbyUserInfoMap& LobbyFn35CE60()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL)
        return data->mUsers;
    static LobbyUserInfoMap empty;
    return empty;
}
// FUNCTION: WON 0x0075cec0
GUIString LobbyFn35CEC0(const GUIString& name)
{
    LobbyUserInfo* info=LobbyFn35CF20(name);
    if(info!=NULL) return info->mPassword;
    return gEmptyGUIString;
}
// FUNCTION: WON 0x0075cf20
LobbyUserInfo* LobbyFn35CF20(const GUIString& name)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data==NULL)
        return NULL;
    LobbyUserInfoMap::iterator it=data->mUsers.find(name);
    if(it==data->mUsers.end()) return NULL;
    return &it->second;
}
// FUNCTION: WON 0x0075d660
const LobbyLanNameInfoMap& LobbyFn35D660()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL)
        return data->mLanNameInfoMap;
    static LobbyLanNameInfoMap empty;
    return empty;
}
// FUNCTION: WON 0x0075d8e0
const LobbyDirectConnectMap& LobbyFn35D8E0()
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL)
        return data->mDirectConnectMap;
    static LobbyDirectConnectMap empty;
    return empty;
}
// FUNCTION: WON 0x0075dc80
const LobbyOptionSet& LobbyFn35DC80(const GUIString& column)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL) {
        std::map<GUIString,LobbyOptionSet,GUIStringLessNoCase>::iterator it=data->mGameOptions.find(column);
        if(it!=data->mGameOptions.end())
            return it->second;
    }
    static LobbyOptionSet empty;
    return empty;
}
// FUNCTION: WON 0x0075e470
void LobbyFn35E470(const GUIString& name)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && name!=data->mS58) {
        data->mS58=name;
        data->mUserDirty=true;
    }
}
}
