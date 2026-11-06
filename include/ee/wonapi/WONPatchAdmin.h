// SOURCE_ONLY_NO_NATIVE_ID: complete genuine same-SDK patch/version administration operations.
#ifndef EE_WONAPI_WONPATCHADMIN_H
#define EE_WONAPI_WONPATCHADMIN_H
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
typedef std::list<SmartPtr<VersionData> > VersionDataList;
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// ActivatePatch - StatusReply
// Used to Activate (make public) or De-Activate a patch
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	NetAddress - string
// ·	IsActive - Boolean (optional -default = false)
//
///////////////////////////////////////////////////////////////////////////////
class ActivatePatchOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	std::string		mNetAddress;		// patch url
	BOOL			mIsActive;

	// Operations
public:


	// Accessors
	void SetProductName(const std::string& theProductName)	{ mProductName = theProductName;	}
	void SetConfigName(const std::string& theConfigName)	{ mConfigName = theConfigName;		}
	void SetFromVersion(const std::string& theFromVersion)	{ mFromVersion = theFromVersion;	}
	void SetToVersion(const std::string& theToVersion)		{ mToVersion = theToVersion;		}
	void SetNetAddress(const std::string& theNetAddress)	{ mNetAddress = theNetAddress;		}		// patch url
	void SetIsActive(BOOL theIsActive)						{ mIsActive = theIsActive;			}

	const std::string&  GetProductName()		{ return mProductName;	}
	const std::string&  GetConfigName()			{ return mConfigName;	}
	const std::string&  GetFromVersion()		{ return mFromVersion;	}
	const std::string&  GetToVersion()			{ return mToVersion;	}
	const std::string&  GetNetAddress()			{ return mNetAddress;	}		// patch url
	BOOL  GetIsActive()							{ return mIsActive;		}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	ActivatePatchOp(ServerContext* theContext);			// Requires List of DBProxy servers
	ActivatePatchOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~ActivatePatchOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<ActivatePatchOp> ActivatePatchOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// AddPatch - StatusReply
// Request to add a patch to the database, will be added as valid.  Its active status is based on the specified parameter
// ·	ProductName - string 
// .	PatchData	- see PatchTypes.h
//
///////////////////////////////////////////////////////////////////////////////
class AddPatchOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	SmartPtr<PatchData>	mPatchData;
	bool			mIsUpdate;


	// Operations
public:

	// Accessors
	void SetPatchData	(PatchData* thePatchData)			{ mPatchData = thePatchData;					}
	void SetProductName (const std::string& theProductName)	{ mProductName = theProductName;				}
	void SetConfigName	(const std::string& theConfigName)	{ mPatchData->SetConfigName(theConfigName);		}
	void SetFromVersion	(const std::string& theFromVersion)	{ mPatchData->SetFromVersion(theFromVersion);	}
	void SetToVersion	(const std::string& theToVersion)	{ mPatchData->SetToVersion(theToVersion);		}
	void SetDisplayName	(const std::string& theDisplayName)	{ mPatchData->SetDisplayName(theDisplayName);	}
	void SetNetAddress	(const std::string& theNetAddress)	{ mPatchData->SetNetAddress(theNetAddress);		}
	void SetCheckSum	(unsigned long theCheckSum)			{ mPatchData->SetCheckSum(theCheckSum);			}
	void SetPatchSize	(unsigned long thePatchSize)		{ mPatchData->SetPatchSize(thePatchSize);		}
	void SetHostName	(const std::string& theHostName)	{ mPatchData->SetHostName(theHostName);			}
	void SetHostURL		(const std::string& theHostURL)		{ mPatchData->SetHostURL(theHostURL);			}
	void SetHostText	(const std::string& theHostText)	{ mPatchData->SetHostText(theHostText);			}
	void SetHostImageURL(const std::string& theHostImageURL){ mPatchData->SetHostImageURL(theHostImageURL);	}
	void SetMustVisitHost(bool theMustVisitHost)			{ mPatchData->SetMustVisitHost(theMustVisitHost);}
	void SetIsActive	(bool theIsActive)					{ mPatchData->SetIsActive(theIsActive);			}
	void SetIsUpdate	(bool theIsUpdate)					{ mIsUpdate = theIsUpdate; }

	SmartPtr<PatchData>		GetPatchData()			{ return mPatchData;					}
	const std::string&  GetProductName()		{ return mProductName;					}
	const std::string&  GetConfigName()			{ return mPatchData->GetConfigName();	}
	const std::string&  GetFromVersion()		{ return mPatchData->GetFromVersion();	}
	const std::string&  GetToVersion()			{ return mPatchData->GetToVersion();	}
	const std::string&  GetDisplayName()		{ return mPatchData->GetDisplayName();	}
	const std::string&  GetNetAddress()			{ return mPatchData->GetNetAddress();	}
	const unsigned long GetCheckSum()			{ return mPatchData->GetCheckSum();		}
	const unsigned long GetPatchSize()			{ return mPatchData->GetPatchSize();	}
	const std::string&  GetHostName()			{ return mPatchData->GetHostName();		}
	const std::string&  GetHostURL()			{ return mPatchData->GetHostURL();		}
	const std::string&  GetHostText()			{ return mPatchData->GetHostText();		}
	const std::string&  GetHostImageURL()		{ return mPatchData->GetHostImageURL(); }
	bool  GetMustVisitHost()					{ return mPatchData->GetMustVisitHost();}
	bool  GetIsActive()							{ return mPatchData->GetIsActive();		}
	bool  GetIsUpdate()							{ return mIsUpdate;						}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	AddPatchOp(ServerContext* theContext);			// Requires List of DBProxy servers
	AddPatchOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~AddPatchOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<AddPatchOp> AddPatchOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// AddVersion - StatusReply
