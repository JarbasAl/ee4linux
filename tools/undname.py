"""Undecorate MSVC C++ names via Windows dbghelp."""
import ctypes
_dbghelp = ctypes.windll.dbghelp
_dbghelp.UnDecorateSymbolName.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_uint, ctypes.c_uint]


def undname(name, flags=0):
    buf = ctypes.create_string_buffer(2048)
    n = _dbghelp.UnDecorateSymbolName(name.encode(), buf, len(buf), flags)
    return buf.value.decode() if n else name
