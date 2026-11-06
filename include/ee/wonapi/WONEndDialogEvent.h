// Complete genuine SDK dialog-completion event owner; SOURCE_ONLY_NO_NATIVE_ID.
#ifndef EE_WONAPI_WONENDDIALOGEVENT_H
#define EE_WONAPI_WONENDDIALOGEVENT_H
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
class EndDialogEvent : public WindowEventBase
{
protected:
	SmartPtr<Window> mWindowRef;
	SmartPtr<Component> mComponent;
	int mResult;

	virtual ~EndDialogEvent();

public:
	EndDialogEvent(int theResult, Window *theWindow, Component *theComponent = NULL);

	virtual void Deliver();
};
}
#endif
