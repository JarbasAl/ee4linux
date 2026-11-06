// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK new derived owner.
#ifndef EE_WONAPI_WONRECVSOMEBYTESOP_H
#define EE_WONAPI_WONRECVSOMEBYTESOP_H


#include "ee/wonapi/WONRouting.h"

namespace WONAPI
{

class RecvSomeBytesOp : public SocketOp
{
protected:
	char *mCurRecv;
	ByteBufferPtr mRecvBytes;
	unsigned short mMaxBytes;
	bool mDoDelay;
	DWORD mStartTime;

	virtual ~RecvSomeBytesOp();

	virtual WONStatus Virtual0C();
	virtual WONStatus Virtual10();
	virtual AsyncOp* Duplicate() { return new RecvSomeBytesOp(mMaxBytes,mSocket,mDoDelay); }

public:
	static DWORD mInitDelay;

public:
	RecvSomeBytesOp(unsigned short theMaxBytes = 1024, AsyncSocket *theSocket = NULL, bool doDelay = false);

	const ByteBuffer* GetBytes() const { return mRecvBytes; }

};

}; // namespace WONAPI

#endif