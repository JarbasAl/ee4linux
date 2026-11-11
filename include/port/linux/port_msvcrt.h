/* The MSVC runtime names that the game uses and glibc lacks, for the native Linux build. include/port/compat.h
   includes it first in every unit. The game runs in the C locale with a single-byte code page, so each _mbs function
   is its single-byte form, as the MSVC runtime implements it for that code page. */
#ifndef EE_PORT_LINUX_MSVCRT_H
#define EE_PORT_LINUX_MSVCRT_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <wctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline int _stricmp(const char* a, const char* b) { return strcasecmp(a, b); }
static inline int _strnicmp(const char* a, const char* b, size_t n) { return strncasecmp(a, b, n); }
static inline int stricmp(const char* a, const char* b) { return strcasecmp(a, b); }
static inline int strnicmp(const char* a, const char* b, size_t n) { return strncasecmp(a, b, n); }

#define _snprintf snprintf
#define _vsnprintf vsnprintf
#define _timezone timezone
#define _SPACE wctype("space")      /* the one ctype class the game tests with iswctype */
static inline char* _strdup(const char* s) { return strdup(s); }

static inline char* _strlwr(char* s)
{
    for (char* p = s; *p; ++p)
        *p = (char)tolower((unsigned char)*p);
    return s;
}

static inline char* _strupr(char* s)
{
    for (char* p = s; *p; ++p)
        *p = (char)toupper((unsigned char)*p);
    return s;
}

/* Radix 10 writes a minus sign for a negative value; every other radix writes the value's 32 bits unsigned. */
static inline char* _itoa(int value, char* buffer, int radix)
{
    char digits[33];
    int count = 0;
    int negative = radix == 10 && value < 0;
    unsigned int rest = negative ? 0u - (unsigned int)value : (unsigned int)value;
    do {
        unsigned int digit = rest % (unsigned int)radix;
        digits[count++] = (char)(digit < 10 ? '0' + digit : 'a' + digit - 10);
        rest /= (unsigned int)radix;
    } while (rest);
    char* out = buffer;
    if (negative)
        *out++ = '-';
    while (count)
        *out++ = digits[--count];
    *out = 0;
    return buffer;
}

static inline int _finite(double x) { return isfinite(x); }

/* One int in the stream's binary form. EOF is also a valid value, as in the MSVC runtime: callers check ferror. */
static inline int _getw(FILE* stream)
{
    int value;
    return fread(&value, sizeof value, 1, stream) == 1 ? value : EOF;
}

static inline int _putw(int value, FILE* stream)
{
    return fwrite(&value, sizeof value, 1, stream) == 1 ? value : EOF;
}

/* The game names FILE by its MSVC structure name (EEPathManager.h) */
#define _iobuf _IO_FILE

#define _MAX_PATH  260
#define _MAX_DRIVE 3
#define _MAX_DIR   256
#define _MAX_FNAME 256
#define _MAX_EXT   256

/* Splits a path at its last separator (either slash) and its last dot after that, as the MSVC runtime does; a
   leading drive letter goes to inDrive. A NULL output is skipped. */
static inline void _splitpath(const char* inPath, char* outDrive, char* outDirectory, char* outName, char* outExtension)
{
    const char* rest = inPath;
    if (outDrive)
        outDrive[0] = 0;
    if (rest[0] && rest[1] == ':') {
        if (outDrive) {
            outDrive[0] = rest[0];
            outDrive[1] = ':';
            outDrive[2] = 0;
        }
        rest += 2;
    }
    const char* name = rest;
    for (const char* p = rest; *p; ++p)
        if (*p == '\\' || *p == '/')
            name = p + 1;
    const char* dot = strrchr(name, '.');
    if (!dot)
        dot = name + strlen(name);
    if (outDirectory) {
        memcpy(outDirectory, rest, (size_t)(name - rest));
        outDirectory[name - rest] = 0;
    }
    if (outName) {
        memcpy(outName, name, (size_t)(dot - name));
        outName[dot - name] = 0;
    }
    if (outExtension)
        strcpy(outExtension, dot);
}

