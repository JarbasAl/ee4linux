/* Wine's windows.h, with what the game needs beyond it on Linux. */
#ifndef EE_PORT_LINUX_WINDOWS_H
#define EE_PORT_LINUX_WINDOWS_H

#include_next <windows.h>

/* The game wraps resource names that are already MAKEINTRESOURCE pointers (IDC_ARROW) in MAKEINTRESOURCE again;
   VC6 truncates the pointer to WORD, Clang refuses a direct pointer-to-WORD cast. */
#undef MAKEINTRESOURCEA
#undef MAKEINTRESOURCEW
#define MAKEINTRESOURCEA(i) ((LPSTR)(ULONG_PTR)(WORD)(ULONG_PTR)(i))
#define MAKEINTRESOURCEW(i) ((LPWSTR)(ULONG_PTR)(WORD)(ULONG_PTR)(i))

#ifdef __cplusplus
#include <type_traits>

/* WON keeps its text in wchar_t, which is 16 bits on Windows and 32 here, and passes it to the W functions. These
   overloads take wchar_t text (src/Port/Linux/Win32Text.cpp). They are templates that accept wchar_t alone, so a
   NULL argument still picks the WCHAR function. */
int ee_port_MultiByteToWideChar(UINT inCodePage, DWORD inFlags, LPCSTR inText, int inLength, wchar_t* outText,
                                int inSize);
int ee_port_WideCharToMultiByte(UINT inCodePage, DWORD inFlags, const wchar_t* inText, int inLength, LPSTR outText,
                                int inSize, LPCSTR inDefault, LPBOOL outUsedDefault);
BOOL ee_port_TextOutW(HDC inDC, int inX, int inY, const wchar_t* inText, int inLength);
BOOL ee_port_GetTextExtentPoint32W(HDC inDC, const wchar_t* inText, int inLength, LPSIZE outSize);

template <class Char, class = typename std::enable_if<std::is_same<Char, wchar_t>::value>::type>
inline int MultiByteToWideChar(UINT inCodePage, DWORD inFlags, LPCSTR inText, int inLength, Char* outText, int inSize)
{
    return ee_port_MultiByteToWideChar(inCodePage, inFlags, inText, inLength, outText, inSize);
}

template <class Char, class = typename std::enable_if<std::is_same<Char, wchar_t>::value>::type>
inline int WideCharToMultiByte(UINT inCodePage, DWORD inFlags, const Char* inText, int inLength, LPSTR outText,
                               int inSize, LPCSTR inDefault, LPBOOL outUsedDefault)
{
    return ee_port_WideCharToMultiByte(inCodePage, inFlags, inText, inLength, outText, inSize, inDefault,
                                       outUsedDefault);
}

template <class Char, class = typename std::enable_if<std::is_same<Char, wchar_t>::value>::type>
inline BOOL TextOutW(HDC inDC, int inX, int inY, const Char* inText, int inLength)
{
    return ee_port_TextOutW(inDC, inX, inY, inText, inLength);
}

template <class Char, class = typename std::enable_if<std::is_same<Char, wchar_t>::value>::type>
inline BOOL GetTextExtentPoint32W(HDC inDC, const Char* inText, int inLength, LPSIZE outSize)
{
    return ee_port_GetTextExtentPoint32W(inDC, inText, inLength, outSize);
}
#endif

#endif
