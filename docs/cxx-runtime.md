# The C++ runtime

`include/port/compat.h`, `include/port/linux/port_msvcrt.h`, `src/Port/Linux/CxxAbi.cpp`

The game was written for Visual C++ 6 and its Dinkum C++ library. The port compiles it with Clang and the system's
C++ library, so a few rules differ. Each difference is closed here, in one place, so the reconstructed source stays
what the VC6 build compiles.

## The dialect

`include/port/compat.h` is included first in every unit:

- `__try` and `__except`: Clang has no structured exception handling. The two WON handlers run their guarded block
  alone (`mingw`) or catch C++ exceptions only (native, where libstdc++ defines `__try` as `try`).
- `char_traits<unsigned char>`: WON keeps byte strings in `std::basic_string<unsigned char>`, which Dinkum allowed
  and the modern libraries do not.
- `min`/`max`: `NOMINMAX` keeps `windows.h`'s macros out of the way, and the templates replace them.
- `typeid` without `<typeinfo>`: VC6 allowed it.
- Small VC6 idioms: the `bool` conversions, the `for`-scope rules, the braced-initializer narrowing, and the
  address-of-temporary argument.

## MSVC's `dynamic_cast`

`src/Port/Linux/CxxAbi.cpp`

MSVC judges a `dynamic_cast` by the type in the object's own vtable. The Itanium ABI, which this build uses, also
requires the object to derive from the pointer's static type, so a cast from a pointer to a class the object does not
derive from fails where MSVC's succeeds.

The game does exactly that: its unit lists have an `RTSDummyUnit` head stored in an `RTSUnit*`, and `RTSUnit::Link`
casts it back. On Windows that cast finds the head; with the Itanium rule it returns `NULL` and the code takes the
wrong branch.

Every native image links with `-Wl,--wrap=__dynamic_cast`, and `CxxAbi.cpp` answers:

- the standard cast runs first;
- if it fails, the object's own type from its vtable decides: when it is the destination type, the cast succeeds at the
  given address, as MSVC's does.

The reference form (`dynamic_cast<T&>`) follows the same rule, because it uses the same function.

## Callbacks and calling conventions

`CallCallback` in `Win32Internal.h` calls a game function with 32-bit arguments whether it is declared `stdcall` or
`cdecl`. Windows calls the game's `cdecl` callbacks through `stdcall` types (the resource enumeration callbacks), and
their callers restore the stack pointer from their own frame. The call saves `esp`, aligns the stack to 16 bytes as the
Linux i386 ABI requires, pushes the arguments, calls, and restores `esp`.

The layer's own functions are `WINAPI` (stdcall on i386) as Wine's headers declare them, so the link and the call
agree.

## The C library

`include/port/linux/port_msvcrt.h` adds the MSVC runtime names glibc lacks, in the C locale with a single-byte code
page, which is how the game runs:

- `_mbs*`: the multi-byte string calls in their single-byte form, as MSVC implements them for that code page;
- `_stricmp`, `_strnicmp`, `_snprintf`, `_vsnprintf`, `_hypot`, `_itoa`, `_ltoa`, `_ultoa`, `_filelength`,
  `_chsize`, `_access`, `_mkdir`, `_rmdir`, `_getcwd`, `_findfirst` and the rest;
- `stricmp` and the other names without the underscore, which VC6 also had.

The 32-bit `long` is the same width on both systems, so the game's integer types need no change.

## Text width

Windows' `wchar_t` is 16 bits; Linux's is 32. WON keeps its text in `wchar_t` and passes it to the `W` functions, so
`include/port/linux/windows.h` adds `wchar_t` overloads for `MultiByteToWideChar`, `WideCharToMultiByte`, `TextOutW`,
`GetTextExtentPoint32W` and the other text calls, which convert at the layer's boundary.

## Memory

`HeapAlloc`, `HeapFree`, `GlobalAlloc`, `LocalAlloc`, `VirtualAlloc` and the C++ `new`/`delete` come from the system's
allocator. The game's Dinkum `operator new[]`/`delete[]` are libstdc++'s in the port. New pages are zero on Linux as
they are on Windows, so the reconstruction's "reads of uninitialised data read zero" fixes
([`FIXES.md`](../FIXES.md)) behave the same way.
