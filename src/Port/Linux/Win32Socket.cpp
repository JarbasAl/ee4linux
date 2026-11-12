// Winsock on BSD sockets for the native Linux build: start-up, Winsock error codes, select on the Windows fd_set,
// ioctlsocket, WSAEventSelect on the layer's event objects, and GetAdaptersInfo. A SOCKET is the file descriptor.
// The BSD calls whose Windows behaviour differs come here under ee_ws_ names (include/port/linux/winsock2.h).
//
// WSAEventSelect follows Winsock's rule: a network event sets the event once, and the call that re-enables it
// (recv for FD_READ, accept for FD_ACCEPT, a send that would block for FD_WRITE) lets it be set again. A poller
// thread watches the selected sockets with poll().
#include "Win32Internal.h"

#include <iphlpapi.h>
#include <ipifcons.h>

#include <fcntl.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netpacket/packet.h>
#include <poll.h>
#include <sys/eventfd.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <map>
#include <thread>
#include <vector>

#undef connect
#undef accept
#undef send
#undef sendto
#undef recv
#undef recvfrom
#undef getsockopt
#undef gethostbyname

namespace {

// The Winsock code for an errno value; Winsock codes stored in errno by WSASetLastError pass through
int WinsockError(int inErrno)
{
    if (inErrno == 0 || inErrno >= WSABASEERR)
        return inErrno;
    switch (inErrno) {
    case EINTR: return WSAEINTR;
    case EBADF: return WSAENOTSOCK;
    case EACCES: return WSAEACCES;
    case EFAULT: return WSAEFAULT;
    case EINVAL: return WSAEINVAL;
    case EMFILE: return WSAEMFILE;
    case EWOULDBLOCK: return WSAEWOULDBLOCK;
    case EINPROGRESS: return WSAEWOULDBLOCK;    // a non-blocking connect under way
    case EALREADY: return WSAEALREADY;
    case ENOTSOCK: return WSAENOTSOCK;
    case EDESTADDRREQ: return WSAEDESTADDRREQ;
    case EMSGSIZE: return WSAEMSGSIZE;
    case EPROTOTYPE: return WSAEPROTOTYPE;
    case ENOPROTOOPT: return WSAENOPROTOOPT;
    case EPROTONOSUPPORT: return WSAEPROTONOSUPPORT;
    case ESOCKTNOSUPPORT: return WSAESOCKTNOSUPPORT;
    case EOPNOTSUPP: return WSAEOPNOTSUPP;
    case EPFNOSUPPORT: return WSAEPFNOSUPPORT;
    case EAFNOSUPPORT: return WSAEAFNOSUPPORT;
    case EADDRINUSE: return WSAEADDRINUSE;
    case EADDRNOTAVAIL: return WSAEADDRNOTAVAIL;
    case ENETDOWN: return WSAENETDOWN;
    case ENETUNREACH: return WSAENETUNREACH;
    case ENETRESET: return WSAENETRESET;
    case ECONNABORTED: return WSAECONNABORTED;
    case ECONNRESET: return WSAECONNRESET;
    case ENOBUFS: return WSAENOBUFS;
    case EISCONN: return WSAEISCONN;
    case ENOTCONN: return WSAENOTCONN;
    case ESHUTDOWN: return WSAESHUTDOWN;
    case EPIPE: return WSAESHUTDOWN;
    case ETOOMANYREFS: return WSAETOOMANYREFS;
    case ETIMEDOUT: return WSAETIMEDOUT;
    case ECONNREFUSED: return WSAECONNREFUSED;
    case ELOOP: return WSAELOOP;
    case ENAMETOOLONG: return WSAENAMETOOLONG;
    case EHOSTDOWN: return WSAEHOSTDOWN;
    case EHOSTUNREACH: return WSAEHOSTUNREACH;
    case ENOTEMPTY: return WSAENOTEMPTY;
    case EUSERS: return WSAEUSERS;
    case EDQUOT: return WSAEDQUOT;
    case ESTALE: return WSAESTALE;
    case EREMOTE: return WSAEREMOTE;
    default: return WSAEINVAL;
    }
}

int Fail(int inWinsockError)
{
    errno = inWinsockError;
    return SOCKET_ERROR;
}

bool IsListening(int inSocket)
{
    int theValue = 0;
    socklen_t theLength = sizeof theValue;
    return ::getsockopt(inSocket, SOL_SOCKET, SO_ACCEPTCONN, &theValue, &theLength) == 0 && theValue;
}

bool IsStream(int inSocket)
{
    int theType = 0;
    socklen_t theLength = sizeof theType;
    return ::getsockopt(inSocket, SOL_SOCKET, SO_TYPE, &theType, &theLength) == 0 && theType == SOCK_STREAM;
}

bool IsConnected(int inSocket)
{
    sockaddr_storage theAddress;
    socklen_t theLength = sizeof theAddress;
    return ::getpeername(inSocket, (sockaddr*)&theAddress, &theLength) == 0;
}

// A non-blocking connect that has not finished
bool IsConnecting(int inSocket)
{
    tcp_info theInfo = {};
    socklen_t theLength = sizeof theInfo;
    return ::getsockopt(inSocket, IPPROTO_TCP, TCP_INFO, &theInfo, &theLength) == 0 && theInfo.tcpi_state == TCP_SYN_SENT;
}

// ---- WSAEventSelect ----
struct Selection {
    unsigned mId;               // tells a socket from a later one with the same descriptor
    HANDLE mEvent;
    long mEvents;               // what the program asked for
    long mArmed;                // what may set the event now
    bool mConnecting = false;
    bool mQuiet = false;        // poll reported only what nothing armed asks for: wait for a re-enabling call
};

std::mutex gSelectionsLock;
std::map<int, Selection> gSelections;
unsigned gNextId = 1;
int gWake = -1;

void Wake()
{
    uint64_t theOne = 1;
    ssize_t theWritten = write(gWake, &theOne, sizeof theOne);
    (void)theWritten;
}

short PollEventsOf(const Selection& inSelection)
{
    short theEvents = 0;
    if (inSelection.mArmed & (FD_READ | FD_ACCEPT | FD_CLOSE))
        theEvents |= POLLIN;
    if (inSelection.mArmed & FD_OOB)
        theEvents |= POLLPRI;
    if ((inSelection.mArmed & FD_WRITE) || inSelection.mConnecting)
        theEvents |= POLLOUT;                   // a connect ends writable or with an error, selected or not
    if (inSelection.mArmed & FD_CLOSE)
        theEvents |= POLLRDHUP;
    return theEvents;
}

// The network events that poll's answer means for this socket
long EventsOf(int inSocket, const Selection& inSelection, short inReturned)
{
    long theEvents = 0;
    long theArmed = inSelection.mArmed;
    if (inSelection.mConnecting) {
        if (inReturned & (POLLOUT | POLLERR | POLLHUP)) {
            theEvents |= FD_CONNECT;
            if (!(inReturned & POLLERR))
                theEvents |= FD_WRITE;
        }
        return theEvents;
    }
    if (inReturned & (POLLIN | POLLRDHUP | POLLHUP | POLLERR)) {
        if (IsListening(inSocket))
            theEvents |= FD_ACCEPT;
        else if (!IsStream(inSocket))
            theEvents |= (inReturned & POLLIN) ? FD_READ : 0;
        else {
            int thePending = 0;
            ioctl(inSocket, FIONREAD, &thePending);
            theEvents |= thePending > 0 ? FD_READ : FD_CLOSE;     // nothing to read: the peer closed or reset
        }
    }
    if (inReturned & POLLPRI)
        theEvents |= FD_OOB;
    if ((inReturned & POLLOUT) && !(inReturned & POLLERR))
        theEvents |= FD_WRITE;
    return theEvents & theArmed;
}

void Poller()
{
    std::vector<pollfd> theFds;
    std::vector<unsigned> theIds;
    for (;;) {
        theFds.assign(1, pollfd{gWake, POLLIN, 0});
        theIds.assign(1, 0);
        {
            std::lock_guard<std::mutex> theLock(gSelectionsLock);
            for (const auto& theEntry : gSelections) {
                short theEvents = PollEventsOf(theEntry.second);
                if (theEvents && !theEntry.second.mQuiet) {
                    theFds.push_back(pollfd{theEntry.first, theEvents, 0});
                    theIds.push_back(theEntry.second.mId);
                }
            }
        }
        if (poll(theFds.data(), theFds.size(), -1) < 0)
            continue;
        if (theFds[0].revents) {
            uint64_t theCount;
            ssize_t theRead = read(gWake, &theCount, sizeof theCount);
            (void)theRead;
        }
        std::lock_guard<std::mutex> theLock(gSelectionsLock);
        for (size_t i = 1; i < theFds.size(); ++i) {
            if (!theFds[i].revents)
                continue;
            auto theFound = gSelections.find(theFds[i].fd);
            if (theFound == gSelections.end() || theFound->second.mId != theIds[i])
                continue;                               // closed, or selected again, while poll ran
            Selection& theSelection = theFound->second;
            long theEvents = EventsOf(theFds[i].fd, theSelection, theFds[i].revents);
            if (theFds[i].revents & (POLLOUT | POLLERR | POLLHUP))
                theSelection.mConnecting = false;
            if (!theEvents) {
                // At the end of a stream with FD_CLOSE not selected, FD_READ has nothing more to report
                if (theFds[i].revents & (POLLRDHUP | POLLHUP | POLLERR))
                    theSelection.mArmed &= ~FD_READ;
                theSelection.mQuiet = true;
                continue;
            }
            theSelection.mArmed &= ~theEvents;
            SetEvent(theSelection.mEvent);
        }
    }
}

void StartPoller()
{
    static std::once_flag sStarted;
    std::call_once(sStarted, [] {
        gWake = eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        std::thread(Poller).detach();
    });
}

// The calls that re-enable network events: the events, those of them selected, may set the event again
void Rearm(int inSocket, long inEvents, bool inConnecting = false)
{
    std::lock_guard<std::mutex> theLock(gSelectionsLock);
    auto theFound = gSelections.find(inSocket);
    if (theFound == gSelections.end())
        return;
    Selection& theSelection = theFound->second;
    long theArmed = theSelection.mArmed | (inEvents & theSelection.mEvents);
    if (theArmed == theSelection.mArmed && !theSelection.mQuiet && (theSelection.mConnecting || !inConnecting))
        return;
    theSelection.mArmed = theArmed;
    theSelection.mQuiet = false;
    theSelection.mConnecting = theSelection.mConnecting || inConnecting;
    Wake();
}

} // namespace

