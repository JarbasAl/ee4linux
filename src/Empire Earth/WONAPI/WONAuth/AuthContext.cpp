// Original-backed borrowed AuthContext session cache; no complete object allocation/lifetime authority.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <string>
#include <list>
#include <set>
#include <map>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAuthPeerData.h"
#include "ee/wonapi/WONMD5Digest.h"
#include "ee/wonapi/WONMD5.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
// SUPPORT: EXE 0x006ddda0 (50B lazy critical-section getter, quota0)
// Genuine file-local SDK identity; generated atexit cleanup2ddde0/12 also quota0.
static WONAPI::CriticalSection& GetVerifierCrit()
{
    static WONAPI::CriticalSection verifierCritical;
    return verifierCritical;
}

// Genuine SDK file-static public verifier bytes, exactly original4d57ac/817B.
static char VerifierKeyBuf[] = 
	"\x30\x82\x03\x2c\x02\x82\x01\x01\x00\x86\x18\x0e\x3d\xd4\x9e\xfa\x42\xb9\x4c\xb3\x3e"
	"\xf7\x2b\x76\xce\xe6\xb0\x27\x8c\x64\x07\x93\x3b\x24\x64\x6c\x47\xb6\x9e\x51\x7b\xbf"
	"\xc9\xd5\xa1\xfb\xcf\x04\xfa\x7b\x8e\xc3\x6e\xd1\x88\xf6\x2e\xbd\x2b\x29\xda\xb5\x76"
	"\x66\x12\xb9\x7a\x64\x73\x19\x53\xa9\x78\xd8\x1e\xe5\x11\x0a\x23\x1c\x08\x21\x33\xad"
	"\x65\x76\x48\xff\xc5\x0b\xa2\xaa\x10\x22\xcb\x63\xbb\x68\xfc\xf0\x15\xf3\xe9\x90\x5f"
	"\x9d\x17\x8e\x77\x67\xec\xde\xdf\x8b\xcc\xbb\xa4\xd6\x06\xfa\x0e\xd8\xc0\x67\xd4\x36"
	"\xc9\x3a\xeb\x3f\xa4\x93\x57\xad\x62\x29\x24\x08\x67\x91\x6a\x0b\xd4\xdc\xad\xa4\x86"
	"\x85\x9f\xed\xf8\xb8\xda\xe1\xac\xf3\xf4\x44\xaf\x85\x8d\x21\x95\x6a\xf4\x6e\xac\x36"
	"\x83\x8f\x70\x0a\xc6\x17\xc1\x90\xc4\x7f\xbb\xe8\x18\xed\x4d\x0b\x8c\xdc\x76\x7f\xa9"
	"\xb2\x45\x0f\xc6\xa8\xca\x0a\x9f\xec\x9f\xe5\xe0\xbd\x71\x63\x06\xef\x32\xba\xe0\x40"
	"\x47\xf1\x91\xa8\xbf\x5e\xad\xb8\x2d\x28\x3a\x94\x63\xfa\x85\x1d\x24\xe3\xd2\x78\x7f"
	"\x0e\x85\xb1\xbb\x3f\x2d\x27\x16\x34\xcf\x94\x7d\x71\xdd\xf3\xec\x41\xc1\x27\xe5\x6f"
	"\xe6\x88\x78\xc8\x65\x15\x45\x3c\x0c\x5f\x30\x60\xc7\x02\x1d\x02\xe1\x4f\x25\x13\x97"
	"\xee\xdb\x46\x94\xb2\x54\xfc\x54\x26\x20\xa2\x6d\x57\x72\xdd\x47\xd2\xf0\x92\x00\x38"
	"\x51\xad\x02\x82\x01\x00\x72\x44\x9d\x49\x40\xff\x84\x75\xd8\x73\x57\x1f\xa7\xff\x52"
	"\x08\x82\x63\xfa\xd1\xa7\x30\xb8\x63\xc5\x10\x45\x8e\x12\xca\x9c\x36\x4f\xc8\x3e\xe8"
	"\x9a\x95\xfc\x48\xae\xcf\x80\xea\x75\xf6\x11\x3b\x2a\xf0\xd9\x52\xfb\x66\xee\x4b\xdb"
	"\x6c\xc4\x06\x28\x12\x28\xd2\x2c\x53\x3f\x08\x86\x89\x69\x6a\x89\x57\x7e\xb0\xd7\x81"
	"\xac\x79\xae\xfb\x56\xc0\x24\x0d\x84\x5a\xe1\x22\xc8\x3f\xba\x3c\x77\xbc\x22\x56\xdf"
	"\xf1\x78\x55\xcc\xd5\x97\x88\x00\x71\x1a\xe8\x8e\xa7\xb6\x87\x62\x55\x4d\x53\x87\xa2"
	"\x49\x3e\xa2\x79\x99\x51\x94\xd7\xe2\xf5\xce\x37\x1e\xc7\x79\xbd\xea\x0a\x31\x56\x1d"
	"\x56\x1a\x8c\xb7\xf3\xa5\xdc\x2a\xcd\xdc\xb1\x5c\x3e\x47\x20\xc7\xc2\x30\x32\xdf\x64"
	"\x91\xd5\xc6\xab\x85\x21\x67\x59\xc6\x09\x7c\x61\xd5\x90\x9b\xff\x5c\x6b\x48\x5e\x41"
	"\x85\xdb\x12\xed\x63\x04\xd3\x35\xca\x86\xce\x64\xa6\x54\x5b\x0f\x77\x90\x7e\xad\xd9"
	"\x56\x5d\x2f\x8a\x52\xd9\xcc\x79\x41\x69\x73\xca\x15\x0a\xbc\x68\x93\xbe\x2f\x03\x7c"
	"\x60\x7b\xf2\xa3\x03\x98\x34\x36\x9f\x88\x8e\xfb\x2b\x8a\x40\xdd\x35\xa9\x21\x2d\x92"
	"\xba\x1b\xe8\x58\xe1\x1c\xc2\x47\x34\x9c\x02\x82\x01\x00\x7f\xb4\x68\x1d\xde\x12\xd3"
	"\x00\x3a\x26\xb6\x60\x7e\xb9\x62\x9f\x31\xcb\x6b\x3a\x4f\x0e\x18\x64\x4f\x65\xad\x79"
	"\x1d\xd7\xe8\xbf\x83\x68\x78\xc4\x9e\x2b\x90\xcd\xd8\xad\x36\x1a\x1d\x4f\x56\x4b\x8a"
	"\x6f\xc3\x77\x93\x5e\xf4\x7d\x61\xfd\x48\xa5\xa5\xbe\x27\xa3\x5d\xa9\x0a\xb0\x77\xc9"
	"\x6d\x8d\xae\x94\x5a\x90\x07\xb4\x96\x1b\xda\x93\x56\xc5\x50\x19\xdd\xa4\xb3\x40\x66"
	"\x0b\xb0\x55\xdd\xd7\x9a\x50\x7b\x9a\xe7\xbc\x63\x65\x9b\x45\x2d\x82\xf3\xe7\x49\x19"
	"\x71\x10\x9d\x46\x2e\x82\x8e\x15\x35\xf8\x53\x99\x3f\x4f\xec\x83\x48\xef\x0f\x27\x28"
	"\x75\x1f\x73\x97\x1e\x14\x16\x24\x16\xe6\x3f\x67\xdc\x50\x4a\xdc\x83\x89\x47\xf9\x41"
	"\xa5\xec\xe1\x86\x08\x46\xf4\xa9\x97\x14\x73\xaf\xe6\x01\x30\xa1\xad\x2f\xb0\x2a\x35"
	"\xdd\xad\x1d\x70\x25\x95\xbf\x07\xd2\x8c\x14\x46\xc4\xd2\x79\x21\xdc\xa9\x21\x6e\xff"
	"\x99\x69\x47\x8b\x67\x95\x98\x4d\x09\x75\xee\x97\x0b\xd9\x3d\x75\xd1\xa8\x82\x1f\xc8"
	"\x15\x2e\x39\x29\x6f\x95\x4b\xf4\x94\x74\xeb\xd8\xe3\x4a\xfb\x41\xa8\x4c\x5c\x3d\xb6"
	"\x42\xc0\x15\x35\x9e\x1a\xe2\x54\x2c\x0c\x0b\x34\x5f\xe2\x35\x8d\x30\x73";
