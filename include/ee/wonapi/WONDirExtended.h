// Complete same-SDK directory protocol operations; no Native layout or runtime replacement claim.
#ifndef EE_WONAPI_WONDIREXTENDED_H
#define EE_WONAPI_WONDIREXTENDED_H
#include "ee/wonapi/WONBuffer.h"
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
enum DirGetFlags
{
	// Bits 0-15 are for decomposition and common flags

	// Decomposition Flags - apply these decompositions for Directories
	DIR_GF_DECOMPROOT      = 0x00000001,  // Add the dir itself
	DIR_GF_DECOMPSERVICES  = 0x00000002,  // Add dir services
	DIR_GF_DECOMPSUBDIRS   = 0x00000004,  // Add dir subdirs
	DIR_GF_DECOMPRECURSIVE = 0x00000008,  // Recursive into dir subdirs

	// Common flags - include these attributes for all entities
	DIR_GF_ADDTYPE         = 0x00000010,  // Add entity types
	DIR_GF_ADDDISPLAYNAME  = 0x00000020,  // Add display names
	DIR_GF_ADDCREATED      = 0x00000040,  // Add creation date/time
	DIR_GF_ADDTOUCHED      = 0x00000080,  // Add touched date/time
	DIR_GF_ADDLIFESPAN     = 0x00000100,  // Add lifespan
	DIR_GF_ADDDOTYPE       = 0x00000200,  // Add DataObject types
	DIR_GF_ADDDODATA       = 0x00000400,  // Add DataObject data
	DIR_GF_ADDDATAOBJECTS  = 0x00000800,  // Add all DataObjects
	DIR_GF_ADDACLS         = 0x00001000,  // Add ACLs
	DIR_GF_ADDCRC          = 0x00002000,  // Add entity CRC
	DIR_GF_ADDUIDS         = 0x00004000,  // Add create and touch user ids
	DIR_GF_ADDORIGIP       = 0x00008000,  // Add originating IP address (admins only)

	// Bits 16-23 are for Directory only fields

	// Directory Flags - include these attributes for directories
	DIR_GF_DIRADDPATH      = 0x00010000,  // Add dir paths (from root)
	DIR_GF_DIRADDNAME      = 0x00020000,  // Add service names
	DIR_GF_DIRADDREQUNIQUE = 0x00040000,  // Add directory unqiue display name flag

	// Bits 24-31 are for Service only fields

	// Service Flags - include these attributes for services
	DIR_GF_SERVADDPATH     = 0x01000000,  // Add dir paths (from root)
	DIR_GF_SERVADDNAME     = 0x02000000,  // Add service names
	DIR_GF_SERVADDNETADDR  = 0x04000000,  // Add service net addresses
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Entity Flags (byte) - Control attributes in add/change requests
enum DirUpdateFlags
{
	DIR_UF_UNIQUEDISPLAYNAME = 0x01,  // Display name must be unique
	DIR_UF_DIRNOTUNIQUE      = 0x02,  // Directory allows duplicate display names
	DIR_UF_DIRREQUNIQUE      = 0x04,  // Directory requires unique display names
	DIR_UF_OVERWRITE         = 0x08,  // Overwrite existing entities

	// These 2 flags SHARE the 0x10 value (by design)
	DIR_UF_SERVRETURNADDR    = 0x10,  // Return service net address in reply
	DIR_UF_DIRNOACLINHERIT   = 0x10,  // Do not inherit parent ACLs for AddDirs