extern "C" {

int WINAPI WSAStartup(WORD inVersion, LPWSADATA outData)
{
    BYTE theMajor = LOBYTE(inVersion), theMinor = HIBYTE(inVersion);
    if (theMajor < 1 || (theMajor == 1 && theMinor < 1))
        return WSAVERNOTSUPPORTED;
    if (!outData)
        return WSAEFAULT;
    memset(outData, 0, sizeof *outData);
    outData->wVersion = theMajor > 2 || (theMajor == 2 && theMinor >= 2) ? MAKEWORD(2, 2) : inVersion;
    outData->wHighVersion = MAKEWORD(2, 2);
    strcpy(outData->szDescription, "WinSock 2.0");
    strcpy(outData->szSystemStatus, "Running");
    outData->iMaxSockets = 32767;
    outData->iMaxUdpDg = 65467;
    return 0;
}

int WINAPI WSACleanup(void) { return 0; }

int WINAPI WSAGetLastError(void) { return WinsockError(errno); }

void WINAPI WSASetLastError(int inError) { errno = inError; }

WSAEVENT WINAPI WSACreateEvent(void) { return CreateEventA(NULL, TRUE, FALSE, NULL); }

BOOL WINAPI WSACloseEvent(WSAEVENT inEvent) { return CloseHandle(inEvent); }

int WINAPI closesocket(SOCKET inSocket)
{
    {
        std::lock_guard<std::mutex> theLock(gSelectionsLock);
        if (gSelections.erase((int)inSocket))
            Wake();
    }
    return close((int)inSocket) == 0 ? 0 : Fail(WinsockError(errno));
}

int WINAPI ioctlsocket(SOCKET inSocket, long inCommand, u_long* ioArgument)
{
    int theValue = ioArgument ? (int)*ioArgument : 0;
    switch (inCommand) {
    case FIONBIO: {
        std::lock_guard<std::mutex> theLock(gSelectionsLock);
        if (!theValue && gSelections.count((int)inSocket))
            return Fail(WSAEINVAL);                 // a socket under WSAEventSelect stays non-blocking
        if (ioctl((int)inSocket, FIONBIO, &theValue) != 0)
            return Fail(WinsockError(errno));
        return 0;
    }
    case FIONREAD:
    case SIOCATMARK:
        if (ioctl((int)inSocket, inCommand, &theValue) != 0)
            return Fail(WinsockError(errno));
        *ioArgument = (u_long)theValue;
        return 0;
    default:
        return Fail(WSAEINVAL);
    }
}

int WINAPI WSAEventSelect(SOCKET inSocket, WSAEVENT inEvent, long inEvents)
{
    int theSocket = (int)inSocket;
    int theNonBlocking = 1;
    if (ioctl(theSocket, FIONBIO, &theNonBlocking) != 0)
        return Fail(WinsockError(errno));
    std::lock_guard<std::mutex> theLock(gSelectionsLock);
    if (!inEvent || !inEvents) {
        if (gSelections.erase(theSocket) && gWake >= 0)
            Wake();
        return 0;
    }
    // A later call replaces the earlier one, and the events that hold now are reported
    Selection& theSelection = gSelections[theSocket];
    theSelection = Selection{gNextId++, inEvent, inEvents, inEvents & ~FD_CONNECT};
    if (IsStream(theSocket) && !IsConnected(theSocket)) {
        theSelection.mConnecting = IsConnecting(theSocket);
        if (theSelection.mConnecting)
            theSelection.mArmed |= inEvents & FD_CONNECT;
        else
            theSelection.mArmed &= ~FD_WRITE;
    }
    StartPoller();
    Wake();
    return 0;
}

int WINAPI ee_ws_connect(SOCKET inSocket, const struct sockaddr* inAddress, int inLength)
{
    int theResult = ::connect((int)inSocket, inAddress, (socklen_t)inLength);
    int theError = errno;
    if ((theResult == 0 || theError == EINPROGRESS) && IsStream((int)inSocket))
        Rearm((int)inSocket, FD_CONNECT | FD_WRITE, true);
    errno = theError;
    return theResult;
}

SOCKET WINAPI ee_ws_accept(SOCKET inSocket, struct sockaddr* outAddress, int* ioLength)
{
    socklen_t theLength = ioLength ? (socklen_t)*ioLength : 0;
    int theResult = ::accept((int)inSocket, outAddress, ioLength ? &theLength : NULL);
    int theError = errno;
    if (ioLength && theResult >= 0)
        *ioLength = (int)theLength;
    Rearm((int)inSocket, FD_ACCEPT);
    errno = theError;
    return theResult >= 0 ? (SOCKET)theResult : INVALID_SOCKET;
}

int WINAPI ee_ws_send(SOCKET inSocket, const char* inData, int inLength, int inFlags)
{
    int theResult = (int)::send((int)inSocket, inData, (size_t)inLength, inFlags | MSG_NOSIGNAL);
    if (theResult < 0 && errno == EWOULDBLOCK) {
        Rearm((int)inSocket, FD_WRITE);
        errno = EWOULDBLOCK;
    }
    return theResult;
}

int WINAPI ee_ws_sendto(SOCKET inSocket, const char* inData, int inLength, int inFlags, const struct sockaddr* inTo,
                        int inToLength)
{
    int theResult = (int)::sendto((int)inSocket, inData, (size_t)inLength, inFlags | MSG_NOSIGNAL, inTo,
                                  (socklen_t)inToLength);
    if (theResult < 0 && errno == EWOULDBLOCK) {
        Rearm((int)inSocket, FD_WRITE);
        errno = EWOULDBLOCK;
    }
    return theResult;
}

int WINAPI ee_ws_recv(SOCKET inSocket, char* outData, int inLength, int inFlags)
{
    int theResult = (int)::recv((int)inSocket, outData, (size_t)inLength, inFlags);
    int theError = errno;
    Rearm((int)inSocket, FD_READ | FD_OOB);
    errno = theError;
    return theResult;
}

int WINAPI ee_ws_recvfrom(SOCKET inSocket, char* outData, int inLength, int inFlags, struct sockaddr* outFrom,
                          int* ioFromLength)
{
    socklen_t theLength = ioFromLength ? (socklen_t)*ioFromLength : 0;
    int theResult = (int)::recvfrom((int)inSocket, outData, (size_t)inLength, inFlags, outFrom,
                                    ioFromLength ? &theLength : NULL);
    int theError = errno;
    if (ioFromLength && theResult >= 0)
        *ioFromLength = (int)theLength;
    Rearm((int)inSocket, FD_READ | FD_OOB);
    errno = theError;
    return theResult;
}

// SO_ERROR gives the pending error as a Winsock code, as WON's connect wait reads it
int WINAPI ee_ws_getsockopt(SOCKET inSocket, int inLevel, int inName, char* outValue, int* ioLength)
{
    socklen_t theLength = (socklen_t)*ioLength;
    int theResult = ::getsockopt((int)inSocket, inLevel, inName, outValue, &theLength);
    *ioLength = (int)theLength;
    if (theResult == 0 && inLevel == SOL_SOCKET && inName == SO_ERROR && theLength >= sizeof(int)) {
        int theError;
        memcpy(&theError, outValue, sizeof theError);
        theError = WinsockError(theError);
        memcpy(outValue, &theError, sizeof theError);
    }
    return theResult;
}

struct hostent* WINAPI ee_ws_gethostbyname(const char* inName)
{
    struct hostent* theHost = ::gethostbyname(inName);
    if (!theHost)
        errno = h_errno == HOST_NOT_FOUND ? WSAHOST_NOT_FOUND
              : h_errno == TRY_AGAIN      ? WSATRY_AGAIN
              : h_errno == NO_DATA        ? WSANO_DATA
                                          : WSANO_RECOVERY;
    return theHost;
}

// The ready sockets of each set, by poll(); the count over the three sets. Winsock refuses three empty sets.
int WINAPI ee_ws_select(int, ee_ws_fd_set* ioRead, ee_ws_fd_set* ioWrite, ee_ws_fd_set* ioExcept,
                        const struct timeval* inTimeout)
{
    ee_ws_fd_set* theSets[3] = {ioRead, ioWrite, ioExcept};
    const short kAsks[3] = {POLLIN, POLLOUT, POLLPRI};
    std::vector<pollfd> theFds;
    for (int i = 0; i < 3; ++i)
        for (u_int j = 0; theSets[i] && j < theSets[i]->fd_count; ++j) {
            int theSocket = (int)theSets[i]->fd_array[j];
            auto theFd = std::find_if(theFds.begin(), theFds.end(), [&](const pollfd& f) { return f.fd == theSocket; });
            if (theFd == theFds.end())
                theFds.push_back(pollfd{theSocket, kAsks[i], 0});
            else
                theFd->events |= kAsks[i];
        }
    if (theFds.empty())
        return Fail(WSAEINVAL);
    using Clock = std::chrono::steady_clock;
    Clock::time_point theDeadline = Clock::now();
    if (inTimeout)
        theDeadline += std::chrono::seconds(inTimeout->tv_sec) + std::chrono::microseconds(inTimeout->tv_usec);
    int theReady;
    do {
        int theWait = -1;
        if (inTimeout) {
            auto theLeft = std::chrono::duration_cast<std::chrono::milliseconds>(
                               theDeadline - Clock::now() + std::chrono::nanoseconds(999999)).count();
            theWait = (int)std::max<long long>(0, theLeft);
        }
        theReady = poll(theFds.data(), theFds.size(), theWait);
    } while (theReady < 0 && errno == EINTR);
    if (theReady < 0)
        return Fail(WinsockError(errno));
    int theCount = 0;
    for (int i = 0; i < 3; ++i) {
        if (!theSets[i])
            continue;
        ee_ws_fd_set theReadySet = {};
        for (u_int j = 0; j < theSets[i]->fd_count; ++j) {
            int theSocket = (int)theSets[i]->fd_array[j];
            short r = std::find_if(theFds.begin(), theFds.end(), [&](const pollfd& f) { return f.fd == theSocket; })
                          ->revents;
            if (r & POLLNVAL)
                return Fail(WSAENOTSOCK);
            // A failed connect shows in the exception set, not as writable; a reset or closed stream reads
            bool theIn = i == 0 ? (r & (POLLIN | POLLHUP | POLLERR)) != 0
                       : i == 1 ? (r & POLLOUT) && !(r & POLLERR)
                                : (r & POLLPRI) || ((r & POLLERR) && !IsConnected(theSocket));
            if (theIn)
                theReadySet.fd_array[theReadySet.fd_count++] = (SOCKET)theSocket;
        }
        *theSets[i] = theReadySet;
        theCount += (int)theReadySet.fd_count;
    }
    return theCount;
}

// The IPv4 adapters that are up, loopback left out, in interface order: name, MAC address and first address
DWORD WINAPI GetAdaptersInfo(PIP_ADAPTER_INFO outInfo, PULONG ioSize)
{
    if (!ioSize)
        return ERROR_INVALID_PARAMETER;
    ifaddrs* theList = nullptr;
    if (getifaddrs(&theList) != 0)
        return ERROR_NO_DATA;
    std::map<unsigned, IP_ADAPTER_INFO> theAdapters;
    for (ifaddrs* a = theList; a; a = a->ifa_next) {
        if (!a->ifa_addr || a->ifa_addr->sa_family != AF_INET || !(a->ifa_flags & IFF_UP) ||
            (a->ifa_flags & IFF_LOOPBACK))
            continue;
        unsigned theIndex = if_nametoindex(a->ifa_name);
        if (theAdapters.count(theIndex))
            continue;
        IP_ADAPTER_INFO& theInfo = theAdapters[theIndex];
        memset(&theInfo, 0, sizeof theInfo);
        snprintf(theInfo.AdapterName, sizeof theInfo.AdapterName, "%s", a->ifa_name);
        snprintf(theInfo.Description, sizeof theInfo.Description, "%s", a->ifa_name);
        theInfo.Index = theIndex;
        theInfo.Type = MIB_IF_TYPE_ETHERNET;
        inet_ntop(AF_INET, &((sockaddr_in*)a->ifa_addr)->sin_addr, theInfo.IpAddressList.IpAddress.String,
                  sizeof theInfo.IpAddressList.IpAddress.String);
        if (a->ifa_netmask)
            inet_ntop(AF_INET, &((sockaddr_in*)a->ifa_netmask)->sin_addr, theInfo.IpAddressList.IpMask.String,
                      sizeof theInfo.IpAddressList.IpMask.String);
        strcpy(theInfo.GatewayList.IpAddress.String, "0.0.0.0");
        strcpy(theInfo.GatewayList.IpMask.String, "0.0.0.0");
    }
    for (ifaddrs* a = theList; a; a = a->ifa_next) {
        if (!a->ifa_addr || a->ifa_addr->sa_family != AF_PACKET)
            continue;
        auto theFound = theAdapters.find(if_nametoindex(a->ifa_name));
        const sockaddr_ll* theLink = (const sockaddr_ll*)a->ifa_addr;
        if (theFound != theAdapters.end() && theLink->sll_halen <= MAX_ADAPTER_ADDRESS_LENGTH) {
            theFound->second.AddressLength = theLink->sll_halen;
            memcpy(theFound->second.Address, theLink->sll_addr, theLink->sll_halen);
        }
    }
    freeifaddrs(theList);
    if (theAdapters.empty())
        return ERROR_NO_DATA;
    ULONG theNeeded = (ULONG)(theAdapters.size() * sizeof(IP_ADAPTER_INFO));
    if (!outInfo || *ioSize < theNeeded) {
        *ioSize = theNeeded;
        return ERROR_BUFFER_OVERFLOW;
    }
    IP_ADAPTER_INFO* theOut = outInfo;
    for (const auto& theEntry : theAdapters) {
        *theOut = theEntry.second;
        theOut->Next = theOut + 1;
        ++theOut;
    }
    theOut[-1].Next = NULL;
    return ERROR_SUCCESS;
}

} // extern "C"
