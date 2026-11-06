// Complete native-backed missing lobby support. No primary quota.
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
// FUNCTION: WON 0x0075e3f0
void LobbyFn35E3F0(unsigned short value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mPort66) {
        data->mPort66=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e410
void LobbyFn35E410(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB54) {
        data->mB54=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e430
void LobbyFn35E430(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB55) {
        data->mB55=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e450
void LobbyFn35E450(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB56) {
        data->mB56=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e4e0
void LobbyFn35E4E0(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB5C) {
        data->mB5C=value;
        data->mUserDirty=true;
        data->mGlobalDirty=value!=data->mB6C;
    }
}

// FUNCTION: WON 0x0075e510
void LobbyFn35E510(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB5E) {
        data->mB5E=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e530
void LobbyFn35E530(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB5F) {
        data->mB5F=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e550
void LobbyFn35E550(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB5D) {
        data->mB5D=value;
        data->mUserDirty=true;
        data->mGlobalDirty=value!=data->mB6D;
    }
}

// FUNCTION: WON 0x0075e580
void LobbyFn35E580(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB60) {
        data->mB60=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e5a0
void LobbyFn35E5A0(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB61) {
        data->mB61=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e5c0
void LobbyFn35E5C0(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB62) {
        data->mB62=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e5e0
void LobbyFn35E5E0(bool value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mB63) {
        data->mB63=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e600
void LobbyFn35E600(unsigned short value)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && value!=data->mShort64) {
        data->mShort64=value;
        data->mUserDirty=true;
    }
}

// FUNCTION: WON 0x0075e660
void LobbyFn35E660(bool force)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && (force || data->mGlobalDirty))
        data->Fn35FAF0();
}

// FUNCTION: WON 0x0075e680
void LobbyFn35E680(bool force)
{
    LobbyPersistentData* data=GetLobbyPersistentData();
    if(data!=NULL && (force || data->mUserDirty))
        data->Fn360770();
}
}
