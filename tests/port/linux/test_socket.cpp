// Tests of the native build's Winsock (src/Port/Linux/Win32Socket.cpp) over loopback: start-up, Winsock error codes,
// select on the Windows fd_set, ioctlsocket, the WSAEventSelect rules (an event is set once, then again only after
// the call that re-enables it), SIGPIPE, and the iphlpapi.dll that LoadLibrary finds. The expected error numbers are
// Winsock's as Microsoft documents them, written out rather than taken from the port's header.
#include "../../../src/Port/Linux/Win32Internal.h"

#include <iphlpapi.h>

#include <cstdio>
#include <cstring>

static int sFailures;
static int sChecks;

#define CHECK(condition)                                                                 \
    do {                                                                                 \
        ++sChecks;                                                                       \
        if (!(condition)) {                                                              \
            ++sFailures;                                                                 \
            fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition);         \
        }                                                                                \
    } while (0)

enum {
    kWouldBlock = 10035,
    kInvalid = 10022,
    kNotSocket = 10038,
    kConnectionRefused = 10061,
    kHostNotFound = 11001,
    kTryAgain = 11002,
};

static sockaddr_in Loopback(unsigned short inPort)
{
    sockaddr_in theAddress = {};
    theAddress.sin_family = AF_INET;
    theAddress.sin_port = htons(inPort);
    theAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    return theAddress;
}

// A listening socket on a free loopback port, and its port
static SOCKET Listener(unsigned short* outPort)
{
    SOCKET theSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in theAddress = Loopback(0);
    bind(theSocket, (sockaddr*)&theAddress, sizeof theAddress);
    listen(theSocket, 4);
    int theLength = sizeof theAddress;
    getsockname(theSocket, (sockaddr*)&theAddress, &theLength);
    *outPort = ntohs(theAddress.sin_port);
    return theSocket;
}

// A loopback port nothing listens on: a bound socket's port, released
static unsigned short ClosedPort()
{
    unsigned short thePort;
    SOCKET theSocket = Listener(&thePort);
    closesocket(theSocket);
    return thePort;
}

static bool Signaled(HANDLE inEvent, DWORD inWait) { return WaitForSingleObject(inEvent, inWait) == WAIT_OBJECT_0; }

static void TestStartup()
{
    WSADATA theData;
    CHECK(WSAStartup(MAKEWORD(1, 1), &theData) == 0 && theData.wVersion == 0x0101 && theData.wHighVersion == 0x0202);
    CHECK(WSAStartup(MAKEWORD(2, 2), &theData) == 0 && theData.wVersion == 0x0202);
    CHECK(WSAStartup(MAKEWORD(3, 0), &theData) == 0 && theData.wVersion == 0x0202);
    CHECK(WSAStartup(MAKEWORD(1, 0), &theData) == 10092);                        // WSAVERNOTSUPPORTED
    WSASetLastError(kInvalid);
    CHECK(WSAGetLastError() == kInvalid);
    WSASetLastError(0);
    CHECK(WSAGetLastError() == 0);
}

static void TestErrors()
{
    // A blocking connect to a closed port is refused
    SOCKET theSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in theAddress = Loopback(ClosedPort());
    CHECK(connect(theSocket, (sockaddr*)&theAddress, sizeof theAddress) == SOCKET_ERROR &&
          WSAGetLastError() == kConnectionRefused);
    closesocket(theSocket);
    CHECK(closesocket(theSocket) == SOCKET_ERROR && WSAGetLastError() == kNotSocket);

    // A non-blocking connect reports WSAEWOULDBLOCK, not WSAEINPROGRESS; the refusal then shows in the exception
    // set, and SO_ERROR gives it as a Winsock code (WON's WaitForConnect)
    theSocket = socket(AF_INET, SOCK_STREAM, 0);
    u_long theOn = 1;
    CHECK(ioctlsocket(theSocket, FIONBIO, &theOn) == 0);
    int theResult = connect(theSocket, (sockaddr*)&theAddress, sizeof theAddress);
    CHECK(theResult == 0 || WSAGetLastError() == kWouldBlock);
    fd_set theWrite, theExcept;
    FD_ZERO(&theWrite);
    FD_ZERO(&theExcept);
    FD_SET(theSocket, &theWrite);
    FD_SET(theSocket, &theExcept);
    timeval theTime = {2, 0};
    CHECK(select(0, NULL, &theWrite, &theExcept, &theTime) == 1);
    CHECK(theWrite.fd_count == 0 && theExcept.fd_count == 1 && FD_ISSET(theSocket, &theExcept));
    int theError = 0, theLength = sizeof theError;
    CHECK(getsockopt(theSocket, SOL_SOCKET, SO_ERROR, (char*)&theError, &theLength) == 0 &&
          theError == kConnectionRefused);
    closesocket(theSocket);

    // A name that cannot exist
    CHECK(gethostbyname("no-such-host.invalid") == NULL &&
          (WSAGetLastError() == kHostNotFound || WSAGetLastError() == kTryAgain));
    CHECK(gethostbyname("localhost") != NULL);
}

