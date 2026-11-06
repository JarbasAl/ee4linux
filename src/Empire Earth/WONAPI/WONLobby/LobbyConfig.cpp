// C:\EEX\Empire Earth\WONAPI\WONLobby\LobbyConfig.cpp (RECONSTRUCTION).
// Original filename and exact TU boundary remain PROVISIONAL: family357f30..358de2
// is supported by the factory, static initializer chain, ctor, RTTI/vtable and EH.
// Completion family357d70..357f24 is excluded. No source-path assertion was found.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyParser.h"
#include "ee/wonapi/WONLobbyConfig.h"
using namespace WONAPI;

// Data4d954c contains originalVA0x757f30 (genuine SDK class-static factory default).
LobbyConfig::ConfigAllocator LobbyConfig::mAllocator = LobbyConfig::DefaultAllocator;

std::string LobbyConfig::mDefaultFileName("WONLobby.cfg");

namespace WONAPI {
bool gLobbyUseLocalIP56634D;
}

// FUNCTION: WON 0x757f30
LobbyConfig* LobbyConfig::DefaultAllocator()
{
    return new LobbyConfig;
}

// FUNCTION: WON 0x758090
LobbyConfig::LobbyConfig()
{
    KeyAdd("DirServer", mL54, NULL, true);
    KeyAdd("RoomDirServer", mL60, NULL, true);
    KeyAdd("AuthServers", mL6C, NULL, true);
    KeyAdd("ProductName", mS78, NULL);
    KeyAdd("ProductDirectory", mW88, NULL);
    KeyAdd("ResourceDirectory", mSC8, NULL);
    KeyAdd("Version", mS98, NULL);
    KeyAdd("LanVersion", mSA8, NULL);
    KeyAdd("Language", mSB8, "");
    KeyAdd("CRCCheck", mCRCCheck, true);
    KeyAdd("CDKeyCheck", mCDKeyCheck, true);
    KeyAdd("Public", mPublic, false);
    KeyAdd("LanBroadcastPort", mLanBroadcastPort, 8585);
    KeyAdd("NoPingInProgress", mNoPingInProgress, true);
    KeyAdd("LobbyPort", mLobbyPort, 8888);
    KeyAdd("CheckForInternet", mCheckForInternet, true);
    KeyAdd("PingCeiling", mPingCeiling, 1000);
    KeyAdd("AllowDuplicateNames", mAllowDuplicateNames, true);
    KeyAdd("AppendNumberToDefaultLanName", mAppendNumberToDefaultLanName, true);
    KeyAdd("AllowJoinGameWithoutPing", mAllowJoinGameWithoutPing, true);

    bool useLocalIP = false;
    RegKey registry(2, "SOFTWARE\\Sierra\\WONLobby", false, true);
    DWORD value = 0;
    if (registry.QueryDWORD("UseLocalIP", value) == 0)
        useLocalIP = value != 0;
    mUseLocalIP = useLocalIP;
    // Original target56634d lies in an existing configuration data field.
    // The canonical external bool is distinct from the member registry default.
    KeyAdd("UseLocalIP", gLobbyUseLocalIP56634D, useLocalIP);
}

// Destructor is implicit, shared original294e04; scalar-delete358570 is generated.
// Container destruction helpers358590/358650 are natural template output.

// FUNCTION: WON 0x758720
bool LobbyConfig::Virtual08(ConfigParser* parser, const std::string& key,
                           StringParser& values)
{
    if (key == "LOBBYDEFINE") {
        std::string name;
        std::string value;
        values.ReadString(name, true);
        values.ReadString(value, false);
        if (!name.empty())
            mMEC[name] = value;
        return true;
    }
    return ConfigObject::Virtual08(parser, key, values);
}

// FUNCTION: WON 0x758900
bool LobbyConfig::Virtual10(ConfigParser* parser)
{
    if (mL54.empty()) {
        parser->SetError("No Directory Servers specified.", false);
        return false;
    }
    if (mS78.empty()) {
        parser->SetError("No product name specified.", false);
        return false;
    }
    if (mS98.empty()) {
        parser->SetError("No version specified.", false);
        return false;
    }
    if (mSB8.empty()) {
        parser->SetError("No language specified.", false);
        return false;
    }
    if (mSA8.empty())
        mSA8 = mS98;
    if (mSC8.empty())
        mSC8 = ".\\" + mS78;
    if (mW88.empty())
        mW88 = std::wstring(L"/") + StringToWString(mS78);
    mMEC["Product"] = GetProductName();
    return true;
}

// FUNCTION: WON 0x758710
LobbyConfig* WONAPI::GetLobbyConfig()
{
    if (gLobbyData.mPtr != NULL)
        return gLobbyData.mPtr->mConfig.mPtr;
    return NULL;
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00758570
// (public: virtual void * __thiscall WONAPI::LobbyConfig::`scalar deleting destructor'(unsigned int)    [??_GLobbyConfig@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x00758590
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::basic_string<char,struc)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00757fa0
// (_$E72    [_$E72])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x00758650
// (public: __thiscall std::map<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class WONAPI::SmartPtr<class WONAP)
// TEMPLATE: WON 0x758020
// TEMPLATE: WON 0x758030
// TEMPLATE: WON 0x757f90

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x007695a0
// (public: class std::list<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,class std::allocator<class std::basic_)
// TEMPLATE: WON 0x0076c080
// (protected: void __thiscall std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class s)
// TEMPLATE: WON 0x0076c0c0
// (protected: class std::_Tree<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >,struct std::pair<class std::basic_)
// TEMPLATE: WON 0x00770050
// (public: void * __thiscall std::pair<class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const ,class WONAPI::Smart)
