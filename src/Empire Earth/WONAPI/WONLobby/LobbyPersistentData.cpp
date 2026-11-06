// C:\EEX\Empire Earth\WONAPI\WONLobby\LobbyPersistentData.cpp (RECONSTRUCTION).
// Original filename PROVISIONAL: no filename assertion found. RTTI identifies LobbyPersistentData;
// ctor35bcb0, dtor35c2c0, vtable451c28 and allocation1164 establish its original family.
// complete typed persistence behavior; constructor follows native member/key/default initialization.
// Other unimplemented bodies remain original-carved. The real1164B constructor and implicit
// member cleanup are typed; no external allocation sites or runtime substitution are granted.
// Original body ranges and typed-EH helpers are retained in

// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <exception>
#include <stdio.h>
#include <stdlib.h>
#include "ee/wonapi/WONCRC16.h"
#include <string.h>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include <time.h>
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyModel.h"
#include "ee/wonapi/WONLobbyConfig.h"
#include "ee/wonapi/EELobbySound.h"
#include "ee/wonapi/WONLobbyPersistence.h"
#include "ee/wonapi/WONLobbyParser.h"

namespace WONAPI {
// FUNCTION: WON 0x0075e6e0
std::string LobbyPersistentData::GetUserFileName(long inUserId)
{
    // Stackextent suggests char[50] rounded52 plus returnconstructionstate4.
    // Natural buffer size remains source-shape hypothesis; no matching evidence yet.
    char aFileName[50];
    sprintf(aFileName, "_wonuser%d.dat", inUserId);
    return aFileName;
}
// FUNCTION: WON 0x0075e770
long LobbyPersistentData::FindUnusedUserId()
{
    if (mUserIds.empty())
        return 1;
    std::set<long>::iterator anItr = mUserIds.begin();
    long anId = *anItr;
    while (anItr != mUserIds.end() && anId == *anItr)
    {
        ++anId;
        ++anItr;
    }
    return anId;
}
// FUNCTION: WON 0x0075f6a0
long LobbyPersistentData::ReadFileHeader(FileReader& inReader)
{
    try
    {
        char aHeader[7];
        aHeader[6] = 0;
        inReader.ReadBytes(aHeader, 6);
        if (strcmp(aHeader, "WONPER") == 0)
            return inReader.ReadLong();
    }
    catch (FileReaderException&)
    {
    }
    return 0;
}
// FUNCTION: WON 0x0075f750
void LobbyPersistentData::WriteFileHeader(FileWriter& outWriter, long inVersion)
{
    try
    {
        outWriter.WriteBytes("WONPER", 6);
        outWriter.WriteLong(inVersion);
    }
    catch (FileWriterException&)
    {
    }
}
}