static void TestSelectAndIoctl()
{
    unsigned short thePort;
    SOCKET theListener = Listener(&thePort);
    SOCKET theClient = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in theAddress = Loopback(thePort);
    CHECK(connect(theClient, (sockaddr*)&theAddress, sizeof theAddress) == 0);
    SOCKET theServer = accept(theListener, NULL, NULL);
    CHECK(theServer != INVALID_SOCKET);

    fd_set theRead, theWrite;
    timeval theNoWait = {0, 0};
    FD_ZERO(&theRead);
    FD_SET(theServer, &theRead);
    FD_SET(theListener, &theRead);
    CHECK(select(0, &theRead, NULL, NULL, &theNoWait) == 0 && theRead.fd_count == 0);
    CHECK(send(theClient, "hello", 5, 0) == 5);
    // Ready in two sets at once: one count for each
    FD_ZERO(&theRead);
    FD_ZERO(&theWrite);
    FD_SET(theServer, &theRead);
    FD_SET(theServer, &theWrite);
    FD_SET(theClient, &theWrite);
    timeval theWait = {2, 0};
    CHECK(select(0, &theRead, &theWrite, NULL, &theWait) == 3 && theRead.fd_count == 1 && theWrite.fd_count == 2);
    u_long thePending = 0;
    CHECK(ioctlsocket(theServer, FIONREAD, &thePending) == 0 && thePending == 5);
    CHECK(ioctlsocket(theServer, 0x7fff, &thePending) == SOCKET_ERROR && WSAGetLastError() == kInvalid);
    // Three empty sets are an error in Winsock
    FD_ZERO(&theRead);
    CHECK(select(0, &theRead, NULL, NULL, &theNoWait) == SOCKET_ERROR && WSAGetLastError() == kInvalid);
    // A non-blocking socket with nothing to read
    char theBuffer[16];
    CHECK(recv(theServer, theBuffer, sizeof theBuffer, 0) == 5 && memcmp(theBuffer, "hello", 5) == 0);
    u_long theOn = 1;
    ioctlsocket(theServer, FIONBIO, &theOn);
    CHECK(recv(theServer, theBuffer, sizeof theBuffer, 0) == SOCKET_ERROR && WSAGetLastError() == kWouldBlock);

    // Sending to a closed connection fails with an error, and no SIGPIPE ends the program
    closesocket(theServer);
    int theSent = 0;
    for (int i = 0; i < 4 && theSent != SOCKET_ERROR; ++i) {
        theSent = send(theClient, "x", 1, 0);
        Sleep(20);
    }
    CHECK(theSent == SOCKET_ERROR && WSAGetLastError() != kWouldBlock);
    closesocket(theClient);
    closesocket(theListener);
}

