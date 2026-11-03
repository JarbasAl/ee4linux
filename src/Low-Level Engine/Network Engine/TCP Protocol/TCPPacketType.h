// src/Low-Level Engine/Network Engine/TCP Protocol/TCPPacketType.h (RECONSTRUCTION). Named integer constants; names PROVISIONAL unless the comment says T1.
#ifndef EE_TCPPACKETTYPE_H
#define EE_TCPPACKETTYPE_H

// Type byte (+0x01) of the LAN game-search datagrams (port 33334): QueryGames sends a GameQuery, the host's search thread answers with a GameAnnouncement. Names PROVISIONAL (the reconstructed struct names).
enum TCPSearchPacketType {
    kSearchPacketGameQuery = 0x15,       // T3 PROVISIONAL: QueryGames sends GameQuery (sizeof 0x43) with type 0x15; the responder accepts 0x43 bytes of type 0x15
    kSearchPacketGameAnnouncement = 0x16,  // T3 PROVISIONAL: the responder sends GameAnnouncement (sizeof 0x88) with type 0x16; QueryGames accepts 0x88 bytes of type 0x16
};

#endif // EE_TCPPACKETTYPE_H
