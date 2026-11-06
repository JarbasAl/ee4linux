// RoomDialog original family; pending canonical RoomCtrl class from room owner.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/WONLobbyRoomServer.h"
#include "ee/wonapi/WONLobbyDialogs.h"
namespace WONAPI {
// Empty constructor, automatic owner initialization, zero credit.
RoomDialog::RoomDialog() {}
// FUNCTION: WON 0x7f9110
RoomDialog::~RoomDialog() {}
// Empty Init shares physical2fa500 ret4; zero-credit support, no alias admission inferred.
void RoomDialog::InitComponent(ComponentInit&) {}
}

// Compiler-generated and header-inline functions this unit emits (names in analysis/exe/names.txt):
// TEMPLATE: WON 0x007f90d0
// (public: __thiscall WONAPI::RoomDialog::RoomDialog(void)    [??0RoomDialog@WONAPI@@QAE@XZ])
// TEMPLATE: WON 0x007f90f0
// (protected: virtual void * __thiscall WONAPI::RoomDialog::`scalar deleting destructor'(unsigned int)    [??_GRoomDialog@WONAPI@@MAEPAXI@Z])
