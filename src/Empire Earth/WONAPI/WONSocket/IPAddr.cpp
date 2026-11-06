// Round3 NEW original IPAddr source candidate. File grouping provisional; SOURCE ONLY.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <winsock2.h>
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include "winsdk/iphlpapi.h"
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// SCRATCH IPAddr setters; complete28 target record, headers remain Model-owned.

// FUNCTION: WON 0x006b2660
bool IPAddr::Fn2B2660(const std::string& inHost,unsigned short inPort)
{
    memset(&mAddr,0,sizeof(mAddr));
    mB24=false;
    unsigned long aHost=inet_addr(inHost.c_str());
    if (aHost==INADDR_NONE) {
        hostent* aEntry=gethostbyname(inHost.c_str());
        if (aEntry==NULL) return false;
        memcpy(&aHost,aEntry->h_addr_list[0],sizeof(aHost));
    }
    if (mB00) mHost=inHost;
    else mHost.erase();
    mAddr.sin_family=AF_INET;
    mAddr.sin_addr.s_addr=aHost;
    mAddr.sin_port=htons(inPort);
    mB24=true;
    return true;
}
// FUNCTION: WON 0x006b2890
bool IPAddr::Fn2B2890(const std::string& inHostAndPort)
{
    std::string::size_type aColon=inHostAndPort.find(':');
    if (aColon==std::string::npos) {
        mB24=false;
        return false;
    }
    return Fn2B2660(inHostAndPort.substr(0,aColon),
        static_cast<unsigned short>(atoi(inHostAndPort.substr(aColon+1).c_str())));
}
// FUNCTION: WON 0x006b29d0
void IPAddr::SetSixByte(const void* inAddress)
{
    memset(&mAddr,0,sizeof(mAddr));
    mAddr.sin_family=AF_INET;
    memcpy(&mAddr.sin_port,inAddress,6);
    mHost.erase();
    mB24=true;
}
// FUNCTION: WON 0x006b2b00
void IPAddr::Fn2B2B00(const sockaddr_in& inAddress)
{
    mAddr=inAddress;
    mHost.erase();
    mB24=true;
}
// FUNCTION: WON 0x006b2c20
void IPAddr::Fn2B2C20(long inHost,unsigned short inPort)
{
    memset(&mAddr,0,sizeof(mAddr));
    mAddr.sin_family=AF_INET;
    mAddr.sin_addr.s_addr=htonl(inHost);
    mAddr.sin_port=htons(inPort);
    mHost.erase();
    mB24=true;
}
// FUNCTION: WON 0x006b2df0
bool IPAddr::Fn2B2DF0(const std::string& inHostAndPort,unsigned short inDefaultPort)
{
    mB24=false;
    std::string::size_type aColon=inHostAndPort.find(':');
    unsigned long aHost;
    unsigned short aPort;
    if (aColon!=std::string::npos) {
        aHost=inet_addr(inHostAndPort.substr(0,aColon).c_str());
        aPort=atoi(inHostAndPort.substr(aColon+1).c_str());
    } else {
        aHost=inet_addr(inHostAndPort.c_str());
        aPort=inDefaultPort;
    }
    if (aHost!=INADDR_NONE) {
        memset(&mAddr,0,sizeof(mAddr));
        mAddr.sin_family=AF_INET;
        mAddr.sin_addr.s_addr=aHost;
        mAddr.sin_port=htons(static_cast<unsigned short>(aPort));
        mHost.erase();
        mB24=true;
    }
    return mB24;
}

// FUNCTION: WON 0x006b2fc0
std::string IPAddr::GetHostString(bool inUseOriginal) const
{
    if (inUseOriginal && !mHost.empty()) return mHost;
    return std::string(inet_ntoa(mAddr.sin_addr), std::allocator<char>());
}
// FUNCTION: WON 0x006b2500
IPAddr IPAddr::GetLocalAddr()
{
    if (mLocalHost!=0) return IPAddr(mLocalHost,0);
    char aHostName[1024];
    int aErr=gethostname(aHostName,1024);
    if (aErr==0) return IPAddr(aHostName,0);
    return IPAddr();
}

