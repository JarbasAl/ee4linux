/* Included first in every unit of the port build (tools/build_port.py), never by the VC6 build.
   It fills the gaps between the VC6 dialect and a modern compiler with its own C++ library. */
#ifndef EE_PORT_COMPAT_H
#define EE_PORT_COMPAT_H

#ifdef EE_PORT_LINUX
#include "port_msvcrt.h"
#endif

/* Windows' long is 32 bits on every target; a 64-bit Linux build's is 64. The x86-64 build compiles the game from a
   copy in which long is 32 bits (tools/long32.py), and defines EE_PORT_LONG32 for the places where the source itself
   must change for a wider pointer. */

#define NOMINMAX        /* windows.h's min and max macros break the C++ library; the templates below replace them */

/* Clang has no structured exception handling for 32-bit mingw or Linux. The two WON handlers (the thread entry's
   crash dump and the symbol lookup's fault guard) run their guarded block alone, or, with libstdc++, which defines
   __try as try itself, catch C++ exceptions only. */
#ifdef EE_PORT_LINUX
#define __try try
#define __except(filter) catch (...)
#else
#define __try if (1)
#define __except(filter) else
#endif

#ifdef __cplusplus
#include <typeinfo>     /* VC6 allows typeid without it */
#include <type_traits>

/* The target's own widths. This header is the port's, so tools/long32.py does not rewrite it and its long is the
   compiler's: the 32-bit long of the rewrite is a property of the game's files, which the layout asserts beside each
   record check (a unit the rewrite missed changes a record's size and fails there). What is left to check here is the
   target itself. */
static_assert(sizeof(int) == 4, "the game's fixed-width fields are int-sized");
static_assert(sizeof(char) == 1, "the game's bytes are bytes");
#ifdef EE_PORT_LINUX64
static_assert(sizeof(void*) == 8, "the x86-64 target is a 64-bit port: pointers are 8 bytes wide");
#else
static_assert(sizeof(void*) == 4, "the 32-bit targets keep the retail's pointer width");
#endif

#include <cstring>
#include <cwchar>
#include <ios>
/* libc++ defines char_traits only for the standard character types; WON keeps byte strings in
   std::basic_string<unsigned char>, which Dinkum allowed. */
namespace std {
template <> struct char_traits<unsigned char> {
    typedef unsigned char char_type;
    typedef unsigned int int_type;
    typedef streamoff off_type;
    typedef streampos pos_type;
    typedef mbstate_t state_type;
    static void assign(char_type& a, const char_type& b) { a = b; }
    static bool eq(char_type a, char_type b) { return a == b; }
    static bool lt(char_type a, char_type b) { return a < b; }
    static int compare(const char_type* a, const char_type* b, size_t n) { return n ? memcmp(a, b, n) : 0; }
    static size_t length(const char_type* s) { size_t n = 0; while (s[n]) ++n; return n; }
    static const char_type* find(const char_type* s, size_t n, const char_type& c)
    {
        return n ? static_cast<const char_type*>(memchr(s, c, n)) : 0;
    }
    static char_type* move(char_type* a, const char_type* b, size_t n) { return n ? static_cast<char_type*>(memmove(a, b, n)) : a; }
    static char_type* copy(char_type* a, const char_type* b, size_t n) { return n ? static_cast<char_type*>(memcpy(a, b, n)) : a; }
    static char_type* assign(char_type* a, size_t n, char_type c) { return n ? static_cast<char_type*>(memset(a, c, n)) : a; }
    static int_type not_eof(int_type c) { return eq_int_type(c, eof()) ? ~eof() : c; }
    static char_type to_char_type(int_type c) { return char_type(c); }
    static int_type to_int_type(char_type c) { return int_type(c); }
    static bool eq_int_type(int_type a, int_type b) { return a == b; }
    static int_type eof() { return int_type(-1); }
};
}

/* The original's code reads some fields of its global objects through names of their own (gNumPlayers is
   gWorld.mNumPlayers): the VC6 link binds both names to one address in the original image. The units that define
   those objects name each such global at its field here, at the port's own field offsets. */
struct EEPortAliases {
    static void BindGame();     // Empire Earth.cpp
    static void BindRender();   // EERenderObject.cpp
};
#ifdef _WIN32
#define EE_PORT_SYMBOL(name) "_" #name      /* C names carry an underscore in 32-bit Windows objects, not in ELF */
#else
#define EE_PORT_SYMBOL(name) #name
#endif
#define EE_PORT_ALIAS(name, object, member) \
    __asm__(".globl " EE_PORT_SYMBOL(name) "\n\t.set " EE_PORT_SYMBOL(name) ", " EE_PORT_SYMBOL(object) " + %c0" \
            : : "i"(__builtin_offsetof(decltype(object), member)))

/* windows.h's macros exactly: (((a) < (b)) ? (a) : (b)) and (((a) > (b)) ? (a) : (b)), with the usual arithmetic
   conversions of both arguments. Equal values and NaN give the same operand as the macros, which keeps the simulation
   identical. */
template <class A, class B>
inline typename std::common_type<A, B>::type min(A a, B b) { return a < b ? a : b; }
template <class A, class B>
inline typename std::common_type<A, B>::type max(A a, B b) { return a > b ? a : b; }
#endif

#endif
