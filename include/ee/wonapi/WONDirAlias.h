#ifndef EE_WONAPI_WONDIRALIAS_H
#define EE_WONAPI_WONDIRALIAS_H
#include "ee/wonapi/WONBuffer.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetAliasDirOp : public ServerRequestOp
{
private:
	// List of requests
	typedef std::list<GetEntityRequestPtr> GetEntityRequestList;
	GetEntityRequestList mRequestList;
	GetEntityRequestList::iterator mCurRequest;

	DWORD mFlags;
	DirDataTypeSet mDataTypes;
	std::string mProductName;

	int  mNumFailedRequests;
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();
	virtual void Virtual18();

public:
	GetAliasDirOp(ServerContext *theDirContext);
	GetAliasDirOp(const IPAddr &theAddr);

	void AddRequest(GetEntityRequest* theRequest);

	void SetProductName(const std::string& theProductName) { mProductName = theProductName; }

	// Add path uses flags from SetPath and DataTypes from AddDataType to construct entity request for the path
	void AddService(const std::wstring& theService);
	void SetFlags   (DWORD theFlags) { mFlags = theFlags; } 
	DWORD GetFlags()				 { return mFlags;	  } 

	// Types of DataObjects to be retrieved
	void AddDataType(const std::string &theType) { mDataTypes.insert(theType); } 
	void ClearDataTypes() { mDataTypes.clear(); } 
	
	// Access to the request list
	const GetEntityRequestList& GetRequestList() { return mRequestList; }
};

typedef SmartPtr<GetAliasDirOp> GetAliasDirOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}
#endif
