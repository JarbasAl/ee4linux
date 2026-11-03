// Platform SDK <iphlpapi.h> / <iptypes.h> STAND-IN: only the declarations this project uses, with the
// documented Windows layouts. The VC6 package has no Platform SDK. Replace with the real 2001-era SDK
// headers if they are obtained (TCP Protocol.dll imports GetAdaptersInfo from iphlpapi.dll).
#ifndef EE_WINSDK_IPHLPAPI_H
#define EE_WINSDK_IPHLPAPI_H
#include <windows.h>
#include <time.h>

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128
#define MAX_ADAPTER_NAME_LENGTH         256
#define MAX_ADAPTER_ADDRESS_LENGTH      8

typedef struct {
    char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

typedef struct _IP_ADDR_STRING {
    struct _IP_ADDR_STRING* Next;
    IP_ADDRESS_STRING IpAddress;
    IP_MASK_STRING IpMask;
    DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

typedef struct _IP_ADAPTER_INFO {
    struct _IP_ADAPTER_INFO* Next;
    DWORD ComboIndex;
    char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
    char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
    UINT AddressLength;
    BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
    DWORD Index;
    UINT Type;
    UINT DhcpEnabled;
    PIP_ADDR_STRING CurrentIpAddress;
    IP_ADDR_STRING IpAddressList;          // +0x1ac (IpAddress.String at +0x1b0: TCP 0x1b3e)
    IP_ADDR_STRING GatewayList;
    IP_ADDR_STRING DhcpServer;
    BOOL HaveWins;
    IP_ADDR_STRING PrimaryWinsServer;
    IP_ADDR_STRING SecondaryWinsServer;
    time_t LeaseObtained;
    time_t LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

#ifdef __cplusplus
extern "C" {
#endif
DWORD WINAPI GetAdaptersInfo(PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
#ifdef __cplusplus
}
#endif

#endif
