// Complete original-backed Component source completion; all selected methods support0.
// UNIT-FLAGS: -O2 -MD -GR -GX -DEE_DINKUM_SXL233
#include <windows.h>
#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>
#include "lle/LowLevelEngine.h"
#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
namespace WONAPI {
// ListItem's ctor, Virtual04 and Virtual10 live in ListArea.cpp (same object: the ctor must be seen before
// ListArea's `new ListItem`).
// FUNCTION: WON 0x0070a800
void ListItem::Virtual08(ListArea*) {}
// Shared ICF native0ac9c8/3B; new ListItem definition only, no globally-new native credit.
void ListItem::Virtual0C(ListArea*,Container*) {}
// Shared ICF native2907a3/3B, each definition0primary; no globally-new native claim.
void ListItem::Virtual20(ListArea*,int,int) {}
// Shared ICF native156dd8/3B, new nominal definition only.
void ListItem::Virtual24(ListArea*) {}
// Shared ICF native2907a3/3B.
void ListItem::Virtual28(ListArea*,int,int) {}
// Shared ICF native2907a3/3B.
void ListItem::Virtual2C(ListArea*,int,int) {}
// Shared ICF native0df42d/3B.
void ListItem::Virtual30(ListArea*,int,int,int) {}
// Shared ICF native0df42d/3B.
void ListItem::Virtual34(ListArea*,int,int,int) {}
}