namespace WONAPI {
extern const char gLobbyGlobalPrefsFileName[]; //4d9d14
extern const long gLobbyGlobalPrefsVersion; //4d9550


// Original35baa0 uses key-sensitive ordered set with insertion-order list; no invented container behavior.
// FUNCTION: WON 0x0075baa0
void LobbyOptionSet::Add(const GUIString& inValue)
{
    std::pair<StringMap::iterator, bool> aResult = mIndex.insert(StringMap::value_type(inValue, mValues.end()));
    if (aResult.second)
    {
        mValues.push_back(inValue);
        StringList::iterator anItr = mValues.end();
        --anItr;
        aResult.first->second = anItr;
    }
}
// FUNCTION: WON 0x0075bc20
bool LobbyOptionSet::Erase(const GUIString& inValue)
{
    StringMap::iterator anItr = mIndex.find(inValue);
    if (anItr != mIndex.end())
    {
        mValues.erase(anItr->second);
        mIndex.erase(anItr);
        return true;
    }
    return false;
}

LobbyOptionSet::LobbyOptionSet() {}
// FUNCTION: WON 0x0075ddb0
LobbyOptionSet::~LobbyOptionSet() {}

LobbyPersistentData* GetLobbyPersistentData()
{
    return gLobbyData.get() != NULL ? gLobbyData->mPersistentData.get() : NULL;
}

// Scratch continuation of original LobbyPersistentData family; NOT admitted source.
// Gates: full FileReader20/FileWriter4, real prefix through mUsers8c, GUIStringNoCase,
// Sound::Fn329BF0 + original SoundPtr566770, and explicit globals4d9d14/4d9550.
// Crypto stays original-carved through individually proved member call contracts.
// Required class methods (descriptive address names):
// GUIString LobbyPersistentData::Fn35F000(FileReader&); // plain ushort text, NOT encrypted
// GUIString LobbyPersistentData::Fn35F190(FileReader&); // encrypted text
// void LobbyPersistentData::Fn35F4E0(FileWriter&, const GUIString&); // plain ushort text
// void LobbyPersistentData::Fn35F530(FileWriter&, const GUIString&); // encrypted text
// void LobbyPersistentData::Fn35F7B0(); void LobbyPersistentData::Fn35FAF0();
// extern const char gLobbyGlobalPrefsFileName[]; //4d9d14
// extern const long gLobbyGlobalPrefsVersion; //4d9550
// extern SmartPtr<Sound> ::LobbyGlobal_ButtonClick_Sound; //566770; original no null guard

// Original35f7b0..35fae8 inclusive, body excluding typed6B catch funclet819B.
// FUNCTION: WON 0x0075f7b0
void LobbyPersistentData::Fn35F7B0()
{
    FileReader aReader;
    if (!aReader.Open(gLobbyGlobalPrefsFileName))
        return;
    mUsers.clear();
    mNetworkHandle = 0;
    try
    {
        if (ReadFileHeader(aReader) != gLobbyGlobalPrefsVersion)
            return;
        unsigned short aCount = aReader.ReadShort();
        for (int i = 0; i < aCount; ++i)
        {
            GUIString aName = Fn35F000(aReader);
            LobbyUserInfo& aUser = mUsers.insert(LobbyUserInfoMap::value_type(aName, LobbyUserInfo())).first->second;
            aUser.mUserName = aName;
            aUser.mLastUseTime = aReader.ReadLong();
            aUser.mUserId = aReader.ReadShort();
            aUser.mValue04 = aReader.ReadLong();
            aUser.mValue08 = aReader.ReadLong();
            aUser.mPassword = Fn35F190(aReader);
            mUserIds.insert(aUser.mUserId);
        }
        mB5C = mB6C = aReader.ReadByte() != 0;
        ::LobbyGlobal_ButtonClick_Sound->Fn329BF0(!mB5C);
        mB5D = mB6D = aReader.ReadByte() != 0;
        // Actual numeric IPv4 adapter70 is a long; synchronized Root/GUI declaration.
        mNetworkHandle = aReader.ReadLong();
    }
    catch (FileReaderException&)
    {
    }
    mGlobalDirty = false;
}

// Original35faf0..35fc63 inclusive, body excluding typed6B catch funclet366B.
// FUNCTION: WON 0x0075faf0
void LobbyPersistentData::Fn35FAF0()
{
    FileWriter aWriter;
    if (!aWriter.Open(gLobbyGlobalPrefsFileName, false))
        return;
    try
    {
        WriteFileHeader(aWriter, gLobbyGlobalPrefsVersion);
        aWriter.WriteShort(mUsers.size());
        for (LobbyUserInfoMap::iterator anItr = mUsers.begin(); anItr != mUsers.end(); ++anItr)
        {
            LobbyUserInfo& aUser = anItr->second;
            Fn35F4E0(aWriter, aUser.mUserName);
            aWriter.WriteLong(aUser.mLastUseTime);
            aWriter.WriteShort(aUser.mUserId);
            aWriter.WriteLong(aUser.mValue04);
            aWriter.WriteLong(aUser.mValue08);
            Fn35F530(aWriter, aUser.mPassword);
        }
        aWriter.WriteByte(mB5C ? 1 : 0);
        aWriter.WriteByte(mB5D ? 1 : 0);
        aWriter.WriteLong(mNetworkHandle);
    }
    catch (FileWriterException&)
    {
    }
    // Original updates snapshots even after a swallowed WriterException.
    mB6C = mB5C;
    mB6D = mB5D;
    mGlobalDirty = false;
}


// Scratch only: actual477B35c720, uses only proved persistence prefix through80.
// Required individually original-carved method GUIString Fn35C520() (default LAN name).
// Required GUIString externs566758 and5669e0 (provisional names below).
// FUNCTION: WON 0x0075c720
void LobbyPersistentData::Fn35C720()
{
    mS74 = GetEmptyGUIStringData();
    mS78 = GetEmptyGUIStringData();
    if (mCurrentUserId != 0)
        mS58 = ::LobbyGlobal_QuickPlay_String; //566758
    else
    {
        mS58 = ::LobbyGlobal_Lan_String; //5669e0
        mS68 = Fn35C520();
    }
    mB54 = true;
    mB55 = true;
    mB56 = true;
    mB5C = true;
    mB5D = true;
    mB60 = true;
    mB5E = true;
    mB5F = true;
    mB61 = true;
    mB62 = true;
    mB63 = false;
    mPort66 = 8585;
    mShort64 = 25;
    mNetworkHandle = 0;
}

// ResetFilters35df00 source is deferred until actual mapped LobbyOptionSet copy
// constructor/assignment graph closes. The map stores list iterators; blindly
// generating the default copy could leave iterators into the source list.
// Existing natural Add/Erase candidates require no object copying.

// Scratch only. Requires admitted true WONFile70 and persistence prefix8c.

// FUNCTION: WON 0x0075c910
LobbyUserInfo* LobbyFn35C910(const GUIString& inUserName, const GUIString& inPassword, bool inExistingOnly)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL)
        return NULL;
    if (!inExistingOnly)
        aData->Fn35EB30();
    LobbyUserInfoMap::iterator anItr = aData->mUsers.insert(
        LobbyUserInfoMap::value_type(inUserName, LobbyUserInfo())).first;
    LobbyUserInfo& aUser = anItr->second;
    if (inExistingOnly && aUser.mLastUseTime == 0)
    {
        aData->mUsers.erase(anItr);
        return NULL;
    }
    if (aUser.mUserId == 0)
    {
        aUser.mUserId = aData->FindUnusedUserId();
        aData->mUserIds.insert(aUser.mUserId);
        WONFile aFile(aData->GetUserFileName(aUser.mUserId));
        aFile.Delete(true);
    }
    aUser.mLastUseTime = time(NULL);
    aUser.mUserName = inUserName;
    aUser.mPassword = inPassword;
    aData->mGlobalDirty = true;
    return &aUser;
}

