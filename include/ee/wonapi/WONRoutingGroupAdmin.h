// SOURCE_ONLY_NO_NATIVE_ID: complete genuine SDK new routing operation owners.
#ifndef EE_WONAPI_WONROUTINGGROUPADMIN_H
#define EE_WONAPI_WONROUTINGGROUPADMIN_H
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingRelinquishCaptaincyOp : public RoutingOp
{
private:
	unsigned short mGroupId;
	unsigned short mNewCaptainId;

	virtual void Virtual0C();
	virtual WONStatus Virtual10(unsigned char theMsgType, ReadBuffer &theMsg);

public:
	RoutingRelinquishCaptaincyOp(RoutingConnection *theConnection, unsigned short theGroupId = 0, unsigned short theNewCaptainId = 0xffff /* SDK RoutingId_Invalid */) : RoutingOp(theConnection), mGroupId(theGroupId), mNewCaptainId(theNewCaptainId) {}

	void SetGroupId(unsigned short theId) { mGroupId = theId; }	
	void SetNewCaptainId(unsigned short theId) { mNewCaptainId = theId; }

	unsigned short GetGroupId() const { return mGroupId; }
	unsigned short GetNewCaptainId() const { return mNewCaptainId; }

	virtual RoutingOpType GetType() const { return (RoutingOpType)43; /* SDK RoutingOp_RelinquishCaptaincy */ }
};


typedef SmartPtr<RoutingRelinquishCaptaincyOp> RoutingRelinquishCaptaincyOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingSetGroupMaxPlayersOp : public RoutingOp
{
private:
	unsigned short mGroupId;
	unsigned short mMaxPlayers;

	virtual void Virtual0C();
	virtual WONStatus Virtual10(unsigned char theMsgType, ReadBuffer &theMsg);

public:
	RoutingSetGroupMaxPlayersOp(RoutingConnection *theConnection, unsigned short theGroupId = 0, unsigned short theMaxPlayers = 0) : RoutingOp(theConnection), mGroupId(theGroupId), mMaxPlayers(theMaxPlayers) {}

	void SetGroupId(unsigned short theId) { mGroupId = theId; }	
	void SetMaxPlayers(unsigned short theMaxPlayers) { mMaxPlayers = theMaxPlayers; }

	unsigned short GetGroupId() const { return mGroupId; }
	unsigned short GetMaxPlayers() const { return mMaxPlayers; }

	virtual RoutingOpType GetType() const { return (RoutingOpType)53; /* SDK RoutingOp_SetGroupMaxPlayers */ }
};


typedef SmartPtr<RoutingSetGroupMaxPlayersOp> RoutingSetGroupMaxPlayersOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingSetGroupNameOp : public RoutingOp
{
private:
	unsigned short mGroupId;
	std::wstring mGroupName;

	virtual void Virtual0C();
	virtual WONStatus Virtual10(unsigned char theMsgType, ReadBuffer &theMsg);

public:
	RoutingSetGroupNameOp(RoutingConnection *theConnection) : RoutingOp(theConnection), mGroupId(0) {}
	RoutingSetGroupNameOp(RoutingConnection *theConnection, unsigned short theGroupId, const std::wstring &theName) : RoutingOp(theConnection), mGroupId(theGroupId), mGroupName(theName) {}

	void SetGroupId(unsigned short theId) { mGroupId = theId; }	
	void SetGroupName(const std::wstring &theGroupName) { mGroupName = theGroupName; }

	unsigned short GetGroupId() const { return mGroupId; }
	const std::wstring& GetGroupName() const { return mGroupName; }

	virtual RoutingOpType GetType() const { return (RoutingOpType)54; /* SDK RoutingOp_SetGroupName */ }
};


typedef SmartPtr<RoutingSetGroupNameOp> RoutingSetGroupNameOpPtr;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingSetGroupPasswordOp : public RoutingOp
{
private:
	unsigned short mGroupId;
	std::wstring mGroupPassword;

	virtual void Virtual0C();
	virtual WONStatus Virtual10(unsigned char theMsgType, ReadBuffer &theMsg);

public:
	RoutingSetGroupPasswordOp(RoutingConnection *theConnection) : RoutingOp(theConnection), mGroupId(0) {}
	RoutingSetGroupPasswordOp(RoutingConnection *theConnection, unsigned short theGroupId, const std::wstring &thePassword) : RoutingOp(theConnection), mGroupId(theGroupId), mGroupPassword(thePassword) {}

	void SetGroupId(unsigned short theId) { mGroupId = theId; }	
	void SetGroupPassword(const std::wstring &theGroupPassword) { mGroupPassword = theGroupPassword; }

	unsigned short GetGroupId() const { return mGroupId; }
	const std::wstring& GetGroupPassword() const { return mGroupPassword; }

	virtual RoutingOpType GetType() const { return (RoutingOpType)55; /* SDK RoutingOp_SetGroupPassword */ }
};


typedef SmartPtr<RoutingSetGroupPasswordOp> RoutingSetGroupPasswordOpPtr;
}
#endif