	DIR_UF_SERVGENNETADDR    = 0x20,  // Generate NetAddr from connection (AddService(Ex) only)
};


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// DataObjectSetMode (byte) - Mode for setting data objects on a DirEntity
enum DirDataObjectSetMode
{
	DIR_DO_ADDREPLACE    = 0,  // Add on not exist, replace on exist
	DIR_DO_ADDIGNORE     = 1,  // Add on not exist, ignore on exist
	DIR_DO_ADDONLY       = 2,  // Add on not exist, error on exist
	DIR_DO_REPLACEIGNORE = 3,  // Replace on exist, ignore on not exist
	DIR_DO_REPLACEONLY   = 4,  // Replace on exist, error on not exist
	DIR_DO_RESETDELETE   = 5,  // Clear existing set first, then add all.
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FindMatchMode (byte) - Mode for find queries
enum DirFindMatchMode
{
	DIR_FMM_EXACT   = 0,  // Compared value must equal search value
	DIR_FMM_BEGIN   = 1,  // Compared value must begin with search value
	DIR_FMM_END     = 2,  // Compared value must end with search value
	DIR_FMM_CONTAIN = 3   // Compared value must contain search value
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// FindFlags (byte) - Control flags for find queries
enum DirFindFlags
{
	DIR_FF_MATCHALL  = 0x01,  // Return all valid matches
	DIR_FF_FULLKEY   = 0x02,  // Match only if all search field match
	DIR_FF_RECURSIVE = 0x04   // Search directories recursively for matches
};

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK AddEntityOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AddEntityOp : public ServerRequestOp
{
protected:
	DWORD mFlags;
	std::wstring mPath;
	std::wstring mName;
	std::wstring mDisplayName;
	ByteBufferPtr mNetAddr;

	unsigned long mLifespan;
	DirDataObjectList mDataObjects;

	bool mIsService;
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	AddEntityOp(ServerContext *theDirContext, bool isService);
	AddEntityOp(const IPAddr &theAddr, bool isService);

	// DirUpdateFlags defined in DirTypes.h
	void SetFlags(DWORD theFlags) { mFlags = theFlags; } 	
	void SetPath(const std::wstring &thePath) { mPath = thePath; } 
	void SetName(const std::wstring &theName) { mName = theName; }
	void SetDisplayName(const std::wstring &theDisplayName) { mDisplayName = theDisplayName; }
	void SetLifespan(unsigned long theLifespan) { mLifespan = theLifespan; }
	void AddDataObject(const std::string &theType, const ByteBuffer* theData) { DirDataObject anObject; anObject.mDataType=theType; anObject.mData=const_cast<ByteBuffer*>(theData); mDataObjects.push_back(anObject); }
	void ClearDataObjects() { mDataObjects.clear(); }
	
	DWORD GetFlags() const { return mFlags; }
	const std::wstring& GetPath() const { return mPath; }
	const std::wstring& GetName() const { return mName; }
	const std::wstring& GetDisplayName() const { return mDisplayName; }
	const DirDataObjectList& GetDataObjects() const { return mDataObjects; }
};

typedef SmartPtr<AddEntityOp> AddEntityOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AddServiceOp : public AddEntityOp
{
public:
	AddServiceOp(ServerContext *theDirContext) : AddEntityOp(theDirContext, true) {}
	AddServiceOp(const IPAddr &theAddr) : AddEntityOp(theAddr, true) {}

	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }
	void SetNetAddrPort(unsigned short thePort);

	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<AddServiceOp> AddServiceOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class AddDirOp : public AddEntityOp
{
public:
	AddDirOp(ServerContext *theDirContext) : AddEntityOp(theDirContext, false) {}
	AddDirOp(const IPAddr &theAddr) : AddEntityOp(theAddr, false) {}
};

typedef SmartPtr<AddDirOp> AddDirOpPtr;

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK ModifyEntityOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ModifyEntityOp : public ServerRequestOp
{
protected:
	DWORD mFlags;
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;

	std::wstring mNewName;
	std::wstring mNewDisplayName;
	ByteBufferPtr mNewNetAddr;
	unsigned long mNewLifespan;

	DirDataObjectList mDataObjects;

	bool mIsService;
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	ModifyEntityOp(ServerContext *theDirContext, bool isService);
	ModifyEntityOp(const IPAddr &theAddr, bool isService);

	// DirUpdateFlags defined in DirTypes.h
	void SetFlags(DWORD theFlags) { mFlags = theFlags; } 
	void SetPath(const std::wstring &thePath) { mPath = thePath; }

	void SetNewName(const std::wstring &theNewName) { mNewName = theNewName; }
	void SetNewDisplayName(const std::wstring &theNewDisplayName) { mNewDisplayName = theNewDisplayName; }
	void SetNewLifespan(unsigned long theLifespan) { mNewLifespan = theLifespan; }
	void AddDataObject(const std::string &theType, const ByteBuffer* theData) { DirDataObject anObject; anObject.mDataType=theType; anObject.mData=const_cast<ByteBuffer*>(theData); mDataObjects.push_back(anObject); }
	void ClearDataObjects() { mDataObjects.clear(); }
	
	DWORD GetFlags() const { return mFlags; }
	const std::wstring& GetPath() const { return mPath; }
	const std::wstring& GetNewName() const { return mNewName; }
	const std::wstring& GetNewDisplayName() const { return mNewDisplayName; }
	unsigned long GetNewLifespan() const { return mNewLifespan; }
	const DirDataObjectList& GetDataObjects() const { return mDataObjects; }
};

typedef SmartPtr<ModifyEntityOp> ModifyEntityOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ModifyServiceOp : public ModifyEntityOp
{
public:
	ModifyServiceOp(ServerContext *theDirContext) : ModifyEntityOp(theDirContext, true) {}
	ModifyServiceOp(const IPAddr &theAddr) : ModifyEntityOp(theAddr, true) {}

	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }
	void SetNewNetAddr(ByteBufferPtr theNetAddr) { mNewNetAddr = theNetAddr; }
	void SetNewNetAddr(const IPAddr& theNetAddr) { mNewNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }

	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
	const ByteBuffer* GetNewNetAddr() const { return mNewNetAddr; }
};

typedef SmartPtr<ModifyServiceOp> ModifyServiceOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ModifyDirOp : public ModifyEntityOp
{
public:
	ModifyDirOp(ServerContext *theDirContext) : ModifyEntityOp(theDirContext, false) {}
	ModifyDirOp(const IPAddr &theAddr) : ModifyEntityOp(theAddr, false) {}
};

typedef SmartPtr<ModifyDirOp> ModifyDirOpPtr;

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK RemoveEntityOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RemoveEntityOp : public ServerRequestOp
{
protected:
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;

	bool mIsService;
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	RemoveEntityOp(ServerContext *theDirContext, bool isService);
	RemoveEntityOp(const IPAddr &theAddr, bool isService);

	void SetPath(const std::wstring &thePath) { mPath = thePath; }	
	const std::wstring& GetPath() const { return mPath; }
};

typedef SmartPtr<RemoveEntityOp> RemoveEntityOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RemoveServiceOp : public RemoveEntityOp
{
public:
	RemoveServiceOp(ServerContext *theDirContext) : RemoveEntityOp(theDirContext, true) {}
	RemoveServiceOp(const IPAddr &theAddr) : RemoveEntityOp(theAddr, true) {}

	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }


	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<RemoveServiceOp> RemoveServiceOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RemoveDirOp : public RemoveEntityOp
{
public:
	RemoveDirOp(ServerContext *theDirContext) : RemoveEntityOp(theDirContext, false) {}
	RemoveDirOp(const IPAddr &theAddr) : RemoveEntityOp(theAddr, false) {}
};

typedef SmartPtr<RemoveDirOp> RemoveDirOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK RenewEntityOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenewEntityOp : public ServerRequestOp
{
protected:
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;
	unsigned long mLifespan;

	bool mIsService;
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	RenewEntityOp(ServerContext *theDirContext, bool isService);
	RenewEntityOp(const IPAddr &theAddr, bool isService);

	void SetPath(const std::wstring &thePath) { mPath = thePath; }	
	void SetLifespan(unsigned long theLifespan) { mLifespan = theLifespan; }

	const std::wstring& GetPath() const { return mPath; }
	unsigned long GetLifespan() { return mLifespan; }
};

typedef SmartPtr<RenewEntityOp> RenewEntityOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenewServiceOp : public RenewEntityOp
{
public:
	RenewServiceOp(ServerContext *theDirContext) : RenewEntityOp(theDirContext, true) {}
	RenewServiceOp(const IPAddr &theAddr) : RenewEntityOp(theAddr, true) {}

	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }


	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<RenewServiceOp> RenewServiceOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenewDirOp : public RenewEntityOp
{
public:
	RenewDirOp(ServerContext *theDirContext) : RenewEntityOp(theDirContext, false) {}
	RenewDirOp(const IPAddr &theAddr) : RenewEntityOp(theAddr, false) {}
};

typedef SmartPtr<RenewDirOp> RenewDirOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK RenameEntityOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenameEntityOp : public ServerRequestOp
{
protected:
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;

	std::wstring mNewDisplayName;
	bool mIsUnique;

	bool mIsService;
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	RenameEntityOp(ServerContext *theDirContext, bool isService);
	RenameEntityOp(const IPAddr &theAddr, bool isService);

	void SetPath(const std::wstring &thePath) { mPath = thePath; }	
	void SetNewDisplayName(const std::wstring &theName) { mNewDisplayName = theName; }
	void SetUnique(bool requireUniqueDisplayName) { mIsUnique = requireUniqueDisplayName; }	


	const std::wstring& GetPath() const { return mPath; }
	const std::wstring& GetNewDisplayName() const { return mNewDisplayName; }
	bool GetUnique() const { return mIsUnique; }
};

typedef SmartPtr<RenameEntityOp> RenameEntityOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenameServiceOp : public RenameEntityOp
{
public:
	RenameServiceOp(ServerContext *theDirContext) : RenameEntityOp(theDirContext, true) {}
	RenameServiceOp(const IPAddr &theAddr) : RenameEntityOp(theAddr, true) {}

	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }


	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<RenameServiceOp> RenameServiceOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RenameDirOp : public RenameEntityOp
{
public:
	RenameDirOp(ServerContext *theDirContext) : RenameEntityOp(theDirContext, false) {}
	RenameDirOp(const IPAddr &theAddr) : RenameEntityOp(theAddr, false) {}
};

typedef SmartPtr<RenameDirOp> RenameDirOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK EntitySetDataObjectsOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


class EntitySetDataObjectsOp : public ServerRequestOp
{
protected:
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;
	DirDataObjectSetMode mDataObjectMode;
	DirDataObjectList mDataObjects;