/* ---- files by Windows path ----
   The game names files with Windows paths: backslashes, a drive letter, any case. These calls take such a path and
   give the Linux one: Z: is the root, as in Wine, and a name that does not exist as given is looked up without regard
   to case (src/Port/Linux/Win32Path.cpp). The game's images link with --wrap=fopen, which sends their fopen calls to
   the layer's __wrap_fopen: a macro would not hold, because <cstdio> undefines fopen. */
int ee_port_stat(const char* inPath, struct stat* outStat);
int _chmod(const char* inPath, int inMode);
char* _getcwd(char* outPath, int inSize);
/* The absolute Windows path of a path, with . and .. resolved */
char* _fullpath(char* outPath, const char* inPath, size_t inSize);
#define _utimbuf utimbuf
int _utime(const char* inPath, struct utimbuf* inTimes);

/* ---- the multibyte-string functions, single-byte ---- */
static inline int _ismbblead(unsigned int c) { (void)c; return 0; }
static inline int _ismbcspace(unsigned int c) { return isspace((int)(unsigned char)c); }
static inline int _ismbcdigit(unsigned int c) { return isdigit((int)(unsigned char)c); }

static inline size_t _mbslen(const unsigned char* s) { return strlen((const char*)s); }
static inline unsigned char* _mbsinc(const unsigned char* s) { return (unsigned char*)s + 1; }

static inline unsigned char* _mbsdec(const unsigned char* start, const unsigned char* current)
{
    return current > start ? (unsigned char*)current - 1 : NULL;
}

/* The bytes of the first n characters, ending at the terminator */
static inline size_t _mbsnbcnt(const unsigned char* s, size_t n)
{
    size_t count = 0;
    while (count < n && s[count])
        ++count;
    return count;
}

/* The characters in the first n bytes, ending at the terminator */
static inline size_t _mbsnccnt(const unsigned char* s, size_t n) { return _mbsnbcnt(s, n); }

static inline unsigned char* _mbsninc(const unsigned char* s, size_t n)
{
    return s ? (unsigned char*)s + _mbsnbcnt(s, n) : NULL;
}

static inline int _mbscmp(const unsigned char* a, const unsigned char* b) { return strcmp((const char*)a, (const char*)b); }
static inline int _mbsicmp(const unsigned char* a, const unsigned char* b) { return strcasecmp((const char*)a, (const char*)b); }

static inline int _mbsncmp(const unsigned char* a, const unsigned char* b, size_t n)
{
    return strncmp((const char*)a, (const char*)b, n);
}

static inline int _mbsnbcmp(const unsigned char* a, const unsigned char* b, size_t n)
{
    return strncmp((const char*)a, (const char*)b, n);
}

static inline int _mbsnbicmp(const unsigned char* a, const unsigned char* b, size_t n)
{
    return strncasecmp((const char*)a, (const char*)b, n);
}

static inline unsigned char* _mbschr(const unsigned char* s, unsigned int c)
{
    return (unsigned char*)strchr((const char*)s, (int)c);
}

static inline unsigned char* _mbsncpy(unsigned char* dst, const unsigned char* src, size_t n)
{
    return (unsigned char*)strncpy((char*)dst, (const char*)src, n);
}

static inline unsigned char* _mbsnbcat(unsigned char* dst, const unsigned char* src, size_t n)
{
    return (unsigned char*)strncat((char*)dst, (const char*)src, n);
}

static inline unsigned char* _mbslwr(unsigned char* s) { return (unsigned char*)_strlwr((char*)s); }
static inline unsigned char* _mbsupr(unsigned char* s) { return (unsigned char*)_strupr((char*)s); }

/* ---- the x87 control word in the MSVC runtime's abstract form ----
   The simulation's lockstep depends on the precision and rounding the game sets here (Utilities/UMath.cpp). Linux
   starts a process at 64-bit precision, Windows at 53-bit: the native main sets _PC_53 first. The port does its
   floating point on the x87 alone (-mfpmath=387), so the SSE control register is left as it is. */