// FUNCTION: WON 0x006b37a0
bool IPAddr::SetLocalHost(long inHost,bool inValidate)
{
    if (inValidate && inHost!=0) {
        std::list<LobbyNetAddress> aAddresses;
        LobbyGetNetAddresses(aAddresses,false);
        std::list<LobbyNetAddress>::iterator aEnd=aAddresses.end();
        std::list<LobbyNetAddress>::iterator it=aAddresses.begin();
        for (;it!=aEnd;++it)
            if (it->mHandle==inHost) break;
        if (it==aEnd) return false;
    }
    mLocalHost=inHost;
    return true;
}


// FUNCTION: WON 0x006b3540
static bool IPAddr_PlatformGetNetworkAdapterList(std::list<LobbyNetAddress>& outAddresses)
{
    typedef DWORD (WINAPI *GetAdaptersInfoFunc)(PIP_ADAPTER_INFO,PULONG);
    static GetAdaptersInfoFunc aGetAdaptersInfoFunc=NULL;
    static bool haveLoaded=false;
    struct AutoUnloadLibrary {
        HMODULE mModule;
        AutoUnloadLibrary() : mModule(NULL) {}
        ~AutoUnloadLibrary() { if(mModule!=NULL) FreeLibrary(mModule); }
    };
    static AutoUnloadLibrary anUnload;
    if(!haveLoaded) {
        HMODULE aModule=LoadLibraryA("Iphlpapi.dll");
        if(aModule!=NULL) {
            anUnload.mModule=aModule;
            aGetAdaptersInfoFunc=(GetAdaptersInfoFunc)GetProcAddress(aModule,"GetAdaptersInfo");
        }
        haveLoaded=true;
    }
    if(aGetAdaptersInfoFunc==NULL) return false;
    DWORD aBufSize=0;
    aGetAdaptersInfoFunc(NULL,&aBufSize);
    if(aBufSize==0 || aBufSize>256000) return false;
    char* aBuf=new char[aBufSize];
    std::auto_ptr<char> aDelBuf(aBuf);
    PIP_ADAPTER_INFO anAdapter=(PIP_ADAPTER_INFO)aBuf;
    aGetAdaptersInfoFunc(anAdapter,&aBufSize);
    while(anAdapter!=NULL) {
        std::string aName=anAdapter->Description;
        long aHost=ntohl(inet_addr(anAdapter->IpAddressList.IpAddress.String));
        outAddresses.push_back(LobbyNetAddress(aName,aHost));
        anAdapter=anAdapter->Next;
    }
    return true;
}
// FUNCTION: WON 0x006b32b0
void LobbyGetNetAddresses(std::list<LobbyNetAddress>& outAddresses,bool inGetDetails)
{
    outAddresses.erase(outAddresses.begin(),outAddresses.end());
    if(inGetDetails && IPAddr_PlatformGetNetworkAdapterList(outAddresses) && !outAddresses.empty()) return;
    char aHostName[1024];
    if(gethostname(aHostName,sizeof(aHostName))!=0) return;
    hostent* anEntry=gethostbyname(aHostName);
    if(anEntry==NULL) return;
    for(int i=0;anEntry->h_addr_list[i]!=NULL;++i) {
        unsigned long anAddress;
        memcpy(&anAddress,anEntry->h_addr_list[i],sizeof(anAddress));
        std::string aName="";
        long aHost=ntohl(anAddress);
        LobbyNetAddress anAddressEntry;
        anAddressEntry.mName=aName;
        anAddressEntry.mHandle=aHost;
        outAddresses.push_back(anAddressEntry);
    }
}
// Complete explicit native IPAddr ownership constructors; no fields/layout additions.
// FUNCTION: WON 0x006b2350
IPAddr::IPAddr()
{
    memset(&mAddr,0,sizeof(mAddr));
    mB24=false;
    mB00=false;
}
// FUNCTION: WON 0x006b2380
IPAddr::IPAddr(const sockaddr_in& inAddress)
{
    mB00=false;
    Fn2B2B00(inAddress);
}
// FUNCTION: WON 0x006b23e0
IPAddr::IPAddr(const std::string& inHost,unsigned short inPort)
{
    mB00=false;
    Fn2B2660(inHost,inPort);
}
// FUNCTION: WON 0x006b2440
IPAddr::IPAddr(const std::string& inHostAndPort)
{
    mB00=false;
    Fn2B2890(inHostAndPort);
}
// FUNCTION: WON 0x006b24a0
IPAddr::IPAddr(long inHost,unsigned short inPort)
{
    mB00=false;
    Fn2B2C20(inHost,inPort);
}
// FUNCTION: WON 0x006b2d50
bool IPAddr::SetWithDefaultPort(const std::string& inHostAndPort,unsigned short inDefaultPort)
{
    if(inHostAndPort.find(':')==std::string::npos) return Fn2B2660(inHostAndPort,inDefaultPort);
    return Fn2B2890(inHostAndPort);
}

}