	bool mIsService;

protected:
	void Init();
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	EntitySetDataObjectsOp(ServerContext *theDirContext, bool isService);
	EntitySetDataObjectsOp(const IPAddr &theAddr, bool isService);

	void SetPath(const std::wstring &thePath) { mPath = thePath; }

	// DataObjectSetMode defined in DirTypes.h
	void SetDataObjectMode(DirDataObjectSetMode theMode) { mDataObjectMode = theMode; }
	void AddDataObject(const std::string &theType, const ByteBuffer* theData) { DirDataObject anObject; anObject.mDataType=theType; anObject.mData=const_cast<ByteBuffer*>(theData); mDataObjects.push_back(anObject); }
	void ClearDataObjects() { mDataObjects.clear(); }
	
	const std::wstring& GetPath() const { return mPath; }
	DirDataObjectSetMode GetDataObjectMode() { return mDataObjectMode; }
	const DirDataObjectList& GetDataObjects() const { return mDataObjects; }
};

typedef SmartPtr<EntitySetDataObjectsOp> EntitySetDataObjectsOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServiceSetDataObjectsOp : public EntitySetDataObjectsOp
{
public:
	ServiceSetDataObjectsOp(ServerContext *theDirContext) : EntitySetDataObjectsOp(theDirContext, true) {}
	ServiceSetDataObjectsOp(const IPAddr &theAddr) : EntitySetDataObjectsOp(theAddr, true) {}

	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }

	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<ServiceSetDataObjectsOp> ServiceSetDataObjectsOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DirSetDataObjectsOp : public EntitySetDataObjectsOp
{
public:
	DirSetDataObjectsOp(ServerContext *theDirContext) : EntitySetDataObjectsOp(theDirContext, false) {}
	DirSetDataObjectsOp(const IPAddr &theAddr) : EntitySetDataObjectsOp(theAddr, false) {}
};

typedef SmartPtr<DirSetDataObjectsOp> DirSetDataObjectsOpPtr;

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK EntityClearDataObjectsOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


class EntityClearDataObjectsOp : public ServerRequestOp
{
protected:
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;