// Original35eb30657B. Comparator36b0c0 signed-key and unconditional insertion
// (equal keys go right, no unique-key rejection) prove time-ordered multi tree.
// Node mapped value is actual LobbyUserInfoMap::iterator, used by mUsers.erase.
// FUNCTION: WON 0x0075eb30
void LobbyPersistentData::Fn35EB30()
{
    if (mUsers.size() <= 50)
        return;
    typedef std::multimap<long, LobbyUserInfoMap::iterator> UserTimeMap;
    UserTimeMap aTimes;
    LobbyUserInfoMap::iterator anItr = mUsers.begin();
    while (anItr != mUsers.end())
    {
        if (anItr->second.mUserId == 0)
            mUsers.erase(anItr++);
        else
        {
            aTimes.insert(UserTimeMap::value_type(anItr->second.mLastUseTime, anItr));
            ++anItr;
        }
    }
    UserTimeMap::iterator aTime = aTimes.begin();
    while (aTime != aTimes.end() && mUsers.size() > 50)
    {
        LobbyUserInfo& aUser = aTime->second->second;
        WONFile aFile(GetUserFileName(aUser.mUserId));
        aFile.Delete(true);
        mUserIds.erase(aUser.mUserId);
        mUsers.erase(aTime->second);
        ++aTime; // original increments before the still-live WONFile destructor
    }
}

// Original friend behavior, scratch only; requires actual complete model LobbyFriend50.
// FUNCTION: WON 0x0075d460
LobbyFriend* LobbyFn35D460(const GUIString& inName, unsigned long inUserId)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL)
        return NULL;
    SmartPtr<LobbyFriend>& aFriend = aData->mFriends.insert(
        LobbyFriendMap::value_type(inUserId, SmartPtr<LobbyFriend>())).first->second;
    if (aFriend.get() == NULL)
    {
        aFriend = new LobbyFriend(inName, inUserId);
        aData->mUserDirty = true;
    }
    return aFriend.get();
}
// Original296B. Preserve assignment: an unsuccessful erase CLEARS old dirty state.
// FUNCTION: WON 0x0075d2c0
void LobbyFn35D2C0(unsigned long inUserId)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData != NULL)
        aData->mUserDirty = aData->mFriends.erase(inUserId) > 0;
}

}

