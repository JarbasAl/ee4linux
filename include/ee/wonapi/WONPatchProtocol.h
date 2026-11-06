// SOURCE_ONLY_NO_NATIVE_ID: complete genuine same-SDK patch-server operations.
#ifndef EE_WONAPI_WONPATCHPROTOCOL_H
#define EE_WONAPI_WONPATCHPROTOCOL_H
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// GetPatchListOp - StatusReply
// Request to add a patch to the database, will be added as valid.  Its active status is based on the specified parameter
// ·	ProductName - string
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	StateFilter - BYTE (All, Active, Inactive) (optional)

///////////////////////////////////////////////////////////////////////////////
class GetPatchListOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	// [in]
	std::string		mProductName;
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	char			mStateFilter;
	

	// [out]
	PatchDataList	mPatchDataList;

	// Operations
public:

	// Accessors
	void SetProductName	(const std::string& theProductName)	{ mProductName	= theProductName;	}
	void SetConfigName	(const std::string& theConfigName)	{ mConfigName	= theConfigName ;	}
	void SetFromVersion	(const std::string& theFromVersion)	{ mFromVersion	= theFromVersion;	}
	void SetToVersion	(const std::string& theToVersion)	{ mToVersion	= theToVersion	;	}
	void SetStateFilter	(char theStateFilter)				{ mStateFilter	= theStateFilter;	}

	const PatchDataList&	GetPatchDataList()	{ return mPatchDataList;}
	const std::string&  GetProductName()		{ return mProductName;	}
	const std::string&  GetConfigName()			{ return mConfigName;	}
	const std::string&  GetFromVersion()		{ return mFromVersion;	}
	const std::string&  GetToVersion()			{ return mToVersion;	}
	char  GetStateFilter()						{ return mStateFilter;	}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	GetPatchListOp(ServerContext* theContext);			// Requires List of DBProxy servers
	GetPatchListOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~GetPatchListOp(); // Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<GetPatchListOp> GetPatchListOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// ReportPatchStatus - StatusReply
// Report the status of a requested patch download.
// ·	ProductName - string
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	NetAddress - string
// .    PatchStatus - BYTE

///////////////////////////////////////////////////////////////////////////////
class ReportPatchStatusOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	std::string		mNetAddress;		// patch url
	BYTE            mPatchStatus;

	SmartPtr<AsyncSocket> mUDPSocket;

	// Operations
public:

	// Accessors
	void SetProductName	(const std::string& theProductName)	{ mProductName	= theProductName;	}
	void SetConfigName	(const std::string& theConfigName)	{ mConfigName	= theConfigName;	}
	void SetFromVersion	(const std::string& theFromVersion)	{ mFromVersion	= theFromVersion;	}
	void SetToVersion	(const std::string& theToVersion)	{ mToVersion	= theToVersion;		}
	void SetNetAddress	(const std::string& theNetAddress)	{ mNetAddress	= theNetAddress;	}
	void SetPatchStatus	(BYTE thePatchStatus)				{ mPatchStatus	= thePatchStatus;	}
	void SetUDPSocket(AsyncSocket *theSocket)				{ mUDPSocket	= theSocket;		}
	
	const std::string&  GetProductName()		{ return mProductName;	}
	const std::string&  GetConfigName()			{ return mConfigName;	}
	const std::string&  GetFromVersion()		{ return mFromVersion;	}
	const std::string&  GetToVersion()			{ return mToVersion;	}
	const std::string&  GetNetAddress()			{ return mNetAddress;	}		// patch url
	BYTE  GetPatchStatus()						{ return mPatchStatus;	}		

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	ReportPatchStatusOp(const IPAddr &theAddr);	// Address of a single DBProxy server

	// Destructor
protected:
	virtual ~ReportPatchStatusOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<ReportPatchStatusOp> ReportPatchStatusOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}
#endif
