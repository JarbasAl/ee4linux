# Files, paths and the registry

`src/Port/Linux/Win32File.cpp`, `Win32Path.cpp`, `Win32Registry.cpp`

## Windows paths on Linux

The game speaks Windows paths (`Z:\home\miro\...`, `Data\Textures\...`). `LinuxPath` converts one: `Z:` is the root,
backslashes become slashes, and a component that does not exist as written is matched without regard to case. A missing
file keeps its last component as written, so an error message names what the game asked for.

`WindowsPath` converts the other way: an absolute Linux path gets `Z:` and backslashes.

The game starts in its own folder and finds its data there, as the retail game does from its installation folder.

## Files

The layer implements the Win32 file API the game uses:

- `CreateFileA` and the CRT's `fopen`, `_open`, `_stat`, `GetFileAttributesA` and the rest, all through the Windows
  path conversion (`port_msvcrt.h` renames the CRT calls that take a path);
- reads, writes, seeks and the file-pointer calls, with Windows' sharing modes;
- `MoveFileA`, `CopyFileA`, `DeleteFileA`, `CreateDirectoryA`, `RemoveDirectoryA`, `SetFileAttributesA`;
- `FindFirstFileA`, `FindNextFileA`, `FindClose`: a directory listing whose names and attributes follow Windows'
  rules (hidden files, directories, the `.` and `..` entries excluded);
- file mappings: `CreateFileMappingA`, `MapViewOfFile`, `UnmapViewOfFile`, `FlushViewOfFile`;
- `GetFileTime`, `SetFileTime`, `GetFileSize`, `SetEndOfFile`, `GetCurrentDirectoryA`, `SetCurrentDirectoryA`,
  `GetTempPathA`, `GetFullPathNameA`.

Line endings are the game's business: the data files carry Windows' `\r\n` and the layer copies bytes.

## The registry

The registry is a text file in the game folder, `ee-port-registry.reg`, in the format of Wine's `.reg` files.
`EE_PORT_REGISTRY` names another file, so several run folders can share or separate their settings.

When the file does not exist, the first registry call imports the game's keys from the Wine prefix (`WINEPREFIX`, else
`~/.wine`): `HKEY_CURRENT_USER` from `user.reg`, `HKEY_LOCAL_MACHINE` from `system.reg`, where a 32-bit program's keys
are under `Software\Wow6432Node`. The import also brings in the base game's folder, which the game uses to find its
data.

A change writes the whole file again, so the file always holds the current settings. The log says how many keys came
in:

```
ee-port: registry: 12 keys from /…/run/ee-port-registry.reg
ee-port: registry: 9 keys imported from the Wine prefix /home/miro/.wine into /…/run/ee-port-registry.reg
```

Implemented: `RegOpenKeyExA`, `RegCreateKeyExA`, `RegCloseKey`, `RegQueryValueExA`, `RegSetValueExA`,
`RegDeleteValueA`, `RegDeleteKeyA`, `RegEnumKeyExA`, `RegEnumValueA`, `RegQueryInfoKeyA`, the four data types
(`REG_SZ`, `REG_DWORD`, `REG_BINARY`, `REG_MULTI_SZ`) and the Wow6432Node redirection the game expects.

The registry is per folder, so the retail game and the port do not fight over one set of settings, and two copies of
the port keep their own.