	DirDataTypeSet mDataTypes;

	bool mIsService;

protected:
	void Init();
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	EntityClearDataObjectsOp(ServerContext *theDirContext, bool isService);
	EntityClearDataObjectsOp(const IPAddr &theAddr, bool isService);

	void SetPath(const std::wstring &thePath) { mPath = thePath; }
	void AddDataType(const std::string &theType) { mDataTypes.insert(theType); }
	void ClearDataTypes() { mDataTypes.clear(); }
	
	const std::wstring& GetPath() const { return mPath; }
};

typedef SmartPtr<EntityClearDataObjectsOp> EntityClearDataObjectsOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServiceClearDataObjectsOp : public EntityClearDataObjectsOp
{
public:
	ServiceClearDataObjectsOp(ServerContext *theDirContext) : EntityClearDataObjectsOp(theDirContext, true) {}
	ServiceClearDataObjectsOp(const IPAddr &theAddr) : EntityClearDataObjectsOp(theAddr, true) {}

	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }

	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<ServiceClearDataObjectsOp> ServiceClearDataObjectsOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DirClearDataObjectsOp : public EntityClearDataObjectsOp
{
public:
	DirClearDataObjectsOp(ServerContext *theDirContext) : EntityClearDataObjectsOp(theDirContext, false) {}
	DirClearDataObjectsOp(const IPAddr &theAddr) : EntityClearDataObjectsOp(theAddr, false) {}
};

typedef SmartPtr<DirClearDataObjectsOp> DirClearDataObjectsOpPtr;

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK EntityModifyDataObjectsOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class EntityModifyDataObjectsOp : public ServerRequestOp
{
private:
	class DataObjectMod
	{
	public:
		DirDataObject mObject;
		bool mIsInsert;
		unsigned short mOffset;

		DataObjectMod() { mIsInsert = false; mOffset = 0; }
		DataObjectMod(const DirDataObject &theObj, bool isInsert, unsigned short theOffset) :
			mObject(theObj), mIsInsert(isInsert), mOffset(theOffset) {}
	};

	typedef std::list<DataObjectMod> DataObjectModList;

protected:
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;
	DataObjectModList mDataObjectMods;

	bool mIsService;

protected:
	void Init();
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();

public:
	EntityModifyDataObjectsOp(ServerContext *theDirContext, bool isService);
	EntityModifyDataObjectsOp(const IPAddr &theAddr, bool isService);

	void SetPath(const std::wstring &thePath) { mPath = thePath; }

	void AddDataObjectMod(const std::string &theType, 
		const ByteBuffer* theData, 
		bool isInsert, 
		unsigned short theOffset) { DirDataObject anObject; anObject.mDataType=theType; anObject.mData=const_cast<ByteBuffer*>(theData); mDataObjectMods.push_back(DataObjectMod(anObject,isInsert,theOffset)); }
	
	void ClearDataObjectMods() { mDataObjectMods.clear(); }
	
	const std::wstring& GetPath() const { return mPath; }
};

typedef SmartPtr<EntityModifyDataObjectsOp> EntityModifyDataObjectsOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class ServiceModifyDataObjectsOp : public EntityModifyDataObjectsOp
{
public:
	ServiceModifyDataObjectsOp(ServerContext *theDirContext) : EntityModifyDataObjectsOp(theDirContext, true) {}
	ServiceModifyDataObjectsOp(const IPAddr &theAddr) : EntityModifyDataObjectsOp(theAddr, true) {}

	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }

	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<ServiceModifyDataObjectsOp> ServiceModifyDataObjectsOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class DirModifyDataObjectsOp : public EntityModifyDataObjectsOp
{
public:
	DirModifyDataObjectsOp(ServerContext *theDirContext) : EntityModifyDataObjectsOp(theDirContext, false) {}
	DirModifyDataObjectsOp(const IPAddr &theAddr) : EntityModifyDataObjectsOp(theAddr, false) {}
};

typedef SmartPtr<DirModifyDataObjectsOp> DirModifyDataObjectsOpPtr;

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK FindEntityOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FindEntityOp : public ServerRequestOp
{
protected:
	DWORD mGetFlags;
	DirFindMatchMode mMatchMode;
	unsigned char mFindFlags;

	std::wstring mPath;
	std::wstring mName;
	std::wstring mDisplayName;
	ByteBufferPtr mNetAddr;

	DirDataObjectList mFindDataObjects;
	DirDataTypeSet mGetDataTypes;

	DirEntityPtr mService;
	DirEntityList mDirEntityList;

	bool mIsService;
	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();
	virtual void Virtual18();

public:
	FindEntityOp(ServerContext *theDirContext, bool isService);
	FindEntityOp(const IPAddr &theAddr, bool isService);

	// DirGetFlags defined in DirTypes.h
	void SetGetFlags(DWORD theFlags) { mGetFlags = theFlags; } 
	void SetMatchMode(DirFindMatchMode theMode) { mMatchMode = theMode; }

	// DirFindFlags defined in DirTypes.h
	void SetFindFlags(unsigned char theFlags) { mFindFlags = theFlags; }

	void SetPath(const std::wstring &thePath) { mPath = thePath; }

	void SetMatchName(const std::wstring &theName) { mName = theName; }
	void SetMatchDisplayName(const std::wstring &theDisplayName) { mDisplayName = theDisplayName; }
	
	void AddFindDataObject(const std::string &theType, const ByteBuffer* theData) { DirDataObject anObject; anObject.mDataType=theType; anObject.mData=const_cast<ByteBuffer*>(theData); mFindDataObjects.push_back(anObject); }
	void ClearFindDataObjects() { mFindDataObjects.clear(); }	

	void AddGetDataObject(const std::string &theType) { mGetDataTypes.insert(theType); }
	void ClearGetDataObjects() { mGetDataTypes.clear(); }

	DWORD GetGetFlags() { return mGetFlags; }
	DirFindMatchMode GetMatchMode() { return mMatchMode; }
	unsigned char GetFindFlags() { return mFindFlags; }
	const std::wstring& GetPath() const { return mPath; }
	const std::wstring& GetName() const { return mName; }
	const std::wstring& GetDisplayName() const { return mDisplayName; }

	// Result Methods
	DirEntityPtr GetService() const { return mService; }
	const DirEntityList GetDirEntityList() const { return mDirEntityList; }
};

typedef SmartPtr<FindEntityOp> FindEntityOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FindServiceOp : public FindEntityOp
{
public:
	FindServiceOp(ServerContext *theDirContext) : FindEntityOp(theDirContext, true) {}
	FindServiceOp(const IPAddr &theAddr) : FindEntityOp(theAddr, true) {}

	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }

