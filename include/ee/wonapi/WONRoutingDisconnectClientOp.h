#ifndef EE_WONAPI_WONROUTINGDISCONNECTCLIENTOP_H
#define EE_WONAPI_WONROUTINGDISCONNECTCLIENTOP_H
#include "ee/wonapi/WONRouting.h"
namespace WONAPI {
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
class RoutingDisconnectClientOp : public RoutingOp
{
private:
	virtual void Virtual0C();

public:
	RoutingDisconnectClientOp(RoutingConnection *theConnection) : RoutingOp(theConnection) {}
	virtual RoutingOpType GetType() const { return (RoutingOpType)18; }
};

typedef SmartPtr<RoutingDisconnectClientOp> RoutingDisconnectClientOpPtr;
}
#endif