namespace WONAPI {
// Genuine existing class-static owners, native566018/1a8 and5661e1/1.
ElGamal AuthContext::mVerifierKey(VerifierKeyBuf,sizeof(VerifierKeyBuf),false);
bool AuthContext::mCheckedVerifierFile=false;

// FUNCTION: WON 0x006dddf0
AuthContext::AuthContext()
{
    Init();
}
// FUNCTION: WON 0x006de370
AuthContext::~AuthContext()
{
}

// FUNCTION: WON 0x006deeb0
void AuthContext::PutSession(const IPAddr& address,AuthSession* session)
{
    AutoCrit lock(mDataCrit);
    if(session==NULL) return;
    SessionStruct& record=mSessionMap[address];
    time_t now=time(NULL);
    record.mLastUseTime=now;
    record.mSessionList.push_back(session);
    for(int i=0;i<5;++i) {
        if(mSessionCleanItr==mSessionMap.end()) {
            mSessionCleanItr=mSessionMap.begin();
            break;
        }
        if(now-mSessionCleanItr->second.mLastUseTime>300)
            mSessionMap.erase(mSessionCleanItr++);
        else ++mSessionCleanItr;
    }
}
// FUNCTION: WON 0x006df080
SmartPtr<AuthSession> AuthContext::GetSession(const IPAddr& address)
{
    AutoCrit lock(mDataCrit);
    SessionMap::iterator found=mSessionMap.find(address);
    if(found==mSessionMap.end()) return NULL;
    time_t now=time(NULL);
    SessionStruct& record=found->second;
    record.mLastUseTime=now;
    SessionList& sessions=record.mSessionList;
    SessionList::iterator next=sessions.begin();
    while(next!=sessions.end()) {
        SmartPtr<AuthSession> session=*next;
        sessions.erase(next++);
        if(now-session->GetLastUseTime()<300) return session;
    }
    return NULL;
}
// Additional bounded borrowed AuthContext lifecycle; no GetCertOp allocation.
// FUNCTION: WON 0x006de7f0
void AuthContext::AutoRefreshCallback(AsyncOp* op)
{
    AutoCrit lock(mDataCrit);
    if(op!=mAutoRefreshOp.get()) return;
    mAutoRefreshOp=NULL;
    if(op->mB39) return;
    RefreshAsync(NULL,INFINITE);
}
// FUNCTION: WON 0x006de8e0
void AuthContext::StopAutoRefresh()
{
    AutoCrit lock(mDataCrit);
    if(mAutoRefreshOp.get()!=NULL) {
        mAutoRefreshOp->Kill(true);
        mAutoRefreshOp=NULL;
    }
}
// FUNCTION: WON 0x006deb30
void AuthContext::Fn2DEB30(bool enabled)
{
    AutoCrit lock(mDataCrit);
    mDoAutoRefresh=enabled;
    StartAutoRefresh(false);
}
// FUNCTION: WON 0x006df260
void AuthContext::Kill()
{
    AutoCrit lock(mDataCrit);
    if(mGetCertOp.get()!=NULL) mGetCertOp->Kill(true);
    StopAutoRefresh();
}

// FUNCTION: WON 0x006de760
void AuthContext::StaticAutoRefreshCallback(SmartPtr<AsyncOp> op,SmartPtr<RefCount> parameter)
{
    static_cast<AuthContext*>(parameter.get())->AutoRefreshCallback(op.get());
}
// FUNCTION: WON 0x006de970
void AuthContext::StartAutoRefresh(bool force)
{
    AutoCrit lock(mDataCrit);
    StopAutoRefresh();
    if(mDoAutoRefresh) {
        bool refreshTimer=force;
        time_t wait=300;
        if(mPeerData->IsValid()) {
            wait=mPeerData->GetExpireDelta()-120;
            if(wait<120) wait=120;
            refreshTimer=true;
        }
        if(refreshTimer) {
            mAutoRefreshOp=new AsyncOp;
            mAutoRefreshOp->mCompletion=
                new ParamCompletion<SmartPtr<AsyncOp>,SmartPtr<RefCount> >(&AuthContext::StaticAutoRefreshCallback,this);
            mAutoRefreshOp->Run(static_cast<long>(static_cast<unsigned long>(wait)*1000UL));
        }
    }
}

// Genuine SDK empty mapped-value ctor, actual complete owners only; support0, no standalone quota.
AuthLoginCommunityData::AuthLoginCommunityData() {}
inline AuthContext::SecretData::SecretData() : mExpireTime(0) {}
inline AuthContext::SecretData::SecretData(time_t expires,const ByteBuffer* secret)
    : mExpireTime(expires),mSecret(secret) {}
// Genuine SDK nested value helpers; native construction is inlined into secret methods, no standalone source quota.
// FUNCTION: WON 0x006e1240
bool AuthContext::LoadSecretList()
{
    AutoCrit lock(mDataCrit);
    mSecretList.clear();
    FILE* file=fopen(mLoginSecretFile.c_str(),"rb");
    if(file==NULL) return false;
    unsigned char count=static_cast<unsigned char>(fgetc(file));
    int i=0;
    while(!feof(file) && i<count) {
        time_t expires=0;
        fread(&expires,sizeof(time_t),1,file);
        char* raw=new char[8];
        fread(raw,1,8,file);
        mSecretList.push_back(SecretData(expires,new ByteBuffer(raw,8,true)));
        ++i;
    }
    fclose(file);
    return true;
}
// FUNCTION: WON 0x006e1460
bool AuthContext::SaveSecretList()
{
    AutoCrit lock(mDataCrit);
    FILE* file=fopen(mLoginSecretFile.c_str(),"wb");
    if(file==NULL) return false;
    unsigned char count=static_cast<unsigned char>(mSecretList.size());
    fwrite(&count,1,1,file);
    for(SecretList::iterator next=mSecretList.begin();next!=mSecretList.end();++next) {
        fwrite(&next->mExpireTime,sizeof(time_t),1,file);
        fwrite(next->mSecret.get()!=NULL?next->mSecret->data():NULL,1,
               next->mSecret.get()!=NULL?next->mSecret->length():0,file);
    }
    fclose(file);
    return true;
}
// FUNCTION: WON 0x006e1560
void AuthContext::AppendLoginSecrets(WriteBuffer& output)
{
    AutoCrit lock(mDataCrit);
    SecretList::iterator next=mSecretList.begin();
    unsigned long countPosition=output.mLength;
    unsigned char count=0;
    output.Fn2A8260(1);
    while(next!=mSecretList.end() && count<256) {
        output.AppendBytes(next->mSecret.get()!=NULL?next->mSecret->data():NULL,
                           next->mSecret.get()!=NULL?next->mSecret->length():0);
        ++count;
        ++next;
    }
    output.SetByte(countPosition,count);
}
// FUNCTION: WON 0x006e1650
void AuthContext::AddSecret(const void* secret)
{
    AutoCrit lock(mDataCrit);
    mSecretList.push_front(SecretData(0,new ByteBuffer(secret,8,false)));
    SaveSecretList();
}
// FUNCTION: WON 0x006e1790
void AuthContext::RemoveSecret(const void* secret)
{
    AutoCrit lock(mDataCrit);
    SecretList::iterator next=mSecretList.begin();
    while(next!=mSecretList.end()) {
        if(next->mExpireTime==0) {
            const ByteBuffer* buffer=next->mSecret.get();
            if(memcmp(buffer!=NULL?buffer->data():NULL,secret,8)==0) {
                mSecretList.erase(next);
                SaveSecretList();
                return;
            }
        }
        ++next;
    }
}
// FUNCTION: WON 0x006e1890
void AuthContext::CommitSecrets(time_t authTime,time_t duration)
{
    AutoCrit lock(mDataCrit);
    if(mSecretList.empty()) return;
    SecretList::iterator next=mSecretList.begin();
    while(next!=mSecretList.end()) {
        if(next->mExpireTime==0) {
            next->mExpireTime=static_cast<time_t>(static_cast<unsigned long>(authTime)+static_cast<unsigned long>(duration)+120UL);
            ++next;
        } else if(next->mExpireTime<=authTime) mSecretList.erase(next++);
        else ++next;
    }
    while(mSecretList.size()>=256) mSecretList.pop_back();
    SaveSecretList();
}
// FUNCTION: WON 0x006e19e0
ConstSmartPtr<AuthPeerData> AuthContext::GetPeerData()
{
    AutoCrit lock(mDataCrit);
    return mPeerData;
}
// FUNCTION: WON 0x006e1a30
bool AuthContext::IsExpired(time_t extra)
{
    AutoCrit lock(mDataCrit);
    return mPeerData->Certificate2Expired(extra);
}
// FUNCTION: WON 0x006e1aa0
void AuthContext::SetPeerData(const AuthPeerData* data)
{
    AutoCrit lock(mDataCrit);
    mPeerData=data;
    mSessionMap.clear();
    mSessionCleanItr=mSessionMap.begin();
}


// FUNCTION: WON 0x006e0c50
void AuthContext::AppendHashes(WriteBuffer& output,const RawBuffer& challenge)
{
    AutoCrit lock(mDataCrit);
    int count=0;
    unsigned long countPosition=output.mLength;
    output.Fn2A8260(1);
    std::map<std::wstring,AuthLoginCommunityData>::iterator next=mCommunityMap.begin();
    while(next!=mCommunityMap.end()) {
        AuthLoginCommunityData& data=next->second;
        if(!data.mSimpleHash.empty()) {
            MD5Digest keyedHash;
            keyedHash.update(challenge);
            keyedHash.update(data.mKeyedHashData);
            RawBuffer keyed=keyedHash.digest();
            output.AppendByte(1);
            output.AppendWString(next->first,2);
            output.AppendBytes(data.mSimpleHash.data(),data.mSimpleHash.length());
            output.AppendBytes(keyed.data(),keyed.length());
            ++count;
        }
        ++next;
    }
    output.SetByte(countPosition,static_cast<unsigned char>(count));
}
// FUNCTION: WON 0x006e0e50
void AuthContext::AppendCommunityData(WriteBuffer& output)
{
    AutoCrit lock(mDataCrit);
    output.AppendByte(0);
    output.AppendByte(static_cast<unsigned char>(mCommunityMap.size()));
    std::map<std::wstring,AuthLoginCommunityData>::iterator community=mCommunityMap.begin();
    while(community!=mCommunityMap.end()) {
        output.AppendWString(community->first,2);
        ++community;
    }
    unsigned long countPosition=output.length();
    output.Fn2A8260(2);
    int count=0;
    community=mCommunityMap.begin();
    while(community!=mCommunityMap.end()) {
        if(community->second.mCDKey.mValid) {
            ConstSmartPtr<ByteBuffer> key(community->second.mCDKey.mP14.get());
            if(key.get()!=NULL) {
                output.AppendByte(1);
                output.AppendShort(static_cast<short>(community->first.length()*2+key->length()+2));
                output.AppendWString(community->first,2);
                output.AppendBytes(key->data(),key->length());
                ++count;
            }
        }
        ++community;
    }
    std::map<std::wstring,CDKey>::iterator joined=mCDKeyCommunityJoinMap.begin();
    while(joined!=mCDKeyCommunityJoinMap.end()) {
        ConstSmartPtr<ByteBuffer> key(joined->second.mP14.get());
        if(key.get()!=NULL) {
            output.AppendByte(7);
            output.AppendShort(static_cast<short>(joined->first.length()*2+6+key->length()));
            output.AppendWString(joined->first,2);
            output.AppendLong(0);
            output.AppendBytes(key->data(),key->length());
            ++count;
        }
        ++joined; //Native advances tree iterator before the held key temporary is destroyed.
    }
    std::map<std::wstring,ConstSmartPtr<ByteBuffer> >::iterator userData=mSetCommunityUserDataMap.begin();
    while(userData!=mSetCommunityUserDataMap.end()) {
        const ByteBuffer* data=userData->second.get();
        if(data!=NULL) {
            output.AppendByte(8);
            output.AppendShort(static_cast<short>(userData->first.length()*2+6+data->length()));
            output.AppendWString(userData->first,2);
            output.AppendLong(0);
            output.AppendBuffer(data,0);
            ++count;
        }
        ++userData;
    }
    if(mSecretList.size()>0) {
        output.AppendByte(6);
        unsigned long position=output.length();
        output.Fn2A8260(2);
        AppendLoginSecrets(output);
        output.SetShort(position,output.length()-position-2);
        ++count;
    }
    std::map<std::wstring,std::wstring>::iterator nickname=mNicknameMap.begin();
    while(nickname!=mNicknameMap.end()) {
        const std::wstring& key=nickname->first;
        const std::wstring& value=nickname->second;
        output.AppendByte(4);
        unsigned long position=output.length();
        output.Fn2A8260(2);
        output.AppendWString(key,2);
        output.SetShort(position,output.length()-position-2);
        ++count;
        if(!value.empty()) {
            output.AppendByte(3);
            position=output.length();
            output.Fn2A8260(2);
            output.AppendWString(key,2);
            output.AppendWString(value,2);
            output.SetShort(position,output.length()-position-2);
            ++count;
        }
        ++nickname;
    }
    output.SetShort(countPosition,static_cast<short>(count));
}

// Bounded AuthContext refresh dispatch on actual GetCert11fc owner and borrowed LightGetCert48 prefix.
// FUNCTION: WON 0x006de560
void AuthContext::StaticGetCertCallback(SmartPtr<AsyncOp> op,SmartPtr<RefCount> param)
{
    static_cast<AuthContext*>(param.get())->GetCertCallback(static_cast<GetCertOp*>(op.get()));
}
// FUNCTION: WON 0x006de5f0
void AuthContext::GetCertCallback(GetCertOp* op)
{
    AutoCrit lock(mDataCrit);
    if(op!=mGetCertOp.get()) return;
    ConstSmartPtr<AuthPeerData> peerData=op->GetPeerData();
    std::set<SmartPtr<LightGetCertOp>,SmartPtr<LightGetCertOp>::Comp>::iterator next=mRefreshSet.begin();
    while(next!=mRefreshSet.end()) {
        LightGetCertOp* waiting=next->get();
        waiting->SetPeerData(peerData);
        waiting->Fn29F220(op->mStatus);
        ++next;
    }
    mRefreshSet.clear();
    mGetCertOp=NULL;
    mRefreshStatus=op->mStatus;
    StartAutoRefresh(true);
}
// FUNCTION: WON 0x006deba0
void AuthContext::RefreshAsync(LightGetCertOp* op,DWORD timeout)
{
    AutoCrit lock(mDataCrit);
    StopAutoRefresh();
    if(op!=NULL) {
        mRefreshSet.insert(op);
        op->Run((long)timeout);
    }
    if(mGetCertOp.get()==NULL) {
        mGetCertOp=new GetCertOp(this);
        mGetCertOp->mCompletion=new ParamCompletion<SmartPtr<AsyncOp>,SmartPtr<RefCount> >(StaticGetCertCallback,this);
        mRefreshStatus=mGetCertOp->Fn29F500(1,60000);
    }
}
// FUNCTION: WON 0x006dedf0
WONStatus AuthContext::RefreshBlock(DWORD timeout)
{
    SmartPtr<GetCertOp> op;
    op=new GetCertOp(this);
    mRefreshStatus=op->Fn29F500(2,timeout);
    if(mRefreshStatus==0) StartAutoRefresh(false);
    WONStatus status=mRefreshStatus;
    return status;
}


// File hashing uses the real separate WONCommon MD558, never WONCrypt MD5Digest.
// FUNCTION: WON 0x006e06d0
bool AuthContext::SetHashFile(const std::wstring& community)
{
    char path[MAX_PATH];
    if(GetModuleFileNameA(GetModuleHandleA(NULL),path,MAX_PATH)>0)
        return SetHashFile(community,path);
    return false;
}
// SUPPORT: EXE 0x006e07f0 (239B one-file-list forwarding adapter, quota0)
bool AuthContext::SetHashFile(const std::wstring& community,const std::string& file)
{
    return SetHashFileList(community,std::list<std::string>(1,file));
}
// FUNCTION: WON 0x006e08e0
bool AuthContext::SetHashFileList(const std::wstring& community,const std::list<std::string>& files)
{
    AutoCrit lock(mDataCrit);
    AuthLoginCommunityData& data=mCommunityMap[community];
    data.mSimpleHash.erase();
    data.mKeyedHashData.erase();
    MD5 simpleHash;
    MD5 hashSection;
    int sectionPosition=0;
    std::list<std::string>::const_iterator next=files.begin();
    while(next!=files.end()) {
        FILE* file=fopen(next->c_str(),"rb");
        if(file==NULL) return false;
        ++next;
        char buffer[16384];
        while(!feof(file)) {
            int count=fread(buffer,1,16384-sectionPosition,file);
            if(count>0) {
                sectionPosition+=count;
                simpleHash.Update(buffer,count);
                hashSection.Update(buffer,count);
                unsigned char digest[16];
                if(sectionPosition==16384 || (feof(file) && next==files.end())) {
                    sectionPosition=0;
                    hashSection.Digest(digest);
                    data.mKeyedHashData.append(digest,16);
                    hashSection.Reset();
                }
            }
        }
        fclose(file);
    }
    unsigned char digest[16];
    simpleHash.Digest(digest);
    data.mSimpleHash.assign(digest,16);
    return true;
}


// Complete locked community selection plus actual SDK mutators used by login consumers.
// FUNCTION: WON 0x006df620
void AuthContext::SetUserName(const std::wstring& name)
{
    AutoCrit lock(mDataCrit);
    mName=name;
}
// FUNCTION: WON 0x006df760
void AuthContext::SetPassword(const std::wstring& password)
{
    AutoCrit lock(mDataCrit);
    mPassword=password;
}
// FUNCTION: WON 0x006df8a0
void AuthContext::AddCommunity(const std::wstring& community)
{
    AutoCrit lock(mDataCrit);
    mCommunityMap.insert(std::map<std::wstring,AuthLoginCommunityData>::value_type(community,AuthLoginCommunityData()));
}
// FUNCTION: WON 0x006dfcc0
void AuthContext::SetCommunity(const std::wstring& community)
{
    AutoCrit lock(mDataCrit);
    std::map<std::wstring,AuthLoginCommunityData>::iterator found=mCommunityMap.find(community);
    if(found!=mCommunityMap.end()) {
        AuthLoginCommunityData data=found->second;
        mCommunityMap.clear();
        mCommunityMap[community]=data;
    }
    else {
        mCommunityMap.clear();
        AddCommunity(community);
    }
}
// FUNCTION: WON 0x006e02d0
void AuthContext::RemoveCommunity(const std::wstring& community)
{
    AutoCrit lock(mDataCrit);
    mCommunityMap.erase(community);
}

// FUNCTION: WON 0x006df440
bool AuthContext::LoadVerifierKey(const std::string& filename)
{
    mCheckedVerifierFile=true;
    FILE* file=fopen(filename.c_str(),"rb");
    if(file==NULL) return false;
    unsigned char buffer[1024];
    RawBuffer key;
    while(!feof(file)) {
        int count=fread(buffer,1,1024,file);
        if(count>0) key.append(buffer,count);
    }
    fclose(file);
    AutoCrit lock(GetVerifierCrit());
    return mVerifierKey.SetPublicKey(key.data(),key.length());
}

// Actual Init operates on the borrowed prefixc0; no AuthContext allocation/lifetime authority.
// FUNCTION: WON 0x006ddb70
void AuthContext::Init()
{
    mSessionCleanItr=mSessionMap.begin();
    mPeerData=new AuthPeerData;
    mLoginSecretFile=WONAPICore::GetDefaultFileDirectory()+"_wonlogin.ks";
    mServerContext=new ServerContext;
    mDoAutoRefresh=false;
    mRefreshStatus=0x186a4;
    AutoCrit lock(GetVerifierCrit());
    if(mCheckedVerifierFile) return;
    LoadVerifierKey(WONAPICore::GetDefaultFileDirectory()+"_wonkver.pub");
}
// FUNCTION: WON 0x006df330
void AuthContext::SetServerContext(ServerContext* context)
{
    AutoCrit lock(mDataCrit);
    mServerContext=context;
}
// FUNCTION: WON 0x006df3e0
SmartPtr<ServerContext> AuthContext::GetServerContext()
{
    AutoCrit lock(mDataCrit);
    return mServerContext;
}

}