static void TestEventSelect()
{
    unsigned short thePort;
    SOCKET theListener = Listener(&thePort);
    HANDLE theAcceptEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    HANDLE theStale = CreateEventA(NULL, FALSE, FALSE, NULL);
    // The later selection replaces the earlier one (the TCP plug-in selects FD_READ, then FD_ACCEPT)
    CHECK(WSAEventSelect(theListener, theStale, FD_READ) == 0);
    CHECK(WSAEventSelect(theListener, theAcceptEvent, FD_ACCEPT) == 0);
    CHECK(!Signaled(theAcceptEvent, 100));

    sockaddr_in theAddress = Loopback(thePort);
    SOCKET theClient = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(connect(theClient, (sockaddr*)&theAddress, sizeof theAddress) == 0);
    CHECK(Signaled(theAcceptEvent, 2000) && !Signaled(theStale, 0));
    // A second waiting connection does not set it again before accept re-enables FD_ACCEPT
    SOCKET theSecond = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(connect(theSecond, (sockaddr*)&theAddress, sizeof theAddress) == 0);
    CHECK(!Signaled(theAcceptEvent, 150));
    SOCKET theServer = accept(theListener, NULL, NULL);
    CHECK(theServer != INVALID_SOCKET);
    CHECK(Signaled(theAcceptEvent, 2000));          // the second one still waits
    SOCKET theServer2 = accept(theListener, NULL, NULL);
    CHECK(theServer2 != INVALID_SOCKET && !Signaled(theAcceptEvent, 150));
    // The listener became non-blocking: with nothing waiting, accept would block
    CHECK(accept(theListener, NULL, NULL) == INVALID_SOCKET && WSAGetLastError() == kWouldBlock);

    // FD_READ: set once for the data, again after each recv while data is left, then not at all
    HANDLE theRead = CreateEventA(NULL, FALSE, FALSE, NULL);
    CHECK(WSAEventSelect(theServer, theRead, FD_READ | FD_CLOSE) == 0);
    CHECK(!Signaled(theRead, 100));
    char theBuffer[16];
    CHECK(recv(theServer, theBuffer, 1, 0) == SOCKET_ERROR && WSAGetLastError() == kWouldBlock);
    CHECK(send(theClient, "abc", 3, 0) == 3);
    CHECK(Signaled(theRead, 2000));
    CHECK(!Signaled(theRead, 150));                 // no recv yet: not set again
    CHECK(recv(theServer, theBuffer, 1, 0) == 1 && theBuffer[0] == 'a');
    CHECK(Signaled(theRead, 2000));                 // two bytes are left
    CHECK(recv(theServer, theBuffer, sizeof theBuffer, 0) == 2);
    CHECK(!Signaled(theRead, 150));
    // FD_CLOSE when the peer closes, once
    closesocket(theClient);
    CHECK(Signaled(theRead, 2000));
    CHECK(recv(theServer, theBuffer, sizeof theBuffer, 0) == 0);
    CHECK(!Signaled(theRead, 150));
    // A socket under WSAEventSelect cannot be made blocking
    u_long theOff = 0;
    CHECK(ioctlsocket(theServer, FIONBIO, &theOff) == SOCKET_ERROR && WSAGetLastError() == kInvalid);

    // FD_WRITE on a connected socket: set once, not again while sends succeed
    HANDLE theWrite = CreateEventA(NULL, FALSE, FALSE, NULL);
    CHECK(WSAEventSelect(theSecond, theWrite, FD_WRITE) == 0);
    CHECK(Signaled(theWrite, 2000));
    CHECK(send(theSecond, "z", 1, 0) == 1);
    CHECK(!Signaled(theWrite, 150));

    // FD_CONNECT for a connect begun after the selection, and FD_WRITE with it
    HANDLE theConnect = CreateEventA(NULL, TRUE, FALSE, NULL);
    SOCKET theThird = socket(AF_INET, SOCK_STREAM, 0);
    CHECK(WSAEventSelect(theThird, theConnect, FD_CONNECT) == 0);
    CHECK(!Signaled(theConnect, 100));              // not connected: nothing to report
    int theResult = connect(theThird, (sockaddr*)&theAddress, sizeof theAddress);
    CHECK(theResult == 0 || WSAGetLastError() == kWouldBlock);
    CHECK(Signaled(theConnect, 2000));

    // Closing a selected socket leaves the others working
    closesocket(theServer);
    HANDLE theOther = CreateEventA(NULL, FALSE, FALSE, NULL);
    CHECK(WSAEventSelect(theServer2, theOther, FD_READ) == 0);
    CHECK(Signaled(theOther, 2000));                // the "z" sent above
    WSAEventSelect(theServer2, NULL, 0);
    closesocket(theServer2);
    closesocket(theSecond);
    closesocket(theThird);
    closesocket(theListener);
    for (HANDLE h : {theAcceptEvent, theStale, theRead, theWrite, theConnect, theOther})
        CloseHandle(h);
}

static void TestAdapters()
{
    HMODULE theModule = LoadLibraryA("IPHLPAPI.DLL");
    typedef DWORD(WINAPI * Function)(PIP_ADAPTER_INFO, PULONG);
    Function theFunction = theModule ? (Function)(void*)GetProcAddress(theModule, "GetAdaptersInfo") : NULL;
    CHECK(theFunction == &GetAdaptersInfo);
    ULONG theSize = 0;
    DWORD theResult = GetAdaptersInfo(NULL, &theSize);
    CHECK(theResult == ERROR_NO_DATA || (theResult == ERROR_BUFFER_OVERFLOW && theSize % sizeof(IP_ADAPTER_INFO) == 0));
    if (theResult == ERROR_BUFFER_OVERFLOW) {
        ULONG theShort = theSize - 1;
        CHECK(GetAdaptersInfo((PIP_ADAPTER_INFO) new char[theSize], &theShort) == ERROR_BUFFER_OVERFLOW &&
              theShort == theSize);
        IP_ADAPTER_INFO* theList = (IP_ADAPTER_INFO*)new char[theSize];
        CHECK(GetAdaptersInfo(theList, &theSize) == ERROR_SUCCESS);
        int theCount = 0;
        for (IP_ADAPTER_INFO* a = theList; a; a = a->Next, ++theCount)
            CHECK(inet_addr(a->IpAddressList.IpAddress.String) != INADDR_NONE &&
                  strcmp(a->IpAddressList.IpAddress.String, "127.0.0.1") != 0);
        CHECK((ULONG)theCount * sizeof(IP_ADAPTER_INFO) == theSize);
    }
    if (theModule)
        FreeLibrary(theModule);
}

int main()
{
    TestStartup();
    TestErrors();
    TestSelectAndIoctl();
    TestEventSelect();
    TestAdapters();
    printf("%d checks, %d failed\n", sChecks, sFailures);
    return sFailures ? 1 : 0;
}
