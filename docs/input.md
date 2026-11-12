# DirectInput, keyboard and mouse

`src/Port/Linux/DirectInput.cpp`, `Win32Keys.cpp`

DirectInput 7 is the game's input: the game creates a system keyboard and a system mouse and reads them from its own
threads. The layer feeds both from SDL's events.

## Key codes

A DirectInput key is its PC scan code (set 1), plus `0x80` for an extended key: `DIK_ESCAPE` is 1, `DIK_UP` is
`0xC8`. SDL gives scancodes; `Win32Keys.cpp` maps them to the virtual-key code, the scan code and the extended flag,
and returns false for a key the game has no code for.

`GetAsyncKeyState`, `GetKeyState` and `MapVirtualKeyA` answer from the same table, so the game's own keyboard handling
and DirectInput agree.

## Devices

A `IDirectInput7` creates devices by GUID: the system keyboard, the system mouse, and the game's own mouse
(`GUID_SysMouse` variants). Each device follows DirectInput's rules:

| Rule | The layer |
| --- | --- |
| acquisition | an acquired foreground device is acquired only while its window's top-level window has the focus, and it loses the acquisition when the focus leaves |
| buffered device | keeps `DIPROP_BUFFERSIZE` events; a device that loses an event reports `DI_BUFFEROVERFLOW` once |
| relative axes | report the movement since the last read |
| absolute axes | report the position in the window |
| buttons and wheel | 4 buttons (left, right, middle, first extra) and the wheel's notches |
| `GetDeviceState` | the current state, from any thread |
| `GetDeviceData` | drains the buffer, from any thread |
| properties | range, dead zone, saturation, buffer size and axis mode are kept and answered |

## The mouse while the game plays

When an exclusive foreground mouse is acquired, SDL goes into relative mouse mode: the system cursor is hidden, the
game draws its own pointer, and the window receives no mouse messages, exactly as on Windows. The layer leaves that
mode when the game releases the mouse, when the window loses the focus, and in full screen when the game shows the
multiplayer lobby.

`ClipCursor` restricts the pointer to a rectangle. The game uses it while the mouse is captured.

## Where the events come from

`Win32Window.cpp` turns SDL's events into window messages and, in the same pump, into DirectInput state: a key, mouse
movement, a button and the wheel. That happens on the thread that pumps, under `gSdlLock`; the game reads the state
from any thread, because the state and the buffers are behind the layer's own locks.

SDL's text input gives the game its characters, as `WM_CHAR`, so a keyboard layout that produces text works as it does
on Windows. `TranslateMessage` adds only the characters SDL does not give as text: Backspace, Tab, Enter and Escape,
and Ctrl with a letter.