namespace WONAPI {

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
AuthContext::AuthContext(const std::wstring &theUserName, const std::wstring& theCommunity,
						 const std::wstring &thePassword) : 
	mName(theUserName),  
	mPassword(thePassword)
{
	Init();
	AddCommunity(theCommunity);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
bool AuthContext::AddAddress(const IPAddr &theAddr)
{
	AutoCrit aCrit(mDataCrit);
	return mServerContext->AddAddress(theAddr);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
bool AuthContext::AddAddressesFromDir(const DirEntityList &theDir, const wchar_t *theNameFilter)
{
	AutoCrit aCrit(mDataCrit);
	return mServerContext->AddAddressesFromDir(theDir, theNameFilter);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
void AuthContext::RemoveAllCommunity()
{
	AutoCrit aCrit(mDataCrit);
	mCommunityMap.clear();
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
void AuthContext::AddCDKeyCommunityJoin(const std::wstring &theCommunity, const CDKey &theKey)
{
	AutoCrit aCrit(mDataCrit);
	mCDKeyCommunityJoinMap[theCommunity] = theKey;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
void AuthContext::AddUserData(const std::wstring &theCommunity, const ByteBuffer *theData)
{
	AutoCrit aCrit(mDataCrit);
	mSetCommunityUserDataMap[theCommunity] = theData;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
void AuthContext::SetLoginSecretFile(const std::string &theFile)
{
	AutoCrit aCrit(mDataCrit);
	mLoginSecretFile = theFile;
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
void AuthContext::AddNickname(const std::wstring &theKey)
{
	AutoCrit aCrit(mDataCrit);
	mNicknameMap[theKey] = L""; 
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
void AuthContext::SetNickname(const std::wstring &theKey, const std::wstring &theName)
{ 
	AutoCrit aCrit(mDataCrit);
	mNicknameMap[theKey] = theName; 
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine same-SDK absent support body; no Native credit.
void AuthContext::ClearNicknames()
{
	AutoCrit aCrit(mDataCrit);
	mNicknameMap.clear();
}
}

namespace WONAPI {

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK locked server-context API.
bool AuthContext::RemoveAddress(const IPAddr &theAddr)
{
	AutoCrit aCrit(mDataCrit);
	return mServerContext->RemoveAddress(theAddr);
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK locked server-context API.
void AuthContext::RemoveAllAddresses()
{
	AutoCrit aCrit(mDataCrit);
	mServerContext->RemoveAllAddresses();
}

// SOURCE_ONLY_NO_NATIVE_ID: genuine SDK locked server-context API.
bool AuthContext::AddAddressesFromDir(const DirEntityMap &theDir, const std::wstring &thePath, const wchar_t *theNameFilter)
{
	AutoCrit aCrit(mDataCrit);
	return mServerContext->AddAddressesFromDir(theDir, thePath, theNameFilter);
}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006ddf90
// (protected: virtual void * __thiscall WONAPI::AuthContext::`scalar deleting destructor'(unsigned int)    [??_GAuthContext@WONAPI@@MAEPAXI@Z])
// TEMPLATE: WON 0x006dfae0
// (public: __thiscall WONAPI::AuthLoginCommunityData::~AuthLoginCommunityData(void)    [??1AuthLoginCommunityData@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x006dfbd0
// (public: __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > con)
// TEMPLATE: WON 0x006dff00
// (public: __thiscall WONAPI::AuthLoginCommunityData::AuthLoginCommunityData(void)    [??0AuthLoginCommunityData@WONAPI@@QAE@XZ])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006ddfb0
// (public: __thiscall std::set<class WONAPI::SmartPtr<class WONAPI::LightGetCertOp>,struct WONAPI::SmartPtr<class WONAPI::LightGetCertOp>::Comp,class std)
// TEMPLATE: WON 0x006de070
// (public: __thiscall std::map<class WONAPI::IPAddr,struct WONAPI::AuthContext::SessionStruct,struct std::less<class WONAPI::IPAddr>,class std::allocator)
// TEMPLATE: WON 0x006de130
// (public: __thiscall std::map<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,clas)
// TEMPLATE: WON 0x006de1f0
// (public: __thiscall std::map<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,clas)
// TEMPLATE: WON 0x006de2b0
// (public: __thiscall std::map<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,clas)
// TEMPLATE: WON 0x006dff60
// (public: __thiscall WONAPI::AuthLoginCommunityData::AuthLoginCommunityData(class WONAPI::AuthLoginCommunityData const &)    [??0AuthLoginCommunityData@)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006ddda0
// (class WONAPI::CriticalSection & __cdecl GetVerifierCrit(void)    [?GetVerifierCrit@@YAAAVCriticalSection@WONAPI@@XZ])
// TEMPLATE: WON 0x006e0140
// (public: class WONAPI::CDKey & __thiscall WONAPI::CDKey::operator=(class WONAPI::CDKey const &)    [??4CDKey@WONAPI@@QAEAAV01@ABV01@@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006ddde0
// (_$E72    [_$E72])
// TEMPLATE: WON 0x006df2d0
// (_$E77    [_$E77])
// TEMPLATE: WON 0x006df2e0
// (_$E73    [_$E73])
// TEMPLATE: WON 0x006df300
// (_$E76    [_$E76])
// TEMPLATE: WON 0x006df310
// (_$E74    [_$E74])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006cac70
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x006e9530
// (public: class std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> > & __thiscall std::basic_str)
// TEMPLATE: WON 0x006e9630
// (public: class std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> > & __thiscall std::basic_str)
// TEMPLATE: WON 0x006ea2e0
// (public: __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > con)
// TEMPLATE: WON 0x006ea3b0
// (public: __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> > con)
// TEMPLATE: WON 0x006ea440
// (public: class std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> > & __thiscall std::basic_str)
// TEMPLATE: WON 0x006ea550
// (public: class std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> > & __thiscall std::basic_str)
// TEMPLATE: WON 0x006ea760
// (private: void __thiscall std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> >::_Eos(unsigned i)
// TEMPLATE: WON 0x006ea780
// (private: void __thiscall std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> >::_Freeze(void)  )
// TEMPLATE: WON 0x006ea820
// (private: bool __thiscall std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> >::_Grow(unsigned )
// TEMPLATE: WON 0x006eaa00
// (private: bool __thiscall std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> >::_Inside(unsigne)
// TEMPLATE: WON 0x006eb590
// (public: bool __thiscall std::list<class WONAPI::SmartPtr<class WONAPI::AuthSession>,class std::allocator<class WONAPI::SmartPtr<class WONAPI::AuthSess)
// TEMPLATE: WON 0x006eb750
// (public: class std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,struct WONAPI::AuthContext::SessionStruct>,struct std::_Map)
// TEMPLATE: WON 0x006eb890
// (public: void __thiscall std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,struct WONAPI::AuthContext::SessionStruct>,struct)
// TEMPLATE: WON 0x006eb8e0
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006ebc90
// (public: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short>)
// TEMPLATE: WON 0x006ebd40
// (protected: struct std::list<struct WONAPI::AuthContext::SecretData,class std::allocator<struct WONAPI::AuthContext::SecretData> >::_Node * __thiscall )
// TEMPLATE: WON 0x006ebe30
// (public: static unsigned char * __cdecl std::char_traits<unsigned char>::copy(unsigned char *,unsigned char const *,unsigned int)    [?copy@?$char_trai)
// TEMPLATE: WON 0x006ebe60
// (public: static unsigned char * __cdecl std::char_traits<unsigned char>::move(unsigned char *,unsigned char const *,unsigned int)    [?move@?$char_trai)
// TEMPLATE: WON 0x006ebeb0
// (public: unsigned int __thiscall std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> >::max_size)
// TEMPLATE: WON 0x006ebec0
// (private: void __thiscall std::basic_string<unsigned char,struct std::char_traits<unsigned char>,class std::allocator<unsigned char> >::_Copy(unsigned )
// TEMPLATE: WON 0x006ed960
// (public: class std::_Tree<class WONAPI::SmartPtr<class WONAPI::LightGetCertOp>,class WONAPI::SmartPtr<class WONAPI::LightGetCertOp>,struct std::_Setk<c)
// TEMPLATE: WON 0x006ee020
// (protected: void __thiscall std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,struct WONAPI::AuthContext::SessionStruct>,str)
// TEMPLATE: WON 0x006ee8d0
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006ee910
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006ee950
// (protected: struct std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,str)
// TEMPLATE: WON 0x006ee9a0
// (public: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short>)
// TEMPLATE: WON 0x006ef0a0
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006ef0e0
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006ef120
// (public: class std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,struct )
// TEMPLATE: WON 0x006ef650
// (protected: void __thiscall std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned sho)
// TEMPLATE: WON 0x006ef890
// (public: void __thiscall std::_Tree<class WONAPI::IPAddr,struct std::pair<class WONAPI::IPAddr const ,struct WONAPI::AuthContext::SessionStruct>,struct)
// TEMPLATE: WON 0x006ef8f0
// (protected: struct std::_Tree<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short> >,str)
// TEMPLATE: WON 0x006f0090
// (public: void * __thiscall std::pair<class std::basic_string<unsigned short,struct std::char_traits<unsigned short>,class std::allocator<unsigned short)
