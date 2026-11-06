// Original-backed AuthBase owned raw bytes, unpack dispatch and expiry.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONAuthCertificate.h"
namespace WONAPI {
// FUNCTION: WON 0x006dd1d0
Auth2Certificate::~Auth2Certificate() {}
// Original code2dd6b0..2dda46 =918B; movedi,edi padding2 and switchtable16 excluded.
// FUNCTION: WON 0x006dd6b0
bool Auth2Certificate::UnpackHook(ReadBuffer& input)
{
    if (!AuthBase::UnpackHook(input))
        return false;
    unsigned short count = input.ReadShort();
    for (int i=0; i<count; ++i) {
        unsigned short type = input.ReadShort();
        unsigned short length = input.ReadShort();
        int start = input.mL00;
        switch (type) {
        case 0: {
            mUserId = input.ReadLong();
            unsigned short keyLength = input.ReadShort();
            if (!mPubKey.SetPublicKey(input.ReadBytes(keyLength), keyLength))
                return false;
            input.ReadWString(mUserName, 2);
            unsigned char communities = input.ReadByte();
            for (int j=0; j<communities; ++j) {
                DWORD id = input.ReadLong();
                unsigned short& trust = mCommunityTrustMap[id];
                trust = input.ReadShort();
            }
            break;
        }
        case 1: {
            DWORD id = input.ReadLong();
            mUserDataMap[id] = input.ReadBuf(2);
            break;
        }
        case 2: {
            std::wstring key, value;
            input.ReadWString(key, 2);
            input.ReadWString(value, 2);
            mNicknameMap[key] = value;
            break;
        }
        case 3: {
            DWORD id = input.ReadLong();
            DWORD keyId = input.ReadLong();
            mKeyIdMap[id] = keyId;
            break;
        }
        }
        input.ReadBytes(length - (input.mL00 - start));
    }
    return true;
}
// FUNCTION: WON 0x006dda60
bool Auth2Certificate::VerifyPermissions(DWORD userId, DWORD community, unsigned short minTrust) const
{
    if (userId != 0 && mUserId != userId)
        return false;
    if (community != 0) {
        CommunityTrustMap::const_iterator found = mCommunityTrustMap.find(community);
        if (found == mCommunityTrustMap.end())
            return false;
        return found->second >= minTrust;
    }
    CommunityTrustMap::const_iterator next = mCommunityTrustMap.begin();
    while (next != mCommunityTrustMap.end()) {
        if (next->second >= minTrust)
            return true;
        ++next;
    }
    return false;
}
// FUNCTION: WON 0x006ddb50
DWORD Auth2Certificate::GetFirstCommunity() const
{
    if(mCommunityTrustMap.empty()) return 0;
    return mCommunityTrustMap.begin()->first;
}
}

namespace WONAPI {

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
std::wstring Auth2Certificate::GetNickname(const std::wstring &theKey) const
{
	NicknameMap::const_iterator anItr = mNicknameMap.find(theKey);
	if(anItr==mNicknameMap.end())
		return L"";
	else
		return anItr->second;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
ByteBufferPtr Auth2Certificate::GetUserData(DWORD theCommunityId) const
{
	UserDataMap::const_iterator anItr = mUserDataMap.find(theCommunityId);
	if(anItr==mUserDataMap.end())
		return NULL;
	else
		return anItr->second;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
DWORD Auth2Certificate::GetKeyId(DWORD theCommunityId) const
{
	KeyIdMap::const_iterator anItr = mKeyIdMap.find(theCommunityId);
	if(anItr==mKeyIdMap.end())
		return 0;
	else
		return anItr->second;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
std::wstring Auth2Certificate::GetFirstNickname() const
{
	if (mNicknameMap.empty())
		return L"";
	else
		return GetNickname(mNicknameMap.begin()->second);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
unsigned short Auth2Certificate::GetFirstCommunityTrust() const
{
	if (mCommunityTrustMap.empty())
		return 0;
	else
		return (mCommunityTrustMap.begin()->second);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
ByteBufferPtr Auth2Certificate::GetFirstUserData() const
{
	if (mUserDataMap.empty())
		return NULL;
	else
		return (mUserDataMap.begin()->second);
}
}

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006dd520
// (protected: virtual void * __thiscall WONAPI::Auth2Certificate::`scalar deleting destructor'(unsigned int)    [??_GAuth2Certificate@WONAPI@@MAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006dd540
// (public: __thiscall std::map<unsigned long,unsigned short,struct std::less<unsigned long>,class std::allocator<struct std::pair<unsigned long const ,un)
// TEMPLATE: WON 0x006dd5f0
// (public: __thiscall std::map<unsigned long,class WONAPI::SmartPtr<class WONAPI::ByteBuffer>,struct std::less<unsigned long>,class std::allocator<struct)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006dd3d0
// (public: virtual __thiscall WONAPI::AuthCertificateBase::~AuthCertificateBase(void)    [??1AuthCertificateBase@WONAPI@@UAE@XZ])

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ca5a0
// (public: void __thiscall std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned short,)
// TEMPLATE: WON 0x006cb600
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned short,struct std)
// TEMPLATE: WON 0x006cbab0
// (protected: void __thiscall std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned sho)
// TEMPLATE: WON 0x006dad30
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006db900
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::ByteBuffer> >,struct std::_Mapk<unsi)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ea130
// (public: __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > con)
// TEMPLATE: WON 0x006eaf60
// (public: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short>)
// TEMPLATE: WON 0x006ecfa0
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::ByteBuffer> >,struct std::_Mapk<unsi)
// TEMPLATE: WON 0x006ef830
// (public: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short>)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006ebf40
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned short,struct std)
// TEMPLATE: WON 0x006ef940
// (public: void __thiscall std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned short,)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006e9760
// (public: __thiscall std::map<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,clas)
// TEMPLATE: WON 0x006eaa30
// (public: struct std::pair<class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned)
// TEMPLATE: WON 0x006eab20
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned short,struct std)
// TEMPLATE: WON 0x006eac20
// (public: struct std::pair<class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned)
// TEMPLATE: WON 0x006ead20
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006eafb0
// (public: struct std::pair<class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::ByteBuffer> >,struc)
// TEMPLATE: WON 0x006eb0a0
// (public: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::ByteBuffer> >,struct std::_Mapk<unsi)
// TEMPLATE: WON 0x006ec3f0
// (protected: void __thiscall std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned sho)
// TEMPLATE: WON 0x006ec430
// (protected: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned short,struct )
// TEMPLATE: WON 0x006ec730
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006ecc60
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006ecca0
// (protected: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,stru)
// TEMPLATE: WON 0x006ed490
// (protected: void __thiscall std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::ByteBuffer> >,struct st)
// TEMPLATE: WON 0x006ed4d0
// (protected: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,class WONAPI::SmartPtr<class WONAPI::ByteBuffer> >,struct std::_Mapk<u)
// TEMPLATE: WON 0x006efe10
// (void __cdecl std::_Construct(struct std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsi)
// TEMPLATE: WON 0x006f0070
// (public: void * __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006eadd0
// (public: __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > con)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x0076f3c0
// (protected: class std::_Tree<unsigned long,struct std::pair<unsigned long const ,unsigned short>,struct std::_Mapk<unsigned long,unsigned short,struct )