	const ByteBuffer* GetNetAddr() const { return mNetAddr; }
};

typedef SmartPtr<FindServiceOp> FindServiceOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class FindDirOp : public FindEntityOp
{
public:
	FindDirOp(ServerContext *theDirContext) : FindEntityOp(theDirContext, false) {}
	FindDirOp(const IPAddr &theAddr) : FindEntityOp(theAddr, false) {}
};

typedef SmartPtr<FindDirOp> FindDirOpPtr;

// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK GetServiceOp family.
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class GetServiceOp : public ServerRequestOp
{
private:
	DWORD mFlags;
	std::wstring mPath;
	std::wstring mName;
	ByteBufferPtr mNetAddr;

	DirDataTypeSet mDataTypes;
	DirEntityPtr mService;

	void Init();

protected:
	virtual WONStatus Virtual10();
	virtual WONStatus Virtual14();
	virtual void Virtual18();

public:
	GetServiceOp(ServerContext *theDirContext);
	GetServiceOp(const IPAddr &theAddr);

	// Path/Name/NetAddr of the service
	void SetPath(const std::wstring &thePath) { mPath = thePath; }
	void SetName(const std::wstring &theName) { mName = theName; }
	void SetNetAddr(ByteBufferPtr theNetAddr) { mNetAddr = theNetAddr; }
	void SetNetAddr(const IPAddr& theNetAddr) { mNetAddr = new ByteBuffer(theNetAddr.GetSixByte(),6,false); }

	const std::wstring& GetPath() { return mPath; }
	const std::wstring& GetName() const { return mName; }
	const ByteBuffer* GetNetAddr() const { return mNetAddr; }	

	// DirGetFlags defined in DirTypes.h
	void SetFlags(DWORD theFlags) { mFlags = theFlags; }
	DWORD GetFlags() { return mFlags; }

	// Datatypes of DataObjects to retrieve
	void AddDataType(const std::string &theType) { mDataTypes.insert(theType); }
	void ClearDataTypes() { mDataTypes.clear(); }

	// The retrieved service
	DirEntityPtr GetService() { return mService; }
};

typedef SmartPtr<GetServiceOp> GetServiceOpPtr;
} // namespace WONAPI
#endif