// Request to add a version to the database, will be added with state of Inactive.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	Version - string
// ·	VersionType - list of strings (optional)
// ·	DescriptionURL - string (optional)
//
///////////////////////////////////////////////////////////////////////////////
class AddVersionOp: public DBProxyOp
{
	// Attributes
protected:
	std::string		mProductName;
	SmartPtr<VersionData>	mVersionData;				// see PatchTypes.h"

	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)
	// Operations
public:

	// Accessors
	void SetProductName		(const std::string& theProductName)		{ mProductName		= theProductName;					}
	void SetConfigName		(const std::string& theConfigName)		{ mVersionData->SetConfigName		(theConfigName);	}
	void SetVersion			(const std::string& theVersion)			{ mVersionData->SetVersion			(theVersion);		}
	void SetDescriptionURL	(const std::string& theDescriptionURL)	{ mVersionData->SetDescriptionURL	(theDescriptionURL);}
	void AddVersionType		(const std::string& theVersionType)		{ mVersionData->AddVersionType		(theVersionType);	}

	const std::string& GetProductName()		{ return mProductName;		}
	const std::string& GetConfigName()		{ return mVersionData->GetConfigName();		}
	const std::string& GetVersion()			{ return mVersionData->GetVersion();		}
	const std::string& GetDescriptionURL()	{ return mVersionData->GetDescriptionURL();	}
	const VersionTypeList& GetVersionTypeList() { return mVersionData->GetVersionTypeList(); }

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	AddVersionOp(ServerContext* theContext);	// Requires gamename and list of patch servers
	AddVersionOp(const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~AddVersionOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<AddVersionOp> AddVersionOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// GetVersionList - GetVersionListReply
// Request a list of valid versions using various filter methods
// ·	ProductName - string 
// ·	ConfigName	- string	(optional)
// ·	State		- byte		(Invalid, Inactive, Active) (optional)
// .    VersionData - see PatchTypes.h
//
///////////////////////////////////////////////////////////////////////////////
class GetVersionListOp: public DBProxyOp
{
	// Attributes
protected:
	std::string			mProductName;
	std::string			mConfigName;
	VersionState		mStateFilter;
	VersionDataList		mVersionDataList;

	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)
	// Operations
public:

	// Accessors
	void SetProductName		(const std::string& theProductName)		{ mProductName		= theProductName;	}
	void SetConfigName		(const std::string& theConfigName)		{ mConfigName		= theConfigName;	}
	void SetStateFilter		(VersionState		theStateFilter)		{ mStateFilter		= theStateFilter;	}

	const std::string& GetProductName()		{ return mProductName;	}
	const std::string& GetConfigName()		{ return mConfigName;	}
	VersionState	   GetStateFilter()		{ return mStateFilter;	}
	const VersionDataList&	GetVersionDataList()	{ return mVersionDataList; }

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	GetVersionListOp(const std::string& productName, ServerContext* theContext);	// Requires gamename and list of patch servers
	GetVersionListOp(const std::string& productName, const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~GetVersionListOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<GetVersionListOp> GetVersionListOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// PatchCleanupOp - StatusReply
// Request to do automatic patch pruning.
///////////////////////////////////////////////////////////////////////////////
class PatchCleanupOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	// Operations
public:

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	PatchCleanupOp(ServerContext* theContext);	// Requires gamename and list of patch servers
	PatchCleanupOp(const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~PatchCleanupOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<PatchCleanupOp> PatchCleanupOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// RemovePatch - StatusReply
// Request to remove a patch.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	FromVersion - string
// ·	ToVersion - string
// ·	NetAddress - string
//
///////////////////////////////////////////////////////////////////////////////
class RemovePatchOp: public DBProxyOp
{
	// Attributes
protected:
	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	std::string		mProductName;
	std::string		mConfigName;
	std::string		mFromVersion;
	std::string		mToVersion;
	std::string		mNetAddress;		// patch url

	// Operations
public:

	// Accessors
	void SetProductName	(const std::string& theProductName)	{ mProductName	= theProductName;	}
	void SetConfigName	(const std::string& theConfigName)	{ mConfigName	= theConfigName;	}
	void SetFromVersion	(const std::string& theFromVersion)	{ mFromVersion	= theFromVersion;	}
	void SetToVersion	(const std::string& theToVersion)	{ mToVersion	= theToVersion;		}
	void SetNetAddress	(const std::string& theNetAddress)	{ mNetAddress	= theNetAddress;	}
	
	const std::string&  GetProductName()		{ return mProductName;	}
	const std::string&  GetConfigName()			{ return mConfigName;	}
	const std::string&  GetFromVersion()		{ return mFromVersion;	}
	const std::string&  GetToVersion()			{ return mToVersion;	}
	const std::string&  GetNetAddress()			{ return mNetAddress;	}		// patch url

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	RemovePatchOp(ServerContext* theContext);			// Requires List of DBProxy servers
	RemovePatchOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~RemovePatchOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<RemovePatchOp> RemovePatchOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// RemoveVersion - StatusReply
// Request to add a version to the database, will be added with state of Inactive.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	Version - string
// ·	VersionType - list of strings (optional)
// ·	DescriptionURL - string (optional)
//
///////////////////////////////////////////////////////////////////////////////
class RemoveVersionOp: public DBProxyOp
{
	// Attributes
protected:
	std::string mProductName;
	std::string mConfigName;
	std::string mVersion;

	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)
	// Operations
public:

	// Accessors
	void SetProductName		(const std::string& theProductName)		{ mProductName		= theProductName;	}
	void SetConfigName		(const std::string& theConfigName)		{ mConfigName		= theConfigName;	}
	void SetVersion			(const std::string& theVersion)			{ mVersion			= theVersion;		}	

	const std::string& GetProductName()		{ return mProductName;		}
	const std::string& GetConfigName()		{ return mConfigName;		}
	const std::string& GetVersion()			{ return mVersion;			}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	RemoveVersionOp(ServerContext* theContext);			// Requires List of DBProxy servers
	RemoveVersionOp(const IPAddr &theAddr);				// or addr of a single server

	// Destructor
protected:
	virtual ~RemoveVersionOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<RemoveVersionOp> RemoveVersionOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// SetVersionState - StatusReply
// Request to change the state of an existing version.
// ·	ProductName - string 
// ·	ConfigName - string (optional)
// ·	Version - string
// ·	State - byte (Invalid / Inactive / Active)
//
///////////////////////////////////////////////////////////////////////////////
class UpdateVersionOp: public DBProxyOp
{
	// Attributes
protected:
	std::string		mProductName;
	SmartPtr<VersionData>	mVersionData;				// see PatchTypes.h"

	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)
	// Operations
public:

	// Accessors
	void SetProductName		(const std::string& theProductName)		{ mProductName		= theProductName;					}
	void SetConfigName		(const std::string& theConfigName)		{ mVersionData->SetConfigName		(theConfigName);	}
	void SetVersion			(const std::string& theVersion)			{ mVersionData->SetVersion			(theVersion);		}
	void SetDescriptionURL	(const std::string& theDescriptionURL)	{ mVersionData->SetDescriptionURL	(theDescriptionURL);}
	void AddVersionType		(const std::string& theVersionType)		{ mVersionData->AddVersionType		(theVersionType);	}
	void SetState	(VersionState theVersionState)					{ mVersionData->SetState			(theVersionState);	}

	const std::string&		GetProductName()	{ return mProductName;						}
	const std::string&		GetConfigName()		{ return mVersionData->GetConfigName();		}
	const std::string&		GetVersion()		{ return mVersionData->GetVersion();		}
	const std::string&		GetDescriptionURL()	{ return mVersionData->GetDescriptionURL();	}
	const VersionTypeList&	GetVersionTypeList(){ return mVersionData->GetVersionTypeList();}
	VersionState			GetState()			{ return mVersionData->GetState();			}

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	UpdateVersionOp(ServerContext* theContext);	// Requires gamename and list of patch servers
	UpdateVersionOp(const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~UpdateVersionOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<UpdateVersionOp> UpdateVersionOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// 
// GetPatchServerModuleListOp - GetPatchServerModuleListOp
// Request a list of available modules
// Reply:
//		unsigned short - Num Modules
//		{
//			unsigned long moduleID; 
//		}
//
///////////////////////////////////////////////////////////////////////////////
class GetPatchServerModuleListOp: public DBProxyOp
{
	// Attributes
protected:
	std::list<unsigned long>  mModuleList;	//HIWORD(aMsgType) LOWORD(aMsgSubType)	


	const unsigned short mMsgType;				// DO NOT MODIFY (MUST MATCH SERVER)

	// Operations
public:
	bool IsModuleAvailable(unsigned short theMsgType, unsigned short theSubMsgType);

protected:
	void Init();

	// Overrides
public:
	virtual WONStatus Virtual14();			// Recv - For Extended Unpack
	virtual void	  Virtual04();				// Send - For Extended Pack

	// Constructor
public:
	GetPatchServerModuleListOp(ServerContext* theContext);	// Requires gamename and list of patch servers
	GetPatchServerModuleListOp(const IPAddr &theAddr);		// or addr of a single server

	// Destructor
protected:
	virtual ~GetPatchServerModuleListOp() {}			// Safety Lock -- Enforces Smart Ptr Use

};
typedef SmartPtr<GetPatchServerModuleListOp> GetPatchServerModuleListOpPtr;		// Smart Ptr Template


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}
#endif