// Private Round7 missing IPAddr source completion; all definitions support0.
namespace WONAPI {
// FUNCTION: WON 0x006b2630
IPAddr IPAddr::GetBroadcastAddr(int inPort)
{
    return IPAddr(-1,static_cast<unsigned short>(inPort));
}
// FUNCTION: WON 0x006b29c0
const char* IPAddr::GetSixByte() const
{
    return reinterpret_cast<const char*>(&mAddr.sin_port);
}
// FUNCTION: WON 0x006b2f90
void IPAddr::SetPort(unsigned short inPort)
{
    mAddr.sin_port=htons(inPort);
}
// FUNCTION: WON 0x006b2fb0
long IPAddr::Fn2B2FB0() const
{
    return ntohl(mAddr.sin_addr.s_addr);
}
// FUNCTION: WON 0x006b3070
std::string IPAddr::GetHostAndPortString(bool inUseOriginal) const
{
    long host=ntohl(Fn2B2FB0());
    std::string hostText;
    unsigned char* bytes=reinterpret_cast<unsigned char*>(&host);
    char text[50];
    sprintf(text,"%s:%d",GetHostString(inUseOriginal).c_str(),GetPort());
    return text;
}
// FUNCTION: WON 0x006b31c0
unsigned short IPAddr::GetPort() const
{
    return ntohs(mAddr.sin_port);
}
// FUNCTION: WON 0x006b31d0
bool IPAddr::IsValid() const
{
    return mB24;
}
// FUNCTION: WON 0x006b31e0
const sockaddr_in& IPAddr::GetSockAddrIn() const
{
    return mAddr;
}
// FUNCTION: WON 0x006b31f0
bool IPAddr::operator<(const IPAddr& other) const
{
    return memcmp(&mAddr.sin_port,&other.mAddr.sin_port,6)<0;
}
// FUNCTION: WON 0x006b3220
bool IPAddr::operator==(const IPAddr& other) const
{
    return memcmp(&mAddr.sin_port,&other.mAddr.sin_port,6)==0;
}
// FUNCTION: WON 0x006b3240
bool IPAddr::operator!=(const IPAddr& other) const
{
    return memcmp(&mAddr.sin_port,&other.mAddr.sin_port,6)!=0;
}
// FUNCTION: WON 0x006b3260
int IPAddr::StringCompare(const IPAddr& other) const
{
    int result=Fn2B2FB0()-other.Fn2B2FB0();
    if(result!=0) return result;
    return GetPort()-other.GetPort();
}
}

// Genuine SDK IPAddr.cpp static host-order local interface, native565f40.
namespace WONAPI {
long IPAddr::mLocalHost = INADDR_ANY;
}

// Compiler-generated for this unit:
// SYNTHETIC: WON 0x006b3790
// (_$E72: atexit destructor of IPAddr_PlatformGetNetworkAdapterList's static anUnload)

// Compiler-generated and header-inline functions this unit emits:
// TEMPLATE: WON 0x006b8710
// (public: __thiscall std::auto_ptr<char>::~auto_ptr<char>(void))
// TEMPLATE: WON 0x006b9130
// (protected: struct std::list<struct WONAPI::LobbyNetAddress,class std::allocator<struct WONAPI::LobbyNetAddress> >::_Node * __thiscall std::list<struct)
// TEMPLATE: WON 0x006b97f0
// (protected: void __thiscall std::list<struct WONAPI::LobbyNetAddress,class std::allocator<struct WONAPI::LobbyNetAddress> >::_Incsize(unsigned int))
// TEMPLATE: WON 0x006b9e30
// (void __cdecl std::_Construct(struct WONAPI::LobbyNetAddress *,struct WONAPI::LobbyNetAddress const &))
