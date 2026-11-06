// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyWONSkinConfig.h"
using namespace WONAPI;
// Exact native7-slot array566250..56626b; storage/cleanup helpers0.
static SmartPtr<CursorRef> gStandardCursors[StandardCursor_Max+1];
// FUNCTION: WON 0x00726540
Cursor::Cursor() {}
// Native328590 shared empty original destructor support0.
Cursor::~Cursor() {}
// Native2c7b10 /326b50 actual empty defaults, helper0.
Cursor* Cursor::GetNative() { return NULL; }
Component* Cursor::GetComponent(int&,int&) { return NULL; }
// FUNCTION: WON 0x00726550
Cursor* Cursor::GetStandardCursor(StandardCursor type) { return gStandardCursors[type].get(); }
// FUNCTION: WON 0x00726560
void Cursor::SetStandardCursor(StandardCursor type,Cursor* cursor) {
    if(gStandardCursors[type].get()==NULL)
        gStandardCursors[type]=new CursorRef(cursor);
    else
        gStandardCursors[type]->SetCursor(cursor);
}
// ComponentCursor follows ~Cursor in the original object (VC6 then omits the -1 EH state before the base
// destructor call in ~ComponentCursor, 0x726770).
namespace WONAPI {
// FUNCTION: WON 0x00726710
ComponentCursor::ComponentCursor(Component* component,int mouseX,int mouseY)
    : mComponent(component)
{
    mMouseX=mouseX;
    mMouseY=mouseY;
}
// FUNCTION: WON 0x007267d0
Component* ComponentCursor::GetComponent(int& mouseX,int& mouseY)
{
    mouseX=mMouseX;
    mouseY=mMouseY;
    return mComponent.get();
}
// Genuine SDK inline false provider; shared native30a7b0, zero new-native/primary credit.
bool ComponentCursor::IsNative() { return false; }
}
// FUNCTION: WON 0x007d0320
MSCursor::~MSCursor() { if(mDoDestroy) DestroyCursor(mNativeCursor); }

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00726690
// (public: virtual void * __thiscall WONAPI::CursorRef::`scalar deleting destructor'(unsigned int)    [??_GCursorRef@WONAPI@@UAEPAXI@Z])
// TEMPLATE: WON 0x007266b0
// (public: virtual __thiscall WONAPI::CursorRef::~CursorRef(void)    [??1CursorRef@WONAPI@@UAE@XZ])
// TEMPLATE: WON 0x00728590
// (protected: virtual __thiscall WONAPI::Cursor::~Cursor(void)    [??1Cursor@WONAPI@@MAE@XZ])
// TEMPLATE: WON 0x007286b0
// (protected: virtual void * __thiscall WONAPI::Cursor::`scalar deleting destructor'(unsigned int)    [??_GCursor@WONAPI@@MAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00726750
// (public: virtual void * __thiscall WONAPI::ComponentCursor::`scalar deleting destructor'(unsigned int)    [??_GComponentCursor@WONAPI@@UAEPAXI@Z])

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x00726770
// (public: virtual __thiscall WONAPI::ComponentCursor::~ComponentCursor(void)    [??1ComponentCursor@WONAPI@@UAE@XZ])