namespace WONAPI {
extern const long gLobbyUserPrefsVersion; //4d9554, root binding gate

// Serialization of the original persistence family; ownership reviewed independently.
// Plain readers retain a bounded unique-owner COW reachability qualification.
// FUNCTION: WON 0x0075ee80
GUIString LobbyPersistentData::Fn35EE80(FileReader& inReader)
{
    unsigned short aLength = inReader.ReadShort();
    GUIString aString(new GUIStringData(aLength));
    for (int i = 0; i < aLength; ++i)
        aString.append((wchar_t)inReader.ReadByte());
    return aString;
}
// FUNCTION: WON 0x0075f000
GUIString LobbyPersistentData::Fn35F000(FileReader& inReader)
{
    unsigned short aLength = inReader.ReadShort();
    GUIString aString(new GUIStringData(aLength));
    for (int i = 0; i < aLength; ++i)
        aString.append((wchar_t)inReader.ReadShort());
    return aString;
}
// FUNCTION: WON 0x0075f3d0
void LobbyPersistentData::Fn35F3D0(FileWriter& outWriter, const GUIString& inText)
{
    std::string aString = (std::string)inText;
    outWriter.WriteShort(aString.length());
    outWriter.WriteBytes(aString.c_str(), aString.length());
}
// FUNCTION: WON 0x0075f4e0
void LobbyPersistentData::Fn35F4E0(FileWriter& outWriter, const GUIString& inText)
{
    outWriter.WriteShort(inText.length());
    for (int i = 0; i < inText.length(); ++i)
        outWriter.WriteShort(inText[i]);
}

// Encrypted text uses the original-carved cipher and natural GUIStringData allocation guards.
// FUNCTION: WON 0x0075f190
GUIString LobbyPersistentData::Fn35F190(FileReader& reader)
{
    unsigned short length = reader.ReadShort();
    WriteBuffer storage(0, true);
    storage.Reserve(length); //2a81e0 declaration gate; grows capacity, does not change length
    reader.ReadBytes(storage.mData, length);
    ByteBufferPtr decrypted = mEncryptKey.Decrypt(storage.mData, length);
    if (decrypted.get() == NULL)
        return GUIString(new GUIStringData(""));
    ReadBuffer decoded(decrypted->data(), decrypted->length());
    try
    {
        std::wstring text;
        decoded.ReadWString(text, 2);
        return GUIString(new GUIStringData(text));
    }
    catch (ReadBufferException&)
    {
        return GUIString(new GUIStringData(""));
    }
}

// FUNCTION: WON 0x0075f530
void LobbyPersistentData::Fn35F530(FileWriter& writer, const GUIString& inText)
{
    WriteBuffer storage(0, true);
    std::wstring text = (std::wstring)inText;
    storage.AppendWString(text, 2);
    ByteBufferPtr encrypted = mEncryptKey.Encrypt(storage.mData, storage.mLength);
    writer.WriteShort(encrypted.get() != NULL ? encrypted->length() : 0);
    writer.WriteBytes(encrypted.get() != NULL ? encrypted->data() : NULL,
                      encrypted.get() != NULL ? encrypted->length() : 0);
}
// User/LAN serialization; complete field declarations grant no full-object lifecycle authority.
// FUNCTION: WON 0x0075fc70
void LobbyPersistentData::Fn35FC70()
{
    Fn35C720();
    mReconnectInfoMap.clear();
    mIgnoredNames.clear();
    mFriends.clear();
    mLanNameInfoMap.clear();
    mDirectConnectMap.clear();
    mGameOptions.clear();
    bool haveGameOptions = false;
    int i;
    FileReader reader;
    std::string fileName = GetUserFileName(mCurrentUserId);
    if (!reader.Open(fileName.c_str()))
    {
        mGameOptions = mDefaultGameOptions;
        return;
    }
    try
    {
        if (ReadFileHeader(reader) != gLobbyUserPrefsVersion)
            return;
        if (mCurrentUserId == 0)
            Fn360B10(reader);
        unsigned short count = reader.ReadShort();
        for (i = 0; i < count; ++i)
        {
            unsigned char bytes[6];
            reader.ReadBytes(bytes, 6);
            IPAddr address;
            address.SetSixByte(bytes);
            LobbyReconnectInfo& info = mReconnectInfoMap.insert(
                std::map<IPAddr, LobbyReconnectInfo>::value_type(address, LobbyReconnectInfo())).first->second;
            info.mLastUseTime = reader.ReadLong();
            info.mReconnectId = reader.ReadLong();
        }
        count = reader.ReadShort();
        for (i = 0; i < count; ++i)
            mIgnoredNames.insert(Fn35F000(reader));
        count = reader.ReadShort();
        for (i = 0; i < count; ++i)
        {
            std::wstring name = (std::wstring)Fn35F000(reader);
            unsigned long userId = reader.ReadLong();
            SmartPtr<LobbyFriend> aFriend = new LobbyFriend(GUIString(new GUIStringData(name)), userId);
            mFriends.insert(LobbyFriendMap::value_type(userId, aFriend));
        }
        count = reader.ReadShort();
        for (i = 0; i < count; ++i)
            mIgnoredUserIds.insert(reader.ReadLong());
        mB54 = reader.ReadByte() == 1;
        mB55 = reader.ReadByte() == 1;
        mB56 = reader.ReadByte() == 1;
        mB61 = reader.ReadByte() == 1;
        mB63 = reader.ReadByte() == 1;
        mB62 = reader.ReadByte() == 1;
        mS58 = Fn35F000(reader);
        mS74 = Fn35F000(reader);
        mS78 = Fn35F000(reader);
        mB5C = reader.ReadByte() == 1;
        mB5D = reader.ReadByte() == 1;
        mB5E = reader.ReadByte() == 1;
        mB5F = reader.ReadByte() == 1;
        mB60 = reader.ReadByte() == 1;
        mShort64 = reader.ReadShort();
        count = reader.ReadShort();
        haveGameOptions = true;
        for (i = 0; i < count; ++i)
        {
            GUIString optionName = Fn35EE80(reader);
            LobbyOptionSet& option = mGameOptions[optionName];
            unsigned short numValues = reader.ReadShort();
            for (int j = 0; j < numValues; ++j)
                option.Add(Fn35F000(reader));
        }
    }
    catch (FileReaderException&) {}
    if (!haveGameOptions)
        mGameOptions = mDefaultGameOptions;
    mGlobalDirty = mB5C != mB6C || mB5D != mB6D;
    mUserDirty = false;
}

// FUNCTION: WON 0x00760b10
void LobbyPersistentData::Fn360B10(FileReader& reader)
{
    int count = reader.ReadShort();
    for (int nameIndex = 0; nameIndex < count; ++nameIndex)
    {
        GUIString name = Fn35F000(reader);
        long lastUseTime = reader.ReadLong();
        mLanNameInfoMap[name] = lastUseTime;
    }
    Fn35EA40(); // existing original-carved LAN-name pruning owner
    count = reader.ReadShort();
    for (int addressIndex = 0; addressIndex < count; ++addressIndex)
    {
        IPAddr address;
        unsigned char bytes[6];
        reader.ReadBytes(bytes, 6);
        address.SetSixByte(bytes);
        GUIString name = Fn35F000(reader);
        mDirectConnectMap[address] = name;
    }
    mPort66 = reader.ReadShort();
}

// FUNCTION: WON 0x00760da0
void LobbyPersistentData::Fn360DA0(FileWriter& writer)
{
    writer.WriteShort((unsigned short)mLanNameInfoMap.size());
    for (std::map<GUIString, long, GUIStringLessNoCase>::const_iterator i = mLanNameInfoMap.begin(); i != mLanNameInfoMap.end(); ++i)
    {
        Fn35F4E0(writer, i->first);
        writer.WriteLong(i->second);
    }
    writer.WriteShort((unsigned short)mDirectConnectMap.size());
    for (std::map<IPAddr, GUIString>::const_iterator j = mDirectConnectMap.begin(); j != mDirectConnectMap.end(); ++j)
    {
        writer.WriteBytes(j->first.GetSixByte(), 6);
        Fn35F4E0(writer, j->second);
    }
    writer.WriteShort(mPort66);
}
// FUNCTION: WON 0x00760770
void LobbyPersistentData::Fn360770()
{
    FileWriter writer;
    std::string fileName = GetUserFileName(mCurrentUserId);
    if (!writer.Open(fileName.c_str(), false))
        return;
    try
    {
        WriteFileHeader(writer, gLobbyUserPrefsVersion); //4d9554; extern gate pending
        if (mCurrentUserId == 0)
            Fn360DA0(writer);
        writer.WriteShort((unsigned short)mReconnectInfoMap.size());
        for (std::map<IPAddr, LobbyReconnectInfo>::const_iterator a = mReconnectInfoMap.begin(); a != mReconnectInfoMap.end(); ++a)
        {
            writer.WriteBytes(a->first.GetSixByte(), 6);
            writer.WriteLong(a->second.mLastUseTime);
            writer.WriteLong(a->second.mReconnectId);
        }
        writer.WriteShort((unsigned short)mIgnoredNames.size());
        for (LobbyIgnoredNameSet::const_iterator n = mIgnoredNames.begin(); n != mIgnoredNames.end(); ++n)
            Fn35F4E0(writer, *n);
        writer.WriteShort((unsigned short)mFriends.size());
        for (LobbyFriendMap::const_iterator f = mFriends.begin(); f != mFriends.end(); ++f)
        {
            Fn35F4E0(writer, f->second->mName);
            writer.WriteLong(f->first);
        }
        writer.WriteShort((unsigned short)mIgnoredUserIds.size());
        for (std::set<unsigned long>::const_iterator u = mIgnoredUserIds.begin(); u != mIgnoredUserIds.end(); ++u)
            writer.WriteLong(*u);
        writer.WriteByte(mB54 ? 1 : 0);
        writer.WriteByte(mB55 ? 1 : 0);
        writer.WriteByte(mB56 ? 1 : 0);
        writer.WriteByte(mB61 ? 1 : 0);
        writer.WriteByte(mB63 ? 1 : 0);
        writer.WriteByte(mB62 ? 1 : 0);
        Fn35F4E0(writer, mS58);
        Fn35F4E0(writer, mS74);
        Fn35F4E0(writer, mS78);
        writer.WriteByte(mB5C ? 1 : 0);
        writer.WriteByte(mB5D ? 1 : 0);
        writer.WriteByte(mB5E ? 1 : 0);
        writer.WriteByte(mB5F ? 1 : 0);
        writer.WriteByte(mB60 ? 1 : 0);
        writer.WriteShort(mShort64);
        writer.WriteShort((unsigned short)mGameOptions.size());
        for (std::map<GUIString, LobbyOptionSet, GUIStringLessNoCase>::const_iterator o = mGameOptions.begin(); o != mGameOptions.end(); ++o)
        {
            Fn35F3D0(writer, o->first);
            // Observed wire quirk: empty option index omits even the zero count; iteration still uses the list.
            if (!o->second.mIndex.empty())
            {
                writer.WriteShort((unsigned short)o->second.mIndex.size());
                for (LobbyOptionSet::StringList::const_iterator v = o->second.mValues.begin(); v != o->second.mValues.end(); ++v)
                    Fn35F4E0(writer, *v);
            }
        }
    }
    catch (FileWriterException&) {}
    mUserDirty = false;
}
}

