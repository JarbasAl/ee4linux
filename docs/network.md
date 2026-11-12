# Network

`src/Port/Linux/Win32Socket.cpp`, `src/Port/EEPortLinux.cpp`, `src/Port/EEPortWonLinks.cpp`

The game's multiplayer runs on the WON library inside the reconstructed executable, and its TCP/IP transport is the
`TCP Protocol.dll` plug-in. Both call Winsock. The layer implements Winsock 2 on the system's BSD sockets.

## Winsock

A `SOCKET` is the file descriptor. The Winsock calls keep Windows' names, structures and error codes:

| Call | Note |
| --- | --- |
| `WSAStartup`, `WSACleanup` | version negotiation, as Winsock 2.2 |
| `socket`, `bind`, `listen`, `connect`, `accept`, `closesocket` | straight onto the BSD calls; `connect` and `accept` re-enable the selected network events |
| `send`, `recv`, `sendto`, `recvfrom` | `send` raises no `SIGPIPE`; a would-block send re-enables `FD_WRITE` |
| `select` | Windows' `fd_set`: a count and an array of sockets, because WON reads those fields |
| `ioctlsocket` | `FIONBIO`, `FIONREAD` |
| `setsockopt`, `getsockopt` | `SO_ERROR` is reported as a Winsock code |
| `gethostbyname`, `gethostname`, `getaddrinfo`, `inet_addr`, `inet_ntoa` | with Winsock's error codes |
| `WSAEventSelect`, `WSAEnumNetworkEvents` | follows Winsock's rule: a network event sets the event once, and only the call that re-enables it (a `recv` for `FD_READ`, an `accept` for `FD_ACCEPT`, a send that would block for `FD_WRITE`) lets it set again. A poller thread watches the selected sockets with `poll()`. |
| `GetAdaptersInfo` | the machine's interfaces, from `getifaddrs`, in `IP_ADAPTER_INFO` form |

The BSD calls whose Windows behaviour differs go to the layer under `ee_ws_` names
(`include/port/linux/winsock2.h`): `select` with the Windows `fd_set`, `connect`, `accept`, `send`, `recv`,
`getsockopt`, `gethostbyname`.

## The transport

`TCP Protocol.dll` is rebuilt from source and runs as a shared object named like the retail file, so the game's
network code loads it as it would the original. The engine's network layer keeps the game's packets and its version
check, including the retail checksum comparison ([build](build.md)).

## WON and GameSpy

The WON library is rebuilt from source. Its services no longer exist, so online play cannot be tested; the LAN and
Direct Connect paths are what the reconstruction's own tests exercise.

GameSpy's query reporting (`gqueryreporting.h`) has no implementation in the native build: `qr_init` fails with the
error a closed socket gives, and `qr_process_queries` and `qr_send_statechanged` do nothing. WON plays on without
advertising the game to a GameSpy master server, which no longer exists either.

## What to expect

A LAN game between a native build and another image works through the same transport code as on Windows: the
simulation, the packets and the version check are the reconstruction's, not the port's. A game against a retail
installation is limited by the retail checksum comparison, as for the Windows images.

Online services (WON, GameSpy, the AoC lobby) are gone for every build, retail included.
