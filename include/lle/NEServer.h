// C:\EEX\Low-Level Engine\Network Engine\NEServer.h (RECONSTRUCTION). A published game server of a protocol plug-in:
// the plug-in's server handle (+0, the plug-in gets a pointer to it) and the owning NEProtocol. Every member is inline;
// NEMessageManager.cpp is the first unit to emit them (0x41175..0x411fb). Created by NEProtocol::Publish.
#ifndef LLE_NESERVER_H
#define LLE_NESERVER_H

#include "lle/NEProtocol.h"

class LLE_API NEServer {
    friend class NEProtocol;
public:
    long Accept(NEConnection*& outConnection) { return this->mProtocol->Accept(this, outConnection); }
    long AcceptForXfer(NEConnection*& outConnection) { return this->mProtocol->AcceptForXfer(this, outConnection); }
    void AllowNewConnections(bool inAllow) { this->mProtocol->AllowNewConnections(this, inAllow); }
    long Close() { return this->mProtocol->Close(this); }
    void* GetData() const { return this->mData; }
    NEProtocol* GetProtocol() { return this->mProtocol; }
    long ReceiveFrom(unsigned long inSize, unsigned char* outBuffer, unsigned long& outReceived,
                     unsigned long& outIndex)
        { return this->mProtocol->ReceiveFrom(this, inSize, outBuffer, outReceived, outIndex); }
    long SendTo(unsigned long inIndex, const unsigned char* inData, unsigned long inLength)
        { return this->mProtocol->SendTo(this, inIndex, inData, inLength); }
    void SetData(void* inData) { this->mData = inData; }
    long XferSendTo(unsigned long inIndex, const unsigned char* inData, unsigned long inLength)
        { return this->mProtocol->XferSendTo(this, inIndex, inData, inLength); }
private:
    NEServer(NEProtocol* inProtocol) : mData(NULL), mProtocol(inProtocol) {}
    ~NEServer() {}                          // user-declared: delete checks for NULL (NEProtocol::Close)   // ICF-folded with NEConnection's

    void* mData;                            // +0x00 the plug-in's handle
    NEProtocol* mProtocol;                  // +0x04
};

#endif
