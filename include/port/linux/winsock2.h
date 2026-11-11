/* Winsock on the system's BSD sockets, for the native Linux build. It replaces Wine's winsock.h and winsock2.h,
   which clash with glibc's socket headers. Winsock is the BSD socket API with a few additions: the BSD calls and
   structures are glibc's, and this header adds the Windows names, error codes and calls (src/Port/Linux/Win32Socket.cpp).
   fd_set has the Windows layout, a count and an array of sockets, because WON reads its fields. */
#ifndef EE_PORT_LINUX_WINSOCK2_H
#define EE_PORT_LINUX_WINSOCK2_H

#include <windef.h>
#include <winbase.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

typedef UINT_PTR SOCKET;
#define INVALID_SOCKET ((SOCKET)~0)
#define SOCKET_ERROR (-1)

typedef struct sockaddr SOCKADDR, *PSOCKADDR, *LPSOCKADDR;
typedef struct sockaddr_in SOCKADDR_IN, *PSOCKADDR_IN, *LPSOCKADDR_IN;
typedef struct in_addr IN_ADDR, *PIN_ADDR, *LPIN_ADDR;
typedef struct hostent HOSTENT, *PHOSTENT, *LPHOSTENT;
typedef struct timeval TIMEVAL, *PTIMEVAL, *LPTIMEVAL;
typedef struct linger LINGER, *PLINGER, *LPLINGER;

#define WSADESCRIPTION_LEN 256
#define WSASYS_STATUS_LEN 128
typedef struct WSAData {
    WORD wVersion;
    WORD wHighVersion;
    char szDescription[WSADESCRIPTION_LEN + 1];
    char szSystemStatus[WSASYS_STATUS_LEN + 1];
    unsigned short iMaxSockets;
    unsigned short iMaxUdpDg;
    char* lpVendorInfo;
} WSADATA, *LPWSADATA;

#define SD_RECEIVE 0
#define SD_SEND 1
#define SD_BOTH 2

/* ---- fd_set in the Windows layout, and select over it ---- */
#define fd_set ee_ws_fd_set
#undef FD_SETSIZE
#define FD_SETSIZE 64
typedef struct ee_ws_fd_set {
    u_int fd_count;
    SOCKET fd_array[FD_SETSIZE];
} ee_ws_fd_set;

static inline void ee_ws_fd_add(SOCKET inSocket, ee_ws_fd_set* ioSet)
{
    for (u_int i = 0; i < ioSet->fd_count; ++i)
        if (ioSet->fd_array[i] == inSocket)
            return;
    if (ioSet->fd_count < FD_SETSIZE)
        ioSet->fd_array[ioSet->fd_count++] = inSocket;
}

static inline void ee_ws_fd_remove(SOCKET inSocket, ee_ws_fd_set* ioSet)
{
    for (u_int i = 0; i < ioSet->fd_count; ++i)
        if (ioSet->fd_array[i] == inSocket) {
            ioSet->fd_array[i] = ioSet->fd_array[--ioSet->fd_count];
            return;
        }
}

static inline int ee_ws_fd_isset(SOCKET inSocket, const ee_ws_fd_set* inSet)
{
    for (u_int i = 0; i < inSet->fd_count; ++i)
        if (inSet->fd_array[i] == inSocket)
            return 1;
    return 0;
}

#undef FD_ZERO
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET
#define FD_ZERO(set) ((set)->fd_count = 0)
#define FD_SET(socket, set) ee_ws_fd_add((SOCKET)(socket), (set))
#define FD_CLR(socket, set) ee_ws_fd_remove((SOCKET)(socket), (set))
#define FD_ISSET(socket, set) ee_ws_fd_isset((SOCKET)(socket), (set))
#define select ee_ws_select

#ifdef __cplusplus
extern "C" {
#endif

/* select on the Windows fd_set: the first argument is ignored, as in Winsock */
int WINAPI ee_ws_select(int inIgnored, ee_ws_fd_set* ioRead, ee_ws_fd_set* ioWrite, ee_ws_fd_set* ioExcept,
                        const struct timeval* inTimeout);

int WINAPI WSAStartup(WORD inVersion, LPWSADATA outData);
int WINAPI WSACleanup(void);
int WINAPI WSAGetLastError(void);
void WINAPI WSASetLastError(int inError);
int WINAPI closesocket(SOCKET inSocket);
int WINAPI ioctlsocket(SOCKET inSocket, long inCommand, u_long* ioArgument);

/* ---- socket events on the port's event objects ---- */
#define FD_READ 0x01
#define FD_WRITE 0x02
#define FD_OOB 0x04
#define FD_ACCEPT 0x08
#define FD_CONNECT 0x10
#define FD_CLOSE 0x20
#define FD_READ_BIT 0
#define FD_WRITE_BIT 1
#define FD_OOB_BIT 2
#define FD_ACCEPT_BIT 3
#define FD_CONNECT_BIT 4
#define FD_CLOSE_BIT 5
#define FD_MAX_EVENTS 10

typedef HANDLE WSAEVENT, *LPWSAEVENT;
#define WSA_INVALID_EVENT ((WSAEVENT)NULL)
typedef struct _WSANETWORKEVENTS {
    long lNetworkEvents;
    int iErrorCode[FD_MAX_EVENTS];
} WSANETWORKEVENTS, *LPWSANETWORKEVENTS;

WSAEVENT WINAPI WSACreateEvent(void);
BOOL WINAPI WSACloseEvent(WSAEVENT inEvent);
int WINAPI WSAEventSelect(SOCKET inSocket, WSAEVENT inEvent, long inEvents);
int WINAPI WSAEnumNetworkEvents(SOCKET inSocket, WSAEVENT inEvent, LPWSANETWORKEVENTS outEvents);

#ifdef __cplusplus
}