namespace WONAPI {
// FUNCTION: WON 0x0075d940
void LobbyFn35D940(const IPAddr& inAddress, int inReconnectId)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL)
        return;
    LobbyReconnectInfo& anInfo = aData->mReconnectInfoMap.insert(
        std::map<IPAddr, LobbyReconnectInfo>::value_type(inAddress, LobbyReconnectInfo())).first->second;
    anInfo.mLastUseTime = time(NULL);
    anInfo.mReconnectId = inReconnectId;
    aData->Fn360770();
}
// FUNCTION: WON 0x0075db00
int LobbyFn35DB00(const IPAddr& inAddress)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL)
        return 0;
    std::map<IPAddr, LobbyReconnectInfo>::iterator anItr = aData->mReconnectInfoMap.find(inAddress);
    return anItr == aData->mReconnectInfoMap.end() ? 0 : anItr->second.mReconnectId;
}
// FUNCTION: WON 0x0075e040
void LobbyAddGameOption(const GUIString& inName, const GUIString& inValues, bool inAppend)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL)
        return;
    LobbyOptionSet& anOptions = aData->mDefaultGameOptions.insert(
        std::map<GUIString, LobbyOptionSet, GUIStringLessNoCase>::value_type(inName, LobbyOptionSet())).first->second;
    if (!anOptions.mValues.empty() && !inAppend)
        return;
    std::wstring aValues = inValues;
    StringParser aParser(aValues.c_str());
    do {
        std::wstring aValue;
        aParser.ReadString(aValue, false);
        if (aValue.empty())
            break;
        anOptions.Add(GUIString(new GUIStringData(aValue)));
    } while (aParser.CheckNextChar(','));
}
}

