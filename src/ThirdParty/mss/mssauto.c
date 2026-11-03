// Miles Sound System 6.x, mss32.lib static member MSS_auto_cleanup (reconstruction; the Miles SDK sources are not
// available). mss.h routes AIL_startup() through it (include/mss/mss.h): it registers AIL_shutdown with atexit so
// the library shuts down at process exit.
// Rich Utc12_2_C 9044: built with VC6 + Processor Pack (msvc6.5pp, c2 9044); the relinked DLL's Rich then has the
// original's Utc12_2_C 9044 row. C, speed-optimised (`add esp, 4` after the call, the stub's call is a tail jump).
// -Ox, not -O2: the same code, but without -Gy the two functions share one plain .text like the original object
// (as two COMDATs they change LINK's /OPT:ICF representative choice for the mixer's folded Dinkum functions).
// Inside mss32.lib AIL_shutdown is not dllimport: the stub jumps to the import thunk.
// The stub (0x100025a0, 5 bytes + nop fill to 0x100025b0) is verifier-exact with --size 0x5; it has no row of its
// own in analysis/mixer/functions.jsonl.
// UNIT-COMPILER: msvc6.5pp
// UNIT-FLAGS: -Ox -MD

#include <stdlib.h>

void __stdcall AIL_shutdown(void);

// PROVISIONAL name (static: no symbol of the original names it)
static void __cdecl MSS_auto_cleanup_shutdown(void)
{
    AIL_shutdown();
}

// FUNCTION: MIXER 0x10002590
long __cdecl MSS_auto_cleanup(void)
{
    atexit(MSS_auto_cleanup_shutdown);
    return 0;
}
