# Windows, messages and the desktop

`src/Port/Linux/Win32Window.cpp`

## The model

An `HWND` is an object of the layer. Making a window makes no native window: a window gets its native SDL window
when it is first drawn to. DirectDraw asks for an OpenGL window, GDI for a software one. An OpenGL request replaces a
software window, and a software request on an OpenGL window returns `NULL`, because DirectDraw owns what that window
shows.

A window has no frame here: its window rectangle is its client rectangle, and SDL draws any decoration outside it.

Windows support: classes (`RegisterClassExA`, `GetClassInfoExA`), `CreateWindowExA`, `DestroyWindow`,
`ShowWindow`, `SetWindowPos`, `MoveWindow`, `AdjustWindowRect`, `GetClientRect`/`GetWindowRect`,
`ClientToScreen`/`ScreenToClient`, `GetWindowPlacement`, `GetWindowTextA`/`SetWindowTextA`,
`GetWindowLongA`/`SetWindowLongA`, focus and capture, `WindowFromPoint`, `GetDesktopWindow`, `GetParent`,
`InvalidateRect`/`ValidateRect`/`UpdateWindow`, `GetSystemMetrics`, `GetSysColor`, `ChangeDisplaySettingsA` and
`EnumDisplaySettingsA`, `MessageBoxA`, the cursors and the clipboard.

## Ownership and pumping

A window belongs to the thread that made it, as on Windows: its messages go to that thread's queue, and that thread
makes and changes the native window.

The start screen makes its own window on its own thread while the game loads, and the game's window belongs to the
main thread. Both are real, visible windows with their own native SDL windows, and each thread pumps SDL's events
while it waits for a message. `gSdlLock` serialises every call into SDL, so the threads take turns.

A window that another thread owns gets no native window from a drawing call; the call returns `NULL` and the drawing
layer waits for the owner's next pump.

## Messages

The pump turns SDL events into window messages: keys, text, mouse movement, buttons and the wheel. Each event's window
is found through the native window's `hwnd` data, and the message goes to that window's thread queue, so a thread
pumps the events of every window, not only its own.

The order of `PeekMessage` matches Windows: sent messages, posted messages, `WM_QUIT`, `WM_PAINT`, then `WM_TIMER`.

`WM_SETFOCUS`, `WM_KILLFOCUS`, `WM_ACTIVATEAPP`, `WM_ACTIVATE`, `WM_SHOWWINDOW`, `WM_SIZE`, `WM_MOVE`, `WM_PAINT` and
`WM_CHAR` are delivered as the game expects. Text comes from SDL's text input; `TranslateMessage` adds only the
characters SDL does not give as text.

## Full screen

`EE_PORT_WINDOWED` keeps the window at the game's mode size. Without it, a display-mode change puts the window in
desktop full screen: the window covers the desktop, and the game's mode is scaled to fit it with its aspect kept, and
centred. The window keeps the game's style: borderless, and skipped by the task bar and the pager.

The modes offered are 640x480, 800x600, and the common sizes up to the desktop's, in 16 and 32 bits at the desktop's
refresh rate ([DirectDraw](display-directdraw.md)).

## Cursors

A cursor is an ARGB image with a hot spot. `CreateCursor` builds one from the game's AND and XOR masks: AND 1 with XOR
0 is transparent, AND 0 with XOR 0 is black, AND 0 with XOR 1 is white, and AND 1 with XOR 1 is black (Windows would
invert the screen, which SDL cannot do). `LoadCursorA`, `LoadCursorFromFileA`, `SetCursor`, `ShowCursor`,
`GetCursorPos`, `SetCursorPos`, `ClipCursor`, `SetCapture` and `ReleaseCapture` follow.

`ShowCursor` keeps a count: the cursor shows while the count is 0 or more. While DirectInput holds the mouse
exclusively, SDL is in relative mouse mode, the system cursor is hidden, and the game draws its own pointer
([input](input.md)).

The SDL cursor is created when it is first shown and freed when the game destroys it; the layer's cursor object
outlives the image, because the game shares loaded cursors.

## The clipboard

Text only, through SDL's clipboard, in UTF-8. `CF_TEXT` is code page 1252, `CF_UNICODETEXT` is `wchar_t` text (32 bits
here, because the game is the only reader of what it writes). `OpenClipboard`, `EmptyClipboard`, `SetClipboardData`,
`GetClipboardData` and `IsClipboardFormatAvailable` are implemented; a block the clipboard owns is freed on the next
`EmptyClipboard`.

## Message boxes

`MessageBoxA` writes the caption and the text to the log, then shows an SDL message box. With
`EE_PORT_NO_MESSAGE_BOXES` it answers `IDOK` at once, which is what the test harness and a scripted run want.

## The input method

The game's IME code runs only on IME messages, which the layer never sends. `ImmGetContext` returns `NULL` and the
rest of the IME calls succeed and do nothing.

## What is not there

`TerminateThread` and `SuspendThread` are not implemented. Named kernel objects are not shared between processes.
There is no window enumeration (`EnumWindows`), no hooks and no DDE.