namespace WONAPI {
// FUNCTION: WON 0x0075d100
void LobbyFn35D100(const GUIString& inName, bool inIgnore)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL) return;
    if (inIgnore)
        aData->mUserDirty = aData->mIgnoredNames.insert(inName).second;
    else
        aData->mUserDirty = aData->mIgnoredNames.erase(inName) > 0;
}
// FUNCTION: WON 0x0075d6c0
void LobbyFn35D6C0(const GUIString& inName)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL || inName.mData->mLength == 0 ||
        inName.mData->Compare(aData->mS68.mData.get()) == 0) return;
    long aTime = time(NULL);
    aData->mLanNameInfoMap[inName] = aTime;
    aData->mS68 = inName;
    aData->Fn35E7E0();
    aData->mUserDirty = true;
}
// FUNCTION: WON 0x0075d770
void LobbyFn35D770()
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL) return;
    LobbyLanNameInfoMap::iterator anItr = aData->mLanNameInfoMap.find(aData->mS68);
    if (anItr != aData->mLanNameInfoMap.end()) {
        aData->mLanNameInfoMap.erase(anItr);
        aData->Fn35EA40();
        aData->mUserDirty = true;
    }
}
// FUNCTION: WON 0x0075d7f0
void LobbyFn35D7F0(const IPAddr& inAddress, const GUIString& inName)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL) return;
    aData->mDirectConnectMap[inAddress] = inName;
    aData->mUserDirty = true;
}
// FUNCTION: WON 0x0075d870
void LobbyFn35D870(const IPAddr& inAddress)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL) return;
    LobbyDirectConnectMap::iterator anItr = aData->mDirectConnectMap.find(inAddress);
    if (anItr != aData->mDirectConnectMap.end()) {
        aData->mDirectConnectMap.erase(anItr);
        aData->mUserDirty = true;
    }
}
// FUNCTION: WON 0x0075db70
void LobbyFn35DB70(const GUIString& inServer, const GUIString& inGroup)
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL) return;
    if (inServer.mData->Compare(aData->mS74.mData.get()) == 0 &&
        inGroup.mData->Compare(aData->mS78.mData.get()) == 0) return;
    aData->mS74 = inServer;
    aData->mS78 = inGroup;
    aData->mUserDirty = true;
}
// FUNCTION: WON 0x0075df00
void LobbyFn35DF00()
{
    LobbyPersistentData* aData = GetLobbyPersistentData();
    if (aData == NULL) return;
    // Native performs ordinary memberwise copying, including stored list iterators.
    aData->mGameOptions = aData->mDefaultGameOptions;
    aData->mUserDirty = true;
}
// FUNCTION: WON 0x0075e7e0
void LobbyPersistentData::Fn35E7E0()
{
    if (mLanNameInfoMap.size() <= 50) return;
    typedef std::multimap<long, LobbyLanNameInfoMap::iterator> TimeMap;
    TimeMap aTimes;
    for (LobbyLanNameInfoMap::iterator anItr = mLanNameInfoMap.begin();
         anItr != mLanNameInfoMap.end(); ++anItr)
        aTimes.insert(TimeMap::value_type(anItr->second, anItr));
    for (TimeMap::iterator aTime = aTimes.begin();
         aTime != aTimes.end() && mLanNameInfoMap.size() > 50; ++aTime)
        mLanNameInfoMap.erase(aTime->second);
}
// FUNCTION: WON 0x0075ea40
void LobbyPersistentData::Fn35EA40()
{
    mS68 = Fn35C520();
    long aLastTime = 0;
    for (LobbyLanNameInfoMap::iterator anItr = mLanNameInfoMap.begin();
         anItr != mLanNameInfoMap.end(); ++anItr) {
        if (aLastTime == 0 || anItr->second > aLastTime) {
            mS68 = anItr->first;
            aLastTime = anItr->second;
        }
    }
}
}