#define _MCW_EM        0x0008001fu
#define _EM_INEXACT    0x00000001u
#define _EM_UNDERFLOW  0x00000002u
#define _EM_OVERFLOW   0x00000004u
#define _EM_ZERODIVIDE 0x00000008u
#define _EM_INVALID    0x00000010u
#define _EM_DENORMAL   0x00080000u
#define _MCW_RC        0x00000300u
#define _RC_NEAR       0x00000000u
#define _RC_DOWN       0x00000100u
#define _RC_UP         0x00000200u
#define _RC_CHOP       0x00000300u
#define _MCW_PC        0x00030000u
#define _PC_64         0x00000000u
#define _PC_53         0x00010000u
#define _PC_24         0x00020000u
#define _MCW_IC        0x00040000u
#define _IC_AFFINE     0x00040000u
#define _IC_PROJECTIVE 0x00000000u

static inline unsigned int ee_port_x87_to_abstract(unsigned short cw)
{
    unsigned int out = 0;
    if (cw & 0x01) out |= _EM_INVALID;
    if (cw & 0x02) out |= _EM_DENORMAL;
    if (cw & 0x04) out |= _EM_ZERODIVIDE;
    if (cw & 0x08) out |= _EM_OVERFLOW;
    if (cw & 0x10) out |= _EM_UNDERFLOW;
    if (cw & 0x20) out |= _EM_INEXACT;
    switch ((cw >> 8) & 3) {
    case 0: out |= _PC_24; break;
    case 2: out |= _PC_53; break;
    default: out |= _PC_64; break;
    }
    switch ((cw >> 10) & 3) {
    case 0: out |= _RC_NEAR; break;
    case 1: out |= _RC_DOWN; break;
    case 2: out |= _RC_UP; break;
    default: out |= _RC_CHOP; break;
    }
    if (cw & 0x1000) out |= _IC_AFFINE;
    return out;
}

static inline unsigned short ee_port_abstract_to_x87(unsigned int value, unsigned short cw)
{
    cw &= (unsigned short)~0x1f3f;
    if (value & _EM_INVALID) cw |= 0x01;
    if (value & _EM_DENORMAL) cw |= 0x02;
    if (value & _EM_ZERODIVIDE) cw |= 0x04;
    if (value & _EM_OVERFLOW) cw |= 0x08;
    if (value & _EM_UNDERFLOW) cw |= 0x10;
    if (value & _EM_INEXACT) cw |= 0x20;
    switch (value & _MCW_PC) {
    case _PC_24: break;
    case _PC_53: cw |= 0x200; break;
    default: cw |= 0x300; break;
    }
    switch (value & _MCW_RC) {
    case _RC_NEAR: break;
    case _RC_DOWN: cw |= 0x400; break;
    case _RC_UP: cw |= 0x800; break;
    default: cw |= 0xc00; break;
    }
    if (value & _IC_AFFINE) cw |= 0x1000;
    return cw;
}

static inline unsigned int _controlfp(unsigned int value, unsigned int mask)
{
    unsigned short cw;
    __asm__ __volatile__("fnstcw %0" : "=m"(cw));
    if (mask) {
        unsigned int merged = (ee_port_x87_to_abstract(cw) & ~mask) | (value & mask);
        cw = ee_port_abstract_to_x87(merged, cw);
        __asm__ __volatile__("fldcw %0" : : "m"(cw));
    }
    return ee_port_x87_to_abstract(cw);
}

#ifdef __cplusplus
}

/* Windows' wide characters are 16 bits (WCHAR, which Wine's headers make char16_t); glibc's wchar_t is 32. These are
   the wide-string calls the game makes on WCHAR buffers, in the C locale: each byte is one character. */
inline size_t mbstowcs(char16_t* outText, const char* inText, size_t inCount)
{
    size_t i = 0;
    for (; i < inCount && inText[i]; ++i)
        outText[i] = (unsigned char)inText[i];
    if (i < inCount)
        outText[i] = 0;
    return i;
}

/* MSVC's struct _stat and _stat(), by Windows path */
struct _stat : public stat {};
inline int _stat(const char* inPath, struct _stat* outStat) { return ee_port_stat(inPath, outStat); }

inline char16_t* wcscpy(char16_t* outText, const char16_t* inText)
{
    char16_t* out = outText;
    while ((*out++ = *inText++) != 0)
        ;
    return outText;
}
#endif

#endif
