// C:\EEX\Low-Level Engine\Network Engine\NEConnection.h (RECONSTRUCTION). A connection of a protocol plug-in: the
// plug-in's connection handle (+0, the plug-in gets a pointer to it) and the owning NEProtocol. Every member is inline;
// NEProtocol.cpp is the first unit to emit them (0x4440f..0x4445d). Created by NEProtocol::Accept/Connect.
#ifndef LLE_NECONNECTION_H
#define LLE_NECONNECTION_H

#include "lle/NEProtocol.h"

class LLE_API NEConnection {
    friend class NEProtocol;
public:
    long Close() { return this->mProtocol->Close(this); }
    void* GetData() const { return this->mData; }
    NEProtocol* GetProtocol() { return this->mProtocol; }
    bool IsAlive() { return this->mProtocol->IsAlive(this); }
    long Receive(unsigned long inSize, unsigned char* outBuffer, unsigned long& outReceived)
        { return this->mProtocol->Receive(this, inSize, outBuffer, outReceived); }
    long Send(const unsigned char* inData, unsigned long inLength) { return this->mProtocol->Send(this, inData, inLength); }
    void SetData(void* inData) { this->mData = inData; }
private:
    NEConnection(NEProtocol* inProtocol) : mData(NULL), mProtocol(inProtocol) {}
    ~NEConnection() {}                          // user-declared: delete checks for NULL (NEProtocol::Close)

    void* mData;                            // +0x00 the plug-in's handle
    NEProtocol* mProtocol;                  // +0x04
};

#endif