namespace WONAPI {
// FUNCTION: WON 0x0075c520
GUIString LobbyPersistentData::Fn35C520()
{
    GUIString aName = ::LobbyOptions_DefaultName_String;
    LobbyConfig* aConfig = GetLobbyConfig();
    if (aConfig != NULL && !aConfig->mAppendNumberToDefaultLanName)
        return aName;
    long anAddress = IPAddr::GetLocalAddr().Fn2B2FB0();
    CRC16 aCRC;
    aCRC.Put(&anAddress,4);
    char aNumber[256];
    _itoa(aCRC.Get() % 10000, aNumber, 10);
    aName.append(GUIString(new GUIStringData(aNumber)));
    return aName;
}
}

namespace WONAPI {
// FUNCTION: WON 0x0075bcb0
LobbyPersistentData::LobbyPersistentData()
{
    WriteBuffer aKey(0,true);
    aKey.Reserve(8);
    aKey.AppendLong(0x22fa655f);
    aKey.AppendLong(CDKey::GetMachineId(NULL));
    mEncryptKey.SetKey(aKey.mData,aKey.mLength);
    // Native deliberately leaves mCurrentUserId untouched before this reset.
    Fn35C720();
    mPort66 = 8585;
    mGlobalDirty = false;
    mUserDirty = false;
    mB6C = true;
    mB6D = true;
}
}

