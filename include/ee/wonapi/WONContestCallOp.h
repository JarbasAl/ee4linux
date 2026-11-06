// SOURCE_ONLY_NO_NATIVE_ID: exact complete SDK contest owner; const buffer aliases expanded.
#ifndef EE_WONAPI_WONCONTESTCALLOP_H
#define EE_WONAPI_WONCONTESTCALLOP_H
#include "ee/wonapi/WONRouting.h"
#include "ee/wonapi/WONAuthPeerData.h"

namespace WONAPI
{

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ContestCallOp : public ServerRequestOp
{
private:
	unsigned short mProcNum;
	ConstSmartPtr<ByteBuffer> mRequestData;
	ConstSmartPtr<ByteBuffer> mReplyData;
	
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();
	virtual void Virtual04();

public:
	ContestCallOp(ServerContext *theContestContext);
	ContestCallOp(const IPAddr &theAddr);

	void SetProcNum(unsigned short theNum) { mProcNum = theNum; }
	void SetRequestData(const ByteBuffer *theData) { mRequestData = theData; }

	unsigned short GetProcNum() const { return mProcNum; }
	ConstSmartPtr<ByteBuffer> GetRequestData() const { return mRequestData; }
	ConstSmartPtr<ByteBuffer> GetReplyData() const { return mReplyData; }
};

typedef SmartPtr<ContestCallOp> ContestCallOpPtr;


}; // namespace WONAPI

#endif
