// Genuine SDK EndDialogEvent constructor/explicit destructor/delivery family.
// SOURCE_ONLY_NO_NATIVE_ID; no Native/primary/physical attribution.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONEndDialogEvent.h"
using namespace WONAPI;
EndDialogEvent::EndDialogEvent(int theResult, Window *theWindow, Component *theComponent) :
  mResult(theResult), mWindowRef(theWindow), mComponent(theComponent) 
{
	mWindow = theWindow; 
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
EndDialogEvent::~EndDialogEvent()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void EndDialogEvent::Deliver()
{
	mWindow->Fn32B0A0(mResult,mComponent);
}
