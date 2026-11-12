// Port build only, as EEPortLinks.cpp: the lobby names that the VC6 link of EE-AOC.exe takes from the original image.
// The includes are the lobby units' own (EELobbyFileDialog.cpp).
#include <windows.h>
#include <string>
#include <list>
#include <vector>

#include "lle/LowLevelEngine.h"

#include "ee/EEWonNet.h"
#include "ee/wonapi/WONLobbyResource.h"
#include "ee/wonapi/WONLobbyGUI.h"
#include "ee/wonapi/EELobbyGameCode.h"

using namespace WONAPI;

// 0x29171b: the shared empty string
GUIString::GUIString() : mData(GetEmptyGUIStringData())
{}

// 0x2f5380 (Component::FireEvent's code): FireEvent reads the second argument as a bool
void Component::Fn2F5380(int inType, int inParam)
{
    this->FireEvent(inType, (unsigned char)inParam != 0);
}

// No retail body is identified (the header's 0x328340 is findNoCase). It compares with or without case, by the
// flag, through the two identified comparisons.
int GUIStringData::Compare(const GUIStringData* inOther, bool inCaseSensitive) const
{
    return inCaseSensitive ? this->Compare(inOther) : this->CompareNoCase(inOther);
}

#ifdef EE_PORT_LINUX
// The Linux C++ ABI emits a class's vtable and typeinfo only with its first non-inline virtual function (see
// EEPortLinks.cpp). No retail body is identified: every listener the lobby makes has its own.
void ComponentListener::HandleComponentEvent(ComponentEvent*)
{
    OutputDebugStringA("ComponentListener::HandleComponentEvent has no source in this build");
    MessageBoxA(NULL, "ComponentListener::HandleComponentEvent has no source in this build", "Empire Earth",
                MB_OK | MB_ICONSTOP);
    ExitProcess(3);
}
#endif