// SOURCE_ONLY_NO_NATIVE_ID: exact same-SDK ServerListFilterSet::clear,
// using the established canonical LobbyOptionSet owner/member names.
namespace WONAPI {
void LobbyOptionSet::clear()
{
    mIndex.clear();
    mValues.clear();
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0075c140
// (public: __thiscall std::map<unsigned long,class WONAPI::SmartPtr<class WONAPI::LobbyFriend>,struct std::less<unsigned long>,class std::allocator<struc)
// TEMPLATE: WON 0x0075c900
// (class WONAPI::LobbyPersistentData * __cdecl WONAPI::GetLobbyPersistentData(void)    [?GetLobbyPersistentData@WONAPI@@YAPAVLobbyPersistentData@1@XZ])
// TEMPLATE: WON 0x0075cc90
// (public: __thiscall WONAPI::LobbyUserInfo::~LobbyUserInfo(void)    [??1LobbyUserInfo@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x0075cd20
// (public: __thiscall std::pair<class WONAPI::GUIString const ,struct WONAPI::LobbyUserInfo>::~pair<class WONAPI::GUIString const ,struct WONAPI::LobbyUs)
// TEMPLATE: WON 0x0075dd20
// (public: __thiscall WONAPI::LobbyOptionSet::LobbyOptionSet(void)    [??0LobbyOptionSet@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0075bfb0
// (public: virtual void * __thiscall WONAPI::LobbyPersistentData::`scalar deleting destructor'(unsigned int)    [??_GLobbyPersistentData@WONAPI@@UAEPAXI@)
// TEMPLATE: WON 0x0075c2c0
// (public: virtual __thiscall WONAPI::LobbyPersistentData::~LobbyPersistentData(void)    [??1LobbyPersistentData@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0075c200
// (public: __thiscall std::map<class WONAPI::GUIString,class WONAPI::LobbyOptionSet,struct WONAPI::GUIStringLessNoCase,class std::allocator<struct std::p)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0075bfd0
// (public: __thiscall std::set<class WONAPI::GUIString,struct WONAPI::GUIStringLessNoCase,class std::allocator<class WONAPI::GUIString> >::~set<class WON)
// TEMPLATE: WON 0x0075e990
// (public: __thiscall std::multimap<long,class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,long>,struct std::_Mapk)
// TEMPLATE: WON 0x0075edd0
// (public: __thiscall std::multimap<long,class std::_Tree<class WONAPI::GUIString,struct std::pair<class WONAPI::GUIString const ,struct WONAPI::LobbyUse)
// TEMPLATE: WON 0x00760610
// (public: class std::map<class WONAPI::GUIString,class WONAPI::LobbyOptionSet,struct WONAPI::GUIStringLessNoCase,class std::allocator<struct std::pair<c)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0075bf00
// (public: __thiscall std::set<long,struct std::less<long>,class std::allocator<long> >::~set<long,struct std::less<long>,class std::allocator<long> >(vo)