/* Winsock's length arguments are int; glibc's are socklen_t */
inline int getsockopt(SOCKET inSocket, int inLevel, int inName, char* outValue, int* ioLength)
{
    socklen_t theLength = (socklen_t)*ioLength;
    int theResult = ::getsockopt((int)inSocket, inLevel, inName, outValue, &theLength);
    *ioLength = (int)theLength;
    return theResult;
}

inline SOCKET accept(SOCKET inSocket, struct sockaddr* outAddress, int* ioLength)
{
    socklen_t theLength = ioLength ? (socklen_t)*ioLength : 0;
    int theResult = ::accept((int)inSocket, outAddress, ioLength ? &theLength : (socklen_t*)NULL);
    if (ioLength)
        *ioLength = (int)theLength;
    return (SOCKET)theResult;
}

inline int getsockname(SOCKET inSocket, struct sockaddr* outAddress, int* ioLength)
{
    socklen_t theLength = (socklen_t)*ioLength;
    int theResult = ::getsockname((int)inSocket, outAddress, &theLength);
    *ioLength = (int)theLength;
    return theResult;
}

inline int getpeername(SOCKET inSocket, struct sockaddr* outAddress, int* ioLength)
{
    socklen_t theLength = (socklen_t)*ioLength;
    int theResult = ::getpeername((int)inSocket, outAddress, &theLength);
    *ioLength = (int)theLength;
    return theResult;
}

inline int recvfrom(SOCKET inSocket, char* outData, int inLength, int inFlags, struct sockaddr* outFrom,
                    int* ioFromLength)
{
    socklen_t theLength = ioFromLength ? (socklen_t)*ioFromLength : 0;
    int theResult = (int)::recvfrom((int)inSocket, outData, (size_t)inLength, inFlags, outFrom,
                                    ioFromLength ? &theLength : (socklen_t*)NULL);
    if (ioFromLength)
        *ioFromLength = (int)theLength;
    return theResult;
}
#endif

/* ---- Winsock's error codes ---- */
#define WSABASEERR 10000
#define WSAEINTR (WSABASEERR + 4)
#define WSAEBADF (WSABASEERR + 9)
#define WSAEACCES (WSABASEERR + 13)
#define WSAEFAULT (WSABASEERR + 14)
#define WSAEINVAL (WSABASEERR + 22)
#define WSAEMFILE (WSABASEERR + 24)
#define WSAEWOULDBLOCK (WSABASEERR + 35)
#define WSAEINPROGRESS (WSABASEERR + 36)
#define WSAEALREADY (WSABASEERR + 37)
#define WSAENOTSOCK (WSABASEERR + 38)
#define WSAEDESTADDRREQ (WSABASEERR + 39)
#define WSAEMSGSIZE (WSABASEERR + 40)
#define WSAEPROTOTYPE (WSABASEERR + 41)
#define WSAENOPROTOOPT (WSABASEERR + 42)
#define WSAEPROTONOSUPPORT (WSABASEERR + 43)
#define WSAESOCKTNOSUPPORT (WSABASEERR + 44)
#define WSAEOPNOTSUPP (WSABASEERR + 45)
#define WSAEPFNOSUPPORT (WSABASEERR + 46)
#define WSAEAFNOSUPPORT (WSABASEERR + 47)
#define WSAEADDRINUSE (WSABASEERR + 48)
#define WSAEADDRNOTAVAIL (WSABASEERR + 49)
#define WSAENETDOWN (WSABASEERR + 50)
#define WSAENETUNREACH (WSABASEERR + 51)
#define WSAENETRESET (WSABASEERR + 52)
#define WSAECONNABORTED (WSABASEERR + 53)
#define WSAECONNRESET (WSABASEERR + 54)
#define WSAENOBUFS (WSABASEERR + 55)
#define WSAEISCONN (WSABASEERR + 56)
#define WSAENOTCONN (WSABASEERR + 57)
#define WSAESHUTDOWN (WSABASEERR + 58)
#define WSAETOOMANYREFS (WSABASEERR + 59)
#define WSAETIMEDOUT (WSABASEERR + 60)
#define WSAECONNREFUSED (WSABASEERR + 61)
#define WSAELOOP (WSABASEERR + 62)
#define WSAENAMETOOLONG (WSABASEERR + 63)
#define WSAEHOSTDOWN (WSABASEERR + 64)
#define WSAEHOSTUNREACH (WSABASEERR + 65)
#define WSAENOTEMPTY (WSABASEERR + 66)
#define WSAEPROCLIM (WSABASEERR + 67)
#define WSAEUSERS (WSABASEERR + 68)
#define WSAEDQUOT (WSABASEERR + 69)
#define WSAESTALE (WSABASEERR + 70)
#define WSAEREMOTE (WSABASEERR + 71)
#define WSASYSNOTREADY (WSABASEERR + 91)
#define WSAVERNOTSUPPORTED (WSABASEERR + 92)
#define WSANOTINITIALISED (WSABASEERR + 93)
#define WSAEDISCON (WSABASEERR + 101)
#define WSAHOST_NOT_FOUND (WSABASEERR + 1001)
#define WSATRY_AGAIN (WSABASEERR + 1002)
#define WSANO_RECOVERY (WSABASEERR + 1003)
#define WSANO_DATA (WSABASEERR + 1004)

#endif
